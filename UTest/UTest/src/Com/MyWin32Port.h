// MyWin32Port.h: interface for the MyWin32Port class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYWIN32PORT_H__C736F122_FDAA_11D2_BDEF_00207813663F__INCLUDED_)
#define AFX_MYWIN32PORT_H__C736F122_FDAA_11D2_BDEF_00207813663F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "Win32Port.h"

const int WM_SERIALROPEN = WM_USER + 0x100b;
const int WM_SERIALRXNOTIFY              = WM_USER + 0x1000;
/*const int WM_PARITY_ERROR_NOTIFY           = WM_USER + 0x1001;
const int WM_BREAK_DETECT_NOTIFY           = WM_USER + 0x1002;
const int WM_FRAMING_ERROR_NOTIFY          = WM_USER + 0x1003;
const int WM_HARDWARE_OVERRUN_ERROR_NOTIFY = WM_USER + 0x1004;
const int WM_SOFTWARE_OVERRUN_ERROR_NOTIFY = WM_USER + 0x1005;
const int WM_CTS_NOTIFY                       = WM_USER + 0x1006;
const int WM_DSR_NOTIFY                       = WM_USER + 0x1007;
const int WM_CD_NOTIFY                       = WM_USER + 0x1008;
const int WM_RI_NOTIFY                       = WM_USER + 0x1009;
const int WM_TX_NOTIFY                       = WM_USER + 0x100a;
*/
class MyWin32Port : public Win32Port  
{
public:
	MyWin32Port() {};
    void OpenMyWin32Port(const HWND notify_window,
                 const string &port_name,
                 long baud_rate = UNCHANGED,
                 char parity = UNCHANGED,
                 int word_length = UNCHANGED,
                 int stop_bits = UNCHANGED,
                 int dtr = SET,
                 int rts = SET,
                 int xon_xoff = DISABLE,
                 int rts_cts = DISABLE,
                 int dtr_dsr = DISABLE ) 
	{
     	OpenWin32Port( port_name,
                 baud_rate,
                 parity,
                 word_length,
                 stop_bits,
                 dtr,
                 rts,
                 xon_xoff,
                 rts_cts,
                 dtr_dsr );

        m_hNotifyWindow=notify_window;
    }
    virtual ~MyWin32Port(){} 
	void CloseMyWin32Port()
	{
		CloseWin32Port();
	}
protected :
    void RxNotify( int byte_count )
    {
        ::PostMessage( m_hNotifyWindow, WM_SERIALRXNOTIFY, byte_count, 0 );
    }
/*    void ParityErrorNotify()
    {
        ::PostMessage( m_hNotifyWindow, WM_PARITY_ERROR_NOTIFY, 0, 0 );
    }
    void FramingErrorNotify()
    {
        ::PostMessage( m_hNotifyWindow, WM_FRAMING_ERROR_NOTIFY, 0, 0 );
    }
    void HardwareOverrunErrorNotify()
    {
        ::PostMessage( m_hNotifyWindow, WM_HARDWARE_OVERRUN_ERROR_NOTIFY, 0, 0 );
    }
    void SoftwareOverrunErrorNotify()
    {
        ::PostMessage( m_hNotifyWindow, WM_SOFTWARE_OVERRUN_ERROR_NOTIFY, 0, 0 );
    }
    void BreakDetectNotify()
    {
        ::PostMessage( m_hNotifyWindow, WM_BREAK_DETECT_NOTIFY, 0, 0 );
    }
    void CtsNotify( bool status )
    {
        ::PostMessage( m_hNotifyWindow, WM_CTS_NOTIFY, status, 0 );
    }
    void DsrNotify( bool status )
    {
        ::PostMessage( m_hNotifyWindow, WM_DSR_NOTIFY, status, 0 );
    }
    void CdNotify( bool status )
    {
        ::PostMessage( m_hNotifyWindow, WM_CD_NOTIFY, status, 0 );
    }
    void RiNotify( bool status )
    {
        ::PostMessage( m_hNotifyWindow, WM_RI_NOTIFY, status, 0 );
    }
    void TxNotify()
    {
        ::PostMessage( m_hNotifyWindow, WM_TX_NOTIFY, 0, 0 );
    }*/
    HWND m_hNotifyWindow;
};


#endif // !defined(AFX_MYWIN32PORT_H__C736F122_FDAA_11D2_BDEF_00207813663F__INCLUDED_)
