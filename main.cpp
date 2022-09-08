#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif
#include "Header.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");



int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(MENU_ID);                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("Code::Blocks Template Windows App"),       /* Title Text */
               WS_OVERLAPPEDWINDOW|WS_VSCROLL|WS_HSCROLL, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               lpszArgument                /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static Input InPut;
    static Output OutPut;
    static int myScrollSize=1000;
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {
        RECT rect;
        GetClientRect(hwnd,&rect);

        LOGFONT lf;
        GetObject(GetStockObject(ANSI_FIXED_FONT), sizeof(LOGFONT), &lf);

        SetConst(&OutPut, &InPut, lf, rect, lParam);

        MyReadFile(&OutPut, &InPut, hwnd, wParam, lParam);

    }
    break;
    case WM_SIZE:
    {
        if(!OutPut.MoodWork)
        {
            SettingScrollRangeWithoutCoding(&OutPut, &InPut, hwnd, wParam, lParam);
        }
        else
        {
            SettingScrollRangeWithCoding(&OutPut, &InPut, hwnd, wParam, lParam);
        }
        InvalidateRect(hwnd,NULL,TRUE);
    }
    break;
    case WM_VSCROLL:
    {
        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            if(OutPut.ScrollInfo.y<=0)
            {
                break;
            }
            OutPut.ScrollInfo.y--;

            break;
        case SB_LINEDOWN:
            if(OutPut.ScrollInfo.y>InPut.CountStr-OutPut.SizeWindow.y/OutPut.lf.lfHeight-1)
            {
                break;
            }
            OutPut.ScrollInfo.y++;

            break;
        case SB_THUMBTRACK:
            if(InPut.CountStr>myScrollSize)
            {
                OutPut.ScrollInfo.y=HIWORD(wParam)/(double)myScrollSize*(double)InPut.CountStr;
                if(OutPut.ScrollInfo.y>InPut.CountStr-OutPut.SizeWindow.y/OutPut.lf.lfHeight)
                {
                    OutPut.ScrollInfo.y-=OutPut.SizeWindow.y/OutPut.lf.lfHeight;
                }
            }
            else
            {
                OutPut.ScrollInfo.y = HIWORD(wParam);
            }

            break;
        default:
            return 0;
        }
        if(InPut.CountStr>myScrollSize)
        {
            SetScrollPos(hwnd,SB_VERT, OutPut.ScrollInfo.y*(double)(myScrollSize/(double)InPut.CountStr), TRUE);
        }
        else
        {
            SetScrollPos(hwnd,SB_VERT, OutPut.ScrollInfo.y, TRUE);
        }
        InvalidateRect(hwnd,NULL,TRUE);
    }
    break;
    case WM_HSCROLL:
        {
    switch (LOWORD(wParam)) {
            case SB_LINERIGHT:

                if(OutPut.ScrollInfo.x>=(InPut.MaxSizeStr-OutPut.SizeWindow.x/OutPut.lf.lfWidth)){
                    break;
                }
                OutPut.ScrollInfo.x++;

                break;
            case SB_LINELEFT:

                if(OutPut.ScrollInfo.x<=0){
                    break;
                }
                OutPut.ScrollInfo.x--;

                break;
            case SB_THUMBTRACK:

                OutPut.ScrollInfo.x = HIWORD(wParam);

                break;
            default: return 0;
            }

            SetScrollPos(hwnd,SB_HORZ, OutPut.ScrollInfo.x, TRUE);

            InvalidateRect(hwnd,NULL,TRUE);

            }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc=BeginPaint(hwnd,&ps);

        SelectObject(hdc,OutPut.hfont1);
        if(OutPut.MoodWork)
        {
            PaintWithCoding(OutPut, InPut,hdc);
        }
        else
        {
            PaintWithoutLCoding(OutPut, InPut,hdc);
        }
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case MENU_FILE_OPEN:
        {
            OPENFILENAME ofn;
            char filename[255];


            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = filename;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(filename);
            ofn.lpstrFilter = "*.TXT\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn) == TRUE)
            {
                printf("\nfile opened\n");

                clearEverything(&OutPut,&InPut);
                InPut.FileName=ofn.lpstrFile;
                MyReadFile(&OutPut, &InPut, hwnd, wParam, lParam);

                if (OutPut.MoodWork == TRUE)
                {
                    SettingScrollRangeWithCoding(&OutPut, &InPut, hwnd, wParam, lParam);
                }
                else
                {
                    SettingScrollRangeWithoutCoding(&OutPut, &InPut, hwnd, wParam, lParam);
                }

            }
        }
        break;
        case MENU_FILE_CLOSE:
        {
            clearEverything(&OutPut,&InPut);
            InvalidateRect(hwnd, NULL, TRUE);
            ShowScrollBar(hwnd, SB_HORZ, FALSE);
            ShowScrollBar(hwnd, SB_VERT, FALSE);

        }
        break;
        case MENU_FILE_EXIT:
        {

             clearEverything(&OutPut,&InPut);
             PostQuitMessage (0);

        }
        break;
        case MENU_CHANGE_MODE:
        {
            int mode = GetMenuState(GetMenu(hwnd), MENU_CHANGE_MODE, MF_BYCOMMAND);
            if (mode & MF_CHECKED)
            {
                CheckMenuItem(GetMenu(hwnd), MENU_CHANGE_MODE, MF_BYCOMMAND | MF_UNCHECKED);
                OutPut.MoodWork = FALSE;
            }
            else
            {
                CheckMenuItem(GetMenu(hwnd), MENU_CHANGE_MODE, MF_BYCOMMAND | MF_CHECKED);
                OutPut.MoodWork = TRUE;
            }
            if(!OutPut.MoodWork)
            {
                SettingScrollRangeWithoutCoding(&OutPut, &InPut, hwnd, wParam, lParam);
            }
            else
            {
                SettingScrollRangeWithCoding(&OutPut, &InPut, hwnd, wParam, lParam);
            }

        }
        break;
        }
    }
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
