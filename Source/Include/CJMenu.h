//*************************************************************************
// CJMenu.h : header file
// Version : 2.5
// Date : October 18, 1999
// Author : Brent Corkum
// 
// Portions of code supplied by:
// Ben Ashley,Girish Bharadwaj,Jean-Edouard Lachand-Robert,
// Robert Edward Caldecott,Kenny Goers,Leonardo Zide,
// Stefan Kuhr, Reiner Jung
//
// Bug Fixes:
// Stefan Kuhr,Martin Vladic,Kim Yoo Chul
//
// You are free to use/modify this code but leave this header intact.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/24/99 2:09a $
 * $Revision: 6 $
 * $Archive: /CodeJock/Include/CJMenu.h $
 *
 * $History: CJMenu.h $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 2:09a
 * Updated in $/CodeJock/Include
 * Upgraded to version 2.5 of BCMenu
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:41p
 * Updated in $/CodeJock/Include
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJMENU_H__
#define __CJMENU_H__

#include <afxtempl.h>

// CCJMenuData class. Fill this class structure to define a single menu item:

class _CJX_EXT_CLASS CCJMenuData
{
	wchar_t *m_szMenuText;
public:
	CCJMenuData () {menuIconNormal=-1;xoffset=-1;bitmap=NULL;
	nFlags=0;nID=0;syncflag=0;m_szMenuText=NULL;};
	void SetAnsiString(LPCSTR szAnsiString);
	void SetWideString(const wchar_t *szWideString);
	const wchar_t *GetWideString(void) {return m_szMenuText;};
	~CCJMenuData ();
	CString GetString(void);//returns the menu text in ANSI or UNICODE
	int xoffset;
	int menuIconNormal;
	UINT nFlags,nID,syncflag;
	CImageList *bitmap;
};

//struct CMenuItemInfo : public MENUITEMINFO {
struct _CJX_EXT_CLASS CMenuItemInfo : public 
//MENUITEMINFO 
#ifndef UNICODE   //SK: this fixes warning C4097: typedef-name 'MENUITEMINFO' used as synonym for class-name 'tagMENUITEMINFOA'
tagMENUITEMINFOA
#else
tagMENUITEMINFOW
#endif
    {
	CMenuItemInfo()
	{ memset(this, 0, sizeof(MENUITEMINFO));
	  cbSize = sizeof(MENUITEMINFO);
	}
};

typedef enum {Normal,TextOnly} HIGHLIGHTSTYLE;

#ifndef UNICODE
#define AppendMenu AppendMenuA
#define InsertMenu InsertMenuA
#define InsertODMenu InsertODMenuA
#define AppendODMenu AppendODMenuA
#define AppendODPopupMenu AppendODPopupMenuA
#define ModifyODMenu ModifyODMenuA
#else
#define AppendMenu AppendMenuW
#define InsertMenu InsertMenuW
#define InsertODMenu InsertODMenuW
#define AppendODMenu AppendODMenuW
#define ModifyODMenu ModifyODMenuW
#define AppendODPopupMenu AppendODPopupMenuW
#endif

class _CJX_EXT_CLASS CCJMenu : public CMenu  // Derived from CMenu
{
	// Construction
public:
	CCJMenu(); 
	
	// Attributes
protected:
	CTypedPtrArray<CPtrArray, CCJMenuData*> m_MenuList;  // Stores list of menu items 
	
	// When loading an owner-drawn menu using a Resource, CCJMenu must keep track of
	// the popup menu's that it creates. Warning, this list *MUST* be destroyed
	// last item first :)
	CTypedPtrArray<CPtrArray, HMENU>  m_SubMenus;  // Stores list of sub-menus 
	
	// Stores a list of all CCJMenu's ever created 
	static CTypedPtrArray<CPtrArray, HMENU>  m_AllSubMenus;
	
	// Operations
public: 
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomMenu)
	//}}AFX_VIRTUAL 
	// Implementation
public:
	static BOOL IsNewShell(void);
	void SetBitmapBackground(COLORREF color);
	void SetDisableOldStyle(void);
	void UnSetDisableOldStyle(void);
	BOOL GetDisableOldStyle(void);
	void UnSetBitmapBackground(void);
	BOOL AddBitmapToImageList(CImageList *list,UINT nResourceID);
	BOOL LoadFromToolBar(UINT nID,UINT nToolBar,int& xoffset);
	void InsertSpaces(void);
	static LRESULT FindKeyboardShortcut(UINT nChar,UINT nFlags,CMenu *pMenu);
	static void UpdateMenu(CMenu *pmenu);
	static BOOL IsMenu(CMenu *submenu);
	static BOOL IsMenu(HMENU submenu);
	void DrawCheckMark(CDC *pDC,int x,int y,COLORREF color);
	void DrawRadioDot(CDC *pDC,int x,int y,COLORREF color);
	CCJMenu *FindMenuOption(int nId,int& nLoc);
	CCJMenuData *FindMenuOption(wchar_t *lpstrText);
	BOOL GetMenuText(UINT id,CString &string,UINT nFlags = MF_BYPOSITION );
	CImageList *checkmaps;
	BOOL checkmapsshare;
	int m_selectcheck;
	int m_unselectcheck;
	void LoadCheckmarkBitmap(int unselect,int select);
	void DitherBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, 
		int nHeight, HBITMAP hbm, int nXSrc, int nYSrc);
	void DitherBlt2(CDC *drawdc, int nXDest, int nYDest, int nWidth, 
		int nHeight, CBitmap &bmp, int nXSrc, int nYSrc);
	static HBITMAP LoadSysColorBitmap(int nResourceId);
	BOOL GetBitmapFromImageList(CDC* pDC,CImageList *imglist,int nIndex,CBitmap &bmp);
	
	virtual ~CCJMenu();  // Virtual Destructor 
	// Drawing: 
	virtual void DrawItem( LPDRAWITEMSTRUCT);  // Draw an item
	virtual void MeasureItem( LPMEASUREITEMSTRUCT );  // Measure an item
	
	// Customizing:
	
	void SetIconSize (int, int);  // Set icon size
	
	BOOL AppendODMenuA(LPCSTR lpstrText, 
		UINT nFlags = MF_OWNERDRAW,
		UINT nID = 0,
		int nIconNormal = -1);  
	
	BOOL AppendODMenuW(wchar_t *lpstrText, 
		UINT nFlags = MF_OWNERDRAW,
		UINT nID = 0,
		int nIconNormal = -1);  
	
	BOOL AppendODMenuA(LPCSTR lpstrText, 
		UINT nFlags,
		UINT nID,
		CImageList *il,
		int xoffset);
	
	BOOL AppendODMenuW(wchar_t *lpstrText, 
		UINT nFlags,
		UINT nID,
		CImageList *il,
		int xoffset);
	
	BOOL AppendMenu( UINT nFlags, UINT nIDNewItem = 0, LPCTSTR lpszNewItem = NULL )
		{ return CMenu::AppendMenu( nFlags, nIDNewItem, lpszNewItem ); }

	BOOL AppendMenu( UINT nFlags, UINT nIDNewItem, const CBitmap* pBmp )
		{ return CMenu::AppendMenu( nFlags, nIDNewItem, pBmp ); }

	BOOL InsertODMenuA(UINT nPosition,
		LPCSTR lpstrText, 
		UINT nFlags = MF_OWNERDRAW,
		UINT nID = 0,
		int nIconNormal = -1); 
	
	BOOL InsertODMenuW(UINT nPosition,
		wchar_t *lpstrText, 
		UINT nFlags = MF_OWNERDRAW,
		UINT nID = 0,
		int nIconNormal = -1);  
	
	BOOL InsertODMenuA(UINT nPosition,
		LPCSTR lpstrText, 
		UINT nFlags,
		UINT nID,
		CImageList *il,
		int xoffset);
	
	BOOL InsertODMenuW(UINT nPosition,
		wchar_t *lpstrText, 
		UINT nFlags,
		UINT nID,
		CImageList *il,
		int xoffset);
	
	BOOL ModifyODMenuA(const char *lpstrText,UINT nID=0,int nIconNormal=-1);
	BOOL ModifyODMenuA(const char *lpstrText,UINT nID,CImageList *il,int xoffset);
	BOOL ModifyODMenuA(const char *lpstrText,UINT nID,CBitmap *bmp);
	BOOL ModifyODMenuA(const char *lpstrText,const char *OptionText,int nIconNormal);
	BOOL ModifyODMenuW(wchar_t *lpstrText,UINT nID=0,int nIconNormal=-1);
	BOOL ModifyODMenuW(wchar_t *lpstrText,UINT nID,CImageList *il,int xoffset);
	BOOL ModifyODMenuW(wchar_t *lpstrText,UINT nID,CBitmap *bmp);
	BOOL ModifyODMenuW(wchar_t *lpstrText,wchar_t *OptionText,int nIconNormal);
	CCJMenuData *NewODMenu(UINT pos,UINT nFlags,UINT nID,CString string);
	void SynchronizeMenu(void);
	void CCJMenu::InitializeMenuList(int value);
	void CCJMenu::DeleteMenuList(void);
	CCJMenuData *CCJMenu::FindMenuList(UINT nID);
	virtual BOOL LoadMenu(LPCTSTR lpszResourceName);  // Load a menu
	virtual BOOL LoadMenu(int nResource);  // ... 
	void AddFromToolBar(CToolBar* pToolBar, int nResourceID);
	BOOL Draw3DCheckmark(CDC *dc, const CRect& rc,BOOL bSelected,
		HBITMAP hbmCheck);
	BOOL LoadToolbar(UINT nToolBar);
	BOOL LoadToolbars(const UINT *arID,int n);
	
	// new overrides for dynamic menu's
	BOOL	RemoveMenu(UINT uiId,UINT nFlags);
	BOOL	DeleteMenu(UINT uiId,UINT nFlags);
	BOOL  AppendMenuA(UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,int nIconNormal=-1);
	BOOL  AppendMenuA(UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset);
	BOOL  AppendMenuA(UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CBitmap *bmp);
	BOOL  AppendMenuW(UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,int nIconNormal=-1);
	BOOL  AppendMenuW(UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset);
	BOOL  AppendMenuW(UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp);
	BOOL  InsertMenuA(UINT nPosition,UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,int nIconNormal=-1);
	BOOL  InsertMenuA(UINT nPosition,UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset);
	BOOL  InsertMenuA(UINT nPosition,UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CBitmap *bmp);
	BOOL  InsertMenuW(UINT nPosition,UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,int nIconNormal=-1);
	BOOL  InsertMenuW(UINT nPosition,UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset);
	BOOL  InsertMenuW(UINT nPosition,UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp);
	CCJMenu* AppendODPopupMenuA(LPCSTR lpstrText);
	CCJMenu* AppendODPopupMenuW(wchar_t *lpstrText);
	
	// Destoying:
	
	virtual BOOL DestroyMenu();
	
	// Generated message map functions
protected:
	int m_iconX,m_iconY;
	COLORREF m_bitmapBackground;
	BOOL m_bitmapBackgroundFlag;
	BOOL disable_old_style;
}; 

#endif

//*************************************************************************
