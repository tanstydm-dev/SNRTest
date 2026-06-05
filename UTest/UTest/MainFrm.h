
// MainFrm.h: CMainFrame 类的接口
//

#pragma once
#include "CSatCNRDlg.h"
#include "CSatDistImg.h"
#include "CHeadernStruct.h"
#include "CErrCal.h"
#include "CPtlMsg.h"
#include "CPtlTab.h"
#include "CMapDlg.h"
class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() noexcept;

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  
	// 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCStatusBar     m_wndStatusBar;
	CFont m_fontStatusBar;
	/*窗体成员*/
	CSatCNRDlg		  m_dlgSatCNR;
	CSatDistImg		  m_dlgSatDist;
	CErrCal			  m_dlgErrCal;
	CDataProc	      m_dlgDataProc;
	CPtlMsg			  m_dlgPtlTree;
	CPtlTab			  m_dlgPtlTab;
	CMapDlg			  m_dlgDgtMap;
	BOOL			  m_bDlgPtlTreeDone;
	BOOL			  m_bDlgPtlTabDone;
	BOOL			  m_bDlgDgtMapDone;
	BOOL			  m_bDlgDataProcDone;
	BOOL			  m_bDlgErrCalDone;
	BOOL			  m_bDlgSatDistDone;
	BOOL			  m_bDlgSatCNRDone;
	BOOL			  m_bBarMenu;
	BOOL			  m_bBarStatus;
	BOOL			  m_bDragFullWindow;
	/*状态栏*/
	S32 m_IndexLinkStatus;	
	S32 m_IndexWorkMode;
	S32 m_IndexFilePath;
	S32 m_IndexRestLine;
	S32	m_IndexComOpenTime;
	S32 m_nWidth;
	S32 m_nHeight;
	S32					m_s32LinkedTime;		/*串口连接时间*/
	S32					m_s32LinkedPort;		/*已连接的串口*/
	CString				m_pstrPortId[20];		/*串口号*/
	U32					m_uSelectedBoteRate;	/*目前配置的波特率*/

protected:
	void InitMenuBar();
	void CreateChildDlgs();
	S32 InitStatusBar();
	void CreateDynamicLayout();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnUpdateStatusBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStartUpTimer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnSelectBoteRate(UINT nID);
	afx_msg void OnConfigureCom(UINT nID);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
};


