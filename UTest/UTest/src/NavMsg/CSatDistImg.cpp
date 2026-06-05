// CSatDistImg.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "CSatDistImg.h"
#include "afxdialogex.h"

#define TEXT_WIDTH		14
#define RGB_RED			RGB(220,20,60)
#define RGB_GREEN		RGB(34,139,34)
#define RGB_BLUE		RGB(0,0,139)
#define RGB_WHITE		RGB(255,255,255)
#define RGB_BLACKBKGRND	RGB(0,0,0)		/*  除坐标轴外的背景色(黑色) */
#define RGB_BACKGRND	RGB(240,240,240)		/*  除坐标轴外的背景色(灰色) */
#define RGB_AXIS		RGB(220,220,220)		/*	坐标轴的颜色 */
#define RGB_POLAR_BKGN	RGB(0,0,0)		/*	坐标系内填充的颜色 */

// CSatDistImg 对话框
CSatDistImg* g_pSatSkyMap = NULL;
IMPLEMENT_DYNAMIC(CSatDistImg, CDialogEx)

CSatDistImg::CSatDistImg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SATDIST, pParent)
{
	g_pSatSkyMap = this;
}

CSatDistImg::~CSatDistImg()
{
}

void CSatDistImg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSatDistImg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CSatDistImg 消息处理程序


BOOL CSatDistImg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//获取系统默认背景颜色

	m_bCanPaint = TRUE;
	// TODO:  在此添加额外的初始化
	m_hPolarBkBh.CreateSolidBrush(RGB_POLAR_BKGN);
	m_penDashed.CreatePen(PS_DASH, 1, RGB_WHITE);
	m_penWhite.CreatePen(PS_SOLID, 0, RGB_WHITE);
	m_penBlue.CreatePen(PS_SOLID, 0, RGB_BLUE);
	m_penRed.CreatePen(PS_SOLID, 0, RGB_RED);
	m_fontBlack.CreateFont(-12, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, _T("微软雅黑"));
	m_fontSatId.CreateFont(-12, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, _T("微软雅黑"));
	m_nDelta = 0;
	m_fRadius = 0;
	m_center = 0;

	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);
	
	/*默认开启BDS、GPS*/
	((CButton*)GetDlgItem(IDC_CHECK_BDSPOS))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_GPSPOS))->SetCheck(BST_CHECKED);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CSatDistImg::CreateDynamicLayout()
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
	pMfcDynamicLayout->AddItem(IDC_IMAGE_SATDIST, NoMove, Size_Both);
	pMfcDynamicLayout->AddItem(IDC_CHECK_BDSPOS, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_CHECK_GPSPOS, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_CHECK_GLPOS, Move_X_100, NoSize);
	pMfcDynamicLayout->AddItem(IDC_CHECK_GAPOS, Move_X_100, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_BDVISSAT, Move_Y_100, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_GPVISSAT, Move_Y_100, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_GLVISSAT, Move_Both, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_GAVISSAT, Move_Both, NoSize);
}
/*转换Alpha图层，获取透明png图片*/
void CSatDistImg::TransAlpha(CImage* pcImg)
{
	S32 i;
	S32 j;

	if (pcImg == NULL)
	{
		return;
	}
	if (pcImg->GetBPP() == 32) //确认该图像包含Alpha通道
	{
		for (i = 0; i < pcImg->GetWidth(); i++)
		{
			for (j = 0; j < pcImg->GetHeight(); j++)
			{
				byte* pByte = (byte*)pcImg->GetPixelAddress(i, j);
				pByte[0] = pByte[0] * pByte[3] / 255;
				pByte[1] = pByte[1] * pByte[3] / 255;
				pByte[2] = pByte[2] * pByte[3] / 255;
			}
		}
	}
}


void CSatDistImg::DrawCoordinate(CDC* pDC, CRect rectpic)
{
	F64 fRadiusTemp = 0.0;

	pDC->FillSolidRect(&rectpic, RGB_BLACKBKGRND);
	/*得到中心坐标和直径*/
	m_center.x = (long)(rectpic.Width() / 2);
	m_center.y = (long)(rectpic.Height() / 2);
	m_fRadius = min(m_center.x,m_center.y) - TEXT_WIDTH;
	m_nDelta = (rectpic.Width() - rectpic.Height()) / 2.0;

	/*画0,30,60仰角*/
	pDC->SelectObject(&m_penWhite);
	pDC->SelectObject(&m_hPolarBkBh);
	pDC->Ellipse((int)(m_center.x - (long)m_fRadius), m_center.y - (long)m_fRadius,
		(int)(m_center.x + (long)m_fRadius), m_center.y + (long)m_fRadius);//0

	pDC->Ellipse(m_center.x - (long)(m_fRadius * 0.8660254), m_center.y - (long)(m_fRadius * 0.8660254),
		m_center.x + (long)(m_fRadius * 0.8660254), m_center.y + (long)(m_fRadius * 0.8660254));//30

	pDC->Ellipse(m_center.x - (long)(m_fRadius / 2), m_center.y - (long)(m_fRadius / 2),
		m_center.x + (long)(m_fRadius / 2), m_center.y + (long)(m_fRadius / 2));//60

	pDC->SelectObject(&m_penDashed);
	pDC->SetBkMode(TRANSPARENT);
	//纵轴
	pDC->MoveTo(m_center.x, (m_center.y - (S32)m_fRadius));
	pDC->LineTo(m_center.x, (m_center.y + (S32)m_fRadius));
	//横轴
	pDC->MoveTo(m_center.x - m_fRadius, m_center.y);
	pDC->LineTo(m_center.x + m_fRadius, m_center.y);

	//图片上的文字
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB_WHITE);
	pDC->SelectObject(m_fontBlack);

	pDC->ExtTextOut(m_center.x - TEXT_WIDTH / 2, ((m_center.y - m_fRadius) - TEXT_WIDTH), NULL, rectpic, _T("N"), NULL);
	pDC->ExtTextOut(m_center.x - TEXT_WIDTH / 2, ((m_center.y + m_fRadius) - TEXT_WIDTH / 2), NULL, rectpic, _T("S"), NULL);
	pDC->ExtTextOut((m_center.x - m_fRadius - TEXT_WIDTH), m_center.y - TEXT_WIDTH / 2, NULL, rectpic, _T("W"), NULL);
	pDC->ExtTextOut((m_center.x + m_fRadius + TEXT_WIDTH / 2), m_center.y - TEXT_WIDTH / 2, NULL, rectpic, _T("E"), NULL);

	fRadiusTemp = m_fRadius * 0.5 * 0.70710678;
	pDC->ExtTextOut(m_center.x - TEXT_WIDTH / 2, m_center.y - TEXT_WIDTH / 2, NULL, rectpic, _T("90°"), NULL);
	pDC->ExtTextOut(m_center.x + (long)(fRadiusTemp), m_center.y - fRadiusTemp, NULL, rectpic, _T("60°"), NULL);
	fRadiusTemp = m_fRadius * 0.8660254 * 0.70710678;
	pDC->ExtTextOut(m_center.x + (long)(fRadiusTemp), m_center.y - fRadiusTemp, NULL, rectpic, _T("30°"), NULL);
	fRadiusTemp = m_fRadius * 0.70710678;
	pDC->ExtTextOut(m_center.x + (long)(fRadiusTemp), m_center.y - fRadiusTemp, NULL, rectpic, _T("0°"), NULL);
}


void CSatDistImg::DrawPos(CDC* pDC, CRect rectpic)
{
	S32 i = 0;
	S32 j = 0;
	CString strLctSatNum = _T("");
	CPoint cpSatPicPos(0, 0);
	CString strSatId;
	CImage ImageCN;
	CImage ImageUS;
	CImage ImageIND;
	CImage ImageEU;
	CImage ImageRUS;

	ImageCN.Load(_T("res\\CN.png"));
	ImageUS.Load(_T("res\\US.png"));
	ImageEU.Load(_T("res\\EU.png"));
	ImageIND.Load(_T("res\\IND.png"));
	ImageRUS.Load(_T("res\\RUS.png"));
	if ((ImageCN.IsNull()) || ((ImageUS.IsNull())) || ((ImageEU.IsNull())) || ((ImageIND.IsNull())))
	{
		MessageBox(_T("png加载失败，请检查图片资源路径！"));
		return;
	}

	TransAlpha(&ImageCN);
	TransAlpha(&ImageUS);
	TransAlpha(&ImageEU);
	TransAlpha(&ImageIND);

	pDC->SelectObject(&m_fontSatId);
	/*与PAINT消息同步*/
	m_bCanPaint = FALSE;
	/*更新可视卫星数*/
	strLctSatNum.Format(_T("BDS:%d/%d"), g_pDataPrco->g_sGSVBuf[0].s32AvlSatNum, g_pDataPrco->g_sGSVBuf[0].s32RcvdSatNum);
	SetDlgItemText(IDC_STATIC_BDVISSAT, strLctSatNum);
	strLctSatNum.Format(_T("GPS:%d/%d"), g_pDataPrco->g_sGSVBuf[1].s32AvlSatNum, g_pDataPrco->g_sGSVBuf[1].s32RcvdSatNum);
	SetDlgItemText(IDC_STATIC_GPVISSAT, strLctSatNum);
	strLctSatNum.Format(_T("GL:%d/%d"), g_pDataPrco->g_sGSVBuf[1].s32AvlSatNum, g_pDataPrco->g_sGSVBuf[1].s32RcvdSatNum);
	SetDlgItemText(IDC_STATIC_GLVISSAT, strLctSatNum);
	strLctSatNum.Format(_T("GA:%d/%d"), g_pDataPrco->g_sGSVBuf[1].s32AvlSatNum, g_pDataPrco->g_sGSVBuf[1].s32RcvdSatNum);
	/*绘制GLONASS卫星天空方位*/
	i = 2;
	if ((IsDlgButtonChecked(IDC_CHECK_GLPOS))
		&& (g_pDataPrco->g_sGSVBuf[i].s32AvlSatNum > 0))
	{
		/*设置画笔参数*/
		pDC->SelectObject(&m_penBlue);
		pDC->SetTextColor(RGB_WHITE);

		for (j = 0; j < g_pDataPrco->g_sGSVBuf[i].s32RcvdSatNum; j++)
		{
			if ((g_pDataPrco->g_sGSVBuf[i].pSatId[j] != 0)
				&& (g_pDataPrco->g_sGSVBuf[i].pElevation[j] > 0.0)
				&& (g_pDataPrco->g_sGSVBuf[i].pElevation[j] < 360.0)
				&& (g_pDataPrco->g_sGSVBuf[i].pAzimuth[j] > 0.0)
				&& (g_pDataPrco->g_sGSVBuf[i].pAzimuth[j] < 360.0))
			{
				/*标识坐标*/
				GetSvPosF64ToXY((S32)(g_pDataPrco->g_sGSVBuf[i].pElevation[j]), (S32)(g_pDataPrco->g_sGSVBuf[i].pAzimuth[j]), &cpSatPicPos, m_center, m_fRadius);
				/*图标样式*/
				ImageRUS.Draw(pDC->m_hDC, cpSatPicPos.x - m_fRadius / 14.0, cpSatPicPos.y - m_fRadius / 14.0, m_fRadius / 7.0, m_fRadius / 7.0);
				/*标上卫星号*/
				strSatId.Format(_T("G%u"), g_pDataPrco->g_sGSVBuf[i].pSatId[j]);
				pDC->ExtTextOut(cpSatPicPos.x - m_fRadius / 20.0, cpSatPicPos.y - m_fRadius / 30.0, NULL, rectpic, strSatId, NULL);
			}
		}
	}
	SetDlgItemText(IDC_STATIC_GAVISSAT, strLctSatNum);
	/*绘制GPS卫星天空方位*/
	i = 1;
	if (   (IsDlgButtonChecked(IDC_CHECK_GPSPOS		  ))
		&& (g_pDataPrco->g_sGSVBuf[i].s32AvlSatNum > 0))
	{
		/*设置画笔参数*/
		pDC->SelectObject(&m_penBlue);
		pDC->SetTextColor(RGB_BLUE);

		for (j = 0; j < g_pDataPrco->g_sGSVBuf[i].s32RcvdSatNum; j++)
		{
			if (   (g_pDataPrco->g_sGSVBuf[i].pSatId[j]     != 0	)
				&& (g_pDataPrco->g_sGSVBuf[i].pElevation[j] >  0.0	)
				&& (g_pDataPrco->g_sGSVBuf[i].pElevation[j] <  360.0)
				&& (g_pDataPrco->g_sGSVBuf[i].pAzimuth[j]   >  0.0	)
				&& (g_pDataPrco->g_sGSVBuf[i].pAzimuth[j]   <  360.0))
			{
				/*标识坐标*/
				GetSvPosF64ToXY((S32)(g_pDataPrco->g_sGSVBuf[i].pElevation[j]), (S32)(g_pDataPrco->g_sGSVBuf[i].pAzimuth[j]), &cpSatPicPos, m_center, m_fRadius);
				/*图标样式*/
				ImageUS.Draw(pDC->m_hDC, cpSatPicPos.x - m_fRadius / 14.0, cpSatPicPos.y - m_fRadius / 14.0, m_fRadius / 7.0, m_fRadius / 7.0);
				/*标上卫星号*/
				strSatId.Format(_T("G%u"), g_pDataPrco->g_sGSVBuf[i].pSatId[j]);
				pDC->ExtTextOut(cpSatPicPos.x - m_fRadius / 20.0, cpSatPicPos.y - m_fRadius / 30.0, NULL, rectpic, strSatId, NULL);
			}
		}
	}
	/*绘制BDS卫星天空方位*/
	i = 0;
	if (   (IsDlgButtonChecked(IDC_CHECK_BDSPOS		  ))
		&& (g_pDataPrco->g_sGSVBuf[i].s32AvlSatNum > 0))
	{
		/*设置画笔参数*/
		pDC->SelectObject(&m_penWhite);
		pDC->SetTextColor(RGB_WHITE);
		for (j = 0; j < g_pDataPrco->g_sGSVBuf[i].s32RcvdSatNum; j++)
		{
			if (   (g_pDataPrco->g_sGSVBuf[i].pSatId[j] != 0		) 
				&& (g_pDataPrco->g_sGSVBuf[i].pElevation[j] > 0.0	) 
				&& (g_pDataPrco->g_sGSVBuf[i].pElevation[j] < 360.0	) 
				&& (g_pDataPrco->g_sGSVBuf[i].pAzimuth[j] > 0.0		) 
				&& (g_pDataPrco->g_sGSVBuf[i].pAzimuth[j] < 360.0	))
			{
				/*标识坐标*/
				GetSvPosF64ToXY((S32)(g_pDataPrco->g_sGSVBuf[i].pElevation[j]), (S32)(g_pDataPrco->g_sGSVBuf[i].pAzimuth[j]), &cpSatPicPos, m_center, m_fRadius);
				/*图标样式*/
				ImageCN.Draw(pDC->m_hDC, cpSatPicPos.x - m_fRadius / 14.0, cpSatPicPos.y - m_fRadius / 14.0, m_fRadius / 7.0, m_fRadius / 7.0);
				/*标上卫星号*/
				strSatId.Format(_T("C%u"), g_pDataPrco->g_sGSVBuf[i].pSatId[j]);
				pDC->ExtTextOut(cpSatPicPos.x - m_fRadius / 20.0, cpSatPicPos.y - m_fRadius / 30.0, NULL, rectpic, strSatId, NULL);
			}
		}
	}
	m_bCanPaint = TRUE;

	ImageCN.Destroy();
	ImageUS.Destroy();
	ImageEU.Destroy();
	ImageIND.Destroy();
}

void CSatDistImg::DrawSatMap()
{
	CRect rect;				//绘图区域
	CDC dcMem;				//用于缓冲作图的内存DC
	CBitmap bmp;			//内存中承载临时图象的位图

	m_bCanPaint = FALSE;

	CWnd* pwnd = GetDlgItem(IDC_IMAGE_SATDIST);
	CDC* pDC = pwnd->GetDC();
	pwnd->GetClientRect(rect);
	dcMem.CreateCompatibleDC(pDC);					//创建与目标DC相兼容的内存DC，
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());//创建兼容位图
	dcMem.SelectObject(&bmp);						//将位图选择进内存DC
	DrawCoordinate(&dcMem, rect);
	DrawPos(&dcMem, rect);
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);//将内存DC上的图象拷贝到前台
	dcMem.DeleteDC();       //删除DC
	bmp.DeleteObject();     //删除位图
	ReleaseDC(pDC);
	m_bCanPaint = TRUE;
}

void CSatDistImg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	CRect rect;			//绘图区域

	if (m_bCanPaint == FALSE)
	{
		return;
	}

	CWnd* pwnd = GetDlgItem(IDC_IMAGE_SATDIST);
	CDC* pDC = pwnd->GetDC();

	pwnd->GetClientRect(rect);
	DrawCoordinate(pDC, rect);
	ReleaseDC(pDC);
}

void CSatDistImg::GetSvPosF64ToXY(int elevation, int azimuth, CPoint* pSvPos, const CPoint& center, double nRadius)
{
	double nRadius_Elv = nRadius * cos(elevation * PI / 180);
	pSvPos->x = (long)(center.x + nRadius_Elv * sin(azimuth * PI / 180));
	pSvPos->y = (long)(center.y - nRadius_Elv * cos(azimuth * PI / 180));
	return;
}

BOOL CSatDistImg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CDialogEx::OnEraseBkgnd(pDC);
}

void CSatDistImg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
}

void CSatDistImg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}

void CSatDistImg::OnSysCommand(UINT nID, LPARAM lParam)
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
