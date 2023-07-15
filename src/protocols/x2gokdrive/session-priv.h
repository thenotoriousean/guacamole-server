#ifndef __GUAC_KDRIVE_SEESION_PRIV_H__
#define __GUAC_KDRIVE_SEESION_PRIV_H__

// version 4 supports clipboard
#define FEATURE_VERSION 4

// Version of client OS for same reason
#define OS_VERSION 3

//connection type
#define TCP 1
#define UDP 2

// events
#define KEYPRESS 2
#define KEYRELEASE 3
#define MOUSEPRESS 4
#define MOUSERELEASE 5
#define MOUSEMOTION 6
#define GEOMETRY 7
#define UPDATE 8
#define SELECTIONEVENT 9
#define CLIENTVERSION 10
#define DEMANDSELECTION 11
#define KEEPALIVE 12
#define OPENUDP 17

#define EVENT_LENGTH 41

// mouse events
#define MouseLeft (1 << 0)
#define MouseMiddle (1 << 1)
#define MouseRight (1 << 2)
#define MouseUp (1 << 3)
#define MouseDown (1 << 4)

#define Button1Mask (1 << 8)
#define Button2Mask (1 << 9)
#define Button3Mask (1 << 10)
#define Button4Mask (1 << 11)
#define Button5Mask (1 << 12)

#define Button1 1
#define Button2 2
#define Button3 3
#define Button4 4
#define Button5 5

// keyboard events
#define XK_LeftShift   0xffe1
#define XK_RightShift  0xffe2
#define XK_LeftCtrl    0xffe3
#define XK_RightCtrl   0xffe4
#define XK_LeftMeta    0xffe7
#define XK_RightMeta   0xffe8
#define XK_LeftAlt     0xffe9
#define XK_RightAlt    0xffea
#define XK_Menu        0xff67

#define ShiftMask (1 << 0)
#define LockMask (1 << 1)
#define ControlMask (1 << 2)
#define Mod1Mask (1 << 3)
#define Mod2Mask (1 << 4)
#define Mod3Mask (1 << 5)
#define Mod4Mask (1 << 6)
#define Mod5Mask (1 << 7)

// header
#define HEADER_SIZE 56
#define REGION_SIZE 64

// data types
#define HEADER 0
#define FRAMEREGION 1
#define REGIONDATA 2
#define CURSORDATA 3
#define CURSORLIST 4
#define FRAMELIST 5
#define SELECTIONBUFFER 6
#define WINUPDATEBUFFER 7

// header types
#define FRAME 0
#define DELETEDFRAMES 1
#define CURSOR 2
#define DELETEDCURSORS 3
#define SELECTION 4
#define SERVER_VERSION 5
#define DEMANDCLIENTSELECTION 6
#define REINIT 7
#define WINUPDATE 8
#define H264STREAM 14

// selection mimes
#define STRING 0
#define UTF_STRING 1
#define PIXMAP 2

// selection types
#define PRIMARY 0
#define CLIPBOARD 1

// image type
#define TYPE_FRAME "frame"
#define TYPE_CURSOR "cursor"

#endif