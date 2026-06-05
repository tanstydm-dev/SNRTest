// CDataProc.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "CDataProc.h"
#include "CPtlMsg.h"
#include "afxdialogex.h"
#include <windows.h>
#include "CSatCNRDlg.h"


BOOL g_bCanPrco;	// 全局变量控制线程
BOOL g_bCanReplay;
CDataProc* g_pDataPrco = NULL;
// CDataProc 对话框

IMPLEMENT_DYNAMIC(CDataProc, CDialogEx)

CDataProc::CDataProc(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DATARCV, pParent)
	, m_bSaveData(FALSE)
	, m_bShowHex(FALSE)
	, m_bStopShow(FALSE)
	, m_bReplaying(FALSE)
	, m_bOpenOK(FALSE)
	, m_bPortOpen(FALSE)
	, m_strPtl(_T(""))
	, g_mMutex(0, 0, 0)	

{
	g_pDataPrco = this;
	g_bCanPrco = FALSE;
	g_bCanReplay = FALSE;
}

CDataProc::~CDataProc()
{
}

void CDataProc::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_SAVEDATA, m_bSaveData);
	DDX_Check(pDX, IDC_CHECK_SHOWHEX, m_bShowHex);
	DDX_Control(pDX, IDC_EDIT_SHOWDATA, m_Edit);
	DDX_Control(pDX, IDC_BUTTON_BGNRPLY, m_btnRplyCtrl);
	DDX_Control(pDX, IDC_BUTTON_SELECTHEX, m_btnSelectHex);
	DDX_Control(pDX, IDC_BUTTON_SLCTDAT, m_btnSlctDat);
	DDX_Control(pDX, IDC_COMBO_COMMAND, m_ComBoxCmd);
	DDX_Control(pDX, IDC_COMBO_COM, m_PortId);
}


BEGIN_MESSAGE_MAP(CDataProc, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_SAVEDATA, &CDataProc::OnBnClickedCheckSavedata)
	ON_BN_CLICKED(IDC_BUTTON_STOPSHOW, &CDataProc::OnBnClickedButtonStopshow)
	ON_BN_CLICKED(IDC_BUTTON_SLCTDAT, &CDataProc::OnBnClickedButtonSlctdat)
	ON_BN_CLICKED(IDC_BUTTON_BGNRPLY, &CDataProc::OnBnClickedButtonBgnrply)
	ON_BN_CLICKED(IDC_BUTTON_CLEANEDIT, &CDataProc::OnBnClickedButtonCleanedit)
	ON_BN_CLICKED(IDC_CHECK_SHOWHEX, &CDataProc::OnBnClickedCheckShowhex)
	ON_BN_CLICKED(IDC_CHECK_SAVEHEX, &CDataProc::OnBnClickedCheckSavehex)
	ON_MESSAGE(WM_SERIALRXNOTIFY_DEFINE, OnSerialRxNotify)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SELECTHEX, &CDataProc::OnBnClickedButtonSelecthex)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CDataProc::OnBnClickedButtonSend)
	ON_WM_SYSCOMMAND()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BUTTON_OPENCOM, &CDataProc::OnBnClickedButtonOpencom)
END_MESSAGE_MAP()


// CDataProc 消息处理程序
void PtlPrcoThread();
void PlayBackDataThread();

BOOL CDataProc::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	/*变量初始化*/
	m_nFileLineCount = 0;
	m_byteFileLine = 0;
	m_strReadAll = _T("");
	m_bRcvFrq = FALSE;
	m_RestLine = 0;
	m_nPtlNum = 0;
	m_bRplyHexFile = FALSE;
	m_bRplyRowFile = FALSE;
	m_bCmdSended = FALSE;
	memset(g_sGSVBuf, 0.0, 4 * sizeof(SGSVFrm));

	/*串口配置*/
	m_uSelectedBoteRate = 115200;
	S32 ret = m_CComSet.InterfaceInit(&m_PortId, 0);
	if (ret == FALSE)
	{
		MessageBox(_T("串口识别失败！"));
	}
	m_PortId.SetCurSel(0);
	/*Combobox初始化*/
// 	m_ComBoxCmd.AddString(_T("$PNnnc,ASK,500,1*FC"));
// 	m_ComBoxCmd.AddString(_T("$PNnnc,ASK,500,2*FC"));
// 	m_ComBoxCmd.AddString(_T("$CCMSS,Z,1,B3I,,,,*FC"));
	m_ComBoxCmd.AddString(_T("$PNnnc,ASK,720,1*FC"));
	m_ComBoxCmd.AddString(_T("$CCRMO,GGA,2,1*FC"));
	m_ComBoxCmd.AddString(_T("$CCMSS,Z,1,B1C,,,*FC"));
// 	m_ComBoxCmd.AddString(_T("$CCMSS,Z,1,B2b,,,,*FC"));
// 	m_ComBoxCmd.AddString(_T("$CCMSS,Z,1,B1I,,,,*FC"));
	m_ComBoxCmd.AddString(_T("$CCMSS,Z,1,B2a,,,,*FC"));

// 	m_ComBoxCmd.AddString(_T("$CCMSS,Z,1,L1C,,,,*FC"));
// 	m_ComBoxCmd.AddString(_T("$CCMSS,Z,1,E1,,,,*FC"));
// 	m_ComBoxCmd.AddString(_T("$CCMSS,Z,1,L1CA,,,,*FC"));
	m_ComBoxCmd.AddString(_T("$PNASK,5,B1C,1,1,0.6,16*FC"));
	m_ComBoxCmd.AddString(_T("$PNASK,5,B1C,1,1,0.3,10*FC"));
	m_ComBoxCmd.AddString(_T("$PNASK,5,B2a,1,1,0.6,16*FC"));
	m_ComBoxCmd.AddString(_T("$PNASK,5,B2a,1,1,0.3,10*FC"));

// 	m_ComBoxCmd.AddString(_T("$PNASK,3,1,1000,1,2,3,4,5,0,1*FC"));
// 	m_ComBoxCmd.AddString(_T("$PNASK,3,1,1000,1,2,3,4,5,0,0*FC"));
// 	m_ComBoxCmd.AddString(_T("$PNASK,2,1*FC"));
// 	m_ComBoxCmd.AddString(_T("$PNASK,1,1*FC"));

	/*	设置edit无限显示 */
	m_Edit.SetLimitText(-1);

	m_btnRplyCtrl.EnableWindow(FALSE);
	/* 缓冲区初始化 */
	U8 pStdPcBdSrRecvHead[2] = { 0x24, 0x42};  
	U8 pStdPcGpsSrRecvHead[2] = { 0x24, 0x47 };
	U8 pStdPcSrRecvTail[2] = { 0x0D, 0x0A };

	FBufferCreate(&m_pRngBufPcSrRecv, MAX_BUFFER_SIZE * 1, m_BufPcSrRecv);
	FBufferExInit(&m_pRngBufPcSrRecv, pStdPcBdSrRecvHead, pStdPcSrRecvTail, 2, 2, 0, 0, 4, FALSE, FALSE, MAX_BUFFER_SIZE);
	FBufferExInit(&m_pRngBufPcSrRecv, pStdPcGpsSrRecvHead, pStdPcSrRecvTail, 2, 2, 0, 0, 4, FALSE, FALSE, MAX_BUFFER_SIZE);

	/*	创建线程 */
	m_hDataRcvThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)PtlPrcoThread,
		NULL,
		0,
		&m_dDataRcvThreadID);

	m_hDataReplayThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)PlayBackDataThread,
		NULL,
		0,
		&m_dDataReplayThreadID);

	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDataProc::CreateDynamicLayout()
{
	/*使能动态布局*/
	this->EnableDynamicLayout();
	CMFCDynamicLayout::MoveSettings noMove;
	CMFCDynamicLayout::MoveSettings moveSetting1;
	CMFCDynamicLayout::MoveSettings moveSetting2;
	CMFCDynamicLayout::SizeSettings noSize;
	CMFCDynamicLayout::SizeSettings sizeSetting1;
	CMFCDynamicLayout::SizeSettings sizeSetting2;
	/*设置move和size方式*/
	moveSetting1.m_nXRatio = 100;
	moveSetting1.m_nYRatio = 0;
	moveSetting2.m_nXRatio = 100 / 2;
	moveSetting2.m_nYRatio = 0;
	sizeSetting1.m_nXRatio = 100;
	sizeSetting1.m_nYRatio = 100;
	sizeSetting2.m_nXRatio = 100/2;
	sizeSetting2.m_nYRatio = 0;
	/*获取Dialog的动态布局指针*/
	CMFCDynamicLayout* pMfcDynamicLayout = this->GetDynamicLayout();
	/*创建动态布局*/
	pMfcDynamicLayout->Create(this);
	/*添加要在动态布局中注册的控件*/
	pMfcDynamicLayout->AddItem(IDC_EDIT_SHOWDATA, noMove, sizeSetting1);
	pMfcDynamicLayout->AddItem(IDC_BUTTON_CLEANEDIT, noMove, noSize);
	pMfcDynamicLayout->AddItem(IDC_BUTTON_STOPSHOW, noMove, noSize);
	pMfcDynamicLayout->AddItem(IDC_BUTTON_BGNRPLY, noMove, noSize);
	pMfcDynamicLayout->AddItem(IDC_CHECK_SAVEDATA, moveSetting1, noSize);
	pMfcDynamicLayout->AddItem(IDC_CHECK_SHOWHEX, moveSetting1, noSize);
	pMfcDynamicLayout->AddItem(IDC_CHECK_SAVEHEX, moveSetting1, noSize);
	pMfcDynamicLayout->AddItem(IDC_BUTTON_SLCTDAT, moveSetting2, sizeSetting2);
	pMfcDynamicLayout->AddItem(IDC_BUTTON_SELECTHEX, moveSetting2, sizeSetting2);
	pMfcDynamicLayout->AddItem(IDC_BUTTON_SEND, noMove, sizeSetting2);
	pMfcDynamicLayout->AddItem(IDC_COMBO_COMMAND, noMove, sizeSetting2);
	pMfcDynamicLayout->AddItem(IDC_BUTTON_OPENCOM, noMove, noSize);
	pMfcDynamicLayout->AddItem(IDC_COMBO_COM, noMove, noSize);
}

/*数据接收函数 */
LRESULT CDataProc::OnSerialRxNotify(WPARAM wParam, LPARAM lParam)
{
	U8 pData[GET_DATA_IN_OPEN_MAX];
	S32 nRcvBytesNum = m_CComSet.RXSpaceUsed();

	UpdateData(TRUE);
	if (nRcvBytesNum != 0)
	{
		m_CComSet.Read((U8*)pData, nRcvBytesNum);
		g_bCanPrco = FALSE;
		FBufferPushForce(&m_pRngBufPcSrRecv, pData, nRcvBytesNum);
		if (g_pDataPrco->m_bSaveData == TRUE)//写入原始0183
		{
			g_pDataPrco->WriteRawData(pData, nRcvBytesNum);
		}
		g_bCanPrco = TRUE;
	}
	UpdateData(FALSE);
	return 0;
}

/*	处理线程 */
void PtlPrcoThread()
{
	while (1)
	{
		if (g_bCanPrco)
		{	
			g_pDataPrco->g_mMutex.Lock();
			while (FBufferHasFrame(&g_pDataPrco->m_pRngBufPcSrRecv))
			{
				U8 pRawData[MAX_READ_LEN] = {0};
				S32 nByteLen = 0;
				S8_CHAR pRawHex[MAX_READ_LEN*2];
				S32 nHexLen = 0;

				/*取出一帧*/
				nByteLen = FBufferPop(&g_pDataPrco->m_pRngBufPcSrRecv, pRawData, MAX_READ_LEN);
				pRawData[nByteLen] = '\0';
				g_pDataPrco->PtlPro(pRawData, nByteLen);
				if (g_pDataPrco->m_bShowHex == TRUE)//转十六进制打印
				{
					nHexLen = ICEByte2HexStr((const U8*)pRawData, pRawHex, nByteLen);
					g_pDataPrco->LogInfo((const S8_CHAR*)pRawHex, LOG_HEX);
				}
				else if (g_pDataPrco->m_bShowHex == FALSE)//直接打印
				{
					g_pDataPrco->LogInfo((const S8_CHAR*)pRawData, LOG_RECEIVE_RAW);
				}
				if (g_pDataPrco->m_bSaveHex == TRUE)
				{
					nHexLen = ICEByte2HexStr((const U8*)pRawData, pRawHex, nByteLen);
					g_pDataPrco->WriteHex(pRawHex, nHexLen);
				}
			}
			g_bCanPrco = FALSE;
			g_pDataPrco->g_mMutex.Unlock();
		}
	}
	return;
}

/*	读线程 */
void PlayBackDataThread()
{
	while (1)
	{
		/*	原始数据测试 */
		if (g_bCanReplay == TRUE)
		{
			g_pDataPrco->g_mMutex.Lock();
			g_bCanPrco = FALSE;
			UINT readed;
			CString strNum = _T("");
			CString strRead = _T("");
			U8 buf[MAX_READ_LEN];
			U8 pU8ReadHex[MAX_READ_LEN + 1];
			U32 number = MAX_READ_LEN;
			S32 nLength;

			if (g_pDataPrco->m_bRplyHexFile == TRUE)
			{
				readed = g_pDataPrco->m_Read.Read(pU8ReadHex, MAX_READ_LEN - 2);//规定16进制数据格式，每次读510个字符，可以保证数据连续
				pU8ReadHex[readed] = '\0';
				strRead = (const S8_CHAR*)pU8ReadHex;
				nLength = HexCStr2Byte(buf, strRead, readed);
				FBufferPushForce(&g_pDataPrco->m_pRngBufPcSrRecv, buf, nLength);
				readed += 2;
			}
			else if ((readed = g_pDataPrco->m_Read.Read(buf, number)) > 0)
			{
				FBufferPushForce(&g_pDataPrco->m_pRngBufPcSrRecv, buf, readed);
				if (g_pDataPrco->m_bSaveData == TRUE)//写入原始0183
				{
					g_pDataPrco->WriteRawData(buf, readed);
				}
			}
			else
			{
				g_pDataPrco->m_Read.Close();
				g_pDataPrco->m_bOpenOK = FALSE;
				g_bCanReplay = FALSE;
			}
			if (readed == number)
			{
				//在EDIT上显示剩余要读取的行数
				g_pDataPrco->m_RestLine--;
				strNum.Format(_T("RestLine:%d"), g_pDataPrco->m_RestLine);
				g_pDataPrco->UpdateRestLine(strNum);
			}
			g_bCanPrco = TRUE;
			Sleep(30);
			g_pDataPrco->g_mMutex.Unlock();
		}
	}
	return;
}
/*打开串口，由cmainframe类调用*/
S32 CDataProc::Open(CString strPortId, U32 uBoteRate)
{
	BOOL bIsOpen = m_CComSet.IsOpen();
	if (bIsOpen)
	{
		m_CComSet.ClosePort();
		memset(m_BufPcSrRecv, 0, MAX_BUFFER_SIZE);
		FBufferClear(&m_pRngBufPcSrRecv, MAX_BUFFER_SIZE, m_BufPcSrRecv);
	}
	if ((strPortId == _T(""))||(uBoteRate == 0))
	{
		return 1;
	}

	bIsOpen = m_CComSet.OpenPort(uBoteRate, m_hWnd, strPortId);
	if (bIsOpen)
	{
		m_bPortOpen = TRUE;
		return 1;
	}
	else
	{
		return 0;
	}
}
/* 存储原始0183*/
BOOL CDataProc::WriteRawData(U8* pData, S32 byteLen)
{
	if (   (pData == NULL) 
		|| (byteLen == 0 ))
	{
		return FALSE;
	}
	if (m_bSaveData == TRUE)
	{
		m_SaveFile.Write(pData, byteLen);
	}

	return TRUE;
}
/* 存储16进制*/
BOOL CDataProc::WriteHex(S8_CHAR* pHex, S32 nbyteLen)
{
	if (   (pHex == NULL )
		|| (nbyteLen == 0))
	{
		return FALSE;
	}
	if (m_bSaveHex == TRUE)
	{
		m_SaveHex.Write(pHex, nbyteLen);
	}

	return TRUE;
}
/*打印*/
void CDataProc::LogInfo(const S8_CHAR* pStr,S32 nInfoSort)
{
	CString strPtl = _T("");
	S32 len = 0;

	if ((m_bStopShow == TRUE)&&(m_bRplyHexFile != FALSE)&&(m_bRplyRowFile != FALSE))
	{
		return;
	}
	strPtl = pStr;
	if (nInfoSort == LOG_HEX)
	{
		strPtl += _T("\r\n");
	}

	m_strPtl += strPtl;
	len = m_Edit.GetWindowTextLength();
	m_Edit.SetSel(len, len);
	m_Edit.ReplaceSel(m_strPtl);
	m_Edit.ScrollWindow(0, 0);
	m_strPtl = _T("");
}
/*生成原始数据文件对象*/
void CDataProc::OnBnClickedCheckSavedata()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	S32 state = 0;

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_SAVEDATA);
	state = pBtn->GetCheck();
	if (state != 1)/*复选框没勾上 */
	{
		m_bSaveData = FALSE;
		m_SaveFile.Close();/*创建文件后对象被占用，需要释放 */
		return;
	}
	else
	{
		TCHAR path[MAX_PATH];

		GetCurrentDirectory(MAX_PATH, path);
		CString csFullPath(path);
		csFullPath += _T("/SaveData");
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
		// 每次点击勾上，则重新创建文件进行写入
		CString strTime = _T("");
		CTime time = CTime::GetCurrentTime();
		strTime.Format(csFullPath + _T("/aReplayData_%02d%02d%02d_%02d%02d%02d.dat"),
			time.GetYear(), time.GetMonth(), time.GetDay(),
			time.GetHour(), time.GetMinute(), time.GetSecond());
		BOOL bOpenOK = m_SaveFile.Open(strTime, CFile::modeWrite | CFile::modeCreate);
		if (bOpenOK == FALSE)
		{
			MessageBox(_T("创建文件失败"));
			m_bSaveData = FALSE;
			pBtn->SetCheck(0);
		}
		else
		{
			m_bSaveData = TRUE;
		}
	}
}
/*生成16进制文件对象*/
void CDataProc::OnBnClickedCheckSavehex()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	S32 state = 0;

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_SAVEHEX);
	state = pBtn->GetCheck();
	if ((state != 1)
		&& (m_bSaveHex == TRUE))/***复选框没勾上 */
	{
		m_bSaveHex = FALSE;
		m_SaveHex.Close();/****创建文件后对象被占用，需要释放 */
		return;
	}
	else
	{
		TCHAR path[MAX_PATH];

		GetCurrentDirectory(MAX_PATH, path);
		CString csFullPath(path);
		csFullPath += _T("/SaveHex");
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
		// 每次点击勾上，则重新创建文件进行写入
		CString strTime = _T("");
		CTime time = CTime::GetCurrentTime();
		strTime.Format(csFullPath + _T("/aReplayHex_%02d%02d%02d_%02d%02d%02d.dat"),
			time.GetYear(), time.GetMonth(), time.GetDay(),
			time.GetHour(), time.GetMinute(), time.GetSecond());
		BOOL bOpenOK = m_SaveHex.Open(strTime, CFile::modeWrite | CFile::modeCreate);
		if (bOpenOK == FALSE)
		{
			MessageBox(_T("创建文件失败"));
			m_bSaveHex = FALSE;
			pBtn->SetCheck(0);
		}
		else
		{
			m_bSaveHex = TRUE;
		}
	}
}
/*暂停显示（不停止接收）*/
void CDataProc::OnBnClickedButtonStopshow()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strButtonStop;
	GetDlgItemText(IDC_BUTTON_STOPSHOW, strButtonStop);
	if (strButtonStop == _T("暂停"))
	{
		m_bStopShow = TRUE;
		SetDlgItemText(IDC_BUTTON_STOPSHOW, _T("继续"));
	}
	else if (strButtonStop == _T("继续"))
	{
		m_bStopShow = FALSE;
		SetDlgItemText(IDC_BUTTON_STOPSHOW, _T("暂停"));
	}
}
/*解协议*/
void CDataProc::PtlPro(U8* pData, S32 nbyteLen)
{
	SGGA aGGA = { 0 };
	STime aSTime = { 0 };       	//UTC时间
	SSrOutPoint aSPoint = { 0 };	//定位点坐标指针
	SGSVFrm sGSV = { 0 };
	BOOL bGSVOk = FALSE;
	SDHV aDHV = { 0 };
	SZDA sZDA = { 0 };
	STime sTime = { 0 };
	SDate sDate = { 0 };
	SGSA sGSA = { 0 };
	SCOV sCOV = { 0 };
	SCNO sCNO = { 0 };
	S32 i,j;
	CString str,strHCNRSatId = _T("");

	if ((pData[3] == 0x47) 
		&& (pData[4] == 0x47) 
		&& (pData[5] == 0x41))       //GGA
	{
		aGGA.pSPoint = &aSPoint;
		aGGA.pSTime = &aSTime;

		GetGGA(pData, &aGGA);
		g_pGGAPtl->GGAInfoShow(aGGA, pData);
		g_pPtlTree->WritePtlData(pData, nbyteLen, _T("GGA"));
		g_pErrCal->GetBLHErr(aGGA);
		g_pMap->SendMessage(WM_MYMESSAGE_SETGGAPOINT, WPARAM(TRUE), (LPARAM)aGGA.pSPoint);
	}
	else if ((pData[3] == 0x47) 
			&& (pData[4] == 0x53) 
			&& (pData[5] == 0x56))      //GSV
	{
		GetGSV(pData, &sGSV);
		if ((pData[1] == 0x42) 
			 && (pData[2] == 0x44))
		{
			bGSVOk = g_pGSVList->SortGSV(SYS_BDS, sGSV, g_sGSVBuf);
			if (!bGSVOk)
			{
				return;
			}
			g_pGSVList->GSVInfoShow(SYS_BDS, bGSVOk, g_sGSVBuf, g_pCNOList->g_CNOBuf);
			for (i = g_sGSVBuf[0].s32RcvdSatNum - 1;i > g_sGSVBuf[0].s32RcvdSatNum - FIVE_HIGHER_CNR - 1; i--)
			{
				if (g_sGSVBuf[0].pfSatCnr[i] > 40)
				{
					str.Format(_T("%u,"), g_sGSVBuf[0].pSatId[i]);
					strHCNRSatId = strHCNRSatId + str;
				}
			}
			if (m_nAddCmbItemIdx != 0)
			{
				m_ComBoxCmd.DeleteString(m_nAddCmbItemIdx);
			}
			m_nAddCmbItemIdx = m_ComBoxCmd.AddString(_T("$PNASK,3,1,1000,") + strHCNRSatId + _T("1*FC"));
			/*满足前五载噪比均大于40时选定指令*/
			if ((!m_bCmdSended) && (g_sGSVBuf[0].pfSatCnr[g_sGSVBuf[0].s32RcvdSatNum - FIVE_HIGHER_CNR - 1] > 40))
			{
				m_ComBoxCmd.SetCurSel(m_nAddCmbItemIdx);
			}
		}
		else if ((pData[1] == 0x47) 
			  && (pData[2] == 0x50))
		{
			bGSVOk = g_pGSVList->SortGSV(SYS_GPS, sGSV, g_sGSVBuf);
			g_pGSVList->GSVInfoShow(SYS_GPS, bGSVOk, g_sGSVBuf, g_pCNOList->g_CNOBuf);
		}
		else if ((pData[1] == 0x47) 
			  && (pData[2] == 0x4C))
		{
			bGSVOk = g_pGSVList->SortGSV(SYS_GLONASS, sGSV, g_sGSVBuf);
			g_pGSVList->GSVInfoShow(SYS_GLONASS, bGSVOk, g_sGSVBuf, g_pCNOList->g_CNOBuf);
		}

		g_pGSVList->GSVStmtLog(pData);
		g_pPtlTree->WritePtlData(pData, nbyteLen, _T("GSV"));
	}
	else if ((pData[3] == 0x44) 
		  && (pData[4] == 0x48) 
		  && (pData[5] == 0x56))       /*DHV*/
	{
		aDHV.pSTime = &aSTime;

		GetDHV(pData, &aDHV);
		g_pDHVList->DHVInfoShow(aDHV, pData);
		g_pPtlTree->WritePtlData(pData, nbyteLen, _T("DHV"));
	}
	else if ((pData[3] == 0x5A) 
		  && (pData[4] == 0x44) 
		  && (pData[5] == 0x41))	/*	ZDA */
	{
		sZDA.psTime = &sTime;
		sZDA.psDate = &sDate;

		GetZDA(pData, &sZDA);
		g_pPtlTree->WritePtlData(pData, nbyteLen, _T("ZDA"));
	}
	else if ((pData[3] == 0x47) 
		  && (pData[4] == 0x53) 
		  && (pData[5] == 0x41))	/*	GSA */
	{
		GetGSA(pData, &sGSA);
		g_pPtlTree->WritePtlData(pData, nbyteLen, _T("GSA"));
	}
	else if ((pData[3] == 0x43) 
		  && (pData[4] == 0x4F) 
		  && (pData[5] == 0x56))	/*	COV */
	{
		GetCOV(pData, &sCOV);
		g_pGGAPtl->COVInfoShow(sCOV);
		g_pPtlTree->WritePtlData(pData, nbyteLen, _T("COV"));
	}
	else if ((pData[3] == 0x43) 
		  && (pData[4] == 0x4E) 
		  && (pData[5] == 0x4F))	/*	CNO */
	{
		
		m_bRcvFrq = TRUE;
		GetCNO(pData, &sCNO);

		j = 0;
		for (i = 0;i < MAX_SAT_INDEX;i++)
		{
			if (sCNO.pfSatCnr[i] != 0)
			{
				sCNO.pfSatCnr[j] = sCNO.pfSatCnr[i];
				sCNO.pu32SatId[j] = sCNO.pu32SatId[i];
				j++;
			}
			if (j == sCNO.u32SatNum)
			{
				break;
			}
		}
		g_pCNOList->CNOInfoShow(sCNO, pData);
		g_pPtlTree->WritePtlData(pData, nbyteLen, _T("CNO"));
	}
}
/*	选择回放0183文件，在串口打开时不能回放 */
void CDataProc::OnBnClickedButtonSlctdat()
{
	if (m_bOpenOK == TRUE)
	{
		g_bCanReplay = FALSE;
		m_Read.Close();
		m_btnRplyCtrl.EnableWindow(FALSE);
		m_bOpenOK = FALSE;
	}
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bPortOpen)//未打开串口配置或串口关闭
	{
		CFile fp;
		CString strRestLine;
		CString strFilePath = _T("");

		LPCTSTR szFilter = _T("ALLSUPORTFILE(*.*)|*.*||");
		CFileDialog dlgFileOpenImg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);
		if (dlgFileOpenImg.DoModal() == IDOK) { strFilePath = dlgFileOpenImg.GetPathName(); }
		else 
		{ 
			return; 
		}

		m_bOpenOK = m_Read.Open(strFilePath, CFile::modeRead);
		if (m_bOpenOK == FALSE)
		{
			MessageBox(_T("请检查文件路径是否有误!"));
			return;
		}
		m_bRplyRowFile = TRUE;
		//更新状态栏
		SStatusBar sStatusBar = { 0 };
		sStatusBar.uItemId = ID_INDICATOR_WORKMODE;
		sStatusBar.strItem = _T("回放");
		GetParent()->SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sStatusBar);
		sStatusBar.uItemId = ID_INDICATOR_FILEPATH;
		sStatusBar.strItem = strFilePath;
		GetParent()->SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sStatusBar);
		//显示剩余行数
		m_RestLine = (m_Read.GetLength()) / MAX_READ_LEN;		//总行数
		strRestLine.Format(_T("RestLine:%d"), m_RestLine);
		UpdateRestLine(strRestLine);

		m_Read.SeekToBegin();
		m_btnSelectHex.EnableWindow(FALSE);
		m_btnRplyCtrl.EnableWindow(TRUE);
	}
	else
	{
		MessageBox(_T("请先关闭串口再进行回放"));
	}
}
/*选择回放16进制文件*/
void CDataProc::OnBnClickedButtonSelecthex()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bOpenOK == TRUE)
	{
		g_bCanReplay = FALSE;
		m_Read.Close();
		m_btnRplyCtrl.EnableWindow(FALSE);
		m_bOpenOK = FALSE;
	}
	if (!m_bPortOpen)//未打开串口配置或串口关闭
	{
		CFile fp;
		CString strRestLine;
		CString strFilePath = _T("");

		LPCTSTR szFilter = _T("ALLSUPORTFILE(*.*)|*.*||");
		CFileDialog dlgFileOpenImg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);
		if (dlgFileOpenImg.DoModal() == IDOK) { strFilePath = dlgFileOpenImg.GetPathName(); }
		else
		{
			return;
		}

		m_bOpenOK = m_Read.Open(strFilePath, CFile::modeRead);
		if (m_bOpenOK == FALSE)
		{
			MessageBox(_T("请检查文件路径是否有误!"));
			return;
		}
		m_bRplyHexFile = TRUE;
		/*更新状态栏*/
		SStatusBar sStatusBar = { 0 };
		sStatusBar.uItemId = ID_INDICATOR_WORKMODE;
		sStatusBar.strItem = _T("回放");
		GetParent()->SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sStatusBar);
		sStatusBar.uItemId = ID_INDICATOR_FILEPATH;
		sStatusBar.strItem = strFilePath;
		GetParent()->SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sStatusBar);
		/*显示剩余行数*/
		m_RestLine = (m_Read.GetLength()) / MAX_READ_LEN;		//总行数
		strRestLine.Format(_T("RestLine:%d"), m_RestLine);
		UpdateRestLine(strRestLine);

		m_Read.SeekToBegin();
		m_btnSlctDat.EnableWindow(FALSE);
		m_btnRplyCtrl.EnableWindow(TRUE);
	}
	else
	{
		MessageBox(_T("请先关闭串口再进行回放"));
	}
}
/*开始回放*/
void CDataProc::OnBnClickedButtonBgnrply()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strReplayBtn;
	SStatusBar sStatusBar = { 0 };

	GetDlgItemText(IDC_BUTTON_BGNRPLY, strReplayBtn);

	if (   (strReplayBtn == _T("开始回放"))	
		&& (m_bPortOpen == FALSE		)		//串口关闭时才可以进行回放
		&& (m_bOpenOK == TRUE			))		//文件已打开
	{
		SetDlgItemText(IDC_EDIT_SHOWDATA, _T(""));	//先清空编辑框再回放
		SetDlgItemText(IDC_BUTTON_BGNRPLY, _T("暂停回放"));
		m_btnSlctDat.EnableWindow(FALSE);
		m_btnSelectHex.EnableWindow(FALSE);
		g_bCanReplay = TRUE;
		m_bStopShow = FALSE;
		/*清缓冲区*/
		memset(m_BufPcSrRecv, 0, MAX_BUFFER_SIZE);
		FBufferClear(&m_pRngBufPcSrRecv, 20480, m_BufPcSrRecv);
	}
	else if (strReplayBtn == _T("暂停回放"))
	{
		m_bStopShow = TRUE;
		g_bCanReplay = FALSE;
		m_btnSlctDat.EnableWindow(TRUE);
		m_btnSelectHex.EnableWindow(TRUE);
		SetDlgItemText(IDC_BUTTON_BGNRPLY, _T("开始回放"));
	}
}
/*cstr 2 char*/
S32 CDataProc::FReadCStrToUChar(U8* pDataReplay)
{
	CString strRestLine;
	S32 nReadSize = 0;

	if (   (m_Read == NULL		 ) // 文件丢失
		|| (g_bCanReplay == FALSE)
		|| (m_bOpenOK == TRUE	 ))
	{
		return 0;
	}
	nReadSize = m_Read.Read(pDataReplay, MAX_READ_LEN);
	if (nReadSize)
	{
		m_RestLine--;
		strRestLine.Format(_T("RestLine:%d"),m_RestLine);
		UpdateRestLine(strRestLine);
	}
	return nReadSize;
}
/*清空edit*/
void CDataProc::OnBnClickedButtonCleanedit()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_EDIT_SHOWDATA,_T(""));
}

void CDataProc::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
	m_CComSet.ClosePort();
	g_bCanPrco = FALSE;
	g_bCanReplay = FALSE;
	m_bReplaying = FALSE;
	CloseHandle(m_hDataReplayThread);
	CloseHandle(m_hDataRcvThread);
}
/*显示16进制*/
void CDataProc::OnBnClickedCheckShowhex()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_SHOWHEX);
	S32 state = pBtn->GetCheck();
	if (state == 1)
	{
		m_bShowHex = TRUE;
	}
	else/***复选框没勾上 */
	{
		m_bShowHex = FALSE;
	}
}
/*更新剩余行数*/
void CDataProc::UpdateRestLine(CString strRestLine)
{
	SStatusBar sRestLine = { 0 };

	sRestLine.uItemId = ID_INDICATOR_RESTLINE;
	sRestLine.strItem = strRestLine;
	GetParent()->SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sRestLine);
}
void CDataProc::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

}

void CDataProc::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 定时任务，可考虑将绘图代码整理到这里，周期绘制

	CDialogEx::OnTimer(nIDEvent);
}


void CDataProc::OnBnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
	//获得下拉框选中的序号
	UpdateData(TRUE);
	S32 length = 0;
	U8 pDst[1024] = "0";
	CString strCmd = _T("");
	S32 index = m_ComBoxCmd.GetCurSel();

	if (!m_bPortOpen)
	{
		MessageBox(_T("COM Not Open!"));
	}

	m_ComBoxCmd.GetLBText(index, strCmd);
	strCmd += _T("\r\n");

	//CString to char
	const size_t strsize = strCmd.GetLength() + 1; // 字符的长度;
	char* pstr = new char[strsize]; //分配空间;
	size_t sz = 0;
	wcstombs_s(&sz, pstr, strsize, strCmd, _TRUNCATE);

	BOOL ret = m_CComSet.Send((U8*)pstr, strsize);

	if (!ret)
	{
		MessageBox(_T("Send Failed！"));
	}
	else
	{
		if (strCmd.Left(16) == _T("$PNASK,3,1,1000,"))
		{
			m_bCmdSended = TRUE;
		}
	}
}

/*禁止用户主动拖动窗口,经调试，SC_SIZE~SC_MOVE的nID是用户主动改变尺寸的消息*/
void CDataProc::OnSysCommand(UINT nID, LPARAM lParam)
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


void CDataProc::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}

void CDataProc::OnBnClickedButtonOpencom()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strOpen;
	CString strCom;
	CString strComTemp;
	S32 nTimerSetting;
	U32	uComId;
	SStatusBar sStatusBar = { 0 };
	SMenuBar sSMenuBar = { 0 };

	/*获得当前控件选的串口号*/
	m_PortId.GetLBText(m_PortId.GetCurSel(), strCom);
	strComTemp = strCom;
	strComTemp.Delete(0, 3);
	uComId = _ttoi(strComTemp);

	GetDlgItemText(IDC_BUTTON_OPENCOM, strOpen);
	/*使用的接收机波特率默认为115200*/
	if ((m_uSelectedBoteRate == 0)
		|| (m_bReplaying == TRUE))
	{
		AfxMessageBox(_T("error！"));
	}
	else if ((m_CComSet.OpenPort(m_uSelectedBoteRate, m_hWnd))
		&& (m_bPortOpen == FALSE)
		&& (strOpen == (_T("打开串口"))))
	{
		/*控件显示更新*/
		SetDlgItemText(IDC_EDIT_SHOWDATA, _T(""));
		SetDlgItemText(IDC_BUTTON_OPENCOM, _T("关闭串口"));
		m_bPortOpen = TRUE;
		/*清缓冲区*/
		memset(m_BufPcSrRecv, 0, MAX_BUFFER_SIZE);
		FBufferClear(&m_pRngBufPcSrRecv, 20480, m_BufPcSrRecv);
		/*更新状态栏*/
		sStatusBar.uItemId = ID_INDICATOR_WORKMODE;
		sStatusBar.strItem = _T("实时");
		GetParent()->SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sStatusBar);
		sStatusBar.uItemId = ID_INDICATOR_LINKSTATUS;
		sStatusBar.strItem = strCom + _T("连接");
		GetParent()->SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sStatusBar);
		/*启用定时器*/
		nTimerSetting = SET_TIMER;
		GetParent()->SendMessage(WM_MYMESSAGE_STARTUPTIMER, WPARAM(TRUE), (LPARAM)&nTimerSetting);
	}
	if (strOpen == (_T("关闭串口")))
	{
		m_bPortOpen = FALSE;
		m_CComSet.ClosePort();
		SetDlgItemText(IDC_BUTTON_OPENCOM, _T("打开串口"));
		/*更新状态栏*/
		sStatusBar.uItemId = ID_INDICATOR_WORKMODE;
		sStatusBar.strItem = _T("空闲");
		GetParent()->SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sStatusBar);
		sStatusBar.uItemId = ID_INDICATOR_LINKSTATUS;
		sStatusBar.strItem = _T("未连接");
		GetParent()->SendMessage(WM_MYMESSAGE_UPDATESTATUSBAR, WPARAM(TRUE), (LPARAM)&sStatusBar);
		/*销毁定时器*/
		nTimerSetting = KILL_TIMER;
		GetParent()->SendMessage(WM_MYMESSAGE_STARTUPTIMER, WPARAM(TRUE), (LPARAM)&nTimerSetting);
		/*刷新UI*/
		InitUI();
	}
}

void CDataProc::InitUI()
{

	g_pSatSkyMap->m_bCanPaint = TRUE;
	g_pSatCnrDlg->m_bCanPaint = TRUE;

	g_pSatSkyMap->SendMessage(WM_PAINT);
	g_pSatCnrDlg->SendMessage(WM_PAINT);

}


