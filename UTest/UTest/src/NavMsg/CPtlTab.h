#pragma once
#include "afxdialogex.h"

#include "CGGAList.h"
#include "CGSAList.h"
#include "CGSVList.h"
#include "CDHVList.h"
#include "CCNOList.h"
// CPtlTab 对话框

class CPtlTab : public CDialogEx
{
	DECLARE_DYNAMIC(CPtlTab)

public:
	CPtlTab(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPtlTab();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_DIALOG_PTLTAB
};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


	CTabCtrl m_tabCtrl;
	S32 m_nCurSelTab;
	CGGAList	m_dlgGGAList;
	CGSAList	m_dlgGSAList;
	CGSVList	m_dlgGSVList;
	CDHVList	m_dlgDHVList;
	CCNOList	m_dlgCNOList;
	BOOL		m_bDlgGGADone;
	BOOL		m_bDlgGSADone;
	BOOL		m_bDlgGSVDone;
	BOOL		m_bDlgDHVDone;
	BOOL		m_bDlgCNODone;
	CRect m_rcMinimumDialog;
	void CreateDynamicLayout();

public:
	void ChangeDlgShow(S32 nShowDlg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
extern CPtlTab* g_pPtlTab;
