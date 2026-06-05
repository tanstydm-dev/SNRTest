#pragma once
#include "CHeadernStruct.h"
#include <iostream>
#include <wrl.h>
#include <wil/com.h>
#include "WebView2.h"

#include <string>
#include <vector>
using namespace std;
using namespace Microsoft::WRL;

// Pointer to WebViewController
static wil::com_ptr<ICoreWebView2Controller> webviewController = nullptr;

// Pointer to WebView window
static wil::com_ptr<ICoreWebView2> webview = nullptr;

#define MAX_LOCALHTML_DIR	128
#ifdef __windows__
#undef __windows__
#endif

#ifdef USE_WEBVIEW2_WIN10
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>
namespace winrtComp = winrt::Windows::UI::Composition;
#endif

// CMapDlg 对话框
class CMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMapDlg)

public:
	CMapDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMapDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	CRect m_rcMinimumDialog;
	void GetModuleDir(CString& strFilePath, S32* nDelPos);

public:
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnMyMessageSetGGAPnt(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMyMessageSetUsrPnt(WPARAM wParam, LPARAM lParam);

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
extern CMapDlg* g_pMap;
