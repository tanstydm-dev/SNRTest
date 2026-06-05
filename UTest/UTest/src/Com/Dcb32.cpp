//
//  DCB32.CPP
//
//  Source code from:
//
//  Serial Communications: A C++ Developer's Guide, 2nd Edition
//  by Mark Nelson, M&T Books, 1999
//
// The m_Dcb member of the Win32Port class is used to hold most
// of the information regarding the current state of the port.
// The only reason that the DCB was encapsulated into this
// class was simply to make the Win32Port code more readable.
// The Win32Port code becomes more readable because a lot of
// the initialization code for the various members is offloaded
// to the Dcb class. Note that this class is derived from the
// DCB structure defined in the WIN32 header files, so any
// function that takes a pointer to a DCB can take a pointer to
// one of these structures as well.

#include "pch.h"
#include <cctype>

#include "rs232.h"
#include "Dcb32.h"

//
// The Dcb32 constructor initializes every member of the base
// class DCB, so that you get to skip all the initialization
// code you would normally have in a class that has a member of
// this type. In particular, the Win32Port constructor gets to
// take advantage of this by passively allowing the Dcb
// constructor to be invoked.
//

Dcb32::Dcb32()
{
    DCBlength = sizeof( DCB ); //Windows API expects this
    BaudRate = 38400;           //Default baud rate
    fBinary = TRUE;            //Under Win32 this should always be true
    fParity = FALSE;           //Our default is no parity
    fOutxCtsFlow = FALSE;      //Default is no RTS/CTS flow control
    fOutxDsrFlow = FALSE;      //Default is no DTR/DSR flow control
    fDtrControl = DTR_CONTROL_ENABLE; //Assert DTR by default
    fDsrSensitivity = FALSE;   //Ignore incoming DSR by default
    fTXContinueOnXoff = FALSE; //No special action on Xoff by default
    fOutX = FALSE;             //No outbound XON/XOFF handshaking
    fInX = FALSE;              //No incoming XON/XOFF handshaking
    fErrorChar = FALSE;           //No special character on line errors
    fNull = FALSE;             //No null detection
    fRtsControl = RTS_CONTROL_ENABLE; //Assert RTS by default
    fAbortOnError = TRUE;      //Always respond to errors
    fDummy2 = 0;
    wReserved = 0;
    XonLim = 128;               //Issue XON when 128 characters left
    XoffLim = 128;             //Issue XOFF when space left = 128
    ByteSize = 8;              //Default word size
    Parity = NOPARITY;         //Default is no parity
    StopBits = ONESTOPBIT;     //Default is one stop bit
    XonChar = 0x11;            //Default XON is ^Q
    XoffChar = 0x13;           //Default XOFF is ^S
    ErrorChar = 0;             //Not using error char, don't care
    EofChar = 0;               //Not using EOF char, don't care
    EvtChar = 0;               //Not using event char, don't care
};

//
// Setting the baud rate is as simple as stuffing it verbatim
// into the DCB. There are some intricacies regarding baud rate
// management in the Win32 API, but we don't get into them
// here.
//
void Dcb32::SetBaudRate( int rate )
{
    if ( rate != UNCHANGED )
        BaudRate = rate;
}

//
// Setting parity is done by passing a single character that is
// the first character in the name of the parity being
// selected. The actual setting of the parity affects two
// members of the DCB. The fParity member tells the driver
// whether or not to pay any attention to parity. The Parity
// member tells it which parity to use if fParity is set.

void Dcb32::SetParity( int parity, RS232Error &error )
{
    switch ( toupper( parity ) ) {
    case 'N'       : fParity = FALSE; Parity = NOPARITY;    break;
    case 'E'       : fParity = TRUE;  Parity = EVENPARITY;  break;
    case 'O'       : fParity = TRUE;  Parity = ODDPARITY;   break;
    case 'M'       : fParity = TRUE;  Parity = MARKPARITY;  break;
    case 'S'       : fParity = TRUE;  Parity = SPACEPARITY; break;
    case UNCHANGED :                              break;
    default        : error = RS232_ILLEGAL_PARITY_SETTING;
    }
};

//
// The word length setting is trivial.
//
void Dcb32::SetWordLength( int word_length, RS232Error &error )
{
    switch ( word_length ) {
    case 5         : ByteSize = 5; break;
    case 6         : ByteSize = 6; break;
    case 7         : ByteSize = 7; break;
    case 8         : ByteSize = 8; break;
    case UNCHANGED : break;
    default        : error = RS232_ILLEGAL_WORD_LENGTH;
    }
}

//
// Setting the number of stop bits is nice and simple. I don't
// take into account the possibility of 1.5 stop bits in this
// function, as it really serves no practical purpose.
//
void Dcb32::SetStopBits( int stop_bits, RS232Error &error )
{
    switch ( stop_bits ) {
    case 1              : StopBits = ONESTOPBIT;  break;
    case 2              : StopBits = TWOSTOPBITS; break;
    case UNCHANGED    :                            break;
    default           : error = RS232_ILLEGAL_STOP_BITS; break;
    }
}

//
// Setting DTR is fairly simple. Note that this has an
// unintended side effect. If DTR/DSR handshaking was enabled
// before this call, it won't be any more. So you should either
// call this function before you set up handshaking, or skip
// the call if handshaking is enabled.
//
void Dcb32::SetDtr( int value )
{
    if ( value == UNCHANGED )
        return;
    else if ( value )
        fDtrControl = DTR_CONTROL_ENABLE;
    else
        fDtrControl = DTR_CONTROL_DISABLE;
}

//
// Setting RTS is fairly simple. Note that this has an
// unintended side effect. If RTS/CTS handshaking was enabled
// before this call, it won't be any more. So you should either
// call this function before you set up handshaking, or skip
// the call if handshaking is enabled.
//
void Dcb32::SetRts( int value )
{
    if ( value == UNCHANGED )
        return;
    else if ( value )
        fRtsControl = RTS_CONTROL_ENABLE;
    else
        fRtsControl = RTS_CONTROL_DISABLE;
}

//
// This function either turns XON/XOFF handshking on or off. It
// assumes that the high and low water marks have already been
// set to where you would like them, and that the definitons of
// the XON and XOFF characters have already been made. Since
// these are set up in the Dcb32 constructor, the odds are that
// they still have good values.
//
void Dcb32::SetXonXoff( int value )
{
    if ( value != UNCHANGED ) {
        if ( value ) {
            fOutX = TRUE;
            fInX = TRUE;
        } else {
            fOutX = FALSE;
            fInX = FALSE;
        }
    }
}

//
// If DTR/DSR handshaking is to be enabled, this function sets
// the control bits for both input and output handshaking. If
// handshaking is not to be enabled, the inbound sensitivity
// bit is turned off. However, we don't do anything about the
// outbound control of DTR if handshaking is to be turned off.
// We assume that the caller will have already set DTR to the
// setting they desire.
//

void Dcb32::SetDtrDsr( int value )
{
    if ( value != UNCHANGED ) {
        if ( value ) {
            fDtrControl = DTR_CONTROL_HANDSHAKE;
            fOutxDsrFlow = TRUE;
        } else
            fOutxDsrFlow = FALSE;
    }
}

//
// If RTS/CTS handshaking is to be enabled, this function sets
// the control bits for both input and output handshaking. If
// handshaking is not to be enabled, the inbound sensitivity
// bit is turned off. However, we don't do anything about the
// outbound control of RTS if handshaking is to be turned off.
// We assume that the caller will have already set RTS to the
// setting they desire.
//
void Dcb32::SetRtsCts( int value )
{
    if ( value != UNCHANGED ) {
        if ( value ) {
            fRtsControl = RTS_CONTROL_HANDSHAKE;
            fOutxCtsFlow = TRUE;
        } else
            fOutxCtsFlow = FALSE;
    }
}

//EOF Dcb32.cpp
