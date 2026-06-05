// ComSet.cpp: implementation of the CComSet class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ComSet.h"
#include "MyWin32Port.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#define MAX_COM_NUM		20		/**< 最大串口个数 */
#define MAX_STRING_LEN	15

MyWin32Port m_MyWin32Port;
CComboBox *m_pComboBox;

CComSet::CComSet()
{

}

CComSet::~CComSet()
{

}

BOOL CComSet::InterfaceInit(CComboBox *pComboBox, unsigned int setCurSel)
{
	int i;
	if ( pComboBox == NULL )
	{
		return FALSE;
	}
	
	m_pComboBox = pComboBox;

	CByteArray nPort;
	TCHAR szPort[MAX_STRING_LEN];
    nPort.RemoveAll();
	COMMCONFIG nConfig;
	DWORD nPortSize;
	
	for(i=0;i<MAX_COM_NUM;i++)
	{
		_stprintf_s(szPort,_T("COM%d"),i);
		nPortSize=sizeof(nConfig);
		if(GetDefaultCommConfig(szPort,&nConfig,&nPortSize))
		{
			nPort.Add(i);
		}
	}
	
	CString strList;
	for(i=0;i<nPort.GetSize();i++)
	{
		strList.Format(_T("COM%d"),nPort[i]);
		m_pComboBox->AddString(strList);
	}
	m_pComboBox->SetCurSel(setCurSel);

	return TRUE;
}

BOOL CComSet::InterfaceInit(CString* pstrPortId ,int* nGetPortNum)
{
	int i;
	if (pstrPortId == NULL)
	{
		return FALSE;
	}

	CByteArray nPort;
	TCHAR szPort[MAX_STRING_LEN];
	nPort.RemoveAll();
	COMMCONFIG nConfig;
	DWORD nPortSize;

	for (i = 0; i < MAX_COM_NUM; i++)
	{
		_stprintf_s(szPort, _T("COM%d"), i);
		nPortSize = sizeof(nConfig);
		if (GetDefaultCommConfig(szPort, &nConfig, &nPortSize))
		{
			nPort.Add(i);
		}
	}
	*nGetPortNum = nPort.GetSize();
	CString strList;
	for (i = 0; i < nPort.GetSize(); i++)
	{
		strList.Format(_T("COM%d"), nPort[i]);
		pstrPortId[i] = strList;
	}

	return TRUE;
}


BOOL CComSet::OpenPort(unsigned int baud, HWND hWnd)
{
	if ( m_pComboBox == NULL )
	{
		return FALSE;
	}

	char  szPort[MAX_STRING_LEN] = {0};
	CString strCom;
	m_pComboBox->GetLBText( m_pComboBox->GetCurSel(), strCom );
	
	if (strCom.GetLength() > 4)	/**< 串口编号\\\\.\\大于9的 *///
	{
		sprintf_s(szPort,"\\\\.\\COM%s%s",strCom.Mid(3,strCom.GetLength()-3), strCom.Mid(4, strCom.GetLength() - 3));
	}
	else
	{
		sprintf_s(szPort,"COM%s",strCom.Mid(3,strCom.GetLength()-3));
	}
	
	m_MyWin32Port.CloseMyWin32Port();
 	m_MyWin32Port.OpenMyWin32Port(hWnd,szPort,baud,'N',8,1,0,0,0,0,0);

	if (!IsOpen())
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CComSet::OpenPort(unsigned int baud, HWND hWnd, CString strCom)
{
	char  szPort[MAX_STRING_LEN] = { 0 };

	if (strCom.GetLength() > 4)	/**< 串口编号\\\\.\\大于9的 *///
	{
		sprintf_s(szPort, "\\\\.\\COM%s%s", strCom.Mid(3, strCom.GetLength() - 3), strCom.Mid(4, strCom.GetLength() - 3));
	}
	else
	{
		sprintf_s(szPort, "COM%s", strCom.Mid(3, strCom.GetLength() - 3));
	}

	m_MyWin32Port.CloseMyWin32Port();
	m_MyWin32Port.OpenMyWin32Port(hWnd, szPort, baud, 'N', 8, 1, 0, 0, 0, 0, 0);

	if (!IsOpen())
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CComSet::ClosePort()
{
	m_MyWin32Port.CloseMyWin32Port();
	if (IsOpen())
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CComSet::IsOpen()
{
	return m_MyWin32Port.m_bIsOpen;
}

BOOL CComSet::Send(unsigned char *pData, unsigned int nByteLen)
{
	if ( pData == NULL )
	{
		return FALSE;
	}

	m_MyWin32Port.Write(pData, nByteLen);

	return TRUE;
}

int CComSet::RXSpaceUsed()
{
	return m_MyWin32Port.RXSpaceUsed();
}

int CComSet::Read( unsigned char *pData, unsigned int nByteLen, long milliseconds)
{
	return m_MyWin32Port.Read(pData, nByteLen, milliseconds);
}
