// CSatCNRDlg.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "CSatCNRDlg.h"
#include "afxdialogex.h"

CSatCNRDlg* g_pSatCnrDlg = NULL;

// CSatCNRDlg 对话框

IMPLEMENT_DYNAMIC(CSatCNRDlg, CDialogEx)

CSatCNRDlg::CSatCNRDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CNR, pParent)
{
	g_pSatCnrDlg = this;
}

CSatCNRDlg::~CSatCNRDlg()
{
}

void CSatCNRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSatCNRDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_SYSCOMMAND()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CNR, &CSatCNRDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()

// CSatCNRDlg 消息处理程序

void CSatCNRDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	CRect rectPic;		//客户区域

	GetClientRect(&rectPic);
	dc.FillSolidRect(rectPic, RGB(255, 255, 255));
}

void CSatCNRDlg::DrawRcvdCNR(SGSVFrm* sGSVBuf, U32 uFrqIdx)
{
	CDC* pDC;
	CDC pDcMem;				//用于缓冲作图的内存DC,BD->GP->GL
	CBitmap pBmp;			//内存中承载临时图象的位图,BD->GP->GL
	CRect pRectPic;			//绘图区域,BD->GP->GL

	/*同步tab页面与频点*/
	uFrqIdx = uFrqIdx - eFrqB1I;
	m_pnFrq[uFrqIdx] = uFrqIdx + eFrqB1I;
	if (m_pnFrq[uFrqIdx] != 0)
	{
		m_tabCtrl.HighlightItem(uFrqIdx);
	}

	pDC = m_pDrawCtrl[uFrqIdx].GetDC();
	m_pDrawCtrl[uFrqIdx].GetClientRect(pRectPic);
	pDcMem.CreateCompatibleDC(pDC);					//创建与目标DC相兼容的内存DC，
	pBmp.CreateCompatibleBitmap(pDC, pRectPic.Width(), pRectPic.Height());//创建兼容位图
	pDcMem.SelectObject(&pBmp);						//将位图选择进内存DC
	pDcMem.FillSolidRect(0, 0, pRectPic.Width(), pRectPic.Height(), RGB_BACK);
	/*根据缓存的CNO信息绘制各频点卫星的载噪比柱状图*/
	UpdateCNR(&pDcMem, pRectPic, sGSVBuf);

	pDC->BitBlt(0, 0, pRectPic.Width(), pRectPic.Height(), &pDcMem, 0, 0, SRCCOPY);//将内存DC上的图象拷贝到前台
	pDcMem.DeleteDC();					//删除DC
	pBmp.DeleteObject();					//删除位图
	ReleaseDC(pDC);
}

BOOL CSatCNRDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	S32 i;
	CRect rect;
	CRect rcTabCtrl;

	m_nCurSelTab = 0;
	memset(m_pnFrq, 0, sizeof(m_pnFrq));
	memset(m_pnTab, 0, sizeof(m_pnTab));

	GetClientRect(&rect);     //取客户区大小
	m_tabCtrl.Create(TCS_TABS | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect, this, IDC_TAB_CNR);
	m_tabCtrl.InsertItem(0, _T("B1I"));
	m_tabCtrl.InsertItem(1, _T("B1C"));
	m_tabCtrl.InsertItem(2, _T("B2a"));
	m_tabCtrl.InsertItem(3, _T("B3I"));
	m_tabCtrl.InsertItem(4, _T("L1"));
	m_tabCtrl.InsertItem(5, _T("G1"));

	m_pnTab[0] = eFrqB1I;
	m_pnTab[1] = eFrqB1C;
	m_pnTab[2] = eFrqB2a;
	m_pnTab[3] = eFrqB3I;
	m_pnTab[4] = eFrqL1;
	m_pnTab[5] = eFrqG1;

	m_tabCtrl.GetClientRect(rcTabCtrl);
	m_tabCtrl.GetItemRect(1, rect);
	rcTabCtrl.top += rect.Height();
	m_pDrawCtrl[0].Create(NULL, WS_CHILD | WS_VISIBLE, rcTabCtrl, &m_tabCtrl, IDC_STATIC_FRQ_B1I);
	m_pDrawCtrl[1].Create(NULL, WS_CHILD | WS_VISIBLE, rcTabCtrl, &m_tabCtrl, IDC_STATIC_FRQ_B1C);
	m_pDrawCtrl[2].Create(NULL, WS_CHILD | WS_VISIBLE, rcTabCtrl, &m_tabCtrl, IDC_STATIC_FRQ_B2A);
	m_pDrawCtrl[3].Create(NULL, WS_CHILD | WS_VISIBLE, rcTabCtrl, &m_tabCtrl, IDC_STATIC_FRQ_B3I);
	m_pDrawCtrl[4].Create(NULL, WS_CHILD | WS_VISIBLE, rcTabCtrl, &m_tabCtrl, IDC_STATIC_FRQ_L1);
	m_pDrawCtrl[5].Create(NULL, WS_CHILD | WS_VISIBLE, rcTabCtrl, &m_tabCtrl, IDC_STATIC_FRQ_G1);
	for (i = 0; (i < MAX_FRQ_NUM - 1); i++) m_pDrawCtrl[i].ShowWindow(FALSE);

	m_Brush.CreateSolidBrush(RGB(255, 255, 255));
	m_bhHCnr.CreateSolidBrush(RGB_HIGHCNR);
	m_bhLCnr.CreateSolidBrush(RGB_LOWCNR);
	m_brushBK1.CreateSolidBrush(RGB_BACK);
	m_brushBK2.CreateSolidBrush(RGB_BACK);
	m_brushBK3.CreateSolidBrush(RGB_BACK);

	m_BkBrush.CreateSolidBrush(RGB_BACK);
	m_SatID.CreateFontW(-9, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, _T("微软雅黑"));
	m_CNRID.CreateFontW(-9, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, _T("微软雅黑"));
	m_nFontWidth = 8;
	m_FreqType = 0;

	/*创建动态布局*/
	CreateDynamicLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CSatCNRDlg::CreateDynamicLayout()
{
	this->EnableDynamicLayout();
	m_tabCtrl.EnableDynamicLayout();
	CMFCDynamicLayout::MoveSettings noMove;
	CMFCDynamicLayout::SizeSettings NoSize;
	CMFCDynamicLayout::SizeSettings Size_Both;

	Size_Both.m_nXRatio = 100;
	Size_Both.m_nYRatio = 100;
	// 获取Dialog的动态布局指针。
	CMFCDynamicLayout* pMfcDynamicLayout = this->GetDynamicLayout();
	CMFCDynamicLayout* pMfcDynamicLayout_tab = m_tabCtrl.GetDynamicLayout();
	// 创建动态布局。
	pMfcDynamicLayout->Create(this);
	pMfcDynamicLayout_tab->Create(&m_tabCtrl);
	// 添加要在动态布局中注册的控件。
	pMfcDynamicLayout->AddItem(IDC_TAB_CNR, noMove, Size_Both);
	pMfcDynamicLayout_tab->AddItem(IDC_STATIC_FRQ_B1I, noMove, Size_Both);
	pMfcDynamicLayout_tab->AddItem(IDC_STATIC_FRQ_B1C, noMove, Size_Both);
	pMfcDynamicLayout_tab->AddItem(IDC_STATIC_FRQ_B2A, noMove, Size_Both);
	pMfcDynamicLayout_tab->AddItem(IDC_STATIC_FRQ_B3I, noMove, Size_Both);
	pMfcDynamicLayout_tab->AddItem(IDC_STATIC_FRQ_L1, noMove, Size_Both);
	pMfcDynamicLayout_tab->AddItem(IDC_STATIC_FRQ_G1, noMove, Size_Both);
}

void CSatCNRDlg::UpdateCNR(CDC* pDC, CRect rect, SGSVFrm* sGSVBuf)
{
	S32 nCount = 0;
	F64 fCNRBar = 0;
	F64 fBarWidth = 0.0;				/*载噪比条的宽度*/
	F64 fBarBeginPos_X = 0.0;			/*第一个载噪比条的横坐标z = picWidth / 10.0*/
	F64 fBarPos_X1 = 0.0;				/*载噪比条左上角点的横坐标*/
	F64 fBarPos_Y1 = 0.0;				/*载噪比条左上角点的纵坐标*/
	F64 fBarPos_X2 = 0.0;				/*载噪比条右下角点的横坐标*/
	F64 fBarPos_Y2 = 0.0;				/*载噪比条右下角点的纵坐标*/
	F64 fGapWidth = 0.0;				/*载噪比条之间间隔的宽度y = x / 10*/
	S32 nSatIndex, nSatNum = 0; 
	S32 i;
	CString strCNRParam = _T("");

	for (i = 0; i < 4; i++)
	{
		if (sGSVBuf[i].s32RcvdSatNum > 0)
		{
			nSatNum = sGSVBuf[i].s32RcvdSatNum;
			pDC->SelectObject(m_CNRID);
			pDC->SetBkMode(TRANSPARENT);
			/*先计算一下条形的尺寸*/
			if (nSatNum > 20)
			{
				fBarWidth = (rect.Width() * 12) / (F64)(11 * nSatNum + 1);
			}
			else
			{
				fBarWidth = (rect.Width() / 18.0);
			}
			fBarBeginPos_X = rect.Width() / 10.0;
			fGapWidth = fBarWidth / 10.0;
			nCount = 0;
			for (nSatIndex = 0; nSatIndex < nSatNum; nSatIndex++)
			{
				/*对于满足条件的载噪比，使用不同颜色的画笔*/
				if (sGSVBuf[i].pnAvlSatId[nSatIndex] > 0)
				{
					pDC->SelectObject(&m_bhHCnr);
				}
				else
				{
					pDC->SelectObject(&m_bhLCnr);
				}
				pDC->SetBkMode(TRANSPARENT);
				/*设置载噪比条位置大小*/
				fCNRBar = (F64)(rect.Height() * 5) / 7.0;
				fBarPos_X1 = fBarBeginPos_X + nCount * (fBarWidth + fGapWidth);
				fBarPos_Y1 = (rect.Height() * 6) / 7.0 - sGSVBuf[i].pfSatCnr[nSatIndex] * fCNRBar / 50 - m_nFontWidth;
				fBarPos_X2 = fBarPos_X1 + fBarWidth;
				fBarPos_Y2 = (rect.Height() * 6) / 7.0;
				pDC->Rectangle(fBarPos_X1, fBarPos_Y1, fBarPos_X2, fBarPos_Y2);
				strCNRParam.Format(_T("%d"), sGSVBuf[i].pSatId[nSatIndex]);
				pDC->ExtTextOutW(fBarPos_X1, fBarPos_Y2, NULL, rect, strCNRParam, NULL);
				strCNRParam.Format(_T("%.1f"), sGSVBuf[i].pfSatCnr[nSatIndex]);
				pDC->ExtTextOutW(fBarPos_X1, fBarPos_Y1 - 2 * m_nFontWidth, NULL, rect, strCNRParam, NULL);
				nCount++;
			}
		}
	}
}


void CSatCNRDlg::OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int mSel = m_tabCtrl.GetCurSel() + 1;

	if (mSel == eFrqB1I)
	{
		m_pDrawCtrl[0].ShowWindow(TRUE);
		m_pDrawCtrl[1].ShowWindow(FALSE);
		m_pDrawCtrl[2].ShowWindow(FALSE);
		m_pDrawCtrl[3].ShowWindow(FALSE);
		m_pDrawCtrl[4].ShowWindow(FALSE);
		m_pDrawCtrl[5].ShowWindow(FALSE);
	}
	if (mSel == eFrqB1C)
	{
		m_pDrawCtrl[0].ShowWindow(FALSE);
		m_pDrawCtrl[1].ShowWindow(TRUE);
		m_pDrawCtrl[2].ShowWindow(FALSE);
		m_pDrawCtrl[3].ShowWindow(FALSE);
		m_pDrawCtrl[4].ShowWindow(FALSE);
		m_pDrawCtrl[5].ShowWindow(FALSE);
	}
	if (mSel == eFrqB2a)
	{
		m_pDrawCtrl[0].ShowWindow(FALSE);
		m_pDrawCtrl[1].ShowWindow(FALSE);
		m_pDrawCtrl[2].ShowWindow(TRUE);
		m_pDrawCtrl[3].ShowWindow(FALSE);
		m_pDrawCtrl[4].ShowWindow(FALSE);
		m_pDrawCtrl[5].ShowWindow(FALSE);
	}
	if (mSel == eFrqB3I)
	{
		m_pDrawCtrl[0].ShowWindow(FALSE);
		m_pDrawCtrl[1].ShowWindow(FALSE);
		m_pDrawCtrl[2].ShowWindow(FALSE);
		m_pDrawCtrl[3].ShowWindow(TRUE);
		m_pDrawCtrl[4].ShowWindow(FALSE);
		m_pDrawCtrl[5].ShowWindow(FALSE);
	}
	if (mSel == eFrqL1)
	{
		m_pDrawCtrl[0].ShowWindow(FALSE);
		m_pDrawCtrl[1].ShowWindow(FALSE);
		m_pDrawCtrl[2].ShowWindow(FALSE);
		m_pDrawCtrl[3].ShowWindow(FALSE);
		m_pDrawCtrl[4].ShowWindow(TRUE);
		m_pDrawCtrl[5].ShowWindow(FALSE);
	}	
	if (mSel == eFrqG1)
	{
		m_pDrawCtrl[0].ShowWindow(FALSE);
		m_pDrawCtrl[1].ShowWindow(FALSE);
		m_pDrawCtrl[2].ShowWindow(FALSE);
		m_pDrawCtrl[3].ShowWindow(FALSE);
		m_pDrawCtrl[4].ShowWindow(FALSE);
		m_pDrawCtrl[5].ShowWindow(TRUE);
	}

	*pResult = 0;
}

void CSatCNRDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
