#pragma once

#include "ErrorCalcute.h"
#include "CDataProc.h"

// CErrCal 对话框
class CErrCal : public CDialogEx
{
	DECLARE_DYNAMIC(CErrCal)

public:
	CErrCal(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CErrCal();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ERRCAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	POINT m_oldPoint;
	CRect m_rcMinimumDialog;

	void CreateDynamicLayout();
	void ReSize();
	DECLARE_MESSAGE_MAP()
public:
	S32 m_GGAErrNum;
	F64 m_pStdPos[3];
	F64 m_f64ElvErr[3];	/*	高程误差 */
	F64 m_f64HorErr[3];	/*	水平误差 */
	BOOL m_bSaveErr;
	BOOL m_bStdPosOk;
	CBrush m_Brush;
	CFile m_ErrFile;


	S32 WriteErrData(CString pCstr);
	void GetBLHErr(SGGA sGGA);
	virtual BOOL OnInitDialog();
	void ShowBLHPos(SGGA sGGA);

	afx_msg void OnBnClickedButtonStartcalc();
	afx_msg void OnBnClickedCheckSavelog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonSetpos();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
extern CErrCal* g_pErrCal;