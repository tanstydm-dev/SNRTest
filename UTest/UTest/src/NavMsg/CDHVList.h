#pragma once
#include "afxdialogex.h"
#include "CMyListCtrl.h"
#include "CHeadernStruct.h"
// CDHVList 对话框

class CDHVList : public CDialogEx
{
	DECLARE_DYNAMIC(CDHVList)

public:
	CDHVList(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDHVList();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DHV };
#endif

protected:
	BOOL m_bListCreated;
	CRect m_rcMinimumDialog;

	void CreateDynamicLayout();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ReSize(CRect rectWorkArea);

	DECLARE_MESSAGE_MAP()
public:
	CMyListCtrl m_DHVListCtrl;
	CEdit m_editDHVData;

public:
	S32 DHVInfoShow(SDHV sDHV, U8* u8DHVStatement);

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
extern CDHVList* g_pDHVList;