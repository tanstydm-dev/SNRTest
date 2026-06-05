// CErrCal.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "CErrCal.h"
#include "CHeadernStruct.h"
#include "afxdialogex.h"

// CErrCal 对话框
CErrCal* g_pErrCal = NULL;
IMPLEMENT_DYNAMIC(CErrCal, CDialogEx)

CErrCal::CErrCal(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ERRCAL, pParent)
	, m_GGAErrNum(0)
	, m_bSaveErr(FALSE)
	, m_bStdPosOk(FALSE)
{
	g_pErrCal = this;
}

CErrCal::~CErrCal()
{
}

void CErrCal::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CErrCal, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CErrCal::OnBnClickedButtonStartcalc)
	ON_BN_CLICKED(IDC_CHECK_SAVELOG, &CErrCal::OnBnClickedCheckSavelog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SETPOS, &CErrCal::OnBnClickedButtonSetpos)
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CErrCal 消息处理程序
BOOL CErrCal::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect recta;
	GetClientRect(&recta);     //取客户区大小  
	m_oldPoint.x = recta.right - recta.left;  //m_oldPoint是成员变量，用于保存上一次的界面尺寸
	m_oldPoint.y = recta.bottom - recta.top;

	// TODO:  在此添加额外的初始化
	memset(m_pStdPos, 0.0,3*sizeof(F64));
	memset(m_f64ElvErr, 0.0, 3 * sizeof(F64));
	memset(m_f64ElvErr, 0.0, 3 * sizeof(F64));

	GetDlgItem(IDC_EDIT_LATITUDE)->SetWindowTextW(_T("28.248918559"));
	GetDlgItem(IDC_EDIT_LONGITUDE)->SetWindowTextW(_T("113.026970246"));
	GetDlgItem(IDC_EDIT_ALTITUDE)->SetWindowTextW(_T(""));

	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CErrCal::CreateDynamicLayout()
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
	pMfcDynamicLayout->AddItem(IDC_EDIT_ELERR, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_EDIT_HORERR, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_ELERR, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_HORERR, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_CHECK_SAVELOG, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_EDIT_LATITUDE, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_LATITUDE, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_LONGITUDE, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_EDIT_LONGITUDE, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_EDIT_ALTITUDE, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_ALTITUDE, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_BUTTON_CALC, NoMove, NoSize);
	pMfcDynamicLayout->AddItem(IDC_STATIC_USERBLH, NoMove, Size_X_100);
	pMfcDynamicLayout->AddItem(IDC_STATIC_POSERROR, NoMove, Size_X_100);
	pMfcDynamicLayout->AddItem(IDC_STATIC_VELERR, NoMove, Size_X_100);
	pMfcDynamicLayout->AddItem(IDC_BUTTON_SETPOS, NoMove, NoSize);
}

void CErrCal::ShowBLHPos(SGGA sGGA)
{
	CString strBLHPos , strBLHDegree , strBLHMin , strDir = _T("");

	strBLHDegree.Format(_T("%d"), sGGA.pSPoint->nLatDegree);
	strBLHMin.Format(_T("%lf"), sGGA.pSPoint->nLatMin);
	strBLHMin.Replace(_T("."), _T(""));
	strDir.Format(_T("%c"), sGGA.pSPoint->sLatDir);
	strBLHPos = strBLHDegree + _T(".") + strBLHMin + strDir;
	GetDlgItem(IDC_EDIT_LATITUDE)->SetWindowTextW(strBLHPos);

	strBLHDegree.Format(_T("%d"), sGGA.pSPoint->nLonDegree);
	strBLHMin.Format(_T("%lf"), sGGA.pSPoint->nLonMin);
	strBLHMin.Replace(_T("."), _T(""));
	strDir.Format(_T("%c"), sGGA.pSPoint->sLonDir);
	strBLHPos = strBLHDegree + _T(".") + strBLHMin + strDir;
	GetDlgItem(IDC_EDIT_LONGITUDE)->SetWindowTextW(strBLHPos);

	strBLHPos.Format(_T("%lf"), sGGA.pSPoint->nHeight);
	GetDlgItem(IDC_EDIT_ALTITUDE)->SetWindowTextW(strBLHPos + _T(" m"));
}


void CErrCal::OnBnClickedButtonStartcalc()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strXPos, strYPos, strZPos;
	GetDlgItem(IDC_EDIT_LATITUDE)->GetWindowTextW(strXPos);
	GetDlgItem(IDC_EDIT_LONGITUDE)->GetWindowTextW(strYPos);
	GetDlgItem(IDC_EDIT_ALTITUDE)->GetWindowTextW(strZPos);
	if ((strXPos != _T(""))
		&& (strYPos != _T(""))
		&& (strZPos != _T("")))
	{
		m_pStdPos[0] = _ttof(strXPos);
		m_pStdPos[1] = _ttof(strYPos);
		m_pStdPos[2] = _ttof(strZPos);
		if ((fabs(m_pStdPos[0]) >= 90) || ((fabs(m_pStdPos[1]) >= 180)))
		{
			return;
		}
		m_bStdPosOk = TRUE;
	}
	else 
	{
		m_bStdPosOk = FALSE;
	}
}

void CErrCal::GetBLHErr(SGGA sGGA)
{
	if (m_bStdPosOk == FALSE)
	{
		return;
	}

	F64 pUsrBlh[3] = { 0 };

	pUsrBlh[0] = sGGA.pSPoint->nLatDegree + sGGA.pSPoint->nLatMin / NUMBER_OF_CLOCK;
	pUsrBlh[1] = sGGA.pSPoint->nLonDegree + sGGA.pSPoint->nLonMin / NUMBER_OF_CLOCK;
	pUsrBlh[2] = sGGA.pSPoint->nHeight;

	CString strHorErr, strElvErr;

	if (m_bStdPosOk == TRUE)
	{
		m_GGAErrNum = m_GGAErrNum + 1;
		CalcPosErr(m_pStdPos, pUsrBlh, m_f64ElvErr, m_f64HorErr, m_GGAErrNum);

		strHorErr.Format(_T("%lf"), m_f64HorErr[0]);
		SetDlgItemText(IDC_EDIT_HORERR, strHorErr);

		strElvErr.Format(_T("%lf"), m_f64ElvErr[0]);
		SetDlgItemText(IDC_EDIT_ELERR, strElvErr);
	}
	if (m_bSaveErr)
	{
		CString strWrite = _T("");
		strWrite = _T("ERROR ") + strHorErr + _T(" , ") + strElvErr + _T("\r\n");
		WriteErrData(strWrite);
	}
}

S32 CErrCal::WriteErrData(CString pCstr)
{
	S32 s32StrLen = 0;

	s32StrLen = pCstr.GetLength();
	U8 pErr[40] = {0};
	USES_CONVERSION;
	memcpy(pErr, W2A(pCstr), s32StrLen);
	if (m_bSaveErr == TRUE) //判断一下文件是否创建
	{
		m_ErrFile.SeekToEnd();
		m_ErrFile.Write(pErr, s32StrLen);
		return 1;
	}
	else
	{
		return 0;
	}
}

void CErrCal::OnBnClickedCheckSavelog()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	S32 state = 0;

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_SAVELOG);
	state = pBtn->GetCheck();

	/*创建文件后对象被占用，需要释放*/
	if (state != 1)//复选框没勾上
	{
		m_bSaveErr = FALSE;
		m_ErrFile.Close();
		return;
	}
	else
	{
		TCHAR path[MAX_PATH];

		GetCurrentDirectory(MAX_PATH, path);
		CString csFullPath(path);
		csFullPath += _T("/ErrInfo");
		if (FALSE == CreateDirectory(csFullPath, NULL))
		{
			if (ERROR_ALREADY_EXISTS == GetLastError())
			{
				TRACE("\n this path already exist");
			}
			else
			{
				TRACE("\n this path is invalid");
			}
		}
		/*每次点击勾上，则重新创建文件进行写入*/
		CString strTime = _T("");
		CTime time = CTime::GetCurrentTime();
		strTime.Format(csFullPath + _T("/aReplayData_%02d%02d%02d_%02d%02d%02d.dat"),
			time.GetYear(), time.GetMonth(), time.GetDay(),
			time.GetHour(), time.GetMinute(), time.GetSecond());
		BOOL bOpenOK = m_ErrFile.Open(strTime, CFile::modeWrite | CFile::modeCreate);
		if (bOpenOK == FALSE)
		{
			MessageBox(_T("创建文件失败"));
			m_bSaveErr = FALSE;
			pBtn->SetCheck(0);
		}
		else
		{
			m_bSaveErr = TRUE;
		}
	}
}

/*	控件自适应 */
void CErrCal::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (nType != SIZE_MINIMIZED)
	{
		ReSize();
	}
	// TODO: 在此处添加消息处理程序代码
}

void CErrCal::ReSize()
{
	float fsp[2];
	POINT Newp; //现在对话框的大小
	CRect recta;
	GetClientRect(&recta);     //取客户区大小  
	Newp.x = recta.right - recta.left;
	Newp.y = recta.bottom - recta.top;
	fsp[0] = (float)Newp.x / m_oldPoint.x;
	fsp[1] = (float)Newp.y / m_oldPoint.y;
	m_oldPoint = Newp;
}


void CErrCal::OnBnClickedButtonSetpos()
{
	// TODO: 在此添加控件通知处理程序代码
	SUserPnt* sUserPnt = new SUserPnt;
	CString strUserLat,strUserLon = _T("");

	GetDlgItem(IDC_EDIT_LATITUDE)->GetWindowTextW(strUserLat);
	GetDlgItem(IDC_EDIT_LONGITUDE)->GetWindowTextW(strUserLon);

	sUserPnt->f64UserLat = _ttof(strUserLat);
	sUserPnt->f64UserLon = _ttof(strUserLon);
// 	g_pMap->SendMessage(WM_MYMESSAGE_SETUSERPOINT, WPARAM(TRUE), (LPARAM)sUserPnt);
	delete sUserPnt;
}


void CErrCal::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}

/*禁止用户主动拖动窗口,经调试，SC_SIZE~SC_MOVE的nID是用户主动改变尺寸的消息*/
void CErrCal::OnSysCommand(UINT nID, LPARAM lParam)
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
