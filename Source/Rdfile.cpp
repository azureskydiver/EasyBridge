//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// Rdfile.cpp : implementation of file reading
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "EasyBvw.h"
#include "Player.h"
#include "mainfrm.h"
#include "MainFrameOpts.h"
#include "filecode.h"
#include "progopts.h"
#include "deck.h"
#include "card.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


//
// Easy Bridge File Format codes
//
const char* tszBlockName[] = {
	"", 
	"[[File Information]]",
	"[[File Description]]",
	"[[Hand Information]]",
	"[[Round Information]]",
	"[[Game Information]]",
	"[[Game Record]]",
	"[[Match Information]]",
	"[[Misc Information]]",
	"[[File Comments]]",
	"[[South Play Analysis]]", 
	"[[West Play Analysis]]", 
	"[[North Play Analysis]]", 
	"[[East Play Analysis]]", 
};

const char* tszItemName[] = {
	// 0..9
	"",
	"Program ID",
	"Program Major Version",
	"Program Minor Version",
	"Program Incremental Version",
	"Program Build Number",
	"Program Build Date",
	"File Date",
	"", "",
	// 10..19
	"Current Hand(North)",
	"Current Hand(East)",
	"Current Hand(South)",
	"Current Hand(West)",
	"Original Hand(North)",
	"Original Hand(East)",
	"Original Hand(South)",
	"Original Hand(West)",
	"", "", 
	// 20..29
	"Lead Player for Current Hand",
	"# Cards Played in Current Round",
	"Trick Card #1 (South)",
	"Trick Card #2 (West)",
	"Trick Card #3 (North)",
	"Trick Card #4 (East)", 
	"", "", "", "",
	// 30..49
	"Game Status Code",
	"Is Rubber In Progress?", 
	"Is Game in Progress?",
	"Is Bidding in Progress?",
	"Was Hand Dealt?",
	"Dealer",
	"Contract Suit",
	"Contract Level",
	"Contract Modifier",
	"Number of Bids Made",
	"Bidding History",
	"Declarer",
	"", "", "", "", "", "", "", "", 
	// 50..79
	"Number of Tricks Played in Game",
	"# of Tricks won in Game by N/S",
	"# of Tricks won in Game by E/W",
	"", "", "", "", "", "", "", 
	"Lead Player for Current Game", 
	"GameTrick #1",
	"GameTrick #2",
	"GameTrick #3",
	"GameTrick #4",
	"GameTrick #5",
	"GameTrick #6",
	"GameTrick #7",
	"GameTrick #8",
	"GameTrick #9",
	"GameTrick #10",
	"GameTrick #11",
	"GameTrick #12",
	"GameTrick #13",
	"", "", "", "", "", "", 
	// 80..99
	"North/South Bonus Points", 
	"North/South Game 1 Points", 
	"North/South Game 2 Points", 
	"North/South Game 3 Points", 
	"North/South Games Won", 
	"East/West Bonus Points", 
	"East/West Game 1 Points", 
	"East/West Game 2 Points", 
	"East/West Game 3 Points", 
	"East/West Games Won", 
	"Current Game", 
	"Bonus Score Record",
	"Game Score Record",
	"", "", 
	"", "", "", "", "", 
	// 100..109
	"Automatically Show File Comments", 
	"Automatically Show Bidding History", 
	"Automatically Show Play History", 
	"Automatically Show Analysis Dialogs", 
	"", "", "", "", "", "", 
	// 
};


//
const int MAXLINELEN = 1024;
const int MAXBUFLEN	 = 1024;
static char szBuffer[MAXBUFLEN+1];
static CString strBuf, strBiddingHistory;
static int nLineLen = 0;
static int nDataPosition = 0;;



//
//---------------------------------------------------------
//
BOOL CEasyBDoc::ReadFile(CArchive& ar) 
{
	int i,nRtnCode,nLineCode,nSectionCode;
	int	nIndex,nValue,nOffset,nLen;
	int nPlayOffset, nPos;
	int nBidIndex = 0;
	CString strMessage,string,partString;
	CCard* pCard;
	double fValue;
	BOOL bValue;
	BOOL bFileInfoFound = FALSE;

	
	// read
	m_nLineNumber = 0;
	try
	{
		for(;;) 
		{

			nRtnCode = ReadLine(ar, strBuf);
next:		if (nRtnCode == EOF)
				break;
			if (nRtnCode == 0)
				continue;
			if ((strBuf[0] == '#') || (strBuf[0] == '*'))
				continue;
			nLineLen = nRtnCode;
			nLineCode = ParseLine(strBuf,nLineLen);
			
			if (nLineCode < 0) 
			{

				// section signifier
				nSectionCode = -nLineCode;
				if (nSectionCode == BLOCK_FILEINFO)
				{
					//
					bFileInfoFound = TRUE;
					// file info; skip this section
					do {
						nRtnCode = ReadLine(ar, strBuf);
					} while ((nRtnCode != EOF) && (strBuf.Left(2) != "[["));
					//
					goto next;
				} 
				//
				if (nSectionCode == BLOCK_FILEDESC) 
				{
					// file description; read until next block
					m_strFileDescription.Empty();
					do {
						nRtnCode = ReadLine(ar, strBuf);
						if (strBuf.Left(2) != "[[")
						{
							strBuf += "\r\n";
							m_strFileDescription += strBuf;
						}
					} while ((nRtnCode != EOF) && (strBuf.Left(2) != "[["));

					// trim ending CR/LF
					int nLen = m_strFileDescription.GetLength();
					if ((nLen >= 2) && (m_strFileDescription[nLen-1] == _T('\n')))
						m_strFileDescription = m_strFileDescription.Left(nLen-2);
					//
					goto next;
				} 
				else if (nSectionCode == BLOCK_COMMENTS) 
				{
					// file comments
					for (;;) 
					{
						nRtnCode = ReadLine(ar, strBuf);
						if ((nRtnCode == EOF) || (strBuf.Left(2) == "[[")) 
						{
							// update file comments dialog if it's open
							CWnd* pWnd = pMAINFRAME->GetDialog(twFileCommentsDialog);
							if (pWnd)
								pWnd->SendMessage(WM_COMMAND, WMS_UPDATE_TEXT, FALSE);
							goto next;
						}
						strBuf += "\r\n";
						m_strFileComments += strBuf;
					}
				}
				else if ((nSectionCode == BLOCK_SOUTH_ANALYSIS) ||
						 (nSectionCode == BLOCK_WEST_ANALYSIS) ||
						 (nSectionCode == BLOCK_NORTH_ANALYSIS) ||
						 (nSectionCode == BLOCK_EAST_ANALYSIS))
				{
					// don't bother to save the analysis
					int nPlayer = nSectionCode - BLOCK_SOUTH_ANALYSIS;
					CString strAnalysisBiffer;
					do {
						nRtnCode = ReadLine(ar, strBuf);
	//					strBuf += "\r\n";
	//					strAnalysisBiffer += strBuf;
					} while ((nRtnCode != EOF) && (strBuf.Left(2) != "[["));
	//				m_player[nPlayer].SetValueString(tszAnalysis,  strAnalysisBiffer);
					goto next;
				}

			} 
			else if (nLineCode > 0) 
			{

				// first skip spaces
				nLen = strBuf.GetLength();
				while ((nDataPosition < nLen) && 
						(strBuf[nDataPosition] == ' '))
					nDataPosition++;
				if (nDataPosition >= nLen)
					continue;
				// then get data item
				string = strBuf.Mid(nDataPosition);
				nValue = atoi(string);
				fValue = atof(string);
				if ((string == "Yes") || (string == "1"))
					bValue = TRUE;
				else
					bValue = FALSE;

				switch(nLineCode) 
				{

					case ITEM_PROGRAM_ID:
						m_strFileProgTitle = string;
						break;

					case ITEM_MAJOR_VERSIONNO:
						m_nFileProgMajorVersion = nValue;
						break;

					case ITEM_MINOR_VERSIONNO:
						m_nFileProgMinorVersion = nValue;
						break;

					case ITEM_INCREMENT_VERSIONNO:
						m_nFileProgIncrementVersion = nValue;
						break;

					case ITEM_BUILD_NUMBER:
						m_nFileProgBuildNumber = nValue;
						break;

					case ITEM_BUILD_DATE:
						m_strFileProgBuildDate = string;
						break;

					case ITEM_FILE_DATE:
						m_strFileDate = string;
						break;

					// hand info, 10..19
					case ITEM_CURRHAND_NORTH:
						AssignCards(string,NORTH);
						break;

					case ITEM_CURRHAND_EAST:
						AssignCards(string,EAST);
						break;

					case ITEM_CURRHAND_SOUTH:
						AssignCards(string,SOUTH);
						break;

					case ITEM_CURRHAND_WEST:
						AssignCards(string,WEST);
						break;

					case ITEM_ORIGHAND_NORTH:
						AssignCards(string,NORTH,TRUE);
						break;

					case ITEM_ORIGHAND_EAST:
						AssignCards(string,EAST,TRUE);
						break;

					case ITEM_ORIGHAND_SOUTH:
						AssignCards(string,SOUTH,TRUE);
						break;

					case ITEM_ORIGHAND_WEST:
						AssignCards(string,WEST,TRUE);
						break;

					// current round info
					case ITEM_CURR_ROUND_LEAD:
						m_nRoundLead = StringToPosition(string);
						break;

					case ITEM_NUM_CARDS_PLAYED_IN_ROUND:
						m_numCardsPlayedInRound = nValue;
						break;

					case ITEM_TRICK_CARD_1:
						m_pCurrTrick[0] = deck.GetCard(string);
						break;

					case ITEM_TRICK_CARD_2:
						m_pCurrTrick[1] = deck.GetCard(string);
						break;

					case ITEM_TRICK_CARD_3:
						m_pCurrTrick[2] = deck.GetCard(string);
						break;

					case ITEM_TRICK_CARD_4:
						m_pCurrTrick[3] = deck.GetCard(string);
						break;

					// game status info
					case ITEM_VIEW_STATUS_CODE:
						pVIEW->SetCurrentMode((CEasyBView::ScreenMode)nValue);
						break;

					case ITEM_RUBBER_IN_PROGRESS:
						theApp.SetValue(tbRubberInProgress, bValue);
						break;

					case ITEM_GAME_IN_PROGRESS:
						// TEMP
//						theApp.SetValue(tbGameInProgress, FALSE);
						theApp.SetValue(tbGameInProgress, bValue);
						break;

					case ITEM_BIDDING_IN_PROGRESS:
						theApp.SetValue(tbBiddingInProgress, bValue);
						break;

					case ITEM_HANDS_DEALT:
						m_bHandsDealt = bValue;
						break;
					
					case ITEM_CONTRACT_SUIT:
						nLen = string.GetLength();
						m_nContractSuit = CharToSuit(string.GetAt(0));
						break;

					case ITEM_CONTRACT_LEVEL:
						m_nContractLevel = nValue;
						break;

					case ITEM_CONTRACT_MODIFIER:
						switch(nValue) 
						{
							case 0:
								m_bDoubled = FALSE;
								m_bRedoubled = FALSE;
								m_nContractModifier = 0;
								break;
							case 1:
								m_bDoubled = TRUE;
								m_bRedoubled = FALSE;
								m_nContractModifier = 1;
								break;
							case 2:
								m_bDoubled = FALSE;
								m_bRedoubled = TRUE;
								m_nContractModifier = 2;
								break;
						}
						break;

					case ITEM_DEALER:
						m_nDealer = StringToPosition(string);
						break;

					case ITEM_NUM_BIDS:
//						m_numBidsMade = nValue;
						break;

					case ITEM_BIDDING_HISTORY:
						strBiddingHistory = string;
						break;

					case ITEM_DECLARER:
						m_nDeclarer = StringToPosition(string);
						m_nContractTeam = GetPlayerTeam(m_nDeclarer);
						m_nDefendingTeam = (m_nContractTeam == NORTH_SOUTH)? EAST_WEST : NORTH_SOUTH;
						break;

					// game record
					case ITEM_NUM_TRICKS_PLAYED:
						m_numTricksPlayed = nValue;
						break;

					case ITEM_NUM_TRICKS_WON_NS:
						m_numTricksWon[0] = nValue;
						break;

					case ITEM_NUM_TRICKS_WON_EW:
						m_numTricksWon[1] = nValue;
						break;

					case ITEM_GAME_LEAD:
						m_nGameLead = StringToPosition(string);
						break;

					case ITEM_GAME_TRICK_1: case ITEM_GAME_TRICK_2: 
					case ITEM_GAME_TRICK_3: case ITEM_GAME_TRICK_4: 
					case ITEM_GAME_TRICK_5: case ITEM_GAME_TRICK_6: 
					case ITEM_GAME_TRICK_7: case ITEM_GAME_TRICK_8: 
					case ITEM_GAME_TRICK_9: case ITEM_GAME_TRICK_10: 
					case ITEM_GAME_TRICK_11: case ITEM_GAME_TRICK_12: 
					case ITEM_GAME_TRICK_13:
						try
						{
							nIndex = nLineCode - ITEM_GAME_TRICK_1;
							nOffset = 0;
							nLen = string.GetLength();
							// first read the lead player for the trick
							partString = string.Mid(nOffset);
							// NCR Get index to position
							int nLeadPos = StringToPosition(partString);
							// NCR Check if valid and skip if not
							if (nLeadPos == -1)  // NB -1 returned by StringToPosition
								break;  // NCR invalid position
							m_nTrickLead[nIndex] = nLeadPos; // Save
							nOffset = string.Find(' ');
							//
							for(i=0;i<4;i++) 
							{
								while((nOffset < nLen) && (string[nOffset] == ' '))
									nOffset++;
								partString = string.Mid(nOffset);
								nOffset += 2;
								if (partString.IsEmpty()) 
								{
									strMessage.Format("Incomplete Trick record at line %d;\n%s",
													  m_nLineNumber, string);
									AfxMessageBox(strMessage);
									break;
								}
								//
								if (partString.Left(2) == "--")
								{
									m_pGameTrick[nIndex][i] = NULL;
								}
								else
								{
									pCard = deck.GetCard(partString);
									m_pGameTrick[nIndex][i] = pCard;
								}
							}
							// insert the trick record into the game record
							// in the proper order
							nPlayOffset = nIndex * 4;
							nPos = m_nTrickLead[nIndex];
							for(i=0;i<4;i++)
							{
								CCard* pCard = m_pGameTrick[nIndex][nPos];
								if (pCard)
									m_nPlayRecord[nPlayOffset+i] = pCard->GetDeckValue();
								nPos = GetNextPlayer(nPos);
							}
							// and finally read the trick's winner 
							while((nOffset < nLen) && (string[nOffset] == ' '))
								nOffset++;
							partString = string.Mid(nOffset);
							m_nTrickWinner[nIndex] = StringToPosition(partString);
						}
						catch(...)
						{
							// error
						}
						break;

					// match info
					case ITEM_SCORE_NS_BONUS:
						m_nBonusScore[NORTH_SOUTH] = nValue;
						break;

					case ITEM_SCORE_NS_GAME0:
						m_nGameScore[0][NORTH_SOUTH] = nValue;
						break;

					case ITEM_SCORE_NS_GAME1:
						m_nGameScore[1][NORTH_SOUTH] = nValue;
						break;

					case ITEM_SCORE_NS_GAME2:
						m_nGameScore[2][NORTH_SOUTH] = nValue;
						break;

					case ITEM_SCORE_NS_GAMES_WON:
						m_numGamesWon[NORTH_SOUTH] = nValue;
						break;

					case ITEM_SCORE_EW_BONUS:
						m_nBonusScore[EAST_WEST] = nValue;
						break;

					case ITEM_SCORE_EW_GAME0:
						m_nGameScore[0][EAST_WEST] = nValue;
						break;

					case ITEM_SCORE_EW_GAME1:
						m_nGameScore[1][EAST_WEST] = nValue;
						break;

					case ITEM_SCORE_EW_GAME2:
						m_nGameScore[2][EAST_WEST] = nValue;
						break;

					case ITEM_SCORE_EW_GAMES_WON:
						m_numGamesWon[EAST_WEST] = nValue;
						break;

					case ITEM_CURRENT_GAME_INDEX:
						m_nCurrGame = nValue-1;
						break;

					case ITEM_BONUS_SCORE_RECORD:
						m_strArrayBonusPointsRecord.Add(StripQuotes(string));
						break;

					case ITEM_GAME_SCORE_RECORD:
						m_strArrayTrickPointsRecord.Add(StripQuotes(string));
						break;

					// misc info
					case ITEM_AUTOSHOW_COMMENTS:
						m_bShowCommentsUponOpen = bValue;
						break;
					case ITEM_AUTOSHOW_BID_HISTORY:
						m_bShowBidHistoryUponOpen = bValue;
						break;
					case ITEM_AUTOSHOW_PLAY_HISTORY:
						m_bShowPlayHistoryUponOpen = bValue;
						break;
					case ITEM_AUTOSHOW_ANALYSES:
						m_bShowAnalysesUponOpen = bValue;
						break;

				}

			} 
			else 
			{

	/*
				// unknown line
				strMessage.Format("Found Unknown line\n%s.\nContinue?",strBuf);
				if (AfxMessageBox(strMessage, MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
					break;
	*/
			}

		}
	}
	catch(...)
	{
		// handle any improper file error here
		ClearAllInfo();
//		AfxMessageBox("An error ocurred while reading the game file.");
		return FALSE;
	}

	// see if this was a valid file
	if (!bFileInfoFound)
	{
		AfxMessageBox("This is not a proper Easy Bridge game file.");
		AfxThrowFileException(CFileException::genericException);
	}

	//
	// do some sanity checks
	//
	m_nContract = MAKEBID(m_nContractSuit, m_nContractLevel);
	if (!ISPLAYER(m_nDeclarer) || !ISBID(m_nContract))
		theApp.SetValue(tbGameInProgress, FALSE);

	//
	// parse the bidding history
	//
	if (!ISPLAYER(m_nDeclarer))
		m_nDeclarer = SOUTH;
	nPos = m_nDealer; // NCR-101 start at dealer NOT m_nDeclarer;
	m_nCurrPlayer = nPos;
	int nTeam = GetPlayerTeam(nPos);
	nOffset = 0;
	// 
	nLen = strBiddingHistory.GetLength();

	for(i=0;;i++) 
	{
		// skip leading spaces
		while((nOffset < nLen) && (strBiddingHistory[nOffset] == ' '))
			nOffset++;
		if (nOffset >= nLen)
			break;				// Exit loop when no data left
		// grab the next bid
		partString = strBiddingHistory.Mid(nOffset);
		int nBid = ContractStringToBid(partString);
		// and record it
		m_nBiddingHistory[m_numBidsMade] = nBid;
		m_numBidsMade++;
		m_nCurrPlayer = GetNextPlayer(m_nCurrPlayer);
		int nBiddingRound =  i/4;  // NCR-101 changed from:  i % 4;  
		m_nBidsByPlayer[nPos][nBiddingRound] = nBid;
		// see if this is an actual numeric bid
		if (ISBID(nBid))
		{
			m_nValidBidHistory[m_numValidBidsMade] = nBid;
			m_numValidBidsMade++;
			m_nLastValidBid = nBid;
			m_nLastValidBidTeam = nTeam;
		}
		// skip over remainder of current bid string
		while((nOffset < nLen) && (strBiddingHistory[nOffset] != ' '))
			nOffset++;
		// and move to the next player
		nPos = GetNextPlayer(nPos);
		nTeam = GetOpposingTeam(nTeam);
	}
	if (ISBID(m_nContract))
		UpdateBiddingHistory();


	// tally some figures
	m_nTotalScore[0] = m_nGameScore[0][0] + m_nGameScore[1][0] +
					   m_nGameScore[2][0] + m_nBonusScore[0];
	m_nTotalScore[1] = m_nGameScore[0][1] + m_nGameScore[1][1] +
					   m_nGameScore[2][1] + m_nBonusScore[1];

	// vulnerability
	if ((m_numGamesWon[0] > 0) && (m_numGamesWon[1] > 0)) 
	{
		m_nVulnerableTeam = BOTH;
		m_bVulnerable[0] = m_bVulnerable[1] = TRUE;
	} 
	else if (m_numGamesWon[0] > 0) 
	{
		m_nVulnerableTeam = NORTH_SOUTH;
		m_bVulnerable[0] = TRUE;
	} 
	else if (m_numGamesWon[1] > 0)
	{
		m_nVulnerableTeam = EAST_WEST;
		m_bVulnerable[1] = TRUE;
	} 
	else 
	{
		m_nVulnerableTeam = NEITHER;
	}

	//
	// set contract info
	//
	m_nContract = ContractParamsToBid(m_nContractSuit,m_nContractLevel);
	m_nTrumpSuit = m_nContractSuit;
	m_nBiddingRound = nBidIndex;
	// set play info 
	if (ISBID(m_nContract) && ISPLAYER(m_nDeclarer))
	{
		// contract has been reached
		m_nDummy = GetPartner((int) m_nDeclarer);
		m_nGameLead = GetNextPlayer(m_nDeclarer);
		m_nRoundLead = m_nGameLead;
		m_nCurrPlayer = m_nRoundLead;
		m_nTrickLead[0] = m_nRoundLead;
//		m_pPlayer[m_nDummy]->SetDummyFlag(TRUE);
//		m_pPlayer[m_nDeclarer]->SetDeclarerFlag(TRUE);
	}
	else
	{
		// contract has NOT been reached, so restart
		m_nCurrPlayer = m_nDealer;
		m_numBidsMade = 0;
	}

	// restore initial hands (temp?)
	for(i=0;i<4;i++)
		m_pPlayer[i]->RestoreInitialHand();

	// not reviewing game
	m_bReviewingGame = FALSE;

	// NCR-GUI1 Set flag for a fully played hand?
	if(m_numTricksPlayed == 13) {
		m_nPlayRound = 13;  // NCR can we use this as a flag???
	} // end NCR-GUI1

	// all done
	return TRUE;
}





//
void CEasyBDoc::AssignCards(CString& str, int nPosition, BOOL bInitialHand)
{
	int i,nOffset=0;
	int nLen = str.GetLength();
	BOOL bError = FALSE;
	CCard* pCard;
	//
	for(i=0;i<13;i++) 
	{
		// skip spaces
		while((nOffset < nLen) && (str[nOffset] == ' '))
			nOffset++;
		// get next card string
		if (nOffset < nLen) 
		{
			pCard = deck.GetCard(str.Mid(nOffset));
			nOffset += 2;
			if (pCard == NULL) 
			{
				bError = TRUE;
			} 
			else 
			{
				if (bInitialHand)
					PLAYER(nPosition).AddCardToInitialHand(pCard);
				else
					PLAYER(nPosition).AddCardToHand(pCard,FALSE);
			}
		} 
		else 
		{
			break;
		}
		if (bError) 
		{
			CString strMessage;
			strMessage.Format("Illegal card assignment on line %d, index %d;\n%s",
								m_nLineNumber,i,strBuf);
			AfxMessageBox(strMessage);
			break;
		}
	}
}



//
//---------------------------------------------------------
//
int CEasyBDoc::ParseLine(CString& string, int nLineLength) 
{
	int i,nEnd;
//	int	nLen,nCode;

	// compare against block designators
	for(i=0;i<NUM_BLOCKS;i++) 
	{
		if (string == tszBlockName[i])
			return -i;
	}
 	// not a block item, so scan the line for an '=' sign
	// to see if this is a data item
	for(nEnd=0;nEnd<nLineLength;nEnd++)
		if (string[nEnd] == '=')
			break;
	// see if we reached the end -- if so, return negative
	if (nEnd == nLineLength)
		return 0;
	// else save the value marker
	nDataPosition = nEnd + 1;
	// and trim ending spaces
	while ((nEnd > 0) && (string[nEnd-1] == ' '))
		nEnd--;
	// and check for a valid item code
	for(i=0;i<NUM_ITEMS;i++) 
	{
		if (string.Left(nEnd) == tszItemName[i])
			return i;
	}
	// nothing found
	return 0;
}




//
//---------------------------------------------------------
//
//
int CEasyBDoc::ReadLine(CArchive&ar, CString& strDest) 
{
	int i,len,nBytesRead;
	BYTE  byte;
	char* pBuf;
	CString strBuffer;

   	// initialize string to null 
	strDest = "";
    // read data
	nBytesRead = 0;
	pBuf = strBuffer.GetBuffer(MAXLINELEN+2);
	do {
		TRY	
		{
			ar >> byte;
			if (byte == '\r')
				continue;
			pBuf[nBytesRead++] = (char) byte;
		}
		CATCH(CArchiveException, arException)
		{
			if (arException->m_cause == CArchiveException::endOfFile)
				if (nBytesRead > 0)
					return 0;
				else
					return EOF;
		}
		AND_CATCH (CFileException, fileException)
		{				 
			if (fileException->m_cause == CFileException::endOfFile)
				if (nBytesRead > 0)
					return 0;
				else
					return EOF;
		}
		END_CATCH
	} while	((byte != '\n') && (nBytesRead <= MAXLINELEN) && (byte != EOF));
	//
	strBuffer.ReleaseBuffer(nBytesRead);

    // check for EOF
    if(nBytesRead == 0)
    	return EOF;

    // find end of line - 10(LF) or 13(CR)
	len = nBytesRead;
    for(i=0;i<nBytesRead;i++) 
	{
    	if((strBuffer[i] == 10) || (strBuffer[i] == 13)) 
		{
        	len = i;
			break;
		}
	}
  	strDest = strBuffer.Left(len);
	// return to calling function
	m_nLineNumber++;
	return len;
}