//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// MainFrameopts.h
//
// constants used in generic document property get/set calls
//

const int NUM_DIALOGS = 6;

//
enum mainframeopts {
	// dialog window codes
	twLayoutDialog,
	twStatusDialog,
	twFileCommentsDialog,
	twHistoryDialog,
	twBiddingHistoryDialog,
	twPlayHistoryDialog,
	twBidDialog,
	twScoreDialog,
	twAnalysisDialog,
	twAutoHintDialog,
	twGameReviewDialog,
	twNNetOutputDialog,
	// property get/set codes
	tnAnalysisDialogRectLeft,
	tnAnalysisDialogRectTop,
	tnAnalysisDialogRectRight,
	tnAnalysisDialogRectBottom,
	tnHistoryDialogWidth,
	tnHistoryDialogHeight,
	tnStatusDialogWidth,
	tnStatusDialogHeight,
	tnReviewDialogPosLeft,
	tnReviewDialogPosTop,
	tnNNetOutputDialogLeft,
	tnNNetOutputDialogTop,
	tnStatusDialogRectLeft,
	tnStatusDialogRectTop,
	tnStatusDialogRectRight,
	tnStatusDialogRectBottom,
	tnCommentsDialogRectLeft,
	tnCommentsDialogRectTop,
	tnCommentsDialogRectRight,
	tnCommentsDialogRectBottom,
	tnAutoHintDialogRectLeft,
	tnAutoHintDialogRectTop,
	tnAutoHintDialogRectRight,
	tnAutoHintDialogRectBottom,
	tbAnalysisDialogActive,
	tnDockingWndWidth,
	tnDockingWndHeight,
	//
	tszFeedbackText,
	tszGIBMonitorText,
	//
	tpFontFixed,
	tpFontStandard,
	tpFontSmall,
	tpFontLarge,
	tpFontAnalysis,
	tpFontHistory,
	tpFontAutoHint,
	tpFontSymbol,
	tpFontDialog,
	tpFontDialogBold,
	//
	tpLogFontAnalysis,
	tpLogFontHistory,
	tpLogFontAutoHint,
	// sys caps
	tnSysScreenWidth,
	tnSysScreenHeight,
	tnSysNumColors,
	tnSysColorPlanes,
	tnSysBitsPerPixel,
	tbSysRCDIBitmap,
	tbSysRCDIBtoDev,
	tnSysClipCaps,
	tnSysRasterCaps,
	//
	tbHighResDisplay,
	tnDefaultWindowWidth,
	tnDefaultWindowHeight,
};

