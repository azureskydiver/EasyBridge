//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// GameReviewDialog.h : header file
//

#include "MyCustomDialog.h"
#include "FlatButton.h"
//#include "CJFlatComboBox.h"
class CGameRecord;


/////////////////////////////////////////////////////////////////////////////
// CGameReviewDialog dialog

class CGameReviewDialog : public CMyCustomDialog
{
// Construction
public:
	CGameReviewDialog(CWnd* pParent = NULL);   // standard constructor
	//
	virtual void Initialize(BOOL bRefresh=TRUE);
	virtual void Reset(BOOL bRefresh=TRUE);
	//
	virtual void UpdateToolTipText(CPoint point);

//
private:
	void	SetGameIndex(int nGame, BOOL bRefresh=TRUE);
	void	SetPlayRound(int nRound);
	void	ShowTrick(int nTrick);
	void	CollapseWindow(BOOL bCollapse);

//
private:
	int		m_numGamesAvailable;
	int		m_numPlaysAvailable;
	int		m_numTricksAvailable;
	int		m_nBidIndex;
	int		m_nPlayIndex;
	int		m_nPlayRound;
	int		m_nPlayerPosition;
	//
	BOOL	m_bCollapsed;
	int		m_nFullWidth;
	int		m_nCollapsedWidth;
	HICON	m_hIconExpand, m_hIconCollapse;
	CFlatButton	m_flatButtons[14];

public:
// Dialog Data
	//{{AFX_DATA(CGameReviewDialog)
	enum { IDD = IDD_GAME_REVIEW };
	CListCtrl	m_listTags;
	CCJFlatComboBox	m_listGameIndex;
	int		m_nGameIndex;
	CString	m_strGameInfo;
	CString	m_strNote;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameReviewDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGameReviewDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnNext();
	afx_msg void OnPrev();
	afx_msg void OnLast();
	afx_msg void OnFirst();
	afx_msg void OnNextGame();
	afx_msg void OnPrevGame();
	afx_msg void OnLastGame();
	afx_msg void OnFirstGame();
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSelchangeGameIndex();
	afx_msg void OnExpandCollapse();
	afx_msg void OnPlay();
	afx_msg void OnSaveOut();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
