
// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "UTest.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_BOTRATE_9600, ID_BOTRATE_115200, &CMainFrame::OnSelectBoteRate)
	ON_COMMAND_RANGE(ID_COM_CFG, ID_COM_CFG, &CMainFrame::OnConfigureCom)
	ON_MESSAGE(WM_MYMESSAGE_UPDATESTATUSBAR, &CMainFrame::OnUpdateStatusBar)
	ON_MESSAGE(WM_MYMESSAGE_STARTUPTIMER, &CMainFrame::OnStartUpTimer)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_TIPS,
	ID_INDICATOR_COMOPENTIME,
	ID_INDICATOR_RESTLINE,
	ID_INDICATOR_WORKMODE,
	ID_INDICATOR_LINKSTATUS,
	ID_INDICATOR_FILEPATH
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
	m_bDlgPtlTabDone = FALSE;
	m_bDlgPtlTreeDone = FALSE;
	m_bDlgDgtMapDone = FALSE;
	m_bDlgSatDistDone = FALSE;
	m_bDlgErrCalDone = FALSE;
	m_bDlgDataProcDone = FALSE;
	m_bDlgSatCNRDone = FALSE;
	m_bBarMenu = FALSE;
	m_bBarStatus = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	//初始化变量

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_SCROLLED; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = FALSE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabCloseButton = FALSE;	// 设置选项卡没有关闭按钮
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = FALSE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = FALSE; // 在选项卡区域的右边缘启用文档菜单
	mdiTabParams.m_nTabBorderSize = 5;	// 设置选项卡边框大小
	mdiTabParams.m_bFlatFrame = TRUE;	// 设置选项卡具有平面样式

	EnableMDITabbedGroups(TRUE, mdiTabParams);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	m_s32LinkedPort = 0;
	m_s32LinkedTime = 0;

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);//显示状态栏
	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);

	/*使不记住窗口停靠状态*/
	EnableLoadDockState(FALSE);

	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	//EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	// 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
	// 将改进任务栏的可用性，因为显示的文档名带有缩略图。

	//菜单栏
	InitMenuBar();
	//状态栏
	InitStatusBar();
	//子对话框
	CreateChildDlgs();

	ModifyStyle(0, FWS_PREFIXTITLE);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	 // 调整主窗口的大小
	// 主窗口已初始化，因此显示它并对其进行更新
	CRect rectWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, rectWorkArea, 0);
	cs.cx = rectWorkArea.Width();
	cs.cy = rectWorkArea.Height();

	return TRUE;
}

void CMainFrame::CreateDynamicLayout()
{
	/*启用动态布局*/
	this->EnableDynamicLayout();
	CMFCDynamicLayout::MoveSettings NoMove;
	CMFCDynamicLayout::MoveSettings Move_X_100;
	CMFCDynamicLayout::MoveSettings Move_Y_100;
	CMFCDynamicLayout::MoveSettings Move_Both;

	CMFCDynamicLayout::SizeSettings NoSize;
	CMFCDynamicLayout::SizeSettings Size_X_100;
	CMFCDynamicLayout::SizeSettings Size_Y_100;
	CMFCDynamicLayout::SizeSettings Size_Both;

	Move_X_100.m_nXRatio = 100;
	Move_Y_100.m_nYRatio = 100;
	Move_Both.m_nXRatio = 100;
	Move_Both.m_nYRatio = 100;

	Size_X_100.m_nXRatio = 100;
	Size_Y_100.m_nYRatio = 100;
	Size_Both.m_nXRatio = 100;
	Size_Both.m_nYRatio = 100;
	// 获取Dialog的动态布局指针。
	CMFCDynamicLayout* pMfcDynamicLayout = this->GetDynamicLayout();
	// 创建动态布局。
	pMfcDynamicLayout->Create(this);
	// 添加要在动态布局中注册的控件。
	pMfcDynamicLayout->AddItem(IDD_DIALOG_CNR, NoMove, Size_Both);
	pMfcDynamicLayout->AddItem(IDD_DIALOG_DATARCV, NoMove, Size_Both);
	pMfcDynamicLayout->AddItem(IDD_DIALOG_ERRCAL, NoMove, Size_Both);
	pMfcDynamicLayout->AddItem(IDD_DIALOG_SATDIST, NoMove, Size_Both);
	pMfcDynamicLayout->AddItem(IDD_DIALOG_PTLTAB, NoMove, Size_Both);
	pMfcDynamicLayout->AddItem(IDD_DIALOG_PTLSHOW, NoMove, Size_Both);
	pMfcDynamicLayout->AddItem(IDD_DIALOG_MAP, NoMove, Size_Both);
}

/*初始化菜单栏*/
void CMainFrame::InitMenuBar()
{
	CMenu* menuMain; //首先 定义CMenu对象	
	CString pstrBoteRate[] = { _T("9600"), _T("115200") };
	UINT nNewMenuBoteRate = ID_BOTRATE_9600;
	CMenu* pSubMenuCom;
	S32 i;

	//自定义的菜单项
	this->m_bAutoMenuEnable = FALSE;//在操作菜单前关闭自动使能，否则操作无效
	menuMain = GetMenu();
	menuMain->AppendMenu(MF_BYPOSITION | MF_POPUP | MF_STRING,
		(UINT)menuMain->m_hMenu, _T("串口通信"));
	//获取索引为2的菜单项，即第3列菜单
	pSubMenuCom = menuMain->GetSubMenu(2);
	for (i = 0; i < 2; i++)
	{
		pSubMenuCom->AppendMenu(MF_STRING | MF_ENABLED, nNewMenuBoteRate++, pstrBoteRate[i]);
	}
	pSubMenuCom->AppendMenu(MF_SEPARATOR); //分隔线
	pSubMenuCom->AppendMenu(MF_STRING | MF_ENABLED, ID_COM_CFG, _T("串口配置"));
	/*波特率默认选择115200*/
	pSubMenuCom->CheckMenuItem(ID_BOTRATE_115200, MF_BYCOMMAND | MF_ENABLED | MF_CHECKED);
}
 /*初始化状态栏*/
S32 CMainFrame::InitStatusBar()
{
	CRect rectClient;
	S32 s32Width = 0;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	//设置状态栏的字体
	m_fontStatusBar.CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SCRIPT, _T("微软雅黑"));
	m_wndStatusBar.SetFont(&m_fontStatusBar);

	GetClientRect(&rectClient);
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	s32Width = rectClient.Width() / 12;
	m_IndexLinkStatus = m_wndStatusBar.CommandToIndex(ID_INDICATOR_LINKSTATUS);
	m_IndexWorkMode = m_wndStatusBar.CommandToIndex(ID_INDICATOR_WORKMODE);
	m_IndexFilePath = m_wndStatusBar.CommandToIndex(ID_INDICATOR_FILEPATH);
	m_IndexRestLine = m_wndStatusBar.CommandToIndex(ID_INDICATOR_RESTLINE);
	m_IndexComOpenTime = m_wndStatusBar.CommandToIndex(ID_INDICATOR_COMOPENTIME);

	//设置窗格宽度
	m_wndStatusBar.SetPaneWidth(m_IndexLinkStatus, s32Width);
	m_wndStatusBar.SetPaneWidth(m_IndexWorkMode, s32Width);
	m_wndStatusBar.SetPaneWidth(m_IndexFilePath, s32Width*4);
	m_wndStatusBar.SetPaneWidth(m_IndexRestLine, s32Width);
	m_wndStatusBar.SetPaneWidth(m_IndexComOpenTime, s32Width);

	m_wndStatusBar.SetPaneText(m_IndexLinkStatus, _T("无连接"));
	m_wndStatusBar.SetPaneText(m_IndexWorkMode, _T("空闲"));
	m_wndStatusBar.SetPaneText(m_IndexFilePath, _T("NULL"));
	m_wndStatusBar.SetPaneText(m_IndexRestLine, _T("RestLine：0"));
	m_wndStatusBar.SetPaneText(m_IndexComOpenTime, _T("0 s"));

	m_bBarMenu = TRUE;

	return 1;
}

void CMainFrame::CreateChildDlgs()
{
	ShowWindow(SW_HIDE);
	CRect rectClient(0, 0, 0, 0);
	CRect rectMenuBar(0, 0, 0, 0);
	/*计算对话框的区域（排除菜单栏和状态栏）*/
	GetClientRect(&rectClient);
	m_wndStatusBar.GetClientRect(&rectMenuBar);
	m_wndStatusBar.ShowWindow(SW_SHOW);
	rectClient.DeflateRect(0,0,0, rectMenuBar.Height());
	m_nWidth = rectClient.Width();
	m_nHeight = rectClient.Height();
	/*创建对话框*/
	m_dlgSatCNR.	Create(IDD_DIALOG_CNR, this);
	m_dlgDataProc.	Create(IDD_DIALOG_DATARCV, this);
	m_dlgErrCal.	Create(IDD_DIALOG_ERRCAL, this);
	m_dlgSatDist.	Create(IDD_DIALOG_SATDIST, this);
	m_dlgPtlTab.	Create(IDD_DIALOG_PTLTAB,this);
	m_dlgPtlTree.	Create(IDD_DIALOG_PTLSHOW,this);
	m_dlgDgtMap.	Create(IDD_DIALOG_MAP, this);
	/*设置对话框尺寸*/
	m_dlgDataProc.	MoveWindow(CRect(0, 0, m_nWidth / 4, m_nHeight / 2));
	m_dlgSatDist.	MoveWindow(CRect(0, 0, m_nWidth / 4, m_nHeight / 2));
	m_dlgErrCal.	MoveWindow(CRect(0, 0, m_nWidth / 4, m_nHeight / 2));
	m_dlgSatCNR.	MoveWindow(CRect(0, 0, m_nWidth / 4, m_nHeight / 2));
	m_dlgPtlTab.	MoveWindow(CRect(0, 0, (m_nWidth * 2) / 5, m_nHeight / 2));
	m_dlgPtlTree.	MoveWindow(CRect(0, 0, m_nWidth / 10, m_nHeight / 2));
	m_dlgDgtMap.	MoveWindow(CRect(0, 0, m_nWidth / 2, m_nHeight / 2));
	/*设置对话框位置*/
	m_dlgDataProc.	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_dlgSatDist.	SetWindowPos(NULL, 0, m_nHeight / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_dlgErrCal.	SetWindowPos(NULL, (m_nWidth * 3) / 4, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_dlgSatCNR.	SetWindowPos(NULL, m_nWidth / 4, m_nHeight / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_dlgPtlTab.	SetWindowPos(NULL, m_nWidth / 2, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_dlgPtlTree.	SetWindowPos(NULL, m_nWidth / 4, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_dlgDgtMap.	SetWindowPos(NULL, m_nWidth / 2, m_nHeight / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	/*显示对话框*/
	m_dlgSatCNR.	ShowWindow(SW_SHOW);
	m_dlgDataProc.	ShowWindow(SW_SHOW);
	m_dlgErrCal.	ShowWindow(SW_SHOW);
	m_dlgSatDist.	ShowWindow(SW_SHOW);
	m_dlgPtlTab.	ShowWindow(SW_SHOW);
	m_dlgPtlTree.	ShowWindow(SW_SHOW);
	m_dlgDgtMap.	ShowWindow(SW_SHOW);
	/*标志子对话框已经创建*/
	m_bDlgPtlTabDone = TRUE;
	m_bDlgDgtMapDone = TRUE;
	m_bDlgSatDistDone = TRUE;
	m_bDlgErrCalDone = TRUE;
	m_bDlgDataProcDone = TRUE;
	m_bDlgSatCNRDone = TRUE;
	m_bDlgPtlTreeDone = TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// 基类将执行真正的工作

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	return TRUE;
}
/*更新状态栏*/
LRESULT CMainFrame::OnUpdateStatusBar(WPARAM wParam, LPARAM lParam)
{
	S32 s32ItemIndex = 0;
	SStatusBar* psStatusBar = (SStatusBar*)lParam;

	if ((psStatusBar->uItemId < ID_INDICATOR_TIPS)
		|| (psStatusBar->uItemId > ID_INDICATOR_RESTLINE))
	{
		return 0;
	}

	UINT u32ItemId = psStatusBar->uItemId;
	CString strItem = psStatusBar->strItem;

	s32ItemIndex = m_wndStatusBar.CommandToIndex(u32ItemId);
	m_wndStatusBar.SetPaneText(s32ItemIndex, strItem);

	return 1;
}
LRESULT CMainFrame::OnStartUpTimer(WPARAM wParam, LPARAM lParam)
{
	S32 nTimerSetting = lParam;

	if ((nTimerSetting < 1) || (nTimerSetting > 2))
	{
		return 0;
	}

	if (nTimerSetting == SET_TIMER)
	{
		/*启用定时器*/
		m_s32LinkedTime = 0;
		SetTimer(TIMER_COMOPENTIME_ID, 1000, NULL);
	}
	else
	{
		/*启用定时器*/
		m_s32LinkedTime = 0;
		KillTimer(TIMER_COMOPENTIME_ID);
	}

	return 1;
}

/*菜单栏选择波特率*/
void CMainFrame::OnSelectBoteRate(UINT nID)
{
	CMenu* pMainMenu = GetMenu();
	CMenu* pSubMenu = pMainMenu->GetSubMenu(2);
	U32 uBoteRate[] = { 9600,115200 };
	UINT nPortIndex = nID - ID_BOTRATE_9600;

	pSubMenu->CheckMenuItem(ID_BOTRATE_9600, MF_BYCOMMAND | MF_ENABLED | MF_UNCHECKED);
	pSubMenu->CheckMenuItem(ID_BOTRATE_115200, MF_BYCOMMAND | MF_ENABLED | MF_UNCHECKED);

	pSubMenu->CheckMenuItem(nID, MF_BYCOMMAND | MF_ENABLED | MF_CHECKED);
	g_pDataPrco->m_uSelectedBoteRate = uBoteRate[nPortIndex];
}
/*TODO:菜单栏打开串口配置窗口*/
void CMainFrame::OnConfigureCom(UINT nID)
{


}
void CMainFrame::OnDestroy()
{
	CMDIFrameWndEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CMDIFrameWndEx::OnClose();
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWndEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rectClient(0, 0, 0, 0);
	CRect rectMenuBar(0, 0, 0, 0);
	if ((nType == SIZE_RESTORED) || (nType == SIZE_MAXIMIZED))
	{
		/*只在对话框和状态栏都创建好后才能使用其对象改变尺寸*/
		if (m_bBarMenu)
		{
			GetClientRect(&rectClient);
			m_wndStatusBar.GetClientRect(&rectMenuBar);
			m_wndStatusBar.ShowWindow(SW_SHOW);
			rectClient.DeflateRect(0, 0, 0, rectMenuBar.Height());
			cx = rectClient.Width();
			cy = rectClient.Height();
		}
		if (m_bDlgPtlTabDone)
		{
			m_dlgPtlTab.MoveWindow(CRect(0, 0, (cx * 2) / 5, cy / 2));
			m_dlgPtlTab.SetWindowPos(NULL, (cx * 7) / 20, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		if (m_bDlgDgtMapDone)
		{
			m_dlgDgtMap.MoveWindow(CRect(0, 0, cx / 2, cy / 2));
			m_dlgDgtMap.SetWindowPos(NULL, cx / 2, cy / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		if (m_bDlgSatDistDone)
		{
			m_dlgSatDist.MoveWindow(CRect(0, 0, cx / 4, cy / 2));
			m_dlgSatDist.SetWindowPos(NULL, 0, cy / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		if (m_bDlgErrCalDone)
		{
			m_dlgErrCal.MoveWindow(CRect(0, 0, cx / 4, cy / 2));
			m_dlgErrCal.SetWindowPos(NULL, (cx * 3) / 4, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		if (m_bDlgDataProcDone)
		{
			m_dlgDataProc.MoveWindow(CRect(0, 0, cx / 4, cy / 2));
			m_dlgDataProc.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		if (m_bDlgSatCNRDone)
		{
			m_dlgSatCNR.MoveWindow(CRect(0, 0, cx / 4, cy / 2));
			m_dlgSatCNR.SetWindowPos(NULL, cx / 4, cy / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		if (m_bDlgPtlTreeDone)
		{
			m_dlgPtlTree.MoveWindow(CRect(0, 0, cx / 10, cy / 2));
			m_dlgPtlTree.SetWindowPos(NULL, cx / 4, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
	}
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SStatusBar sLinkedTime = { 0 };
	CString	strLinkedTime = _T("");

	if (nIDEvent == TIMER_COMOPENTIME_ID)
	{
		m_s32LinkedTime += 1;
		strLinkedTime.Format(_T("%d"), m_s32LinkedTime);
		sLinkedTime.uItemId = ID_INDICATOR_COMOPENTIME;
		sLinkedTime.strItem = strLinkedTime + _T(" s");

		SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sLinkedTime);
	}
	CMDIFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_nWidth*0.95;
	lpMMI->ptMinTrackSize.y = m_nHeight*0.95;
}


void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*处理点击关闭按钮事件，否则this被回收*/
	if (nHitTest == HTCLOSE)
	{
		CMDIFrameWndEx::OnNcLButtonDown(nHitTest, point);
		return;
	}
	/*以下为改变窗口时，绘制虚框设置*/
	// 1，查询当前系统“拖动显示窗口内容”设置
	SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &m_bDragFullWindow, NULL);
	// 2，如果需要修改设置，则在每次进入CDialog::OnNcLButtonDown默认处理之前修改
	if (m_bDragFullWindow)
	{
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, NULL);
	}
	// 3，默认处理，系统会自动绘制虚框
	CMDIFrameWndEx::OnNcLButtonDown(nHitTest, point);
	// 4，默认处理完毕后，还原系统设置
	if (m_bDragFullWindow)
	{
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, NULL);
	}
}
