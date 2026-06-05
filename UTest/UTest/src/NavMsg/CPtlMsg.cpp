// CPtlMsg.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "afxdialogex.h"
#include "CPtlMsg.h"
#include "CPtlTab.h"

// CPtlMsg 对话框
CPtlMsg* g_pPtlTree = NULL;
IMPLEMENT_DYNAMIC(CPtlMsg, CDialogEx)

CPtlMsg::CPtlMsg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PTLSHOW, pParent)
{
	g_pPtlTree = this;
}

CPtlMsg::~CPtlMsg()
{
}

void CPtlMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PTL, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CPtlMsg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PTL, &CPtlMsg::OnTvnSelchangedTreePtl)
	ON_NOTIFY(NM_CLICK, IDC_TREE_PTL, &CPtlMsg::OnNMClickTreePtl)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CPtlMsg 消息处理程序


BOOL CPtlMsg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rectClient;
	GetClientRect(&rectClient);

	m_bGGAFileReady = FALSE;
	m_bGSAFileReady = FALSE;
	m_bGSVFileReady = FALSE;
	m_bDHVFileReady = FALSE;
	m_bCNOFileReady = FALSE;
	m_bCOVFileReady = FALSE;
	m_bZDAFileReady = FALSE;
	//添加父节点
	HTREEITEM rootNmea = m_treeCtrl.InsertItem(_T("NMEA"));
	m_treeCtrl.SetItemData(rootNmea, 1);
	HTREEITEM parent1 = m_treeCtrl.InsertItem(_T("RTCM3"));
	m_treeCtrl.SetItemData(parent1, 11);
	HTREEITEM parent2 = m_treeCtrl.InsertItem(_T("UNICORE"));
	m_treeCtrl.SetItemData(parent2, 21);
	//添加根节点rootNmea的子节点
	HTREEITEM rootDHV = m_treeCtrl.InsertItem(_T("DHV"), rootNmea);
	m_treeCtrl.SetItemData(rootDHV, 2);
	HTREEITEM rootGSV = m_treeCtrl.InsertItem(_T("GSV"), rootNmea);
	m_treeCtrl.SetItemData(rootGSV, 3);
	HTREEITEM rootCNO = m_treeCtrl.InsertItem(_T("CNO"), rootNmea);
	m_treeCtrl.SetItemData(rootCNO, 4);
	HTREEITEM rootGGA = m_treeCtrl.InsertItem(_T("GGA"), rootNmea);
	m_treeCtrl.SetItemData(rootGGA, 5);
	HTREEITEM rootGSA = m_treeCtrl.InsertItem(_T("GSA"), rootNmea);
	m_treeCtrl.SetItemData(rootGSA, 6);

	m_treeCtrl.Expand(rootNmea, TVE_EXPAND);
	m_treeCtrl.Expand(parent1, TVE_EXPAND);
	m_treeCtrl.Expand(parent2, TVE_EXPAND);
	m_treeCtrl.Select(rootGGA, TVGN_CARET);

	/*创建动态布局*/
	CreateDynamicLayout();
	/*获得最小尺寸的区域*/
	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPtlMsg::CreateDynamicLayout()
{
	this->EnableDynamicLayout();
	CMFCDynamicLayout::MoveSettings NoMove;
	CMFCDynamicLayout::MoveSettings Move_X_100;
	CMFCDynamicLayout::MoveSettings Move_Y_100;
	CMFCDynamicLayout::MoveSettings Move_Both;

	CMFCDynamicLayout::SizeSettings NoSize;
	CMFCDynamicLayout::SizeSettings Size_X_100;
	CMFCDynamicLayout::SizeSettings Size_Y_100;
	CMFCDynamicLayout::SizeSettings Size_Both;

	Move_X_100.m_nXRatio = 100;
	Move_Y_100.m_nYRatio = 100;
	Move_Both.m_nXRatio = 100;
	Move_Both.m_nYRatio = 100;

	Size_X_100.m_nXRatio = 100;
	Size_Y_100.m_nYRatio = 100;
	Size_Both.m_nXRatio = 100;
	Size_Both.m_nYRatio = 100;
	// 获取Dialog的动态布局指针。
	CMFCDynamicLayout* pMfcDynamicLayout = this->GetDynamicLayout();
	// 创建动态布局。
	pMfcDynamicLayout->Create(this);
	// 添加要在动态布局中注册的控件。
	pMfcDynamicLayout->AddItem(IDC_TREE_PTL, NoMove, Size_Both);
}

void CPtlMsg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
// 	if (nType != SIZE_MINIMIZED)
// 	{
// 		ReSize();
// 	}
}

void CPtlMsg::ReSize()
{
	float fsp[2];
	POINT Newp; //现在对话框的大小	
	CRect recta;
	GetClientRect(&recta);     //取客户区大小  

	Newp.x = recta.right - recta.left;
	Newp.y = recta.bottom - recta.top;
	fsp[0] = (float)Newp.x / m_oldPoint.x;
	fsp[1] = (float)Newp.y / m_oldPoint.y;
	CRect Rect;
	int woc;
	CPoint OldTLPoint, TLPoint; //左上角
	CPoint OldBRPoint, BRPoint; //右下角
	HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);  //列出所有控件  
	while (hwndChild)
	{
		woc = ::GetDlgCtrlID(hwndChild);//取得ID
		GetDlgItem(woc)->GetWindowRect(Rect);
		ScreenToClient(Rect);

		OldTLPoint = Rect.TopLeft();
		TLPoint.x = long(OldTLPoint.x * fsp[0]);
		TLPoint.y = long(OldTLPoint.y * fsp[1]);
		OldBRPoint = Rect.BottomRight();
		BRPoint.x = long(OldBRPoint.x * fsp[0]);
		BRPoint.y = long(OldBRPoint.y * fsp[1]);
		Rect.SetRect(TLPoint, BRPoint);

		GetDlgItem(woc)->MoveWindow(Rect, TRUE);  //控件自适应
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
	m_oldPoint = Newp;

}


void CPtlMsg::OnTvnSelchangedTreePtl(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//获取当前鼠标点击消息的坐标点
	CPoint pt = GetCurrentMessage()->pt;

	//将鼠标的屏幕坐标，转换成树形控件的客户区坐标
	m_treeCtrl.ScreenToClient(&pt);

	UINT uFlags = 0;
	HTREEITEM hItem = m_treeCtrl.HitTest(pt, &uFlags);//然后做点击测试
	//HitTest一般是指鼠标点击位置上是哪个树结点,从而得到鼠标位置的树结点
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))//如果点击的位置是在节点位置上
	{
		m_treeCtrl.SelectItem(hItem);
		//获取开始我们设置的数据，注意这就是我为什么开始要每个节点设置不同的数据的原因。
		S32 nDat = m_treeCtrl.GetItemData(hItem);
		g_pPtlTab->ChangeDlgShow(nDat);
		*pResult = 0;
	}

}

void CPtlMsg::OnNMClickTreePtl(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint oPoint;
	BOOL bCheck = FALSE;
	S32 nSelectItem = 0;
	UINT nFlag = 0;

	GetCursorPos(&oPoint);
	m_treeCtrl.ScreenToClient(&oPoint);
	HTREEITEM oSelectItem = m_treeCtrl.HitTest(oPoint,&nFlag);
	if (oSelectItem ==NULL)
	{
		return;
	}

	nSelectItem = m_treeCtrl.GetItemData(oSelectItem);/** 获取当前勾选的复选框索引 */
	m_treeCtrl.SelectItem(oSelectItem);

	if (nFlag & TVHT_ONITEMSTATEICON)
	{
		bCheck = !m_treeCtrl.GetCheck(oSelectItem);
		//为了一致化树形控件的选中状态，需设置当前选中项的复选框为改变后的状态
		m_treeCtrl.SetCheck(oSelectItem, bCheck);
		//一致化树形控件复选框状态
		ConsistentChildCheck(oSelectItem);
		ConsistentParentCheck(oSelectItem);
		//将当前选中项的复选框状态复原，IFC会自动响应复选框状态的改变绘制
		m_treeCtrl.SetCheck(oSelectItem, !bCheck);
	}
	if (bCheck)
	{
		if (nSelectItem == 2)
		{
			m_bDHVFileReady = CheckSavedata(_T("DHV"));
		}
		if (nSelectItem == 3)
		{
			m_bGSVFileReady = CheckSavedata(_T("GSV"));
		}
		if (nSelectItem == 4)
		{
			m_bCNOFileReady = CheckSavedata(_T("CNO"));
		}
		if (nSelectItem == 5)
		{
			m_bGGAFileReady = CheckSavedata(_T("GGA"));
		}
		if (nSelectItem == 6)
		{
			m_bGSAFileReady = CheckSavedata(_T("GSA"));
		}
		if (nSelectItem == 7)
		{
		}
	}
	else
	{
		if ((nSelectItem == 2)&&(m_bDHVFileReady))
		{
			m_bDHVFileReady = FALSE;
			m_fileSaveDHV.Close();
		}
		if ((nSelectItem == 3)&&(m_bGSVFileReady))
		{
			m_bGSVFileReady = FALSE;
			m_fileSaveGSV.Close();
		}
		if ((nSelectItem == 4)&&(m_bCNOFileReady))
		{
			m_bCNOFileReady = FALSE;
			m_fileSaveCNO.Close();
		}
		if ((nSelectItem == 5)&&(m_bGGAFileReady))
		{
			m_bGGAFileReady = FALSE;
			m_fileSaveGGA.Close();
		}
		if ((nSelectItem == 6)&&(m_bGSAFileReady))
		{
			m_bGSAFileReady = FALSE;
			m_fileSaveGSA.Close();
		}
		if ((nSelectItem == 7))
		{
		}
	}
	*pResult = 0;
}

BOOL CPtlMsg::CheckSavedata(CString strPtl)
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString strTime = _T("");
	CString strFileName = _T("");
	BOOL bOpenOK = FALSE;
	TCHAR path[MAX_PATH];

	if (strPtl == _T(""))
	{
		return FALSE;
	}

	GetCurrentDirectory(MAX_PATH, path);
	CString csFullPath(path);
	csFullPath += _T("\\PtlMsg/") + strPtl;
	if (FALSE == CreateDirectory(csFullPath, NULL))
	{
		if (ERROR_ALREADY_EXISTS == GetLastError())
		{
			TRACE("\n this path already exist");
		}
		else
		{
			TRACE("\n this path is invalid");
		}
	}

	// 每次点击勾上，则重新创建文件进行写入
	CTime time = CTime::GetCurrentTime();
	strFileName = csFullPath + _T("/aReplay") + strPtl + _T("_%02d%02d%02d_%02d%02d%02d.dat");
	strTime.Format(strFileName,
		time.GetYear(), time.GetMonth(), time.GetDay(),
		time.GetHour(), time.GetMinute(), time.GetSecond());
	if (strPtl == _T("GGA"))
	{
		 bOpenOK = m_fileSaveGGA.Open(strTime, CFile::modeWrite | CFile::modeCreate);
	}
	else if (strPtl == _T("GSA"))
	{
		bOpenOK = m_fileSaveGSA.Open(strTime, CFile::modeWrite | CFile::modeCreate);
	}
	else if (strPtl == _T("GSV"))
	{
		bOpenOK = m_fileSaveGSV.Open(strTime, CFile::modeWrite | CFile::modeCreate);
	}
	else if (strPtl == _T("DHV"))
	{
		bOpenOK = m_fileSaveDHV.Open(strTime, CFile::modeWrite | CFile::modeCreate);
	}
	else if (strPtl == _T("COV"))
	{
		bOpenOK = m_fileSaveCOV.Open(strTime, CFile::modeWrite | CFile::modeCreate);
	}
	else if (strPtl == _T("ZDA"))
	{
		bOpenOK = m_fileSaveZDA.Open(strTime, CFile::modeWrite | CFile::modeCreate);
	}
	else if (strPtl == _T("CNO"))
	{
		bOpenOK = m_fileSaveCNO.Open(strTime, CFile::modeWrite | CFile::modeCreate);
	}
	if (bOpenOK == FALSE)
	{
		MessageBox(_T("创建文件失败"));
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CPtlMsg::WritePtlData(U8* pData, S32 nbyteLen, CString strPtl)
{
	if ((pData == NULL)
		|| (nbyteLen == 0)
		|| (strPtl == _T("")))
	{
		return FALSE;
	}

	if (strPtl == _T("GGA") && (m_bGGAFileReady == TRUE))
	{
		m_fileSaveGGA.Write(pData,nbyteLen);
	}
	else if (strPtl == _T("GSA") && (m_bGSAFileReady == TRUE))
	{
		m_fileSaveGSA.Write(pData, nbyteLen);
	}
	else if (strPtl == _T("GSV") && (m_bGSVFileReady == TRUE))
	{
		m_fileSaveGSV.Write(pData, nbyteLen);
	}
	else if (strPtl == _T("DHV") && (m_bDHVFileReady == TRUE))
	{
		m_fileSaveDHV.Write(pData, nbyteLen);
	}
	else if (strPtl == _T("COV") && (m_bCOVFileReady == TRUE))
	{
		m_fileSaveCOV.Write(pData, nbyteLen);
	}
	else if (strPtl == _T("ZDA") && (m_bZDAFileReady == TRUE))
	{
		m_fileSaveZDA.Write(pData, nbyteLen);
	}
	else if (strPtl == _T("CNO") && (m_bCNOFileReady == TRUE))
	{
		m_fileSaveCNO.Write(pData, nbyteLen);
	}

	return TRUE;
}

void CPtlMsg::ConsistentParentCheck(HTREEITEM hTreeItem)
{
	//获取当前选中项的父节点，如果父节点为空则返回，否则处理父节点状态
	HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hTreeItem);
	if (hParentItem != NULL)
	{
		//依次判断当前选中项的父节点的各个子节点的状态，
		HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hParentItem);
		while (hChildItem != NULL)
		{
			//如果父节点有一个子节点未被选中，则父节点设置为未选中状态//同时递归处理父节点的父节点
			if (m_treeCtrl.GetCheck(hChildItem) == FALSE)
			{
				m_treeCtrl.SetCheck(hParentItem, FALSE);
				return (ConsistentParentCheck(hParentItem));
			}
			//获取父节点的下一个子节点
			hChildItem = m_treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
		}
		//如果父节点的所有子节点都选中，则父节点设置为选中状态，同时//递归处理父节点的父节点
		m_treeCtrl.SetCheck(hParentItem, TRUE);
		return ConsistentParentCheck(hParentItem);
	}
}
void CPtlMsg::ConsistentChildCheck(HTREEITEM hTreeItem)
{
	//获取当前选中项的选择复选框状态
	BOOL bCheck = m_treeCtrl.GetCheck(hTreeItem);
	//如果当前节点存在子节点，则一致化子节点状态
	if(m_treeCtrl.ItemHasChildren(hTreeItem))
	{
		//依次一致化子节点的子节点状态
		HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hTreeItem);
		while (hChildItem != NULL)
		{
			m_treeCtrl.SetCheck(hChildItem, bCheck);
			ConsistentChildCheck(hChildItem);
			hChildItem = m_treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}

void CPtlMsg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


int CPtlMsg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}


void CPtlMsg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();
}


void CPtlMsg::OnSysCommand(UINT nID, LPARAM lParam)
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

