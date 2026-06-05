#pragma once
#include "afxdialogex.h"
#include "CMyListCtrl.h"
#include "CHeadernStruct.h"
#include "CSatDistImg.h"
#include "CSatCNRDlg.h"
// CGSVList 对话框

class CGSVList : public CDialogEx
{
	DECLARE_DYNAMIC(CGSVList)

public:
	CGSVList(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGSVList();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GSV };
#endif
protected:
	CRect m_rcMinimumDialog;
	BOOL m_bListCreated;
	void CreateDynamicLayout();
	void ReSize(CRect recta);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMyListCtrl m_GSVListCtrl;
	S32  m_nListLine;
	BOOL g_bIsGPGSVAvl;
	BOOL g_bIsBDGSVAvl;
	BOOL g_bIsGLGSVAvl;
	SCalcMeanStd m_sCnrBuf[3];


public:
	S32 SortGSV(S32 nNavSys, SGSVFrm sGSV, SGSVFrm* sGSVBuf);
	void Sort(U32 uSatId[], F64 fCNR[], S32 n);
	S32 GSVStmtLog(U8* u8GSVStatement);
	S32 GSVInfoShow(S32 NavSys, BOOL bGSVOk, SGSVFrm* sGSVBuf, SCNOBuf sCNOBuf);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CEdit m_editGSVData;
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
extern CGSVList* g_pGSVList;