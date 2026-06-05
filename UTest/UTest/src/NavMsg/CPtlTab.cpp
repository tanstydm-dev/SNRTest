// CPtlTab.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "afxdialogex.h"
#include "CHeadernStruct.h"
#include "CPtlTab.h"

CPtlTab* g_pPtlTab = NULL;
// CPtlTab 对话框

IMPLEMENT_DYNAMIC(CPtlTab, CDialogEx)

CPtlTab::CPtlTab(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PTLTAB, pParent)
	, m_bDlgGGADone(FALSE)
	, m_bDlgGSADone(FALSE)
	, m_bDlgGSVDone(FALSE)
	, m_bDlgDHVDone(FALSE)
	, m_bDlgCNODone(FALSE)
{
	g_pPtlTab = this;
}

CPtlTab::~CPtlTab()
{
}

void CPtlTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPtlTab, CDialogEx)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CPtlTab 消息处理程序


void CPtlTab::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
	if (m_bDlgGSADone)
	{
		m_dlgGSAList.MoveWindow(CRect(0, 0, cx, cy));
		m_dlgGSAList.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	if (m_bDlgGGADone)
	{
		m_dlgGGAList.MoveWindow(CRect(0, 0, cx, cy));
		m_dlgGGAList.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	if (m_bDlgGSVDone)
	{
		m_dlgGSVList.MoveWindow(CRect(0, 0, cx, cy));
		m_dlgGSVList.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	if (m_bDlgDHVDone)
	{
		m_dlgDHVList.MoveWindow(CRect(0, 0, cx, cy));
		m_dlgDHVList.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	if (m_bDlgCNODone)
	{
		m_dlgCNOList.MoveWindow(CRect(0, 0, cx, cy));
		m_dlgCNOList.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
}


BOOL CPtlTab::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	CRect rcTabCtrl;

	GetClientRect(&rect);     //取客户区大小  

	m_tabCtrl.Create(TCS_TABS | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN |WS_CLIPSIBLINGS, rect, this, IDC_TAB_PTL);
	/*为Tab Control 增加页面*/
	m_tabCtrl.InsertItem(0, _T("GGA"));
	m_tabCtrl.InsertItem(1, _T("DHV"));
	m_tabCtrl.InsertItem(2, _T("GSV"));
	m_tabCtrl.InsertItem(3, _T("GSA"));
	m_tabCtrl.InsertItem(4, _T("CNO"));


	m_dlgGSAList.Create(IDD_DIALOG_GSA, &m_tabCtrl);
	m_dlgGGAList.Create(IDD_DIALOG_GGA, &m_tabCtrl);
	m_dlgGSVList.Create(IDD_DIALOG_GSV, &m_tabCtrl);
	m_dlgDHVList.Create(IDD_DIALOG_DHV, &m_tabCtrl);
	m_dlgCNOList.Create(IDD_DIALOG_CNO, &m_tabCtrl);

	/*设定在Tab内显示的范围*/
	m_tabCtrl.GetClientRect(rcTabCtrl);
	rcTabCtrl.top += 0;
	rcTabCtrl.bottom -= 0;
	rcTabCtrl.left += 0;
	rcTabCtrl.right -= 0;

	m_dlgGSAList.SetWindowPos(NULL, rcTabCtrl.left, rcTabCtrl.top, rcTabCtrl.Width(), rcTabCtrl.Height(), SWP_HIDEWINDOW);
	m_dlgGGAList.SetWindowPos(NULL, rcTabCtrl.left, rcTabCtrl.top, rcTabCtrl.Width(), rcTabCtrl.Height(), SWP_SHOWWINDOW);
	m_dlgGSVList.SetWindowPos(NULL, rcTabCtrl.left, rcTabCtrl.top, rcTabCtrl.Width(), rcTabCtrl.Height(), SWP_HIDEWINDOW);
	m_dlgDHVList.SetWindowPos(NULL, rcTabCtrl.left, rcTabCtrl.top, rcTabCtrl.Width(), rcTabCtrl.Height(), SWP_HIDEWINDOW);
	m_dlgCNOList.SetWindowPos(NULL, rcTabCtrl.left, rcTabCtrl.top, rcTabCtrl.Width(), rcTabCtrl.Height(), SWP_HIDEWINDOW);

	m_bDlgGGADone = TRUE;
	m_bDlgGSADone = TRUE;
	m_bDlgGSVDone = TRUE;
	m_bDlgDHVDone = TRUE;
	m_bDlgCNODone = TRUE;

	//保存当前选择
	m_nCurSelTab = 0;

	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPtlTab::CreateDynamicLayout()
{
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
	pMfcDynamicLayout->AddItem(IDC_TAB_PTL, NoMove, Size_Both);
}

/*改变tab显示的协议*/
void CPtlTab::ChangeDlgShow(S32 nShowDlg)
{
	CRect rc;

	if ((nShowDlg >= 7) || (nShowDlg < 2))
	{
		return;
	}

	if (nShowDlg == 2)		//DHV
	{
		m_dlgDHVList.ShowWindow(SW_SHOW);
		m_dlgGSVList.ShowWindow(SW_HIDE);
		m_dlgGGAList.ShowWindow(SW_HIDE);
		m_dlgGSAList.ShowWindow(SW_HIDE);
		m_dlgCNOList.ShowWindow(SW_HIDE);
	}
	else if (nShowDlg == 3)	//GSV
	{
		m_dlgGSVList.ShowWindow(SW_SHOW);
		m_dlgGGAList.ShowWindow(SW_HIDE);
		m_dlgGSAList.ShowWindow(SW_HIDE);
		m_dlgDHVList.ShowWindow(SW_HIDE);
		m_dlgCNOList.ShowWindow(SW_HIDE);
	}
	else if (nShowDlg == 4)	//CNO
	{
		m_dlgCNOList.ShowWindow(SW_SHOW);
		m_dlgGSAList.ShowWindow(SW_HIDE);
		m_dlgGSVList.ShowWindow(SW_HIDE);
		m_dlgDHVList.ShowWindow(SW_HIDE);
		m_dlgGGAList.ShowWindow(SW_HIDE);
	}
	else if (nShowDlg == 5)	//GGA
	{
		m_dlgGGAList.ShowWindow(SW_SHOW);
		m_dlgGSAList.ShowWindow(SW_HIDE);
		m_dlgGSVList.ShowWindow(SW_HIDE);
		m_dlgDHVList.ShowWindow(SW_HIDE);
		m_dlgCNOList.ShowWindow(SW_HIDE);
	}
	else if (nShowDlg == 6)	//GSA
	{
		m_dlgGSAList.ShowWindow(SW_SHOW);
		m_dlgGGAList.ShowWindow(SW_HIDE);
		m_dlgGSVList.ShowWindow(SW_HIDE);
		m_dlgDHVList.ShowWindow(SW_HIDE);
		m_dlgCNOList.ShowWindow(SW_HIDE);

	}
	return;
}

void CPtlTab::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

/*禁止用户主动拖动窗口,经调试，SC_SIZE~SC_MOVE的nID是用户主动改变尺寸的消息*/
void CPtlTab::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ((nID >= SC_SIZE) && (nID <= SC_MOVE))
	{
		return;
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CPtlTab::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}
