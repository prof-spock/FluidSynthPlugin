/**
 * @file
 * The <C>MyWindows</C> file specifies the required interface to the
 * windows API.
 *
 * @author Dr. Thomas Tensi
 * @date   2023-08
 */

/*====================*/
  
#pragma once

namespace Windows {

    /*===========*/
    /* constants */
    /*===========*/

    /* maximum path length */
    #define MAX_PATH 1000

    /* constants for dynamic loading of libraries */
    #define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR     0x00000100
    #define LOAD_LIBRARY_SEARCH_APPLICATION_DIR  0x00000200
    #define LOAD_LIBRARY_SEARCH_SYSTEM32         0x00000800
    #define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS     0x00001000

    /* constants for dynamic library module handles */
    #define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT  0x00000002
    #define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS        0x00000004

    /* bitmap creation constants */
    #define CBM_INIT 4
    
    /* brush style constants */
    #define COLOR_WINDOW 5

    /* code page constants */
    #define CP_ACP 0

    /* window class style constants */
    #define CS_GLOBALCLASS 0x4000
    #define CS_HREDRAW     0x0002
    #define CS_VREDRAW     0x0001

    /* window messages */
    #define WM_CHAR    0x0102
    #define WM_DESTROY 0x0002
    #define WM_PAINT   0x000F

    /* window style constants */
    #define WS_CAPTION     0x00C00000L
    #define WS_MINIMIZEBOX 0x00010000L
    #define WS_SYSMENU     0x00080000L
    #define WS_SIZEBOX     0x00040000L
    #define WS_VISIBLE     0x10000000L

    /* text alignment constants */
    #define DT_CENTER      0x0001
    #define DT_LEFT        0x0000
    #define DT_RIGHT       0x0002
    #define DT_SINGLELINE  0x0020
    #define DT_VCENTER     0x0004

    /* font weight constants */
    #define FW_BOLD        700
    #define FW_NORMAL      400

    /* logical font face size */
    #define LF_FACESIZE    32

    /* raster operations for bitmap blitting */
    #define HALFTONE 4
    #define SRCCOPY 13369376
    
    /* background mode */
    #define TRANSPARENT    0

    /*=======*/
    /* types */
    /*=======*/

    #define DLLImport  __declspec(dllimport)
    #define STDCALL    __stdcall

    typedef int BOOL;
    typedef unsigned char BYTE;
    typedef unsigned long DWORD;
    typedef __int64 (STDCALL* FARPROC)();
    typedef void* HANDLE;
    typedef const char* LPCSTR;
    typedef const wchar_t* LPCWSTR;
    typedef void* LPVOID;
    typedef long LONG;
    typedef unsigned int UINT;
    typedef wchar_t WCHAR;    
    typedef unsigned short WORD;
    typedef void VOID;

    #if defined(_WIN64)
       typedef __int64 LONG_PTR;
       typedef unsigned __int64 UINT_PTR;
    #else
       typedef long LONG_PTR;
       typedef unsigned int UINT_PTR;
    #endif

    typedef WORD ATOM;
    typedef DWORD COLORREF;    
    typedef HANDLE HBITMAP;
    typedef HANDLE HBRUSH;
    typedef HANDLE HCURSOR;
    typedef HANDLE HDC;
    typedef HANDLE HFONT;
    typedef HANDLE HGDIOBJ;
    typedef HANDLE HICON;
    typedef HANDLE HINSTANCE;
    typedef HANDLE HMENU;
    typedef HINSTANCE HMODULE;
    typedef HANDLE HWND;
    typedef LONG_PTR LPARAM;
    typedef WCHAR* LPWSTR;
    typedef LONG_PTR LRESULT;
    typedef UINT_PTR WPARAM;

    typedef LRESULT (*WNDPROC) (HWND, UINT, WPARAM, LPARAM);

    /* structures */
    typedef struct tagRGBQUAD {
        BYTE rgbBlue;
        BYTE rgbGreen;
        BYTE rgbRed;
        BYTE rgbReserved;
    } RGBQUAD;

    typedef struct tagBITMAP {
        LONG   bmType;
        LONG   bmWidth;
        LONG   bmHeight;
        LONG   bmWidthBytes;
        WORD   bmPlanes;
        WORD   bmBitsPixel;
        LPVOID bmBits;
    } BITMAP, *PBITMAP, *NPBITMAP, *LPBITMAP;

    typedef struct tagBITMAPINFOHEADER {
        DWORD biSize;
        LONG  biWidth;
        LONG  biHeight;
        WORD  biPlanes;
        WORD  biBitCount;
        DWORD biCompression;
        DWORD biSizeImage;
        LONG  biXPelsPerMeter;
        LONG  biYPelsPerMeter;
        DWORD biClrUsed;
        DWORD biClrImportant;
    } BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

    typedef struct tagBITMAPINFO {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD          bmiColors[1];
    } BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;

    typedef struct tagPOINT {
        LONG x;
        LONG y;
    } POINT, * PPOINT, * NPPOINT, * LPPOINT;

    typedef struct tagMSG {
        HWND   hwnd;
        UINT   message;
        WPARAM wParam;
        LPARAM lParam;
        DWORD  time;
        POINT  pt;
        DWORD  lPrivate;
    } MSG, *PMSG, *NPMSG, *LPMSG;

    typedef struct tagDRAWTEXTPARAMS {
        UINT cbSize;
        int  iTabLength;
        int  iLeftMargin;
        int  iRightMargin;
        UINT uiLengthDrawn;
    } DRAWTEXTPARAMS, *LPDRAWTEXTPARAMS;
    
    typedef struct tagLOGFONTW {
        LONG lfHeight;
        LONG lfWidth;
        LONG lfEscapement;
        LONG lfOrientation;
        LONG lfWeight;
        BYTE lfItalic;
        BYTE lfUnderline;
        BYTE lfStrikeOut;
        BYTE lfCharSet;
        BYTE lfOutPrecision;
        BYTE lfClipPrecision;
        BYTE lfQuality;
        BYTE lfPitchAndFamily;
        WCHAR lfFaceName[LF_FACESIZE];
    } LOGFONTW;

    typedef struct tagRECT {
        LONG left;
        LONG top;
        LONG right;
        LONG bottom;
    } RECT, *PRECT, *NPRECT, *LPRECT;
    
    typedef struct tagPAINTSTRUCT {
        HDC  hdc;
        BOOL fErase;
        RECT rcPaint;
        BOOL fRestore;
        BOOL fIncUpdate;
        BYTE rgbReserved[32];
    } PAINTSTRUCT, *PPAINTSTRUCT, *NPPAINTSTRUCT, *LPPAINTSTRUCT;
    
    typedef struct tagWNDCLASSEXW {
        UINT      cbSize;
        UINT      style;
        WNDPROC   lpfnWndProc;
        int       cbClsExtra;
        int       cbWndExtra;
        HINSTANCE hInstance;
        HICON     hIcon;
        HCURSOR   hCursor;
        HBRUSH    hbrBackground;
        LPCWSTR   lpszMenuName;
        LPCWSTR   lpszClassName;
        HICON     hIconSm;
    } WNDCLASSEXW;

    /*=====================*/
    /* function prototypes */
    /*=====================*/

    extern "C" DLLImport HDC BeginPaint (HWND hWnd,
                                         LPPAINTSTRUCT lpPaint);

    extern "C" DLLImport HBITMAP CreateDIBSection (HDC hdc,
                                                   const BITMAPINFO *pbmi,
                                                   UINT usage,
                                                   VOID **ppvBits,
                                                   HANDLE hSection,
                                                   DWORD offset);
    
    extern "C" DLLImport HFONT CreateFontIndirectW (const LOGFONTW *lplf);
    
    extern "C" DLLImport HBRUSH CreateSolidBrush (COLORREF color);    

    extern "C" DLLImport HWND CreateWindowExW (DWORD     dwExStyle,
                                               LPCWSTR   lpClassName,
                                               LPCWSTR   lpWindowName,
                                               DWORD     dwStyle,
                                               int       X,
                                               int       Y,
                                               int       nWidth,
                                               int       nHeight,
                                               HWND      hWndParent,
                                               HMENU     hMenu,
                                               HINSTANCE hInstance,
                                               LPVOID    lpParam);

    extern "C" DLLImport LRESULT DefWindowProcW (HWND hWnd,
                                                 UINT msg,
                                                 WPARAM wParam,
                                                 LPARAM lParam);

    extern "C" DLLImport BOOL DeleteObject (HGDIOBJ handle);

    extern "C" DLLImport BOOL DestroyWindow (HWND hWnd);

    extern "C" DLLImport LRESULT DispatchMessageW (const MSG *lpMsg);

    extern "C" DLLImport int DrawTextExW (HDC hdc,
                                          LPWSTR lpchText,
                                          int cchText,
                                          LPRECT lprc,
                                          UINT format,
                                          LPDRAWTEXTPARAMS lpdtp);    

    extern "C" DLLImport BOOL EndPaint (HWND hWnd,
                                        const PAINTSTRUCT *lpPaint);    

    extern "C" DLLImport int FillRect (HDC hDC,
                                       const RECT *lprc,
                                       HBRUSH hbr);

    extern "C" DLLImport BOOL STDCALL FreeLibrary (HMODULE hLibModule);

    extern "C" DLLImport BOOL GetClientRect (HWND hWnd, LPRECT lpRect);
    
    extern "C" DLLImport HDC GetDC (HWND hWnd);

    extern "C" DLLImport DWORD GetLastError ();

    extern "C" DLLImport BOOL GetMessageW (LPMSG lpMsg,
                                           HWND  hWnd,
                                           UINT  wMsgFilterMin,
                                           UINT  wMsgFilterMax);

    extern "C" DLLImport DWORD GetModuleFileNameW (HMODULE hModule,
                                                   LPWSTR  lpFilename,
                                                   DWORD   nSize);

    extern "C" DLLImport BOOL GetModuleHandleExW (DWORD    dwFlags,
                                                  LPCWSTR  lpModuleName,
                                                  HMODULE* phModule);

    extern "C" DLLImport HMODULE GetModuleHandleW (LPCWSTR lpModuleName);

    extern "C" DLLImport int GetObjectW (HANDLE h,
                                         int byteCount, LPVOID buffer);

    extern "C" DLLImport FARPROC STDCALL GetProcAddress (HMODULE hModule,
                                                         LPCSTR lpProcName);

    extern "C" DLLImport BOOL InvalidateRect (HWND hWnd,
                                              const RECT *lpRect,
                                              BOOL bErase);

    extern "C" DLLImport BOOL IsWindow (HWND hWnd);

    extern "C" DLLImport HMODULE STDCALL LoadLibraryExA (LPCSTR lpLibFileName,
                                                         HANDLE hFile,
                                                         DWORD dwFlags);

    extern "C" DLLImport BOOL MoveWindow (HWND hWnd,
                                          int  X,
                                          int  Y,
                                          int  nWidth,
                                          int  nHeight,
                                          BOOL bRepaint);

    extern "C" DLLImport int MultiByteToWideChar (UINT codePage,
                                                  DWORD dwFlags,
                                                  LPCSTR lpMultiByteStr,
                                                  int cbMultiByte,
                                                  LPWSTR lpWideCharStr,
                                                  int cchWideChar);


    extern "C" DLLImport BOOL PeekMessageW (LPMSG lpMsg,
                                            HWND  hWnd,
                                            UINT  wMsgFilterMin,
                                            UINT  wMsgFilterMax,
                                            UINT  wRemoveMsg);

    extern "C" DLLImport void PostQuitMessage (int nExitCode);

    extern "C" DLLImport ATOM RegisterClassExW (LPVOID wndClass);

    extern "C" DLLImport HGDIOBJ SelectObject (HDC hdc, HGDIOBJ j);

    extern "C" DLLImport int SetBkMode (HDC hdc, int mode);

    extern "C" DLLImport BOOL SetDllDirectoryW (LPCWSTR lpPathName);

    extern "C" DLLImport void SetLastError(DWORD dwErrCode);

    extern "C" DLLImport int SetStretchBltMode (HDC hdc, int mode);

    extern "C" DLLImport COLORREF SetTextColor (HDC hdc, COLORREF color);

    extern "C" DLLImport void Sleep (DWORD dwMilliseconds);

    extern "C" DLLImport int StretchDIBits (HDC hdc,
                                            int xDest,
                                            int yDest,
                                            int destWidth,
                                            int destHeight,
                                            int xSrc,
                                            int ySrc,
                                            int srcWidth,
                                            int srcHeight,
                                            const VOID *lpBits,
                                            const BITMAPINFO *lpbmi,
                                            UINT iUsage,
                                            DWORD rop);

    extern "C" DLLImport BOOL TranslateMessage (const MSG *lpMsg);

    extern "C" DLLImport BOOL UnregisterClassW (LPCWSTR lpClassName,
                                                HINSTANCE hInstance);

    extern "C" DLLImport BOOL ValidateRect (HWND hWnd,
                                            const RECT *lpRect);

}

/*--------------------*/
/* utility macros     */
/*--------------------*/

#define WND_makeLPCWSTR(target, source, temp) \
    std::wstring temp = std::wstring(source.begin(), source.end()); \
    Windows::LPCWSTR target = temp.c_str();

#define WND_makeLPWSTR(target, source, temp) \
    Windows::WCHAR temp[500];                                      \
    Windows::MultiByteToWideChar(CP_ACP, 0, source.c_str(), -1, temp, 500); \
    Windows::LPWSTR target = &temp[0];
