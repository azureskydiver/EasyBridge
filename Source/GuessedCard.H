//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GuessedCard.h
//

#ifndef __GUESSEDCARD__
#define __GUESSEDCARD__

class CCard;


class CGuessedCard {

// public functions
public:
	void Init(int nSuit, int nFaceValue);
	void Clear();
	// inline functions
	CCard* GetCard() { return m_pCard; }
	int GetSuit() { return m_nSuit; }
	char GetCardLetter() { return ::GetCardLetter(m_nFaceValue); }
	char GetSuitLetter() { return ::GetSuitLetter(m_nSuit); }
	LPCTSTR GetName() { return (LPCTSTR) m_szName; }
	LPCTSTR GetFullName() { return CardToString(MAKEDECKVALUE(m_nSuit, m_nFaceValue)); }
	LPCTSTR GetReverseFullName() { return CardToReverseString(MAKEDECKVALUE(m_nSuit, m_nFaceValue)); }
	LPCTSTR GetFaceName() { return GetCardName(m_nFaceValue); }
	int GetFaceValue() { return m_nFaceValue; }
	int GetDeckValue() { return MAKEDECKVALUE(m_nSuit, m_nFaceValue); }
	// basic operations
	void SetProbableLocation(int nPos) { m_nLocation = nPos; }
	int GetProbableLocation() { return m_nLocation; }
	void SetDefiniteLocation(int nPos) { m_nLocation = nPos; m_fFixProbability = 1.0f; }
	int GetDefiniteLocation() { return (m_fFixProbability == 1.0f)? m_nLocation : UNKNOWN; }
	BOOL LocationIdentified() { return (m_fFixProbability == 1.0)? TRUE: FALSE; }
	//
	void MarkAsPlayed(int nHand)  { m_bOutstanding = FALSE; m_nLocation = nHand; m_fFixProbability = 1.0f;}
	void SetPlayed(BOOL bPlayed=TRUE) { m_bOutstanding = !bPlayed; }
	void SetOutstanding(BOOL bOutstanding=TRUE) { m_bOutstanding = bOutstanding; }
	BOOL IsOutstanding() { return m_bOutstanding; }
	BOOL WasPlayed() { return !m_bOutstanding;}
	//
	void ClearFixProbability() { m_fFixProbability = 0.0f; }
	void SetFixProbability(float fProb) { if (fProb < 0) return; m_fFixProbability = fProb; if (m_fFixProbability > 1.0f) m_fFixProbability = 1.0f; }
	float ScaleFixProbability(float fAdj);
	float AdjustFixProbability(float fAdj);
	float AdjustFixProbabilityIncremental(float fAdj);
	float ClampFixProbability() { if (m_fFixProbability >= 1.0f) m_fFixProbability = 0.9999f; else if (m_fFixProbability < 0.0f) m_fFixProbability = 0.0f; return m_fFixProbability; }
	float GetFixProbability() { return m_fFixProbability; }

// data
private:
	TCHAR	m_szName[3];
	int 	m_nSuit,m_nFaceValue;
	BOOL	m_bOutstanding;
	int		m_nLocation;
	float	m_fFixProbability;
	CCard*  m_pCard;

// construction / destruction
public:
	CGuessedCard(int nSuit, int nFaceValue, BOOL bOutstanding=TRUE, int nLocation=UNKNOWN, float fFixProbability=0.0);
	CGuessedCard(CCard* pCard, BOOL bOutstanding=TRUE, int nLocation=UNKNOWN, float fFixProbability=0.0);
	CGuessedCard();
	~CGuessedCard();
};

#endif