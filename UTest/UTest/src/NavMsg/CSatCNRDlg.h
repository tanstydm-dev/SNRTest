#pragma once

#include "CHeadernStruct.h"

// CSatCNRDlg 对话框
#define RGB_HIGHCNR		RGB(30,144,255)
#define RGB_LOWCNR		RGB(220,20,60)
#define RGB_BACK		RGB(240,240,240)
#define RGB_BACK_BD		RGB(248,248,255)
#define RGB_BACK_GP		RGB(255,240,245)
#define RGB_BACK_GL		RGB(240,255,255)

#define FONT_SIZE		20

class CSatCNRDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSatCNRDlg)

public:
	CSatCNRDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSatCNRDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CNR };
#endif

public:
	CPen m_penDashed;		/*虚线笔*/
	CPen m_penHCnr;			/*达标的载噪比用的画笔*/
	CPen m_penLCnr;			/*低载噪比用的画笔*/
	CBrush m_bhHCnr;			/*达标的载噪比用的画刷*/
	CBrush m_bhLCnr;			/*低载噪比用的画刷*/

	CTabCtrl m_tabCtrl;
	S32 m_nCurSelTab;
	S32 m_pnTab[MAX_FRQ_NUM];	/**< 已经打开的tab */
	S32 m_pnFrq[MAX_FRQ_NUM];	/**< 使用的频点 */
	CStatic m_pDrawCtrl[MAX_FRQ_NUM];

	/*背景画刷*/
	CBrush m_brushBK1;		
	CBrush m_brushBK2;
	CBrush m_brushBK3;
	CBrush m_BkBrush;
	CBrush m_Brush;
	CFont m_SatID;
	CFont m_CNRID;
	S32 m_nFontWidth;		/*绘图所有字体大小*/
	S32 m_FreqType;
	BOOL m_bCanPaint;		/*用于PAINT消息与绘制载噪比之间的同步*/
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	void CreateDynamicLayout();
public:
	void UpdateCNR(CDC* pDC, CRect rect, SGSVFrm* sGSVBuf);
	void DrawRcvdCNR(SGSVFrm* sGSVBuf, U32 uFrqIdx);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
};
extern CSatCNRDlg* g_pSatCnrDlg;