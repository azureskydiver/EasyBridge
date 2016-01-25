//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
// NCR-PBNI - Added non-export file logic - min of 4 tags needed
//----------------------------------------------------------------------------------------

// RdfilePBN.cpp : implementation of PBN file reading
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "EasyBvw.h"
#include "Player.h"
#include "mainfrm.h"
#include "MainFrameOpts.h"
#include "filecodePBN.h"
#include "progopts.h"
#include "GameRecord.h"
#include "deck.h"
#include "card.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


//
// PBN File Format codes
//
const char* tszTagName[] = {
	"",			// 0
	"Event",		
	"Site",
	"Date",
	"Round",
	"Board",
	"West",
	"North",
	"East",
	"South",
	"Dealer",		// 10
	"Vulnerable",	
	"Deal", 
	"Scoring",
	"Declarer", 
	"Contract",		// 15
	"Result", 
	"", "", "", 
	"Auction",			// 20
	"", "", "", "", 
	"Play",				// 25
	"", "", "", "", 
	"Note",				// 30
	"Description", 
	"Generator", 
	"OptimumScore", "",   // NCR-776 for BridgeWebs files
	"", "", "", "", 
	"*",				// 39
};



//
const int MAXLINELEN = 1024;
const int MAXBUFLEN	 = 1024;
static char szBuffer[MAXBUFLEN+1];
static CString strBuf;
static int nLineLen = 0;
static int nPreviousTag = 0;
static int nGameIndex = 0;
static CGameRecord* pGameRecord = NULL;
static CStringArray strLines;
static int numLines;

static bool isExportFile = true;  // NCR-PGNI  default to export file




//
//---------------------------------------------------------
//
BOOL CEasyBDoc::ReadFilePBN(CArchive& ar) 
{
	int nLineCode, nValue, nCode;
	CString strMessage, string, strTag, strValue, partString, strModifier;
	double fValue;
	BOOL bValue;
	CString strLine;


	// init some flags
	m_bReviewingGame = FALSE;
	m_bGameReviewAvailable = FALSE;

	// preload the file
	numLines = PreloadPBNFile(ar, strLines);
/*
	int sizeOf = strLines.GetSize();   // NCR DEBUG ONLY
	for(int xxx = 0; xxx < sizeOf; xxx++) {
		CString nxtStr = strLines.GetAt(xxx);
		ASSERT(!nxtStr.IsEmpty());
	}
*/
	// read in the ines
	pGameRecord = NULL;
	m_nLineNumber = 0;
	nGameIndex = 0;
	nPreviousTag = 0;
	int numGamesLoaded = 0;
	int numInvalidGames = 0;
	int numTagsRead = 0;
	int numDeclarers = 0;		// NCR use to test reviewable deals ie no Declarer means no review

	//
	for(;;) 
	{
		// check for EOF
		if (m_nLineNumber >= numLines)
			break;

		strBuf = strLines.GetAt(m_nLineNumber++);

		//
		strBuf.TrimLeft();
		strBuf.TrimRight();
		nLineLen = strBuf.GetLength();

		// see if this is a blank line (but watch out for multiple empty lines)
		if (strBuf.IsEmpty())
		{
			// flush out the current game
			if (pGameRecord)
			{
				pGameRecord->AnalyzePlayRecord();
				if (pGameRecord->IsValid(isExportFile))   // NCR_PBNI added isExportFile
				{
					m_gameRecords.Add(pGameRecord);
					numGamesLoaded++;
					nGameIndex++;
				}
				else
				{
					delete pGameRecord;
					numInvalidGames++;
				}
				pGameRecord = NULL;
				nPreviousTag = 0;
				continue;
			}
		}

		// else parse the line for tokens
		nLineCode = ParseLinePBN(strBuf, strTag, strValue, nLineLen);

		// check for error
		if (nLineCode < 0)
		{
			// junk tag?
			if (m_nLineNumber == 0)
			{
				AfxMessageBox("This is not a proper PBN format file.");
				AfxThrowFileException(CFileException::genericException);
			}
			continue;
		}

		//
		// else the tag is OK
		//

		// see if the value is "#" -- then substitute
		if ((strValue == "#") && (nGameIndex > 0))
		{
			// look up the value from the previous game
			CGameRecord* pPrevGame = m_gameRecords[nGameIndex-1];
			std::map<CString,CString>::const_iterator iter = pPrevGame->m_mapTagValues.find(strTag);
			if (iter != pPrevGame->m_mapTagValues.end())
				strValue = (*iter).second;
		}

		//
		// get data value
		//
		nValue = atoi(strValue);
		fValue = atof(strValue);
		if ((strValue == "Yes") || (strValue == "1"))
			bValue = TRUE;
		else
			bValue = FALSE;

		//
		// if this is a valid tag, alloc a new game object
		//
		if ((!strTag.IsEmpty()) && (pGameRecord == NULL))
		{
			pGameRecord = new CGameRecord;
			numTagsRead = 0;
		}


		//
		//-------------------------------------------------------------------
		//
		// process item
		//

		switch(nLineCode) 
		{
			case TAG_EVENT:
				break;

			case TAG_SITE:
				// ignore for now
				break;

			case TAG_DATE:
				// ignore for now
				break;

			case TAG_ROUND:
				// ignore for now
				break;

			case TAG_BOARD:
				// ignore for now
				break;

			case TAG_WEST:
				// ignore for now
				break;

			case TAG_NORTH:
				// ignore for now
				break;

			case TAG_EAST:
				// ignore for now
				break;

			case TAG_SOUTH:
				// ignore for now
				break;

			// dealer
			case TAG_DEALER:
				if (!strValue.IsEmpty() && (strValue[0] != '?'))
					pGameRecord->m_nDealer = CharToPosition(strValue[0]);
				break;

			// vulnerability
			case TAG_VULNERABLE:
				if (strValue.CompareNoCase("NS") == 0)
					pGameRecord->m_nVulnerability = NORTH_SOUTH;
				else if (strValue.CompareNoCase("EW") == 0)
					pGameRecord->m_nVulnerability = EAST_WEST;
				else if ((strValue.CompareNoCase("All") == 0) || (strValue.CompareNoCase("Both") == 0))
					pGameRecord->m_nVulnerability = BOTH;
				else if((strValue.CompareNoCase("None") == 0) || (strValue.CompareNoCase("-") == 0)) // NCR-PBNI
					pGameRecord->m_nVulnerability = NEITHER;
				else      // error if not above
					AfxMessageBox("Invalid Vulnerabliity: " + strValue);
				break;

			case TAG_DEAL:
//				AssignCardsPBN(strValue);
				break;

			case TAG_SCORING:
				// ignore for now
				break;

			case TAG_DECLARER:
				if (!strValue.IsEmpty() && (strValue[0] != '?')) {
					pGameRecord->m_nDeclarer = CharToPosition(strValue[0]);
					numDeclarers++;   // NCR count for need to do review
				}
				break;

			case TAG_CONTRACT:
				{
				// decipher the contract
				if ((strValue.GetLength() < 2) || (strValue[0] == '?'))
				{
					// contract has not yet been set
					theApp.SetValue(tbGameInProgress, FALSE);
					break;
				}
				int nContractLevel = atoi(strValue);
				int nContractSuit = CharToSuit(strValue[1]);
				int nContract = MAKEBID(nContractSuit, nContractLevel);
				int nContractModifier = 0;
				//
				pGameRecord->m_nContract = nContract;
				// get modifier
				if (nContractSuit == NOTRUMP)
					strModifier = strValue.Mid(3);
				else
					strModifier = strValue.Mid(2);
				if (strModifier.CompareNoCase("X") == 0)
					nContractModifier = 1;
				else if (strModifier.CompareNoCase("XX") == 0)
					nContractModifier = 2;
				//
				pGameRecord->m_nContractModifier = nContractModifier;
				break;
				}

			case TAG_RESULT:
				// ignore for now
				break;

			// ge the auction
			case TAG_AUCTION:
				if (strValue.IsEmpty())
					break;
				// parse the bids
				nCode = ParseBidsPBN(ar, strValue);
				break;

			case TAG_PLAY:
				// read in the plays
				if (strValue.IsEmpty() || (strValue[0] == '?'))
					break;
				// parse the plays
				nCode = ParsePlaysPBN(ar, strValue);
				// mark this file as being reviewable
				if (ISBID(pGameRecord->m_nContract))
				{
					// contract must also be valid
					m_bReviewingGame = TRUE;
					m_bGameReviewAvailable = TRUE;
				}
				break;

			case TAG_NOTE:
				// add to the appropriate list
				if (nPreviousTag == TAG_AUCTION)
					pGameRecord->m_strBiddingNotes.Add(strValue);
				else if (nPreviousTag == TAG_PLAY)
					pGameRecord->m_strPlayNotes.Add(strValue);
				break;


			case TAG_OPTIMUMSCORE:  // NCR-776 for BridgeWebs files
				// ignore 
				break;

			case TAG_TERMINATOR:
				// ignore for now
				break;
		} // end switch(nLineCode)

		// save this tag
		nPreviousTag = nLineCode;
		numTagsRead++;

		// save the tag-value pair in the map
		if (pGameRecord)
		{
			// but don't save notes here
			if (strTag.CompareNoCase("Note") != 0)
				pGameRecord->SetTagValue(strTag, strValue);
		}
		
	} // end for(;;) 

	// see if the file ended with no ending empty line
//	if (nGameIndex == 0 && pGameRecord)
	if (pGameRecord && (numTagsRead >= 1))
	{
		pGameRecord->AnalyzePlayRecord();

		if (pGameRecord->IsValid(isExportFile))  // NCR-PBNI added isExportFile
		{
			m_gameRecords.Add(pGameRecord);
			numGamesLoaded++;
			nGameIndex++;
		}
		//
		pGameRecord = NULL;
	}

	// if the file has > 1 game record, set game review mode
	if (nGameIndex > 1) 
	{
		m_bReviewingGame = TRUE;
		m_bGameReviewAvailable = TRUE;
	}

	// see if we have a dangling (invalid) game object
	if (pGameRecord)
		delete pGameRecord;

	// all done
	return TRUE;
}




//
// ParseBidsPBN()
//
int CEasyBDoc::ParseBidsPBN(CArchive& ar, const CString& strValue)
{
	CString string, partString;

	// read in the bids
	int nExitCode = 0;
	BOOL bCommentStarted = FALSE;
	for(;;) 
	{
		if (m_nLineNumber >= numLines)
			break;
		partString = strLines.GetAt(m_nLineNumber++);
		if ( !partString.IsEmpty() && 
			 ((partString[0] == '[') || (partString[0] == '*')) )
			break;
		string += partString + " ";
	}

	// check if we read up to the next tag, or are at the end of the game record
	if (partString.IsEmpty() || (partString[0] == '['))
	{
		strBuf = partString;
		if (m_nLineNumber < numLines)
			m_nLineNumber--;
	}

	// and process
	int nEnd, nOffset = 0;
	int nBidIndex = 0;
	int numBids = 0;
	int nLen = string.GetLength();

	//
	for(;;) 
	{
		// skip leading spaces
		while((nOffset < nLen) && (string[nOffset] == ' '))
			nOffset++;
		if (nOffset < nLen) 
		{
			// grab the next chunk
			nEnd = string.Mid(nOffset).Find(' ');
			if (nEnd < 0)
				nEnd = nLen;
			partString = string.Mid(nOffset, nEnd);
			nOffset += nEnd + 1;
			if (partString[0] == '-')
			{
				// ignore
				continue;
			}
			else if (partString[0] == '=')
			{
				// this is a note reference
				int nRef = atoi(partString.Mid(1));
				pGameRecord->m_mapBiddingNotes.SetAt(nBidIndex, nRef);
			}
			else if (partString[0] == '$')
			{
				// this is a NAG -- ignore it
			}
			else if ((partString.CompareNoCase("AP") == 0) || (partString[0] == '*'))
			{
				// All Pass
				for(int i=0;i<3;i++)
					pGameRecord->m_nBids.Add(BID_PASS);
				numBids += 3;
			}
			else
			{
				// this is a bid, so record it
				int nBid = ::StringToBid(partString);
				if (nBid == BID_NONE)
				{
					// error!
//					AfxMessageBox("Invalid bid string!");
//					AfxThrowFileException(CFileException::generic);
					break;
				}
				//
				pGameRecord->m_nBids.Add(nBid);
				numBids++;
			}
		}
		else
		{
			return nExitCode;
		}
	}

	// 
	return 0;
}




//
// ParsePlaysPBN()
//
int CEasyBDoc::ParsePlaysPBN(CArchive& ar, const CString& strValue)
{
	CString string, partString;

	// read in the rest of the plays
	int nExitCode = 0;
	BOOL bCommentStarted = FALSE;
	for(;;) 
	{
		if (m_nLineNumber >= numLines)
			break;
		partString = strLines.GetAt(m_nLineNumber++);
		if ( (!partString.IsEmpty() && 
			 ((partString[0] == '[') || (partString.Find('*') >= 0)))
			 || partString.IsEmpty() )  // NCR allow empty string to end this section
			break;
		string += partString + " ";
	}

	// check if we read up to the next tag, or are at the end of the game record
	if (partString.IsEmpty() || (partString[0] == '['))
	{
		strBuf = partString;
		if (m_nLineNumber < numLines)
			m_nLineNumber--;
	}

	// and process
	int nEnd, nRoundWinner;
	int nOffset = 0;
	int nPlayIndex = 0;
	int nRound = 0;
	int nLen = string.GetLength();
	//
	int nStartingPos = CharToPosition(strValue[0]);
	pGameRecord->m_nRoundLead[0] = nStartingPos;
	pGameRecord->m_numCardsPlayed = 0;

	//
	for(;;) 
	{
		// skip leading spaces
		while((nOffset < nLen) && (string[nOffset] == ' '))
			nOffset++;
		if (nOffset < nLen) 
		{
			// grab the next chunk
			nEnd = string.Mid(nOffset).Find(' ');
			if (nEnd < 0)
				nEnd = nLen;
			partString = string.Mid(nOffset, nEnd);
			nOffset += nEnd + 1;
			if (partString[0] == '=')
			{
				// this is a note reference
				int nRef = atoi(partString.Mid(1));
				pGameRecord->m_mapPlayNotes.SetAt(nPlayIndex, nRef);
			}
			else if (partString[0] == '-')
			{
				// this is a blank play
				nPlayIndex++;
			}
			else if (partString[0] == '$')
			{
				// this is a NAG -- ignore it
			}
			else
			{
				// this is a play, so record it
				int nCardVal = ::StringToDeckValue(partString);
				int nPos = nStartingPos;
				int nPosOffset = nPlayIndex % 4;
				for(int i=0;i<nPosOffset;i++)
					nPos = GetNextPlayer(nPos);
				//
				pGameRecord->m_nGameTrick[nRound][nPos] = nCardVal;
				pGameRecord->m_nPlayRecord[nPlayIndex] = nCardVal;
				nPlayIndex++;
				pGameRecord->m_numCardsPlayed++;
				// if a round is finished, record the round winner
				if (((nPlayIndex) % 4) == 0)
				{
					nRoundWinner = pGameRecord->DetermineRoundWinner(nRound);
					pGameRecord->m_nRoundLead[++nRound] = nRoundWinner;
				}
			}
		}
		else
		{
			return nExitCode;
		}
	}
}




//
// PreloadPBNFile()
//
// - preload the PBN file and strip comments
//
int CEasyBDoc::PreloadPBNFile(CArchive& ar, CStringArray& strLines)
{
	int numLinesRead = 0;
	strLines.RemoveAll();

	// read in the file
	CString strBuf;
	DWORD dwSize = DWORD(ar.GetFile()->GetLength());
	PBYTE pBuf = (PBYTE) strBuf.GetBuffer(dwSize);
	int numBytesRead = ar.Read(pBuf, dwSize);
	ASSERT(numBytesRead == dwSize);
	strBuf.ReleaseBuffer(dwSize);

	// check for export tag
	int nPos = strBuf.Find(_T("% EXPORT"));
	if (nPos < 0)   // NCR_PBNI allow as first line (was < 1)
	{
		AfxMessageBox("The Files is not in PBN Export format.");
//		AfxThrowFileException(CFileException::generic);
		isExportFile = false;  // NCR_PBNI Not export file - fewer tags required
	}

	// remove comments
	nPos = strBuf.Find(_T('{'));
	while(nPos >= 0)
	{
		// see if this comment started a new line
		bool bNewLine = false;
		if ((nPos > 0) && ((strBuf[nPos-1] == _T('\n')) || (strBuf[nPos-1] == _T('\r'))))
			bNewLine = true;

		// find the end of the comment section
		CString strMid = strBuf.Mid(nPos);
		int nEnd = strMid.Find(_T('}'));
		ASSERT(nEnd >= 0);

		// search for the next nonspace character
		nEnd++;
		int nLen = strMid.GetLength();
		// NCR test if comment at end of file
		if(nLen == nEnd)
			break;		// NCR exit if at end
		while ((nEnd < nLen) && (_istspace(strMid[nEnd])))
			nEnd++;

		// and trim and re-combine
		if ((nEnd < nLen) && (strMid[nEnd] == _T('[')) && !bNewLine)
			strBuf = strBuf.Left(nPos) + _T('\n') + strMid.Mid(nEnd);
		else
			strBuf = strBuf.Left(nPos) + strMid.Mid(nEnd);
		nPos = strBuf.Find(_T('{'));
	} // end while() removing comment

	// remove '!' suffixes
	strBuf.Remove(_T('!'));

	// remove any question marks that are not preceded by a quote character
	// '?' characters may be used in tag values as well as used for quality ratings
	for(int i=0;i<strBuf.GetLength();i++)
	{
		if ((i > 0) && (strBuf[i] == _T('?')) && (strBuf[i-1] != _T('\"')))
		{
			strBuf = strBuf.Left(i) + strBuf.Mid(i+1);
			i--;
		}
	}

	// organize into lines
	int nLen, nIndex = -1;
	BOOL bEmptyLineAdded = FALSE;
	do
	{
		// locate the end of the current line
		nLen = strBuf.GetLength();
		nIndex = -1;
		for(int i=0;i<nLen;i++)
		{
			if ((strBuf[i] == _T('\r')) || (strBuf[i] == _T('\n')))
			{
				nIndex = i;
				break;
			}
		}
		// add the line if not empty
		if (nIndex >= 0)
		{
			// add the string if it's not a comment
			if (strBuf[0] != _T('%'))
			{
				// test for an empty line by removing whitespace
				CString strTest = strBuf.Left(nIndex);
				strTest.TrimLeft();
				if (strTest.IsEmpty())
				{
					if (!bEmptyLineAdded)
					{
						strLines.Add(_T(""));
						bEmptyLineAdded = TRUE;
						numLinesRead++;
					}
				}
				else
				{
					// it's a nonblank line
					// trim any rest-of-line comments
					int nPos = strTest.Find(_T(';'));
					if (nPos >= 0)
						strTest = strTest.Left(nPos);

					// and add the line
					strLines.Add(strTest);
					bEmptyLineAdded = FALSE;
					numLinesRead++;
				}
			}

			// and skip over the CF/LF pair
			int nCharCount = 0;
			while ( (nIndex < nLen) && ((strBuf[nIndex] == _T('\r')) || (strBuf[nIndex] == _T('\n'))) )
			{
				nIndex++;
				nCharCount++;
			}

			// if more than one CR/LF pair (or two LF's) was found, add an empty line
			if ( ( (nCharCount > 2) || ((nIndex >= 2) && (strBuf[nIndex-1] == _T('\n')) && (strBuf[nIndex-2] == _T('\n'))) ) && 
				   (!bEmptyLineAdded))
			{
				strLines.Add(_T(""));
				numLinesRead++;
				bEmptyLineAdded = TRUE;
			}

			// chop off the part of the string that was read
			if (nIndex < nLen)
			{
				strBuf = strBuf.Mid(nIndex);
			}
			else
			{
				// reached the end of file
				strBuf.Empty();
				// add an empty line just to make sure
				strLines.Add(_T(""));
				numLinesRead++;
				bEmptyLineAdded = TRUE;
			}
		}
	}
	while ((nIndex >= 0) && !strBuf.IsEmpty());

	// done
	return numLinesRead;
}





//
// AssignCardsPBN()
//
void CEasyBDoc::AssignCardsPBN(const CString& str)
{
	CString strHoldings = str;
	strHoldings.TrimLeft();

	// get the starting player
	int nPlayer = CharToPosition(strHoldings[0]);
	strHoldings = strHoldings.Mid(2);	// go past the "X:" mark
	strHoldings.TrimLeft();

	// read in the holdings string
	int nIndex;
	for(int i=0;i<4;i++)
	{
		int numCards = 0;
		// read in each suit
		for(int nSuit=SPADES;nSuit>=CLUBS;nSuit--)
		{
			if (i < 3)
			{
				// first 3 players
				if (nSuit != CLUBS)
					nIndex = strHoldings.Find('.');
				else
					nIndex = strHoldings.Find(' ');
			}
			else
			{
				// last player
				if (nSuit != CLUBS)
					nIndex = strHoldings.Find('.');
				else
					nIndex = strHoldings.GetLength();

			}
			CString strSuit = strHoldings.Left(nIndex);
			strSuit.TrimLeft();
			// check for end of string
			if (strSuit == _T("."))
				strSuit = _T("");
			//
			for(int j=0;j<strSuit.GetLength();j++)
			{
				int nValue = CharToFaceValue(strSuit[j]);
				int nIndex = MAKEDECKVALUE(nSuit, nValue);
				CCard* pCard = deck.GetSortedCard(nIndex);
				ASSERT(pCard);
				m_pPlayer[nPlayer]->AddCardToHand(pCard,FALSE);
				m_pPlayer[nPlayer]->AddCardToInitialHand(pCard);
				numCards++;
			}
			// move to the next suit
			if (strHoldings.GetLength() > nIndex+1)
				strHoldings = strHoldings.Mid(nIndex+1);
			else
				strHoldings = strHoldings.Mid(nIndex);
		}
		// verify
		if (numCards != 13)
		{
			ClearAllInfo();
			pVIEW->Notify(WM_COMMAND, WMS_REFRESH_DISPLAY);
			AfxMessageBox("Invalid deal string!");
			AfxThrowFileException(CFileException::genericException);
		}
		//
		nPlayer = GetNextPlayer(nPlayer);
	}
	//
	m_bHandsDealt = TRUE;
}





//
int CEasyBDoc::ParseLinePBN(CString& string, CString& strTag, CString& strValue, int nLineLength) 
{
	// clear output
	strTag = strValue = "";
	// look for opening brackets
	string.TrimLeft();
	string.TrimRight();
	//
	int nLen = string.GetLength();
	if (nLen < 2)
		return 0;
	if ((string[0] != '[') || (string[string.GetLength()-1] != ']'))
		return 0;

	// retrieve the tag text
	int nIndex = string.Find(' ');
	if (nIndex < 0)
		return 0;
	strTag = string.Mid(1, nIndex-1);

	// force tag to be uppercase
	strTag.MakeUpper();

	// retrieve the value
	strValue = string.Mid(nIndex+1, nLen-nIndex-2);

	// strip quotes
	if (strValue[0] == '\"')
		strValue = strValue.Mid(1);
	nLen = strValue.GetLength();
	if (strValue[nLen-1] == '\"')
		strValue = strValue.Left(nLen-1);
	strValue.TrimLeft();
	strValue.TrimRight();

	// compare against known tags
	for(int i=0;i<NUM_TAGS;i++) 
	{
		if (strTag.CompareNoCase(tszTagName[i]) == 0)
			return i;
	}

	// else nothing found
	return -1;
}

//------------------------------------------------------------------------
// NCR Added code here for PLL file

BOOL CEasyBDoc::ReadFilePLL(CArchive& ar) 
{	
	const int DealDataLen = 772;   // number of bytes for a deal
	// read in the file
	CString strBuf;
	DWORD dwSize = DWORD(ar.GetFile()->GetLength());
	PBYTE pBuf = (PBYTE) strBuf.GetBuffer(dwSize);
	int numBytesRead = ar.Read(pBuf, dwSize);
	ASSERT(numBytesRead == dwSize);

    // Check first 2 bytes
	if(pBuf[0] != 0x01 || pBuf[1] != 0x04) {
		AfxMessageBox("The File is not in PLL format.");
		AfxThrowFileException(CFileException::genericException);
	}

	int nbrDeals = numBytesRead / DealDataLen;
	if(nbrDeals > 1) {
		AfxMessageBox("EasyBridge currently only reads first deal.");
	}
	// Ok, now parse the hands - we'll only get the first deal
	pGameRecord = new CGameRecord;
	
	const int FirstByte = 48;  // first byte of hands
	int dealer = pBuf[FirstByte + 32]; // 0=N, 1=E, 2=S, 3=W
	const Position PPL2EB[] = {NORTH, EAST, SOUTH, WEST};
	dealer = PPL2EB[dealer];  // convert
	pGameRecord->m_nDealer = dealer;  // save

	int vulner = pBuf[FirstByte + 33];
	CString theDeal[4][4];      // hands and suits
	const int bb[] = {1,2,4,8,16,32,64,128}; // bit test masks
	const char * LowFaceValue[] = {"2", "3", "4", "5", "6", "7", "8", "9"};
	const char * HighFaceValue[] = {"T", "J", "Q", "K", "A"};

	for(int jj = 0; jj < 16; jj++) {
		char byte1 = pBuf[FirstByte+(jj*2)+1];  // High card flags T-A
		char byte2 = pBuf[FirstByte+(jj*2)+0];  // Low card flags 2-9 
		int handIdx = jj / 4;
		int suitIdx = 3 - (jj % 4);
		// First get the High cards from byte1
    	int kk; // NCR-FFS added here, removed below
		for(/*int*/ kk = 4; kk >=0; kk--) {
			if((byte1 & bb[kk]) != 0)
				theDeal[handIdx][suitIdx] += HighFaceValue[kk];
		} // end for(kk) thru High card byte
		// now the low cards
		for(kk = 7; kk >=0; kk--) {
			if((byte2 & bb[kk]) != 0)
				theDeal[handIdx][suitIdx] += LowFaceValue[kk];
		} // end for(kk) thru Low card byte

	} // end jj

	// Now build the deal a la PBN
	CString aDeal = "N:";
	for (int i = 0; i < 4; i++) {
		aDeal += theDeal[i][0] + "." +theDeal[i][1] + "." +theDeal[i][2] + "." +theDeal[i][3] 
			      + ((i < 3) ? " " : "");  // nothing at the end
    }
/*
	// Hard code for testing
	pGameRecord->m_nDealer = NORTH; //SOUTH=0, WEST=1, NORTH=2, EAST=3,
	pGameRecord->m_nVulnerability = EAST_WEST; //NEITHER=-1, NORTH_SOUTH=0, EAST_WEST=1, BOTH=2
	// Following deal from N-Sbid1H_Make4H.pbn
	CString aDeal = "S:KQ62.Q97.K765.52 94.AKJ4.Q92.JT97 875.852.AJT3.K63 AJT3.T63.84.AQ84";

	// Following from EasyBridgeGame2.pbn   hand order: W N E S
//	CString aDeal = "W:6.A852.KT43.Q965 A532.94.AJ85.A74 KQJ87.3.Q62.KT83 T94.KQJT76.97.J2";
//	pGameRecord->m_nDealer = EAST; //SOUTH=0, WEST=1, NORTH=2, EAST=3,
*/
	pGameRecord->SetTagValue("DEAL", aDeal);  //NOTE ALL CAPS for Keys!!! <<<<<<
	pGameRecord->SetTagValue("BOARD", "1");   // Hardcoded value ???

//	AssignCardsPBN(aDeal);  // use this method
//	theApp.SetValue(tbGameInProgress, FALSE);

	pGameRecord->AnalyzePlayRecord();
//	if(pGameRecord->IsValid()) {  // this only for PBN
		m_gameRecords.Add(pGameRecord);
//	}

	return TRUE;
}
