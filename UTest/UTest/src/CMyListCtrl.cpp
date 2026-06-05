// CMyListCtrl.cpp: 实现文件
//

#include "pch.h"
#include "UTest.h"
#include "CMyListCtrl.h"


// CMyListCtrl

IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)

CMyListCtrl::CMyListCtrl()
{
	m_u32RowHeight = 0;
}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()



// CMyListCtrl 消息处理程序
void CMyListCtrl::SetRowHeigt(int nHeight)
{
	m_u32RowHeight = nHeight;

	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}

void CMyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (m_u32RowHeight > 0)
	{
		lpMeasureItemStruct->itemHeight = m_u32RowHeight;
	}
}


void CMyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	LVITEM lvi = { 0 };
	lvi.mask = LVIF_STATE;//|LVIF_IMAGE; 
	lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	lvi.iItem = lpDrawItemStruct->itemID;
	BOOL bGet = GetItem(&lvi);

	BOOL bSelect = ((lvi.state & LVIS_DROPHILITED) || ((lvi.state & LVIS_SELECTED)
		&& ((GetFocus() == this) || (GetStyle() & LVS_SHOWSELALWAYS))));

	//画文本背景 
	CRect bgRect = lpDrawItemStruct->rcItem;
	if (bSelect)//设置选中颜色
	{
		pDC->SetTextColor(RGB(255, 255, 255));//白色文本
		pDC->FillRect(bgRect, &CBrush(RGB(30, 144, 255)));//蓝色背景
	}

	//绘制文本
	if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		//得到列数
		//int nCollumn = GetHeaderCtrl()->GetItemCount();

		//循环处理
		CString szText;
		for (int i = 0; i < GetHeaderCtrl()->GetItemCount(); i++)
		{
			CRect rcItem;
			if (!GetSubItemRect(lpDrawItemStruct->itemID, i, LVIR_LABEL, rcItem))
			{
				continue;
			}

			szText = GetItemText(lpDrawItemStruct->itemID, i);

			rcItem.left += 5; rcItem.right -= 1;
			pDC->DrawText(szText, lstrlen(szText), &rcItem,
				DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
		}
	}
}


void CMyListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
