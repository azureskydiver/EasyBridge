//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Globals.cpp : Some global functions
//

#include "stdafx.h"
#include "EasyB.h"
#include "deck.h"
#include "card.h"
#include "ConventionSet.h"


/*
// default order in which suits get displayed, left to right
const int defSuitDisplaySequence[4][4] = {
	{ SPADES, HEARTS, CLUBS, DIAMONDS },
	{ SPADES, HEARTS, DIAMONDS, CLUBS },
	{ DIAMONDS, CLUBS, HEARTS, SPADES },
	{ CLUBS, DIAMONDS, HEARTS, SPADES }
};
*/
const LPCTSTR szPosition[4] = { "South", "West", "North", "East" };
const LPCTSTR szPositionShort[4] = { "Sou.", "West", "Nor.", "East" };
const LPCTSTR tszTeam[4] = {	"North/South", "East/West", "Both", "Neither" };
const LPCTSTR tszTeamBrief[4] = { "N/S", "E/W", "Both", "Neither" };
const LPCTSTR szCardName[15] = { "", "", "Deuce", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace" };
const LPCTSTR szSuitName[5] = { "Clubs", "Diamonds", "Hearts", "Spades", "No Trump" };
const LPCTSTR szSuitNameSingular[5] = { "Club", "Diamond", "Heart", "Spade", "No Trump" };
const LPCTSTR szSuitNameShort[5] = { "C", "D", "H", "S", "NT" };
const LPCTSTR szFaceValueName[] = {
	"", "", "two", "three", "four", "five", "six", "seven",
	"eight", "nine", "ten", "Jack", "Queen", "King", "Ace"
};

const TCHAR cSuit[5] = { 'C', 'D', 'H', 'S', 'N' };
const TCHAR cCard[] = { 
	' ', ' ', '2', '3', '4', '5', '6', '7', 
	'8', '9', 'T', 'J', 'Q', 'K', 'A' 
};
const TCHAR cFaceCard[5] = { 'T', 'J', 'Q', 'K', 'A' };

const int numDefCardBacks = 1;




/////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//
/////////////////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////////////////
//
// Global Operations
//
/////////////////////////////////////////////////////////////////////////////





//
// FormString()
//
const CString FormString(LPCTSTR szFormat, ...)
{
	const int tnBufSize = 1024;
	CString strResult;
	LPTSTR szBuffer = strResult.GetBuffer(tnBufSize);
	va_list list;
	va_start(list, szFormat);
	_vsnprintf(szBuffer, tnBufSize, szFormat, list);
	va_end(list);
	strResult.ReleaseBuffer();
	return strResult;
}


//
const CString FormString(int nVal) 
{ 
	return FormString("%ld",nVal); 
}


//
const CString FormString(double fVal) 
{ 
	return FormString("%f",fVal); 
}


//
CString WrapInQuotes(const CString& strItem)
{
	return CString(_T('\"')) + strItem + _T('\"');
}


//
CString StripQuotes(const CString& strItem)
{
	CString strOutput;
	if ((strItem.GetLength() > 0) && (strItem[0] == _T('\"')))
		strOutput = strItem.Mid(1);
	else
		strOutput = strItem;
	//
	int nLength = strOutput.GetLength();
	if ((nLength > 0) && (strOutput[nLength-1] == _T('\"')))
		strOutput = strOutput.Left(nLength-1);
	//
	return strOutput;
}






#define ROUND(x) (int) ((((x) - (int)(x)) >= 0.5)? (x)+1 : (x) )



// return calue between 0 and nMax
int GetRandomValue(int nMax)
{
	// avoid rounding bias
//	int nValue = (int) ((rand() / (float)RAND_MAX) * nMax + 0.4999);
	int nValue = Round(((rand() / (float)RAND_MAX) * (nMax+1)) - 0.4999f);

	// sanity check
	if (nValue < 0)
		nValue = 0;
	if (nValue > nMax)
		nValue = nMax;

	// done
	return nValue;
}




//
// Round()
//
int Round(float fValue)
{
	if ( ((int)(fValue + 0.5f)) > ((int) fValue) )
		return ((int) fValue) + 1;
	else
		return ((int) fValue);
}



//
BOOL IsHonor(int nFaceValue)
{
	ASSERT(nFaceValue <= ACE);
	if (nFaceValue >= JACK)
		return TRUE;
	else
		return FALSE;
}




//
// StringToDeckValue()
//
// return deck value (0..51) matching given string
// returns -1 if error
//
int StringToDeckValue(LPCTSTR psz)
{
	int i,nSuit,nValue,nCardVal,nIndex=0;
	char c1,c2;

	// skip leading spaces
	while(psz[nIndex] == ' ')
		nIndex++;
	//
	c1 = psz[nIndex++];
	c2 = psz[nIndex];
	//
	if ((c1 == 'C') || (c1 == 'D') || (c1 == 'H') || (c1 == 'S')) 
	{
		// suit-number order
		for(i=0;i<4;i++)
			if ((c1 == cSuit[i]) || (toupper(c1) == cSuit[i]))
				break;
		nSuit = i;
		if (isdigit(c2)) 
		{
			nValue = c2 - '0';
		} 
		else 
		{
			for(i=0;i<5;i++)
				if ((c2 == cFaceCard[i]) || (toupper(c2) == cFaceCard[i]))
					break;
			if (i == 5)
				return -1;
			nValue = i + 10;
		}
	} 
	else 
	{
		if (isdigit(c1)) 
		{
			nValue = c1 - '0';
		} 
		else 
		{
			for(i=0;i<5;i++)
				if ((c1 == cFaceCard[i]) || (toupper(c1) == cFaceCard[i]))
					break;
			if (i == 5)
				return -1;
			nValue = i + 10;
		}
		// number-suit order
		for(i=0;i<4;i++)
			if ((c2 == cSuit[i]) || (toupper(c2) == cSuit[i]))
				break;
		if (i == 4)
			return -1;
		nSuit = i;
	}
	// sanity check
	if ((nValue < 2) || (nValue > 14) || 
					(nSuit < CLUBS) || (nSuit > SPADES))
		return -1;
	//
	nCardVal = nSuit*13 + nValue - 2;
	return nCardVal;
}


//
int StringToPosition(LPCTSTR psz)
{
	int i,nIndex=0;

	// skip leading spaces
	while(psz[nIndex] == ' ')
		nIndex++;
	//
	for(i=0;i<4;i++)
		if(strncmp(psz+nIndex,szPosition[i],4)==0)
			break;
	if (i < 4)
		return i;
	else 
		return -1;
}


//
TCHAR PositionToChar(int nPos)
{
	if ((nPos < 0) || (nPos > 3))
		return '?';
	else
		return szPosition[nPos][0];
}


//
LPCTSTR PositionToString(int nPos)
{
	if ((nPos < 0) || (nPos > 3))
		return "????";
	else
		return szPosition[nPos];
}


//
LPCTSTR PositionToShortString(int nPos)
{
	if ((nPos < 0) || (nPos > 3))
		return "????";
	else
		return szPositionShort[nPos];
}


//
int GetOpposingTeam(int nTeam)
{
	if (nTeam == NORTH_SOUTH)
		return EAST_WEST;
	else if (nTeam == EAST_WEST)
		return NORTH_SOUTH;
	else
		return NONE;
}


//
int GetPlayerTeam(int nPosition)
{
	if ((nPosition == NORTH) || (nPosition == SOUTH))
		return NORTH_SOUTH;
	else if ((nPosition == EAST) || (nPosition == WEST))
		return EAST_WEST;
	else
		return NEITHER;
}


//
LPCTSTR TeamToString(int nTeam)
{
	if (nTeam == NORTH_SOUTH)
		return tszTeam[0];
	else if (nTeam == EAST_WEST)
		return tszTeam[1];
	else if (nTeam == BOTH)
		return tszTeam[2];
	else if (nTeam == NEITHER)
		return tszTeam[3];
	else
		return _T("Error");
}


//
LPCTSTR TeamToShortString(int nTeam)
{
	if (nTeam == NORTH_SOUTH)
		return tszTeamBrief[0];
	else if (nTeam == EAST_WEST)
		return tszTeamBrief[1];
	else if (nTeam == BOTH)
		return tszTeamBrief[2];
	else  if (nTeam == BOTH)
		return tszTeamBrief[3];
	else
		return _T("???");
}

//
int CharToSuit(TCHAR c)
{
	int i;
	//
	for(i=0;i<5;i++)
		if ((c == cSuit[i]) || (toupper(c) == cSuit[i]))
			break;
	if (i < 5)
		return i;
	else 
		return -1;
}

//
int CharToPosition(TCHAR c)
{
	switch(c)
	{
		case 'S':
			return SOUTH;
		case 'W':
			return WEST;
		case 'N':
			return NORTH;
		case 'E':
			return EAST;
	}
	//
	return NONE;
}

//
int CharToFaceValue(TCHAR c)
{
	int nValue = 0;
	//
	char t = c;
	if (islower(c))
		char t = toupper(c);
	//
	if (isdigit(c))
	{
		nValue = c - '0';
	}
	else
	{
		switch (c)
		{
			case 'T':
				nValue = TEN;
				break;
			case 'J':
				nValue = JACK;
				break;
			case 'Q':
				nValue = QUEEN;
				break;
			case 'K':
				nValue = KING;
				break;
			case 'A':
				nValue = ACE;
				break;
		}
	}
	//
	return nValue;
}


//
TCHAR GetSuitLetter(int i)
{
	if ((i < CLUBS) || (i > SPADES))
		return ' ';
	else
		return cSuit[i];
}

//
TCHAR GetCardLetter(int i)
{
	if ((i < 2) || (i > ACE))
		return '?';
	else
		return cCard[i];
}

//
LPCTSTR GetSuitName(int nSuit)
{
	VERIFY(ISSUIT(nSuit));
	return szSuitName[nSuit];
}

//
LPCTSTR GetCardName(int nFaceValue)
{
	VERIFY(ISFACEVAL(nFaceValue));
	return szFaceValueName[nFaceValue];
}


//
LPCTSTR CardValToString(int i)
{
	if (!ISFACEVAL(i))
		return "???";
	else
		return szCardName[i];
}


//
// CardToString()
// 
// eg., Queen of Hearts
//
const CString CardToString(int i)
{
	CString strLabel;
	if (!ISCARD(i))
		strLabel = "???";
	else
		strLabel.Format("%s of %s",szCardName[FACEVAL(i)],szSuitName[CARDSUIT(i)]);
	return strLabel;
}



//
// CardToShortString()
//
// e.g., HQ
//
const CString CardToShortString(int i)
{
	CString strLabel;
	if (!ISCARD(i))
		strLabel = "??";
	else
		strLabel.Format("%c%c",cSuit[CARDSUIT(i)],cCard[FACEVAL(i)]);
	return strLabel;
}


//
// CardToReverseString()
//
// e.g., Heart Queen
//
const CString CardToReverseString(int i)
{
	CString strLabel;
	if (!ISCARD(i))
		strLabel = "???";
	else
		strLabel.Format("%s %s",SuitToSingularString(CARDSUIT(i)),szCardName[FACEVAL(i)]);
	return strLabel;
}

//
LPCTSTR SuitToString(int nSuit)
{
	if ((nSuit < CLUBS) || (nSuit > NOTRUMP))
		return "????";
	else
		return szSuitName[nSuit];
}


//
// same as above, but truncate ending 's'
//
LPCTSTR SuitToSingularString(int nSuit)
{
	if ((nSuit < CLUBS) || (nSuit > NOTRUMP)) 
		return "????";
	else 
		return szSuitNameSingular[nSuit];
}



//
// ContractStringToBid()
//
// return a numeric equivalent to a bid
// 0=pass, 1=1C, 2=1D, 3=1H, 4=1S, 5=1NT,
//         6=2C, 7=2D, 8=2H, 9=2S, 10=2NT, etc...
// returns -1 if error
//
int ContractStringToBid(LPCTSTR psz)
{
	// get a modifiable copy
	CString strBid = psz;
	// skip leading spaces
	strBid.TrimLeft();

	// see if this is a pass
	if ((strBid.Left(4).CompareNoCase("pass") == 0) ||
		(strBid.Left(1).CompareNoCase("-") == 0))
		return 0;

	// see if it's a double or redouble
	if ((strBid.Left(6).CompareNoCase("double") == 0) ||
		(strBid.Left(3).CompareNoCase("dbl") == 0) ||
		(strBid.Left(1).CompareNoCase("x") == 0))
		return BID_DOUBLE;
	if ((strBid.Left(8).CompareNoCase("redouble") == 0) ||
		(strBid.Left(5).CompareNoCase("redbl") == 0) ||
		(strBid.Left(2).CompareNoCase("xx") == 0))
		return BID_REDOUBLE;

	// else it's a normal bid
	ASSERT(strBid.GetLength() >= 2);
	char c1 = strBid[0];
	char c2 = strBid[1];

	//
	int nValue = c1 - '0';
	for(int i=0;i<4;i++)
		if ((c2 == cSuit[i]) || (toupper(c2) == cSuit[i]))
			break;
	int nSuit = i;

	// check limits
	if ((nValue < 1) || (nValue > 7) || 
					(nSuit < CLUBS) || (nSuit > NOTRUMP))
		return -1;
	int nBid = (nValue-1)*5 + nSuit + 1;
	//
	return nBid;
}



//
// ContractParamsToBid()
//
// return a numeric equivalent to a bid
// returns -1 if error
//
int ContractParamsToBid(int nSuit, int nValue)
{
	// check limits
	if ((nValue < 1) || (nValue > 7) || 
					(nSuit < CLUBS) || (nSuit > NOTRUMP))
		return -1;
	int nBid = (nValue-1)*5 + nSuit + 1;
	//
	return nBid;
}


//
// BidToShortString()
//
const CString BidToShortString(int nBid)
{
	CString strBid;
	if ((nBid < BID_PASS) || (nBid > BID_REDOUBLE))
		return "??";
	int nValue = ((nBid-1) / 5) + 1;
	int nSuit = (nBid-1) % 5;
	if (nBid == BID_DOUBLE) 
	{
		strBid = "Dbl";
	} 
	else if (nBid == BID_REDOUBLE) 
	{
		strBid = "Redbl";
	} 
	else if (nBid > BID_PASS) 
	{
		if (nSuit < 4)
			strBid.Format("%d%c",nValue,cSuit[nSuit]);
		else
			strBid.Format("%dNT",nValue);
	} 
	else 
	{
		strBid = "Pass";
	}
	return strBid;
}


//
// BidToBriefString(int nBid)
//
// provide extremely short bid descriptions
// used primarily for GIB interface
//
const CString BidToBriefString(int nBid)
{
	CString strBid;
	if ((nBid < BID_PASS) || (nBid > BID_REDOUBLE))
		return "??";
	int nValue = ((nBid-1) / 5) + 1;
	int nSuit = (nBid-1) % 5;
	if (nBid == BID_DOUBLE) 
	{
		strBid = "x";
	} 
	else if (nBid == BID_REDOUBLE) 
	{
		strBid = "xx";
	} 
	else if (nBid > BID_PASS) 
	{
		if (nSuit < 4)
			strBid.Format("%d%c",nValue,cSuit[nSuit]);
		else
			strBid.Format("%dn",nValue);
	} 
	else 
	{
		strBid = "p";
	}
	return strBid;
}


//
// BidToPBNString(int nBid)
//
// provide bid descriptions compatible w/ PBN
//
const CString BidToPBNString(int nBid)
{
	CString strBid;
	if ((nBid < BID_PASS) || (nBid > BID_REDOUBLE))
		return "??";
	int nValue = ((nBid-1) / 5) + 1;
	int nSuit = (nBid-1) % 5;
	if (nBid == BID_DOUBLE) 
	{
		strBid = "X";
	} 
	else if (nBid == BID_REDOUBLE) 
	{
		strBid = "XX";
	} 
	else if (nBid > BID_PASS) 
	{
		if (nSuit < 4)
			strBid.Format("%d%c", nValue, cSuit[nSuit]);
		else
			strBid.Format("%dNT", nValue);
	} 
	else 
	{
		strBid = "Pass";
	}
	return strBid;
}



//
// BidToFullString()
//
const CString BidToFullString(int nBid)
{
	CString strBid;
	if ((nBid < BID_PASS) || (nBid > BID_REDOUBLE))
		return "??";
	int nValue = ((nBid-1) / 5) + 1;
	int nSuit = (nBid-1) % 5;
	if (nBid == BID_DOUBLE) 
	{
		strBid = "Double";
	} 
	else if (nBid == BID_REDOUBLE) 
	{
		strBid = "Redouble";
	} 
	else if (nBid > BID_PASS) 
	{
		strBid.Format("%d %s",nValue,szSuitName[nSuit]);
	} 
	else 
	{
		strBid = "Pass";
	}
	// trim plural 's' from a 1-level suit bid
	// i.e., 1 Clubs => 1 Club
	if ((nValue == 1) && ISSUIT(nSuit) && (nSuit != NOTRUMP))
		strBid.ReleaseBuffer(strBid.GetLength()-1);
	//
	return strBid;
}


const CString ContractToString(int nContract, int nModifier)
{
	CString strBid;
	strBid.Format("%d%s", 
				  BID_LEVEL(nContract), 
				  szSuitNameShort[BID_SUIT(nContract)]);
	if (nModifier > 0)
		strBid += FormString(" %s", ((nModifier == 1)? "X" : "XX"));
	return strBid;
}


//
const CString ContractToFullString(int nContract, int nModifier)
{
	CString strBid;
	strBid.Format("%d %s%s", 
				  BID_LEVEL(nContract), 
				  ((BID_LEVEL(nContract) == 1) ? szSuitNameSingular[BID_SUIT(nContract)] : szSuitName[BID_SUIT(nContract)]),
				  ((nModifier == 1)? " (doubled)" :
				   (nModifier == 2)? " (redoubled)" : ""));
	return strBid;
}



//
// StringToBid()
//
int StringToBid(LPCTSTR szBid)
{
	CString strBid = szBid;
	strBid.TrimLeft();
	strBid.TrimRight();
	//
	if (strBid.CompareNoCase("Pass") == 0)
		return BID_PASS;
	else if (strBid.CompareNoCase("X") == 0)
		return BID_DOUBLE;
	else if (strBid.CompareNoCase("XX") == 0)
		return BID_REDOUBLE;

	// else parse the bid
	if ((strBid.GetLength() < 2) || (!isdigit(strBid[0])) || (!isalpha(strBid[1])))
		return BID_NONE;
	int nLevel = atoi(strBid.Left(1));
	int nSuit = CharToSuit(strBid[1]);
	if ((nLevel < 1) || (nLevel > 7) || (nSuit < 0) || (nSuit > 5))
		return BID_NONE;
	return MAKEBID(nSuit, nLevel);
}


//
int GetPrevPlayer(int nPos)
{
	switch(nPos) 
	{
		case SOUTH:
			return EAST;
		case WEST:
			return SOUTH;
		case NORTH:
			return WEST;
		case EAST:
			return NORTH;
	}
	// error!
	return NONE;
}


//
int GetNextPlayer(int nPos)
{
	switch((Position)nPos) 
	{
		case SOUTH:
			return WEST;
		case WEST:
			return NORTH;
		case NORTH:
			return EAST;
		case EAST:
			return SOUTH;
	}
	// error!
	return NONE;
}


//
int GetNextSuit(int nSuit)
{
	switch(nSuit) 
	{
		case CLUBS:
			return DIAMONDS;
		case DIAMONDS:
			return HEARTS;
		case HEARTS:
			return SPADES;
		case SPADES:
			return CLUBS;
	}
	// error!
	return NONE;
}



//
int GetPrevSuit(int nSuit)
{
	switch(nSuit) 
	{
		case CLUBS:
			return SPADES;
		case DIAMONDS:
			return CLUBS;
		case HEARTS:
			return DIAMONDS;
		case SPADES:
			return HEARTS;
	}
	// error!
	return NONE;
}


//
int GetPartner(int nPos)
{
	switch (nPos) 
	{
		case SOUTH:
			return NORTH;
		case WEST:
			return EAST;
		case NORTH:
			return SOUTH;
		case EAST:
			return WEST;
		default:
			AfxMessageBox("Illegal Call to GetPartner()!");
			return SOUTH;
	}
}




///////////////////////////////////////////////////////////////////////////
//
//
// Misc Utilities
//
//
///////////////////////////////////////////////////////////////////////////


//
// Pause()
//
// a wait routine that doesn't block the program the way ::Sleep does
// Note: is this supported on Win95???
//
void Pause(int nMilliseconds)
{
/*
	LARGE_INTEGER lnTime = nMilliseconds * 1000000;		// nanoseconds -> milliseconds
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	SetWaitableTimer(hTimer, &lnTime, 0, NULL, NULL, FALSE);
	const int tnMaxTimeout = 30 * 1000;			// 30 seconds max

	//
	DWORD dCode = WaitForSingleObject(hTimer, tnMaxTimeout);
	if (dCode == WAIT_TIMEOUT)
	{
		// oops!
	}
*/
}

