//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Globals.h
//
// useful global functions
//
#ifndef __GLOBALS__
#define __GLOBALS__

//extern const LPCTSTR szSuitName[];

// utility templates
template<class T> const T& Min(const T& x, const T& y)
{
	return ((x < y)? x : y);
}
//
template<class T> const T& Max(const T& x, const T& y)
{
	return ((x > y)? x : y);
}
//
template<class T> const T Abs(const T x)
{
	return ((x >= 0)? x : -x);
}


//
// string manipulation operations
//
const CString FormString(LPCTSTR szFormat, ...);
const CString FormString(int nVal);
const CString FormString(double fVal);
//
CString WrapInQuotes(const CString& strItem);
CString StripQuotes(const CString& strItem);


//
// misc utilities
//
const CString	BidToShortString(int nBid);
const CString	BidToBriefString(int nBid);
const CString	BidToPBNString(int nBid);
const CString	BidToFullString(int nBid);
const CString	ContractToString(int nContract, int nModifier=0);
const CString	ContractToFullString(int nContract, int nModifier=0);
int				StringToBid(LPCTSTR szBid);
int				StringToDeckValue(LPCTSTR psz);
int				StringToPosition(LPCTSTR psz);
TCHAR			PositionToChar(int nPos);
LPCTSTR			PositionToString(int nPos);
LPCTSTR			PositionToShortString(int nPos);
int				GetOpposingTeam(int nTeam);
int				GetPlayerTeam(int nPosition);
LPCTSTR			TeamToString(int nTeam);
LPCTSTR			TeamToShortString(int nTeam);
int				CharToSuit(TCHAR c);
int				CharToFaceValue(TCHAR c);
int				CharToPosition(TCHAR c);
TCHAR			GetSuitLetter(int i);
LPCTSTR			GetSuitName(int nSuit);
TCHAR			GetCardLetter(int i);
LPCTSTR			GetCardName(int nFaceValue);
LPCTSTR			CardValToString(int i);
const CString	CardToString(int i);
const CString	CardToShortString(int i);
const CString	CardToReverseString(int i);
LPCTSTR			SuitToString(int nSuit);
LPCTSTR			SuitToSingularString(int nSuit);
int				ContractStringToBid(LPCTSTR psz);
int				ContractParamsToBid(int nSuit, int nValue);
int				GetPrevPlayer(int nPos);
int				GetNextPlayer(int nPos);
int				GetPrevPlayer(int nPos);
int				GetNextPlayer(int nPos);
int				GetNextSuit(int nSuit);
int				GetPrevSuit(int nSuit);
int				GetPartner(int nPos);
//
BOOL	IsHonor(int nFaceValue);
//
int		GetRandomValue(int nMax);
int		Round(float fValue);

//
void	Pause(int nMilliseconds);

#endif
