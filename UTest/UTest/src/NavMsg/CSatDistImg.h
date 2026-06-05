#pragma once

#include "CDataProc.h"
// CSatDistImg 对话框

class CSatDistImg : public CDialogEx
{
	DECLARE_DYNAMIC(CSatDistImg)

public:
	CSatDistImg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSatDistImg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SATDIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void CreateDynamicLayout();
	CBrush m_hPolarBkBh;		/**<画极坐标背景*/
	CPen m_penFont;
	CPen m_penBlue;
	CPen m_penRed;
	CPen m_penAxis;
	CPen m_penDashed;			/*	画虚线 */
	CPen m_penWhite;
	CFont m_fontBlack;			/**<黑体(中)*/
	CFont m_fontSatId;			/**<黑体(小)*/
	CPoint m_center;			/**<图片的中心点*/
	F64 m_fRadius;				/**<最外面大圆的半径*/
	F64 m_nDelta;				/**<图片的(宽度-高度)/2*/
	CRect m_rcMinimumDialog;
	CBitmap m_bmpBDS;
	CBitmap m_bmpGPS;
	CBitmap m_bmpGLS;


	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bCanPaint;

	virtual BOOL OnInitDialog();
	void DrawCoordinate(CDC* pDC, CRect rectpic);
	void DrawPos(CDC* pDC, CRect rectpic);
	void GetSvPosF64ToXY(int elevation, int azimuth, CPoint* pSvPos, const CPoint& center, double nRadius);
	void DrawSatMap();
	void TransAlpha(CImage* cImg);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
}; extern CSatDistImg* g_pSatSkyMap;
