// CMapDlg.cpp: 实现文件
//

#include "pch.h"
#include "afxdialogex.h"
#include "UTest.h"
#include "CMapDlg.h"
#include "winuser.h"
#include <ShObjIdl_core.h>
#include <Shellapi.h>
#include <ShlObj_core.h>

// CMapDlg 对话框
CMapDlg* g_pMap = NULL;
IMPLEMENT_DYNAMIC(CMapDlg, CDialogEx)

#ifdef __windows__
#undef __windows__
#endif
static constexpr UINT s_runAsyncWindowMessage = WM_APP;

CMapDlg::CMapDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MAP, pParent)
{
	g_pMap = this;
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
}

CMapDlg::~CMapDlg()
{
	CoUninitialize();
}

void CMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMapDlg, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_MYMESSAGE_SETGGAPOINT, &CMapDlg::OnMyMessageSetGGAPnt)
	ON_MESSAGE(WM_MYMESSAGE_SETUSERPOINT, &CMapDlg::OnMyMessageSetUsrPnt)
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMapDlg 消息处理程序


BOOL CMapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

LRESULT CMapDlg::OnMyMessageSetGGAPnt(WPARAM wParam, LPARAM lParam)
{
	CWnd WSD;
	CString strLat, strLatDegree, strLatMin, strLatDir = _T("");
	CString strLon, strLonDegree, strLonMin, strLonDir = _T("");
	SSrOutPoint* pSPoint = (SSrOutPoint*)lParam;	//通过消息传递参数

	strLatDegree.Format(_T("%d"), pSPoint->nLatDegree);
	strLatMin.Format(_T("%lf"), pSPoint->nLatMin);
	strLatMin.Remove('.');
	strLatDir.Format(_T("%c"), pSPoint->sLatDir);
	strLat = strLatDegree + _T(".") + strLatMin/* + strLatDir*/;

	strLonDegree.Format(_T("%d"), pSPoint->nLonDegree);
	strLonMin.Format(_T("%lf"), pSPoint->nLonMin);
	strLonMin.Remove('.');
	strLonDir.Format(_T("%c"), pSPoint->sLonDir);
	strLon = strLonDegree + _T(".") + strLonMin/* + strLonDir*/;

	// 判断经纬度信息是否为全数字
	if (!(strLat.SpanIncluding(_T("0123456789.SN")) == strLat) && !(strLon.SpanIncluding(_T("0123456789.EW")) == strLon))
	{
		WSD.MessageBox(_T("坐标信息格式不正确！"));
		return 0;
	}
	if ((!strLat.IsEmpty()) && (!strLon.IsEmpty()))
	{
		// 构造要执行的 JavaScript 代码  
		CString script = _T("js_markPoints([[") + strLon + _T(",") + strLat + _T("]])");
		webview->ExecuteScript(script, NULL);
		return 1;
	}
	else
	{
		WSD.MessageBox(_T("位置信息不完整！"));
		return 0;
	}
}

LRESULT CMapDlg::OnMyMessageSetUsrPnt(WPARAM wParam, LPARAM lParam)
{
	CWnd WSD;
	CString strUserLat = _T("");
	CString strUserLon = _T("");

	SUserPnt* sUserPnt = (SUserPnt*)lParam;
	strUserLat.Format(_T("%lf"), sUserPnt->f64UserLat);
	strUserLon.Format(_T("%lf"), sUserPnt->f64UserLon);

	// 判断经纬度信息是否为全数字
	if (!(strUserLat.SpanIncluding(_T("0123456789.SN")) == strUserLat) && !(strUserLon.SpanIncluding(_T("0123456789.EW")) == strUserLon))
	{
		WSD.MessageBox(_T("坐标信息格式不正确！"));
		return 0;
	}
	if ((!strUserLat.IsEmpty()) && (!strUserLon.IsEmpty()))
	{
		// 构造要执行的 JavaScript 代码  
		CString script = _T("js_markPoints([[") + strUserLon + _T(",") + strUserLat + _T("]])");
		webview->ExecuteScript(script, NULL);
		return 1;
	}
	else
	{
		WSD.MessageBox(_T("位置信息不完整！"));
	}
}

void CMapDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}


void CMapDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ((nID >= SC_SIZE) && (nID <= SC_MOVE))
	{
		return;
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CMapDlg::GetModuleDir(CString& strFilePath, S32* nDelPos)
{
	char pFileName[MAX_PATH] = { 0 };
	HMODULE module = GetModuleHandle(0);

	GetModuleFileNameA(module, pFileName, MAX_PATH);
	strFilePath = pFileName;
	*nDelPos = strFilePath.ReverseFind('\\');
	if (nDelPos < 0)
	{
		strFilePath = _T("");
	}
}


int CMapDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	HWND hWnd = this->m_hWnd;
	
#if 1

	S32 nDelPos = 0;
	CString strUrl = _T("");
	/*获取exe文件路径，html放在这儿*/
	GetModuleDir(strUrl, &nDelPos);
	if (nDelPos != 0)
	{
		S32 s32CstrLen = strUrl.GetLength();
		strUrl.Delete(nDelPos, (s32CstrLen - nDelPos));
		strUrl += _T("/Map/BMap.html");
	}

	// TODO:  在此添加您专用的创建代码
		// <-- WebView2 sample code starts here -->
		// Step 3 - Create a single WebView within the parent window
		// Locate the browser and set up the environment for WebView
	CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[hWnd, strUrl](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

				// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
				env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[hWnd, strUrl](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
						if (controller != nullptr) {
							webviewController = controller;
							webviewController->get_CoreWebView2(&webview);
						}

						// Add a few settings for the webview
						// The demo step is redundant since the values are the default settings
						wil::com_ptr<ICoreWebView2Settings> settings;
						webview->get_Settings(&settings);
						settings->put_IsScriptEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						settings->put_IsWebMessageEnabled(TRUE);

						// Resize WebView to fit the bounds of the parent window
						RECT bounds;
						::GetClientRect(hWnd, &bounds);
						webviewController->put_Bounds(bounds);

						// Schedule an async task to navigate to Bing
						webview->Navigate(strUrl);

						// <NavigationEvents>
						// Step 4 - Navigation events
						// register an ICoreWebView2NavigationStartingEventHandler to cancel any non-https navigation
						EventRegistrationToken token;
						webview->add_NavigationStarting(Callback<ICoreWebView2NavigationStartingEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
								wil::unique_cotaskmem_string uri;
								args->get_Uri(&uri);
								std::wstring source(uri.get());
								/*if (source.substr(0, 5) != L"https") {
									args->put_Cancel(true);
								}*/
								return S_OK;
							}).Get(), &token);
						// </NavigationEvents>

						// <Scripting>
						// Step 5 - Scripting
						// Schedule an async task to add initialization script that freezes the Object object
						webview->AddScriptToExecuteOnDocumentCreated(L"Object.freeze(Object);", nullptr);
						// Schedule an async task to get the document URL
						webview->ExecuteScript(L"window.document.URL;", Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
							[](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
								LPCWSTR URL = resultObjectAsJson;
								//doSomethingWithURL(URL);
								return S_OK;
							}).Get());
						// </Scripting>

						// <CommunicationHostWeb>
						// Step 6 - Communication between host and web content
						// Set an event handler for the host to return received message back to the web content
						webview->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
								wil::unique_cotaskmem_string message;
								args->TryGetWebMessageAsString(&message);//从html接收数据
								// processMessage(&message);
								webview->PostWebMessageAsString(message.get());//发送数据到html
								return S_OK;
							}).Get(), &token);

						// Schedule an async task to add initialization script that
						// 1) Add an listener to print message from the host
						// 2) Post document URL to the host
						webview->AddScriptToExecuteOnDocumentCreated(
							L"window.chrome.webview.addEventListener(\'message\', event => alert(event.data));" \
							L"window.chrome.webview.postMessage(window.document.URL);",
							nullptr);
						// </CommunicationHostWeb>

						return S_OK;
					}).Get());
				return S_OK;
			}).Get());
#endif

	SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return 0;
}


void CMapDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (webviewController != nullptr) {
		RECT bounds;
		GetClientRect(&bounds);
		webviewController->put_Bounds(bounds);
	};
}
