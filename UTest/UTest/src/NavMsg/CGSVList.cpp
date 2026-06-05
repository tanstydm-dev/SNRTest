// CGSVList.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "afxdialogex.h"
#include "CGSVList.h"


// CGSVList 对话框
CGSVList* g_pGSVList = NULL;
IMPLEMENT_DYNAMIC(CGSVList, CDialogEx)

CGSVList::CGSVList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_GSV, pParent)
	, m_bListCreated(FALSE)
{
	g_pGSVList = this;
}

CGSVList::~CGSVList()
{
}

void CGSVList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GSV, m_GSVListCtrl);
	DDX_Control(pDX, IDC_EDIT_GSVDATA, m_editGSVData);
}


BEGIN_MESSAGE_MAP(CGSVList, CDialogEx)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CGSVList 消息处理程序


BOOL CGSVList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// TODO:  在此添加额外的初始化
	CRect rectListCtrl;
	GetClientRect(&rectListCtrl);

	S32 s32ColumnWidth = rectListCtrl.Width();
	S32 s32ItemHeight = rectListCtrl.Height() / 15;

	m_GSVListCtrl.ModifyStyle(0, LVS_OWNERDRAWFIXED);
	m_GSVListCtrl.SetExtendedStyle(WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_EX_DOUBLEBUFFER | WS_CLIPCHILDREN
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_VSCROLL);
	m_GSVListCtrl.SetRowHeigt(s32ItemHeight);
	m_GSVListCtrl.InsertColumn(0, _T("卫星系统"), LVCFMT_CENTER, s32ColumnWidth / 7);//第0列无法居中显示，一般将其隐藏
	m_GSVListCtrl.InsertColumn(1, _T("卫星号"), LVCFMT_CENTER, s32ColumnWidth / 7); // 插入第2列的列名
	m_GSVListCtrl.InsertColumn(2, _T("高度角[deg]"), LVCFMT_CENTER, s32ColumnWidth / 7); // 插入第3列的列名
	m_GSVListCtrl.InsertColumn(3, _T("方位角[deg]"), LVCFMT_CENTER, s32ColumnWidth / 7); // 插入第4列的列名
	m_GSVListCtrl.InsertColumn(4, _T("C/N0[dBHz]"), LVCFMT_CENTER, s32ColumnWidth / 7); // 插入第4列的列名
	m_GSVListCtrl.InsertColumn(5, _T("均值[dBHz]"), LVCFMT_CENTER, s32ColumnWidth / 7); // 插入第4列的列名
	m_GSVListCtrl.InsertColumn(6, _T("标准差"), LVCFMT_CENTER, s32ColumnWidth / 7); // 插入第4列的列名
	m_bListCreated = TRUE;
	m_nListLine = 0;
	/*	设置edit无限显示 */
	m_editGSVData.SetLimitText(-1);
	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	memset(m_sCnrBuf,0,3 * sizeof(SCalcMeanStd));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGSVList::CreateDynamicLayout()
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
	pMfcDynamicLayout->AddItem(IDC_EDIT_GSVDATA, moveSetting1, sizeSetting1);
	pMfcDynamicLayout->AddItem(IDC_LIST_GSV, NoMove, sizeSetting2);
}

S32 CGSVList::SortGSV(S32 nNavSys, SGSVFrm sGSV, SGSVFrm* sGSVBuf)
{
	S32 i = 0;
	S32 j = 0;

	if ((nNavSys < 0) || (nNavSys > 3))
	{
		return ERR_BAD_ARGS;
	}

	if ((sGSVBuf[nNavSys].nIndex == sGSVBuf[nNavSys].nTotalNum))
	{
		sGSVBuf[nNavSys].nIndex = 0;
		sGSVBuf[nNavSys].nTotalNum = 0;
		sGSVBuf[nNavSys].s32AvlSatNum = 0;
		sGSVBuf[nNavSys].s32RcvdSatNum = 0;
		memset(sGSVBuf[nNavSys].pSatId, 0, sizeof(sGSVBuf[nNavSys].pSatId));
		memset(sGSVBuf[nNavSys].pElevation, 0.0, sizeof(sGSVBuf[nNavSys].pElevation));
		memset(sGSVBuf[nNavSys].pAzimuth, 0.0, sizeof(sGSVBuf[nNavSys].pAzimuth));
		memset(sGSVBuf[nNavSys].pfSatCnr, 0.0, sizeof(sGSVBuf[nNavSys].pfSatCnr));
		memset(sGSVBuf[nNavSys].pnAvlSatId, 0, sizeof(sGSVBuf[nNavSys].pnAvlSatId));
	}
	/*	缓存当前语句至sGSVBuffer */
	for (i = 0; i < MAX_PROC_SAT_NUM; i++)
	{
		if (sGSV.pSatId[i] != 0)
		{
			sGSVBuf[nNavSys].pSatId[i] = sGSV.pSatId[i];
			sGSVBuf[nNavSys].pElevation[i] = sGSV.pElevation[i];
			sGSVBuf[nNavSys].pAzimuth[i] = sGSV.pAzimuth[i];
			sGSVBuf[nNavSys].pfSatCnr[i] = sGSV.pfSatCnr[i];
		}
	}
	sGSVBuf[nNavSys].nTotalNum = sGSV.nTotalNum;
	sGSVBuf[nNavSys].nIndex = sGSV.nIndex;
	sGSVBuf[nNavSys].s32RcvdSatNum = sGSV.s32RcvdSatNum;

	if (sGSV.nIndex != sGSV.nTotalNum)
	{
		if (nNavSys == SYS_BDS)
		{
			g_bIsBDGSVAvl = FALSE;
		}
		else if (nNavSys == SYS_GPS)
		{
			g_bIsGPGSVAvl = FALSE;
		}
		else if (nNavSys == SYS_GLONASS)
		{
			g_bIsGLGSVAvl = FALSE;
		}
		return 0;
	}

	for (i = 0; i < MAX_PROC_SAT_NUM; i++)
	{
		if (sGSVBuf[nNavSys].pSatId[i] != 0)
		{
			sGSVBuf[nNavSys].pSatId[j] = sGSVBuf[nNavSys].pSatId[i];
			sGSVBuf[nNavSys].pElevation[j] = sGSVBuf[nNavSys].pElevation[i];
			sGSVBuf[nNavSys].pAzimuth[j] = sGSVBuf[nNavSys].pAzimuth[i];
			sGSVBuf[nNavSys].pfSatCnr[j] = sGSVBuf[nNavSys].pfSatCnr[i];
			j++;
		}
	}

	if (sGSVBuf[nNavSys].s32RcvdSatNum == j)
	{
		if (nNavSys == SYS_BDS)
		{
			g_bIsBDGSVAvl = TRUE;
		}
		else if (nNavSys == SYS_GPS)
		{
			g_bIsGPGSVAvl = TRUE;
		}
		else if (nNavSys == SYS_GLONASS)
		{
			g_bIsGLGSVAvl = TRUE;
		}
		return  1;
	}

	if (nNavSys == SYS_BDS)
	{
		g_bIsBDGSVAvl = FALSE;
	}
	else if (nNavSys == SYS_GPS)
	{
		g_bIsGPGSVAvl = FALSE;
	}
	else if (nNavSys == SYS_GLONASS)
	{
		g_bIsGLGSVAvl = FALSE;
	}

	return 1;
}

S32 CGSVList::GSVStmtLog(U8* u8GSVStatement)
{
	CString str, strGSV;
	S32 nLength = 0;

	if (u8GSVStatement == NULL)
	{
		return 0;
	}
	 
	strGSV = u8GSVStatement;
	nLength = m_editGSVData.GetWindowTextLength();
	m_editGSVData.SetSel(nLength, nLength);
	m_editGSVData.ReplaceSel(strGSV);
	m_editGSVData.ScrollWindow(0, 0);

	return 1;
}

S32 CGSVList::GSVInfoShow(S32 NavSys, BOOL bGSVOk, SGSVFrm* sGSVBuf, SCNOBuf sCNOBuf)
{
	S32 i = 0;
	S32 s32RcvNavSum = 0;
	CString strAvlSatNum = _T("");
	CString strGSVParam = _T("");
	
	if (!bGSVOk)
	{
		return 0;
	}

	if (g_bIsBDGSVAvl)
	{
		s32RcvNavSum += sGSVBuf[0].s32RcvdSatNum;
	}
	if (g_bIsGPGSVAvl)
	{
		s32RcvNavSum += sGSVBuf[1].s32RcvdSatNum;
	}
	if (g_bIsGLGSVAvl)
	{
		s32RcvNavSum += sGSVBuf[2].s32RcvdSatNum;
	}

	m_GSVListCtrl.SetRedraw(FALSE);
	/*想有几行加几行，少几行删几行*/
	for (i = m_nListLine; i < s32RcvNavSum; i++)
	{
		m_GSVListCtrl.InsertItem(i, LVCFMT_LEFT, 100);
	}

	/*	得到信噪比均值方差等 */
	CalcCnr(sGSVBuf[0].pfSatCnr, (U32)sGSVBuf[0].s32RcvdSatNum, g_bIsBDGSVAvl, &m_sCnrBuf[0]);
	CalcCnr(sGSVBuf[1].pfSatCnr, (U32)sGSVBuf[1].s32RcvdSatNum, g_bIsGPGSVAvl, &m_sCnrBuf[1]);
	CalcCnr(sGSVBuf[2].pfSatCnr, (U32)sGSVBuf[2].s32RcvdSatNum, g_bIsGLGSVAvl, &m_sCnrBuf[2]);

	sGSVBuf[0].s32AvlSatNum = 0;
	sGSVBuf[1].s32AvlSatNum = 0;
	sGSVBuf[2].s32AvlSatNum = 0;
	m_nListLine = 0;
	for (i = 0; i < sGSVBuf[0].s32RcvdSatNum; i++)
	{
		if ((sGSVBuf[0].pElevation[i] > 0.0)
			&& (sGSVBuf[0].pElevation[i] < 360.0)
			&& (sGSVBuf[0].pAzimuth[i] > 0.0)
			&& (sGSVBuf[0].pAzimuth[i] < 360.0))
		{
			m_GSVListCtrl.SetItemText(m_nListLine, 0, _T("BDS"));

			strGSVParam.Format(_T("%u"), sGSVBuf[0].pSatId[i]);
			m_GSVListCtrl.SetItemText(m_nListLine, 1, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%.3f"), sGSVBuf[0].pElevation[i]);		//卫星高度角
			m_GSVListCtrl.SetItemText(m_nListLine, 2, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%.3f"), sGSVBuf[0].pAzimuth[i]);       //卫星方位角
			m_GSVListCtrl.SetItemText(m_nListLine, 3, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%.1f"), sGSVBuf[0].pfSatCnr[i]);           //载噪比
			m_GSVListCtrl.SetItemText(m_nListLine, 4, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%lf"), m_sCnrBuf[0].pf64CnrAvg[i]);    //载噪比均值
			m_GSVListCtrl.SetItemText(m_nListLine, 5, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%lf"), m_sCnrBuf[0].pf64CnrStdErr[i]); //载噪比标准差
			m_GSVListCtrl.SetItemText(m_nListLine, 6, (LPCTSTR)strGSVParam);

			m_nListLine++;
			sGSVBuf[0].s32AvlSatNum++;
			sGSVBuf[0].pnAvlSatId[i] = sGSVBuf[0].pSatId[i];
		}
	}
	for (i = 0; i < sGSVBuf[1].s32RcvdSatNum; i++)
	{
		if ((sGSVBuf[1].pElevation[i] > 0.0)
			&& (sGSVBuf[1].pElevation[i] < 360.0)
			&& (sGSVBuf[1].pAzimuth[i] > 0.0)
			&& (sGSVBuf[1].pAzimuth[i] < 360.0))
		{
			m_GSVListCtrl.SetItemText(m_nListLine, 0, _T("GPS"));

			strGSVParam.Format(_T("%u"), sGSVBuf[1].pSatId[i]);
			m_GSVListCtrl.SetItemText(m_nListLine, 1, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%.3f"), sGSVBuf[1].pElevation[i]);		//卫星高度角
			m_GSVListCtrl.SetItemText(m_nListLine, 2, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%.3f"), sGSVBuf[1].pAzimuth[i]);       //卫星方位角
			m_GSVListCtrl.SetItemText(m_nListLine, 3, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%.1f"), sGSVBuf[1].pfSatCnr[i]);           //载噪比
			m_GSVListCtrl.SetItemText(m_nListLine, 4, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%lf"), m_sCnrBuf[0].pf64CnrAvg[i]);    //载噪比均值
			m_GSVListCtrl.SetItemText(m_nListLine, 5, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%lf"), m_sCnrBuf[0].pf64CnrStdErr[i]); //载噪比标准差
			m_GSVListCtrl.SetItemText(m_nListLine, 6, (LPCTSTR)strGSVParam);

			m_nListLine++;
			sGSVBuf[1].s32AvlSatNum++;
			sGSVBuf[1].pnAvlSatId[i] = sGSVBuf[1].pSatId[i];
		}
	}
	for (i = 0; i < sGSVBuf[2].s32RcvdSatNum; i++)
	{
		if ((sGSVBuf[2].pElevation[i] > 0.0)
			&& (sGSVBuf[2].pElevation[i] < 360.0)
			&& (sGSVBuf[2].pAzimuth[i] > 0.0)
			&& (sGSVBuf[2].pAzimuth[i] < 360.0))
		{
			m_GSVListCtrl.SetItemText(m_nListLine, 0, _T("GPS"));

			strGSVParam.Format(_T("%u"), sGSVBuf[2].pSatId[i]);
			m_GSVListCtrl.SetItemText(m_nListLine, 1, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%.3f"), sGSVBuf[2].pElevation[i]);		//卫星高度角
			m_GSVListCtrl.SetItemText(m_nListLine, 2, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%.3f"), sGSVBuf[2].pAzimuth[i]);       //卫星方位角
			m_GSVListCtrl.SetItemText(m_nListLine, 3, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%.1f"), sGSVBuf[2].pfSatCnr[i]);           //载噪比
			m_GSVListCtrl.SetItemText(m_nListLine, 4, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%lf"), m_sCnrBuf[0].pf64CnrAvg[i]);    //载噪比均值
			m_GSVListCtrl.SetItemText(m_nListLine, 5, (LPCTSTR)strGSVParam);

			strGSVParam.Format(_T("%lf"), m_sCnrBuf[0].pf64CnrStdErr[i]); //载噪比标准差
			m_GSVListCtrl.SetItemText(m_nListLine, 6, (LPCTSTR)strGSVParam);

			m_nListLine++;
			sGSVBuf[2].s32AvlSatNum++;
			sGSVBuf[2].pnAvlSatId[i] = sGSVBuf[2].pSatId[i];
		}
	}
	for (i = m_nListLine; i <= s32RcvNavSum; i++)
	{
		m_GSVListCtrl.DeleteItem(i);
	}
	m_GSVListCtrl.SetRedraw(TRUE);

	g_pSatSkyMap->DrawSatMap();
	/*更新所有频点的载噪比*/
	for (i = 0;i<MAX_FRQ_NUM;i++)
	{
		if (sCNOBuf.peFrqId[i] != 0)
		{
			g_pSatCnrDlg->DrawRcvdCNR(sGSVBuf, sCNOBuf.peFrqId[i]);
		}
	}

	/*把不可见星的载噪比置零，防止排序后选中不可见星*/
	for (i = 0; i < sGSVBuf[0].s32RcvdSatNum; i++)
	{
		if (sGSVBuf[0].pnAvlSatId == 0)
		{
			sGSVBuf[0].pfSatCnr[i] = 0;
		}
	}
	/*排序，为了后面选出载噪比前五的可见星*/
	Sort(sGSVBuf[0].pSatId, sGSVBuf[0].pfSatCnr, sGSVBuf[0].s32RcvdSatNum);

	return 1;
}

void CGSVList::Sort(U32 uSatId[], F64 fCNR[], S32 n)
{
	S32 i, j, nTemp;
	F64 fTemp;

	for (i = 1; i < n; i++)
	{
		fTemp = fCNR[i];
		nTemp = uSatId[i];
		j = i - 1;
		// 将fCNR[i]插入到已排序序列fCNR[0...i-1]中
		while (j >= 0 && fCNR[j] > fTemp)
		{
			fCNR[j + 1] = fCNR[j]; // 向后移动元素，为key腾出空间
			uSatId[j + 1] = uSatId[j]; // 向后移动元素，为key腾出空间
			j = j - 1;
		}
		fCNR[j + 1] = fTemp; // 插入元素到正确位置
		uSatId[j + 1] = nTemp; // 插入元素到正确位置
	}
}

void CGSVList::OnSize(UINT nType, int cx, int cy)
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


void CGSVList::ReSize(CRect recta)
{
	if (m_bListCreated)
	{
		m_GSVListCtrl.SetRowHeigt(recta.Height() / 10);
		m_GSVListCtrl.SetColumnWidth(0, recta.Width() / 5);
		m_GSVListCtrl.SetColumnWidth(1, recta.Width() / 5);
		m_GSVListCtrl.SetColumnWidth(2, recta.Width() / 5);
		m_GSVListCtrl.SetColumnWidth(3, (recta.Width() * 2) / 5);
	}
}


void CGSVList::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}


void CGSVList::OnSysCommand(UINT nID, LPARAM lParam)
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



int CGSVList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return 0;
}
