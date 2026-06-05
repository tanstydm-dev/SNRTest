#pragma once
#include "afxdialogex.h"
#include "CMyListCtrl.h"

// CGSAList 对话框

class CGSAList : public CDialogEx
{
	DECLARE_DYNAMIC(CGSAList)

public:
	CGSAList(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGSAList();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GSA };
#endif

protected:
	BOOL m_bListCreated;
	CRect m_rcMinimumDialog;

	void CreateDynamicLayout();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ReSize(CRect recta);

	DECLARE_MESSAGE_MAP()

public:
	CMyListCtrl m_GSAListCtrl;
	CEdit m_editGSAData;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
