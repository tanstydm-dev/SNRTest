#pragma once
#include "afxdialogex.h"
#include "CMyListCtrl.h"
#include "CHeadernStruct.h"

// CCNOList 对话框

class CCNOList : public CDialogEx
{
	DECLARE_DYNAMIC(CCNOList)

public:
	CCNOList(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCNOList();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CNO };
#endif
protected:
	BOOL m_bListCreated;
	S32  m_nListLine;
	CRect m_rcMinimumDialog;

	void CreateDynamicLayout();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ReSize(CRect recta);

	DECLARE_MESSAGE_MAP()

public:
	CMyListCtrl m_CNOListCtrl;
	CEdit m_CNOEdit;
	SCNOBuf g_CNOBuf;
	S32 m_pnFrqReady[MAX_FRQ_NUM];

public:
	virtual BOOL OnInitDialog();
	S32 CNOInfoShow(SCNO sCNO,U8* u8CNOStatement);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
extern CCNOList* g_pCNOList;