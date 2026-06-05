// CDHVList.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "afxdialogex.h"
#include "CDHVList.h"


// CDHVList 对话框
CDHVList* g_pDHVList = NULL;
IMPLEMENT_DYNAMIC(CDHVList, CDialogEx)

CDHVList::CDHVList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DHV, pParent)
	, m_bListCreated(FALSE)
{
	g_pDHVList = this;
}

CDHVList::~CDHVList()
{
}

void CDHVList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DHV, m_DHVListCtrl);
	DDX_Control(pDX, IDC_EDIT_DHVDATA, m_editDHVData);
}


BEGIN_MESSAGE_MAP(CDHVList, CDialogEx)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDHVList 消息处理程序


BOOL CDHVList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rectListCtrl;
	GetClientRect(&rectListCtrl);

	S32 s32ColumnWidth = rectListCtrl.Width();
	S32 s32ItemHeight = rectListCtrl.Height() / 10;

	m_DHVListCtrl.ModifyStyle(0,LVS_OWNERDRAWFIXED);
	m_DHVListCtrl.SetExtendedStyle(WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_EX_DOUBLEBUFFER | WS_CLIPCHILDREN 
				| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_VSCROLL);
	m_DHVListCtrl.SetRowHeigt(s32ItemHeight);
	m_DHVListCtrl.InsertColumn(0, _T("参数(Parameter)"), LVCFMT_LEFT, s32ColumnWidth / 6);//第0列无法居中显示，一般将其隐藏
	m_DHVListCtrl.InsertColumn(1, _T("值(Value)"), LVCFMT_LEFT, s32ColumnWidth / 4); // 插入第2列的列名
	m_DHVListCtrl.InsertColumn(2, _T("单位(Unit)"), LVCFMT_RIGHT, s32ColumnWidth / 4); // 插入第3列的列名
	m_DHVListCtrl.InsertColumn(3, _T("描述(Description)"), LVCFMT_LEFT, s32ColumnWidth / 3); // 插入第4列的列名

	m_DHVListCtrl.InsertItem(0,_T("协调世界时"));
	m_DHVListCtrl.SetItemText(0,2,_T("hhmmss.ss"));
	m_DHVListCtrl.SetItemText(0,3,_T("Coordinated Universal Time"));
	m_DHVListCtrl.InsertItem(1, _T("三维速度"));
	m_DHVListCtrl.SetItemText(1, 2, _T("Km/h"));
	m_DHVListCtrl.SetItemText(1, 3, _T("3-dimensional velocity"));
	m_DHVListCtrl.InsertItem(2, _T("X轴速度"));
	m_DHVListCtrl.SetItemText(2, 2, _T("Km/h"));
	m_DHVListCtrl.SetItemText(2, 3, _T("Velocity in the x-axis direction"));
	m_DHVListCtrl.InsertItem(3, _T("Y轴速度"));
	m_DHVListCtrl.SetItemText(3, 2, _T("Km/h"));
	m_DHVListCtrl.SetItemText(3, 3, _T("Velocity in the y-axis direction"));
	m_DHVListCtrl.InsertItem(4, _T("Z轴速度"));
	m_DHVListCtrl.SetItemText(4, 2, _T("Km/h"));
	m_DHVListCtrl.SetItemText(4, 3, _T("Velocity in the z-axis direction"));
	m_DHVListCtrl.InsertItem(5, _T("对地速度"));
	m_DHVListCtrl.SetItemText(5, 2, _T("Km/h"));
	m_DHVListCtrl.SetItemText(5, 3, _T("Ground Velocity"));
	m_DHVListCtrl.InsertItem(6, _T("最大速度"));
	m_DHVListCtrl.SetItemText(6, 2, _T("Km/h"));
	m_DHVListCtrl.SetItemText(6, 3, _T("Maximum velocity"));
	m_DHVListCtrl.InsertItem(7, _T("平均速度"));
	m_DHVListCtrl.SetItemText(7, 2, _T("Km/h"));
	m_DHVListCtrl.SetItemText(7, 3, _T("Average velocity"));
	m_DHVListCtrl.InsertItem(8, _T("全程平均速度"));
	m_DHVListCtrl.SetItemText(8, 2, _T("Km/h"));
	m_DHVListCtrl.SetItemText(8, 3, _T("Average speed for the whole journey"));
	m_DHVListCtrl.InsertItem(9, _T("有效速度"));
	m_DHVListCtrl.SetItemText(9, 2, _T("Km/h"));
	m_DHVListCtrl.SetItemText(9, 3, _T("effective speed"));

	m_bListCreated = TRUE;
	/*	设置edit无限显示 */
	m_editDHVData.SetLimitText(-1);
	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDHVList::CreateDynamicLayout()
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
	pMfcDynamicLayout->AddItem(IDC_EDIT_DHVDATA, moveSetting1, sizeSetting1);
	pMfcDynamicLayout->AddItem(IDC_LIST_DHV, NoMove, sizeSetting2);
}

S32 CDHVList::DHVInfoShow(SDHV sDHV, U8* u8DHVStatement)
{
	//读入RNSS定位信息
	CString str, strDHV = _T("");
	S32 nLength = 0;

	strDHV = u8DHVStatement;
	nLength = m_editDHVData.GetWindowTextLength();
	m_editDHVData.SetSel(nLength, nLength);
	m_editDHVData.ReplaceSel(strDHV);
	m_editDHVData.ScrollWindow(0, 0);

/*
	str.Format(_T("%.3f"), sDHV.speed);             //UTC
	m_DHVListCtrl.SetItemText(0, 1, str);
*/

	str.Format(_T("%.3f"), sDHV.speed);             //速度
	m_DHVListCtrl.SetItemText(1, 1, str);

	str.Format(_T("%.3f"), sDHV.xSpeed);           //x 轴速度
	m_DHVListCtrl.SetItemText(2, 1, str);

	str.Format(_T("%.3f"), sDHV.ySpeed);            //y 轴速度
	m_DHVListCtrl.SetItemText(3, 1, str);

	str.Format(_T("%.3f"), sDHV.zSpeed);            //z 轴速度
	m_DHVListCtrl.SetItemText(4, 1, str);

	str.Format(_T("%.3f"), sDHV.maxSpeed);             //最大速度
	m_DHVListCtrl.SetItemText(5, 1, str);

	str.Format(_T("%.3f"), sDHV.avgSpeed);             //平均速度
	m_DHVListCtrl.SetItemText(6, 1, str);

	str.Format(_T("%.3f"), sDHV.wholeSpeed);             //全程平均速度
	m_DHVListCtrl.SetItemText(7, 1, str);

	str.Format(_T("%.3f"), sDHV.validSpeed);             //有效速度
	m_DHVListCtrl.SetItemText(8, 1, str);

	return 0;
}

void CDHVList::OnSize(UINT nType, int cx, int cy)
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


void CDHVList::ReSize(CRect recta)
{
	if (m_bListCreated)
	{
		m_DHVListCtrl.SetRowHeigt(recta.Height() / 10);
		m_DHVListCtrl.SetColumnWidth(0, recta.Width() / 5);
		m_DHVListCtrl.SetColumnWidth(1, recta.Width() / 5);
		m_DHVListCtrl.SetColumnWidth(2, recta.Width() / 5);
		m_DHVListCtrl.SetColumnWidth(3, (recta.Width() * 2) / 5);
	}
}

void CDHVList::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}


void CDHVList::OnSysCommand(UINT nID, LPARAM lParam)
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


int CDHVList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return 0;
}
