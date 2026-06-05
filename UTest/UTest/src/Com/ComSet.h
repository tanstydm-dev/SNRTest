/**
@file		ComSet.h 串口通信类-只支持单个串口 
@author		wp4nnc@gmail.com
@date		2009/07/06 15:14
@version	v1.0 Copyright(c) 1995-2009 - All Rights Reserved
@warning 	
@manual

  1.使用ComboBox控件初始化，调用InterfaceInit函数

  2.增加串口接收数据的消息函数

	ON_MESSAGE(WM_SERIALRXNOTIFY_DEFINE,OnSerialRxNotify)
	afx_msg LRESULT OnSerialRxNotify(WPARAM wParam,LPARAM lParam);

	LRESULT CTestDllDlg::OnSerialRxNotify(WPARAM wParam,LPARAM lParam)
	{
		unsigned char pBuffer[1024];

		int number=m_myComSet.RXSpaceUsed();

		if (number!=0)
		{
			m_myComSet.Read((unsigned char*)pBuffer,number);
		}

		CString str;
		str.Format(_T("%d"), pBuffer);
		SetDlgItemText(IDC_STATIC_RECVSHOW, str);
		return 0;
	}

  3.具体调用可参见测试工程"testDll"。

*************************************************************************/

#if !defined(AFX_COMSET_H__E998CBF0_A20A_44AD_B904_1AB1BAC51FA0__INCLUDED_)
#define AFX_COMSET_H__E998CBF0_A20A_44AD_B904_1AB1BAC51FA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const int WM_SERIALRXNOTIFY_DEFINE              = WM_USER + 0x1000;
const int WM_SERIALROPEN_DEFINE                 = WM_USER + 0x100b;

class CComSet  
{
public:
	CComSet();
	virtual ~CComSet();

public:
	/** 初始化函数
	@param pComboBox	需要显示串口个数信息的ComboBox控件
	@param setCurSel	控件初始化显示位置,缺省为0(第一个)
	@return TRUE:初始化成功	FALSE:输入控件变量为空
	*/
	BOOL InterfaceInit(CComboBox *pComboBox, unsigned int setCurSel = 0);
	BOOL InterfaceInit(CString* pstrPortId, int* nGetPortNum);

	/** 打开控件对应串口
	@param baud  波特率设置
	@param hWnd  hWnd填"m_hWnd"
	@return TRUE:打开成功	FALSE:打开失败
	*/
	BOOL OpenPort(unsigned int baud, HWND hWnd);
	BOOL OpenPort(unsigned int baud, HWND hWnd, CString strCom);

	/** 关闭当前串口
	@return TRUE:关闭成功	FALSE:关闭失败
	*/
	BOOL ClosePort();

	/** 判断当前串口是否打开
	@param   
	@return TRUE:打开	FALSE:未打开
	*/
	BOOL IsOpen(void);


	/** 串口发送函数
	@param pData		发送U8数据
	@param nByteLen		发送数据字节数
	@return				TRUE:发送成功	FALSE:发送失败
	*/
	BOOL Send(unsigned char *pData, unsigned int nByteLen);

	/** 串口来数据字节长度
	@return 串口来数据字节长度
	*/
	int  RXSpaceUsed();

	/** 读取串口来数据
	@param pData		接收U8数据
	@param nByteLen		接收数据字节长度
	@return 
	*/
	int  Read( unsigned char *pData, unsigned int nByteLen, long milliseconds = 0 );
};

#endif // !defined(AFX_COMSET_H__E998CBF0_A20A_44AD_B904_1AB1BAC51FA0__INCLUDED_)

