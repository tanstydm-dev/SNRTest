//
//  DCB32.H
//
//  Source code from:
//
//  Serial Communications: A C++ Developer's Guide, 2nd Edition
//  by Mark Nelson, M&T Books, 1999
//
//  Please see the book for information on usage.
//
// This file contains the class definition for my private Dcb32
// class. This guy is used internally by the Win32Port class to
// hold the current DCB. It exists only because I can put in a
// few convenience functions that are used to initialize
// members of the DCB. They make code elsewhere in the program
// a lot less cluttered.
//

#ifndef _DCB32_H
#define _DCB32_H

#include <windows.h>

struct Dcb32 : public DCB
{
    Dcb32();
    void SetBaudRate( int rate );
    void SetParity( int parity, RS232Error &error );
    void SetWordLength( int word_length, RS232Error &error );
    void SetStopBits( int stop_bits, RS232Error &error );
    void SetDtr( int value );
    void SetRts( int value );
    void SetXonXoff( int value );
    void SetDtrDsr( int value );
    void SetRtsCts( int value );
};

#endif // #ifndef _DCB32_H

//EOF DCB32.H

