// CGGAList.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "CGGAList.h"
#include "afxdialogex.h"
#include "CErrCal.h"
#include "CPtlMsg.h"
CGGAList* g_pGGAPtl = NULL;
// CGGAList 对话框

IMPLEMENT_DYNAMIC(CGGAList, CDialogEx)

CGGAList::CGGAList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_GGA, pParent)
	, m_bListCreated(FALSE)
{
	g_pGGAPtl = this;
}

CGGAList::~CGGAList()
{
}

void CGGAList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GGA, m_GGAListCtrl);
	DDX_Control(pDX, IDC_EDIT_GGADATA, m_editGGAData);
}


BEGIN_MESSAGE_MAP(CGGAList, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CGGAList 消息处理程序

#define GGASHOW_F64_INUI(f64inGGAnum,IDinshow)\
strGSVUtc.Format(_T("%lf"), f64inGGAnum);\
g_pGGAPtl->SetDlgItemText(IDinshow, strGSVUtc);\

#define GGASHOW_S32_INUI(s32inGGAnum, IDinshow)\
strGSVUtc.Format(_T("%d"), s32inGGAnum); \
g_pGGAPtl->SetDlgItemText(IDinshow, strGSVUtc); \


BOOL CGGAList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Brush.CreateSolidBrush(RGB(255,255,255));

	CRect rectDlg;
	GetClientRect(&rectDlg);

	S32 s32ColumnWidth = rectDlg.Width();
	S32 s32ItemHeight = rectDlg.Height() / 12;

	m_GGAListCtrl.ModifyStyle(0, LVS_OWNERDRAWFIXED);
	m_GGAListCtrl.SetExtendedStyle(WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_EX_DOUBLEBUFFER | WS_CLIPCHILDREN
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_VSCROLL);

	m_GGAListCtrl.InsertColumn(0, _T("参数(Parameter)"), LVCFMT_LEFT, s32ColumnWidth / 5);//第0列无法居中显示，一般将其隐藏
	m_GGAListCtrl.InsertColumn(1, _T("值(Value)"), LVCFMT_RIGHT, s32ColumnWidth / 5); // 插入第2列的列名
	m_GGAListCtrl.InsertColumn(2, _T("单位(Unit)"), LVCFMT_RIGHT, s32ColumnWidth / 5); // 插入第3列的列名
	m_GGAListCtrl.InsertColumn(3, _T("描述(Description)"), LVCFMT_LEFT, (s32ColumnWidth * 2) / 5); // 插入第4列的列名

// 	m_GGAListCtrl.InsertItem (0, _T("协调世界时"));
// 	m_GGAListCtrl.SetItemText(0, 2, _T("hhmmss.ss"));
// 	m_GGAListCtrl.SetItemText(0, 3, _T("Coordinated Universal Time"));
	m_GGAListCtrl.InsertItem(0, _T("周内秒"));
	m_GGAListCtrl.SetItemText(0, 2, _T("s"));
	m_GGAListCtrl.SetItemText(0, 3, _T("second of week"));

	m_GGAListCtrl.InsertItem (1, _T("纬度"));
	m_GGAListCtrl.SetItemText(1, 2, _T("dd.mmmmmm"));
	m_GGAListCtrl.SetItemText(1, 3, _T("Latitude"));
	m_GGAListCtrl.InsertItem(2, _T("纬度方向"));
	m_GGAListCtrl.SetItemText(2, 2, _T(""));
	m_GGAListCtrl.SetItemText(2, 3, _T("N-北纬，S-南纬"));
	m_GGAListCtrl.InsertItem (3, _T("经度"));
	m_GGAListCtrl.SetItemText(3, 2, _T("dd.mmmmmm"));
	m_GGAListCtrl.SetItemText(3, 3, _T("Longitude"));
	m_GGAListCtrl.InsertItem(4, _T("经度方向"));
	m_GGAListCtrl.SetItemText(4, 2, _T(""));
	m_GGAListCtrl.SetItemText(4, 3, _T("E-东经，W-西经"));
	m_GGAListCtrl.InsertItem (5, _T("状态指示"));
	m_GGAListCtrl.SetItemText(5, 2, _T(""));
	m_GGAListCtrl.SetItemText(5, 3, _T("0-无效,1-定位有效,2-差分定位有效,3-PPS模式,定位有效,4-RTK模式,5-浮动RTK,6-估算模式,7-手动输入模式,8-模拟器模式"));
	m_GGAListCtrl.InsertItem(6, _T("参与定位的卫星数"));
	m_GGAListCtrl.SetItemText(6, 2, _T(""));
	m_GGAListCtrl.SetItemText(6, 3, _T("Number of satellites used for decoding"));
	m_GGAListCtrl.InsertItem (7, _T("HDOP"));
	m_GGAListCtrl.SetItemText(7, 2, _T(""));
	m_GGAListCtrl.SetItemText(7, 3, _T("水平精度因子"));
	m_GGAListCtrl.InsertItem(8, _T("天线大地高"));
	m_GGAListCtrl.SetItemText(8, 2, _T("m"));
	m_GGAListCtrl.SetItemText(8, 3, _T("基于平均海平面的高程"));
	m_GGAListCtrl.InsertItem(9, _T("高程异常"));
	m_GGAListCtrl.SetItemText(9, 2, _T("m"));
	m_GGAListCtrl.SetItemText(9, 3, _T("等于椭球高-海平面高程"));
	m_GGAListCtrl.InsertItem (10, _T("差分数据龄期"));
	m_GGAListCtrl.SetItemText(10, 2, _T(""));
	m_GGAListCtrl.SetItemText(10, 3, _T(""));
	m_GGAListCtrl.InsertItem(11, _T("差分站台ID号"));
	m_GGAListCtrl.SetItemText(11, 2, _T(""));
	m_GGAListCtrl.SetItemText(11, 3, _T(""));
	m_GGAListCtrl.InsertItem(12, _T("VDOP值"));
	m_GGAListCtrl.SetItemText(12, 2, _T(""));
	m_GGAListCtrl.SetItemText(12, 3, _T("垂直精度因子"));

	/*	设置edit无限显示 */
	m_editGGAData.SetLimitText(-1);
	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);
	m_bListCreated = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CGGAList::CreateDynamicLayout()
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
	pMfcDynamicLayout->AddItem(IDC_EDIT_GGADATA, moveSetting1, sizeSetting1);
	pMfcDynamicLayout->AddItem(IDC_LIST_GGA, NoMove, sizeSetting2);
}

void CGGAList::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
}



HBRUSH CGGAList::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}


S32 CGGAList::GGAInfoShow(SGGA sGGA, U8* u8GGAStatement)
{
	CString strGSVUtc, strGSVHour, strGSVMin, strGSVSec, strTimeSep;
	CString strBLHPos, strBLHDegree, strBLHMin, strDir;
	CString str, strGGA;
	S32 nLength = 0;


	/*语句打印*/
	strGGA = u8GGAStatement;
	nLength = m_editGGAData.GetWindowTextLength();
	m_editGGAData.SetSel(nLength, nLength);
	m_editGGAData.ReplaceSel(strGGA);
	m_editGGAData.ScrollWindow(0, 0);

	/*读入RNSS定位信息*/
// 	strGSVHour.Format(_T("%d"), sGGA.pSTime->nHour);
// 	strGSVMin.Format(_T("%d"), sGGA.pSTime->nMin);
// 	strGSVSec.Format(_T("%lf"), sGGA.pSTime->nSec);
// 	strTimeSep = _T(":");
// 	strGSVUtc = strGSVHour + strTimeSep + strGSVMin + strTimeSep + strGSVSec;
// 	m_GGAListCtrl.SetItemText(0, 1, strGSVUtc);

	strBLHDegree.Format(_T("%d"), sGGA.pSPoint->nLatDegree);
	strBLHMin.Format(_T("%lf"), sGGA.pSPoint->nLatMin);
	strBLHMin.Replace(_T("."), _T(""));
	strDir.Format(_T("%c"), sGGA.pSPoint->sLatDir);
	strBLHPos = strBLHDegree + _T(".") + strBLHMin;
	m_GGAListCtrl.SetItemText(1, 1, strBLHPos);
	m_GGAListCtrl.SetItemText(2, 1, strDir);

	strBLHDegree.Format(_T("%d"), sGGA.pSPoint->nLonDegree);
	strBLHMin.Format(_T("%lf"), sGGA.pSPoint->nLonMin);
	strBLHMin.Replace(_T("."),_T(""));
	strDir.Format(_T("%c"), sGGA.pSPoint->sLonDir);
	strBLHPos = strBLHDegree + _T(".") + strBLHMin;
	m_GGAListCtrl.SetItemText(3, 1, strBLHPos);
	m_GGAListCtrl.SetItemText(4, 1, strDir);

	str.Format(_T("%u"), sGGA.nState);
	m_GGAListCtrl.SetItemText(5, 1, str);
	str.Format(_T("%u"), sGGA.nSvNum);
	m_GGAListCtrl.SetItemText(6, 1, str);
	str.Format(_T("%lf"), sGGA.dHDOP);
	m_GGAListCtrl.SetItemText(7, 1, str);
	str.Format(_T("%lf"), sGGA.pSPoint->nHeight);
	m_GGAListCtrl.SetItemText(8, 1, str);
	str.Format(_T("%lf"), sGGA.pSPoint->nHeightDif);
	m_GGAListCtrl.SetItemText(9, 1, str);
	str.Format(_T("%lf"), sGGA.dVDOP);
	m_GGAListCtrl.SetItemText(12, 1, str);

	return 0;
}

S32 CGGAList::COVInfoShow(SCOV sCOV)
{
	CString strSow, strCOV;
	strSow.Format(_T("%u"), sCOV.u32Sec);
	m_GGAListCtrl.SetItemText(0, 1, strSow);
	return 0;
}


void CGGAList::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (nType != SIZE_MINIMIZED)
	{
		ReSize();
	}


	// TODO: 在此处添加消息处理程序代码
}

void CGGAList::ReSize()
{
	CRect recta;
	GetClientRect(&recta);     //取客户区大小  
	if (m_bListCreated)
	{
		m_GGAListCtrl.SetRowHeigt(recta.Height() / 10);
		m_GGAListCtrl.SetColumnWidth(0, recta.Width() / 5);
		m_GGAListCtrl.SetColumnWidth(1, recta.Width() / 5);
		m_GGAListCtrl.SetColumnWidth(2, recta.Width() / 5);
		m_GGAListCtrl.SetColumnWidth(3, (recta.Width() * 2) / 5);
	}
}

void CGGAList::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}


void CGGAList::OnSysCommand(UINT nID, LPARAM lParam)
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


int CGGAList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return 0;
}
