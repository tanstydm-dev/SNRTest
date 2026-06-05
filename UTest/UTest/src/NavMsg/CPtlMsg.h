#pragma once
#include "afxdialogex.h"
#include "UTEST.h"

// CPtlMsg 对话框

class CPtlMsg : public CDialogEx
{
	DECLARE_DYNAMIC(CPtlMsg)

public:
	CPtlMsg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPtlMsg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PTLSHOW };
#endif

protected:
	POINT m_oldPoint;
	CTreeCtrl m_treeCtrl;
	CRect m_rcMinimumDialog;

	void CreateDynamicLayout();
	void ReSize();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ConsistentParentCheck(HTREEITEM hTreeItem);
	void ConsistentChildCheck(HTREEITEM hTreeItem);
	BOOL CheckSavedata(CString strPtl);

	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bGGAFileReady;
	BOOL m_bGSAFileReady;
	BOOL m_bGSVFileReady;
	BOOL m_bDHVFileReady;
	BOOL m_bCNOFileReady;
	BOOL m_bCOVFileReady;
	BOOL m_bZDAFileReady;
	CFile m_fileSaveGGA;
	CFile m_fileSaveGSA;
	CFile m_fileSaveGSV;
	CFile m_fileSaveCOV;
	CFile m_fileSaveDHV;
	CFile m_fileSaveZDA;
	CFile m_fileSaveCNO;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTvnSelchangedTreePtl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClickTreePtl(NMHDR* pNMHDR, LRESULT* pResult);

	BOOL WritePtlData(U8* pData, S32 nbyteLen, CString strPtl);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
extern CPtlMsg* g_pPtlTree;
