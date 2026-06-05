#pragma once

#include "PtlNEMA0183Out.h"
#include "CMyListCtrl.h"
// CGGAList 对话框

class CGGAList : public CDialogEx
{
	DECLARE_DYNAMIC(CGGAList)

public:
	CGGAList(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGGAList();
	CBrush m_Brush;
	S32 GGAInfoShow(SGGA sGGA, U8* u8GGAStatement);
	S32 COVInfoShow(SCOV sCOV);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GGA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

	BOOL m_bListCreated;
	CEdit m_editGGAData;
	CRect m_rcMinimumDialog;

	void CreateDynamicLayout();
	void ReSize();

public:
	CMyListCtrl m_GGAListCtrl;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
}; extern CGGAList* g_pGGAPtl;
