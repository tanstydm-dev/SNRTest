#pragma once


#include "CHeadernStruct.h"
#include "CGGAList.h"
#include "CSatDistImg.h"
#include "CErrCal.h"
#include "CMapDlg.h"
#include "CGSAList.h"
#include "CGSVList.h"
#include "CDHVList.h"
#include "CCNOList.h"
#include <algorithm>

#define  NUMBER_OF_CLOCK		60
#define  LOG_HEX				1
#define  LOG_RECEIVE_RAW		0
#define	 MAX_READ_LEN			512
#define  MAX_BUFFER_SIZE		20480
#define  GET_DATA_IN_OPEN_MAX   10240		/*一次性接收最大数据，满了即需清零 */
#define  FIVE_HIGHER_CNR		5
// CDataProc 对话框

class CDataProc : public CDialogEx
{
	DECLARE_DYNAMIC(CDataProc)

public:
	CDataProc(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDataProc();


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DATARCV };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	CRect m_rcMinimumDialog;

	void CreateDynamicLayout();

public:
	CMutex g_mMutex;			/*互斥量同步读线程和处理线程*/
	HANDLE m_hDataRcvThread;	/*	处理 */
	DWORD  m_dDataRcvThreadID;
	HANDLE m_hDataReplayThread;	/*	回放 */
	DWORD  m_dDataReplayThreadID;
	BOOL m_bSaveData;
	BOOL m_bShowHex;
	BOOL m_bSaveHex;
	BOOL m_bRplyRowFile;	
	BOOL m_bRplyHexFile;
	BOOL m_bStopShow;	
	BOOL m_bReplaying;		/*	是否开始回放 */
	BOOL m_bOpenOK;			/*	文件是否打开 */
	BOOL m_bRcvFrq;			/*	是否收到GSA频点信息 */
	S32 m_nPtlNum;
	S32 m_RestLine;			/*	读文件剩余行数 */
	CString m_strPtl;
	CString m_strReadAll;		/*	从文件中取出数据存储到此内存中 */
	volatile U32 m_nFileLineCount;	//行数
	volatile U32 m_byteFileLine;	//文件剩余行数
	FrameBufferStruct m_pRngBufPcSrRecv;
	S8 m_BufPcSrRecv[MAX_BUFFER_SIZE];
	SGSVFrm g_sGSVBuf[4];

	CFile m_SaveFile;	/*	用于写入文件 */
	CFile m_SaveHex;	/*	用于写入16进制 */
	CFile m_Read;		/*	读文件 */
	CButton m_btnRplyCtrl;
	CButton m_btnSelectHex;
	CButton m_btnSlctDat;
	CEdit m_Edit;			/*	绑定edit */
	S32 m_nAddCmbItemIdx;	/**< 上一次添加ComBox字符串的索引 */
	BOOL m_bCmdSended;		/**< 是否已经发送了自动生成的指令，若是，则不再自动选中 */
	CComboBox m_ComBoxCmd;	/*指令框*/
	/*串口相关*/
	CComboBox m_PortId;			/*串口号*/
	BOOL m_bPortOpen;			/*串口打开状态*/
	CComSet m_CComSet;			/*串口句柄*/
	U32	m_uSelectedBoteRate;	/*目前配置的波特率*/

	S32 Open(CString strPortId, U32 uBoteRate);
	virtual BOOL OnInitDialog();
	void PtlPro(U8* pData, S32 nbyteLen);
	void LogInfo(const S8_CHAR* pStr, S32 nInfoSort);
	void UpdateRestLine(CString strRestLine);
	S32 FReadCStrToUChar(U8* pDataReplay);
	BOOL WriteRawData(U8* pData, S32 byteLen);
	BOOL WriteHex(S8_CHAR* pHex, S32 nbyteLen);
	void InitUI();
	afx_msg LRESULT OnSerialRxNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCheckSavedata();
	afx_msg void OnBnClickedButtonStopshow();
	afx_msg void OnBnClickedButtonSlctdat();
	afx_msg void OnBnClickedButtonBgnrply();
	afx_msg void OnBnClickedButtonCleanedit();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheckShowhex();
	afx_msg void OnBnClickedCheckSavehex();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonSelecthex();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedButtonOpencom();
};
extern CDataProc* g_pDataPrco;