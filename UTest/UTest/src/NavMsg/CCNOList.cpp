// CCNOList.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "afxdialogex.h"
#include "CCNOList.h"
#include "CSatCNRDlg.h"


// CCNOList 对话框
CCNOList* g_pCNOList = NULL;

IMPLEMENT_DYNAMIC(CCNOList, CDialogEx)

CCNOList::CCNOList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CNO, pParent)
	, m_bListCreated(FALSE)
{
	g_pCNOList = this;
}

CCNOList::~CCNOList()
{
}

void CCNOList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CNO, m_CNOListCtrl);
	DDX_Control(pDX, IDC_EDIT_CNO, m_CNOEdit);
}


BEGIN_MESSAGE_MAP(CCNOList, CDialogEx)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CCNOList 消息处理程序


BOOL CCNOList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rectListCtrl;
	GetClientRect(&rectListCtrl);

	S32 s32ColumnWidth = rectListCtrl.Width();
	S32 s32ItemHeight = rectListCtrl.Height() / 15;

	m_CNOListCtrl.ModifyStyle(0, LVS_OWNERDRAWFIXED);
	m_CNOListCtrl.SetExtendedStyle(WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_EX_DOUBLEBUFFER | WS_CLIPCHILDREN
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_VSCROLL);
	m_CNOListCtrl.SetRowHeigt(s32ItemHeight);
	m_CNOListCtrl.InsertColumn(0, _T("卫星系统"), LVCFMT_CENTER, s32ColumnWidth / 4);//第0列无法居中显示，一般将其隐藏
	m_CNOListCtrl.InsertColumn(0, _T("频点"), LVCFMT_CENTER, s32ColumnWidth / 4);//第0列无法居中显示，一般将其隐藏
	m_CNOListCtrl.InsertColumn(1, _T("卫星号"), LVCFMT_CENTER, s32ColumnWidth / 4); // 插入第2列的列名
	m_CNOListCtrl.InsertColumn(2, _T("载噪比"), LVCFMT_CENTER, s32ColumnWidth / 4); // 插入第3列的列名

	m_nListLine = 0;
	/*	设置edit无限显示 */
	m_CNOEdit.SetLimitText(-1);
	g_CNOBuf = { 0 };
	memset(m_pnFrqReady, 0, sizeof(m_pnFrqReady));
	m_bListCreated = TRUE;

	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCNOList::CreateDynamicLayout()
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
	pMfcDynamicLayout->AddItem(IDC_EDIT_CNO, moveSetting1, sizeSetting1);
	pMfcDynamicLayout->AddItem(IDC_LIST_CNO, NoMove, sizeSetting2);
}

void CCNOList::OnSize(UINT nType, int cx, int cy)
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

void CCNOList::ReSize(CRect recta)
{
	if (m_bListCreated)
	{
		m_CNOListCtrl.SetRowHeigt(recta.Height() / 10);
		m_CNOListCtrl.SetColumnWidth(0, recta.Width() / 5);
		m_CNOListCtrl.SetColumnWidth(1, recta.Width() / 5);
		m_CNOListCtrl.SetColumnWidth(2, recta.Width() / 5);
		m_CNOListCtrl.SetColumnWidth(3, (recta.Width() * 2) / 5);
	}
}



S32 CCNOList::CNOInfoShow(SCNO sCNO, U8* u8CNOStatement)
{
	CString strCNO,strSatId,strCnr = _T("");
	S32 nLength,i,j,nTotalSatNum = 0;
	U32 uFrqIdx = 0;
	CString pStrFrq[MAX_FRQ_NUM] = { _T("B1I"),_T("B1C"),_T("B2a"),_T("B3I"),_T("L1"),_T("G1") };

	if ((u8CNOStatement == NULL) || (sCNO.eFrqId < eFrqB1I))
	{
		return 0;
	}

	/*语句打印*/
	strCNO = u8CNOStatement;
	nLength = m_CNOEdit.GetWindowTextLength();
	m_CNOEdit.SetSel(nLength, nLength);
	m_CNOEdit.ReplaceSel(strCNO);
	m_CNOEdit.ScrollWindow(0, 0);

	/*CNO信息缓存*/
	uFrqIdx = sCNO.eFrqId - 1;
	g_CNOBuf.peFrqId[uFrqIdx] = sCNO.eFrqId;
	g_CNOBuf.pu32SatNum[uFrqIdx] = sCNO.u32SatNum;
	memcpy(g_CNOBuf.pu32SatId[uFrqIdx], sCNO.pu32SatId, sizeof(sCNO.pu32SatId));
	memcpy(g_CNOBuf.pfSatCnr[uFrqIdx], sCNO.pfSatCnr, sizeof(sCNO.pfSatCnr));
	m_pnFrqReady[uFrqIdx] = 1;

	for (i = 0;i < MAX_FRQ_NUM; i++)
	{
		nTotalSatNum += g_CNOBuf.pu32SatNum[i];
	}
	/*想有几行加几行，少几行删几行*/
	for (i = m_nListLine; i < nTotalSatNum; i++)
	{
		m_CNOListCtrl.InsertItem(i, LVCFMT_LEFT, 100);
	}

	m_nListLine = 0;
	for (j = 0;j < MAX_FRQ_NUM;j++)
	{
		for (i = 0; (i < g_CNOBuf.pu32SatNum[j]) && (m_pnFrqReady[i]); i++)
		{
			if (g_CNOBuf.pu32SatId[j][i] != 0)
			{
				strSatId.Format(_T("%d"), g_CNOBuf.pu32SatId[j][i]);
				strCnr.Format(_T("%.1lf"), g_CNOBuf.pfSatCnr[j][i]);
				m_CNOListCtrl.SetItemText(m_nListLine, 0, pStrFrq[j]);
				m_CNOListCtrl.SetItemText(m_nListLine, 1, strSatId);
				m_CNOListCtrl.SetItemText(m_nListLine, 2, strCnr);
				m_nListLine++;
			}
		}
	}
	for (i = m_nListLine; i <= nTotalSatNum; i++)
	{
		m_CNOListCtrl.DeleteItem(i);
	}

	return 1;
}


void CCNOList::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}


void CCNOList::OnSysCommand(UINT nID, LPARAM lParam)
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

int CCNOList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return 0;
}
