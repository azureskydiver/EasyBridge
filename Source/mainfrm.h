//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MAINFRAME__
#define __MAINFRAME__

#include "ObjectWithProperties.h"
#include "EventProcessor.h"
class CMyStatusBar;
//class CDockingTextWnd;
class CHistoryWnd;
class CStatusWnd;
class CMainFrameopts;
class CCardLayoutDialog;
class CFileCommentsDialog;
class CAnalysisDialog;
class CGameReviewDialog;
class CBidDialog;
class CDailyTipDialog;
class CAutoHintDialog;
class CNNetOutputDialog;
//class CScoreDialog;



class CMainFrame : public CCJFrameWnd, public CObjectWithProperties, public CEventProcessor
{
// public operations
public:
	// property get/set operations
	LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	LPCTSTR GetValueString(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, LPCTSTR szValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	// Event Processor
	bool Notify(long lCode, long param1=0, long param2=0, long param3=0) { return false; }
	//
	void Initialize();
	void Terminate();
	BOOL IsClosing() const { return m_bIsClosing; }
	//
	void SetStatusText(const char*, int nPane=0, BOOL bLockPane=TRUE);
	void SetStatusText(int nIndex, BOOL bLockPane=TRUE);
	void LockStatusBar(BOOL bLock = TRUE);
	void ClearStatusText(int nIndex=0, BOOL bClearToEmpty=FALSE);
	void SetModeIndicator(LPCTSTR szText=NULL);
	void ClearModeIndicator();
	void SetAllIndicators();
	void ClearAllIndicators();
	void DisplayTricks(BOOL bClear=FALSE);
	void DisplayContract(BOOL bClear=FALSE);
	void DisplayDeclarer(BOOL bClear=FALSE);
	void DisplayVulnerable(BOOL bClear=FALSE);
	void SetFeedbackText(LPCTSTR szText=NULL);
	void SetGIBMonitorText(LPCTSTR szText);
	void AppendGIBMonitorText(LPCTSTR szText);
	void SuspendHints();
	void ResumeHints();
	void ClearAutoHints();
	void UpdateStatusWindow(int nPage=-1, BOOL bShow=FALSE);
	void ClearStatusWindow();
	void HideAllDialogs();
	void RestoreAllDialogs();
	void MakeDialogVisible(int nWindow, int nIndex=0);
	void HideDialog(int nWindow, int nIndex=0);
	BOOL IsDialogVisible(int nWindow, int nIndex=0);
	void EnableDialog(int nWindow, BOOL bEnable, int nIndex);
	void InformChildFrameOpened(int nWindow, int nIndex=0);
	void InformChildFrameClosed(int nWindow, int nIndex=0);
	int SetFont(int nFont, LOGFONT& lf);
	void SetDefaultSwapCommand(int nOperation) { m_nDefaultCardSwapOperation = nOperation; }
	CWnd* GetDialog(int nWindow, int nIndex=0);
	CBidDialog* GetBidDialog() { return m_pBidDlg; }
	void DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf);
	void SetPlayHistory(LPCTSTR szText, BOOL bUseSuitSymbols=FALSE);
	void SetPlainPlayHistory(LPCTSTR szText) { m_strPlainPlayHistory = szText; }
	void SetBiddingHistory(LPCTSTR szText, BOOL bUseSuitSymbols=FALSE);
	void SetPlainBiddingHistory(LPCTSTR szText) { m_strPlainBiddingHistory = szText; }
	void SetAnalysisText(int nPosition, LPCTSTR szText);
	const CString GetPlayHistory() const { return m_strPlayHistory; }
	const CString GetPlainPlayHistory() const { return m_strPlainPlayHistory; }
	const CString GetBiddingHistory() const { return m_strBiddingHistory; }
	const CString GetPlainBiddingHistory() const { return m_strPlainBiddingHistory; }
	// static functions to set the status bar text
	static void SetStatusMessage(LPCTSTR szMessage, int nPane=0);
	static void ClearStatusMessage();
	static void ResetStatusMessage();

// data
public:
	//
	CFont		m_fixedFont;
	CFont		m_standardFont;
	CFont		m_smallFont;
	CFont		m_largeFont;
	CFont		m_tricksFont;
	CFont		m_analysisFont;
	CFont		m_historyFont;
	CFont		m_symbolFont;
	CFont		m_autoHintFont;
	CFont		m_dialogFont;
	CFont		m_dialogBoldFont;

private:
	//
	CMenu					m_popupMenu;
	WINDOWPLACEMENT			m_winPlacement;
	//
	CCardLayoutDialog* 		m_pLayoutDlg;
	CFileCommentsDialog* 	m_pFileCommentsDlg;
	CAnalysisDialog*		m_pDlgAnalysis[4];
	CGameReviewDialog*		m_pGameReviewDlg;
	CBidDialog*				m_pBidDlg;
	CDailyTipDialog*		m_pDailyTipDialog;
	CNNetOutputDialog*		m_pNNetOutputDialog;
//	CScoreDialog*			m_pScoreDialog;
	//
//	CDockingTextWnd*		m_pWndBiddingHistory;
//	CDockingTextWnd*		m_pWndPlayHistory;
	CHistoryWnd*			m_pWndHistory;
	CStatusWnd*				m_pWndStatus;
	CAutoHintDialog*		m_pAutoHintDialog;
	//
	CRect					m_rectAnalysisDialogs[4];
	POINT					m_pointHistoryDialog;
	POINT					m_pointStatusDialog;
	POINT					m_pointGameReviewDialog;
	POINT					m_pointNNetOutputDialog;
	CRect					m_rectStatusDialog;
	CRect					m_rectCommentsDialog;
	CRect					m_rectAutoHintDialog;
	int						m_nDockingWndWidth;
	int						m_nDockingWndHeight;

	//
	BOOL	m_bStatusDialogActive;
	BOOL	m_bCommentsDialogActive;
	BOOL	m_bHistoryDialogActive;
	BOOL	m_bBHDialogActive;
	BOOL	m_bPHDialogActive;
	BOOL	m_bBidDialogActive;
	BOOL	m_bScoreDialogActive;
	BOOL	m_bAnalysisDialogActive[4];	
	//
	BOOL	m_bStatusDialogActiveSave;
	BOOL	m_bHistoryDialogActiveSave;
	BOOL	m_bAnalysisDialogActiveSave[4];
	//
	CString	m_strFeedback;
	CString	m_strGIBMonitor;
	CString	m_strBiddingHistory;
	CString	m_strPlainBiddingHistory;
	CString	m_strPlayHistory;
	CString	m_strPlainPlayHistory;
	//
	LOGFONT		m_lfAnalysis;
	LOGFONT		m_lfHistory;
	LOGFONT		m_lfSymbol;
	LOGFONT		m_lfAutoHint;
	// sys caps
	int		m_nSysScreenWidth;
	int		m_nSysScreenHeight;
	int		m_nSysNumColors;
	int		m_nSysColorPlanes;
	int		m_nSysBitsPerPixel;
	BOOL	m_bSysRCDIBitmap;
	BOOL	m_bSysRCDIBtoDev;
	int		m_nSysClipCaps;
	int		m_nSysRasterCaps;
	//
	BOOL	m_bHighResDisplay;
	int		m_nDefaultCardSwapOperation;
	int		m_nStatusBarLock;
	BOOL	m_bIsClosing;

// Operations
private:
	void MakeStatusDialogVisible();
	void ShowCommentsDialog(BOOL bShow=TRUE);
	void SetNonBoldDialogFont(CDialog* pDialog);
	void OnUpdatePane(CCmdUI* pCmdUI);
 
// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnBarCheck(UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMyStatusBar*		m_pWndStatusBar;
	CCJToolBar			m_wndToolBar;
	CCJToolBar			m_wndSecondaryToolBar;
	CCJToolBar			m_wndTestToolBar;
//	CCoolMenuManager	m_menuManager;

// Generated message map functions
public:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnDisplayOptions();
	afx_msg void OnDestroy();
	afx_msg void OnHelpIntroduction();
	afx_msg void OnHelpMenus();
	afx_msg void OnHelpRules();
	afx_msg void OnHelpGlossary();
	afx_msg void OnHelpSearch();
	afx_msg void OnHelpMisc();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateDealOptions(CCmdUI* pCmdUI);
	afx_msg void OnDealOptions();
	afx_msg void OnShowAnalysisEast();
	afx_msg void OnShowAnalysisNorth();
	afx_msg void OnShowAnalysisSouth();
	afx_msg void OnShowAnalysisWest();
	afx_msg void OnShowAnalysisAll();
	afx_msg void OnUpdateShowAnalysis(CCmdUI* pCmdUI);
	afx_msg void OnGameOptions();
	afx_msg void OnUpdateViewFileComments(CCmdUI* pCmdUI);
	afx_msg void OnViewFileComments();
	afx_msg void OnUpdateBiddingOptions(CCmdUI* pCmdUI);
	afx_msg void OnBiddingOptions();
	afx_msg void OnUpdateShowStatus(CCmdUI* pCmdUI);
	afx_msg void OnShowStatus();
	afx_msg void OnUpdateShowAnalysisEast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowAnalysisNorth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowAnalysisSouth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowAnalysisWest(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelpContents();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	afx_msg void OnHelpQuickStart();
	afx_msg void OnHelpFaq();
	afx_msg void OnToggleAnalysisTracing();
	afx_msg void OnUpdateToggleAnalysisTracing(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnSwapCards();
	afx_msg void OnHelpTipOfTheDay();
	afx_msg void OnHelpReadme();
	afx_msg void OnUpdateViewNeuralNet(CCmdUI* pCmdUI);
	afx_msg void OnViewNeuralNet();
	afx_msg void OnUpdateExposeAllCards(CCmdUI* pCmdUI);
	afx_msg void OnExposeAllCards();
	afx_msg void OnUpdateGameAutoHint(CCmdUI* pCmdUI);
	afx_msg void OnGameAutoHint();
	afx_msg void OnUpdateTrainingMode(CCmdUI* pCmdUI);
	afx_msg void OnTrainingMode();
	afx_msg void OnUpdatePlayModeNormal(CCmdUI* pCmdUI);
	afx_msg void OnPlayModeNormal();
	afx_msg void OnPlayModeManual();
	afx_msg void OnUpdatePlayModeManual(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePlayModeManualDefend(CCmdUI* pCmdUI);
	afx_msg void OnPlayModeManualDefend();
	afx_msg void OnUpdatePlayModeFullAuto(CCmdUI* pCmdUI);
	afx_msg void OnPlayModeFullAuto();
	afx_msg void OnUpdatePlayModeLock(CCmdUI* pCmdUI);
	afx_msg void OnPlayModeLock();
	afx_msg void OnUpdateProgConfigWizard(CCmdUI* pCmdUI);
	afx_msg void OnProgConfigWizard();
	afx_msg void OnUpdateShowCommentIdentifiers(CCmdUI* pCmdUI);
	afx_msg void OnShowCommentIdentifiers();
	afx_msg void OnUpdateManualBidding(CCmdUI* pCmdUI);
	afx_msg void OnManualBidding();
	afx_msg void OnUpdateManualPlay(CCmdUI* pCmdUI);
	afx_msg void OnManualPlay();
	//}}AFX_MSG
	afx_msg void OnToolbarDropDown(NMTOOLBAR* pnmh, LRESULT* plRes);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
