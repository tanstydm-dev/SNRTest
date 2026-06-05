// CGSAList.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "afxdialogex.h"
#include "CGSAList.h"


// CGSAList 对话框

IMPLEMENT_DYNAMIC(CGSAList, CDialogEx)

CGSAList::CGSAList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_GSA, pParent)
	, m_bListCreated(FALSE)
{

}

CGSAList::~CGSAList()
{
}

void CGSAList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GSA, m_GSAListCtrl);
	DDX_Control(pDX, IDC_EDIT_GSADATA, m_editGSAData);
}


BEGIN_MESSAGE_MAP(CGSAList, CDialogEx)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CGSAList 消息处理程序


BOOL CGSAList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rectListCtrl;
	GetClientRect(&rectListCtrl);

	S32 s32ColumnWidth = rectListCtrl.Width() / 7;
	S32 s32ItemHeight = rectListCtrl.Height() / 10;

	m_GSAListCtrl.ModifyStyle(0, LVS_OWNERDRAWFIXED);
	m_GSAListCtrl.SetExtendedStyle(WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_EX_DOUBLEBUFFER | WS_CLIPCHILDREN
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_VSCROLL);
	m_GSAListCtrl.SetRowHeigt(s32ItemHeight);
	m_GSAListCtrl.InsertColumn(0, _T("GPS"), LVCFMT_CENTER, s32ColumnWidth);
	m_GSAListCtrl.InsertColumn(1, _T("BDS"), LVCFMT_CENTER, s32ColumnWidth);
	m_GSAListCtrl.InsertColumn(2, _T("GLO"), LVCFMT_CENTER, s32ColumnWidth);
	m_GSAListCtrl.InsertColumn(3, _T("PDOP"), LVCFMT_CENTER, s32ColumnWidth);
	m_GSAListCtrl.InsertColumn(4, _T("HDOP"), LVCFMT_CENTER, s32ColumnWidth);
	m_GSAListCtrl.InsertColumn(5, _T("VDOP"), LVCFMT_CENTER, s32ColumnWidth);
	m_GSAListCtrl.InsertColumn(6, _T("TDOP"), LVCFMT_CENTER, s32ColumnWidth);

	m_bListCreated = TRUE;

	for (S32 i = 0;i < 15;i++)
	{
		m_GSAListCtrl.InsertItem(i, LVCFMT_LEFT, s32ColumnWidth);
	}
	/*	设置edit无限显示 */
	m_editGSAData.SetLimitText(-1);

	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGSAList::CreateDynamicLayout()
{
	this->EnableDynamicLayout();
	CMFCDynamicLayout::MoveSettings NoMove;
	CMFCDynamicLayout::MoveSettings moveSetting1;
	CMFCDynamicLayout::MoveSettings Move_Both;

	CMFCDynamicLayout::SizeSettings NoSize;
	CMFCDynamicLayout::SizeSettings sizeSetting1;
	CMFCDynamicLayout::SizeSettings sizeSetting2;

	moveSetting1.m_nXRatio = 0;
	moveSetting1.m_nYRatio = 100 * (32.0 / 40.0);
	Move_Both.m_nXRatio = 100;
	Move_Both.m_nYRatio = 100;

	sizeSetting1.m_nXRatio = 100;
	sizeSetting1.m_nYRatio = 100 * (8.0 / 40.0);
	sizeSetting2.m_nXRatio = 100;
	sizeSetting2.m_nYRatio = 100 * (32.0 / 40.0);
	// 获取Dialog的动态布局指针。
	CMFCDynamicLayout* pMfcDynamicLayout = this->GetDynamicLayout();
	// 创建动态布局。
	pMfcDynamicLayout->Create(this);
	// 添加要在动态布局中注册的控件。
	pMfcDynamicLayout->AddItem(IDC_EDIT_GSADATA, moveSetting1, sizeSetting1);
	pMfcDynamicLayout->AddItem(IDC_LIST_GSA, NoMove, sizeSetting2);
}

void CGSAList::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rectWorkArea;
	GetClientRect(&rectWorkArea);     //取客户区大小  
	if (nType != SIZE_MINIMIZED)
	{
		ReSize(rectWorkArea);
	}
}


void CGSAList::ReSize(CRect recta)
{
	if (m_bListCreated)
	{
		m_GSAListCtrl.SetRowHeigt(recta.Height() / 10);
		m_GSAListCtrl.SetColumnWidth(0, recta.Width() / 5);
		m_GSAListCtrl.SetColumnWidth(1, recta.Width() / 5);
		m_GSAListCtrl.SetColumnWidth(2, recta.Width() / 5);
		m_GSAListCtrl.SetColumnWidth(3, (recta.Width() * 2) / 5);
	}
}


void CGSAList::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}


void CGSAList::OnSysCommand(UINT nID, LPARAM lParam)
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


int CGSAList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return 0;
}
