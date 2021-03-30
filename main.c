#include <windows.h>
#include "resource.h"
#include <stdio.h>

const char g_szClassName[] = "textPad";
#define IDC_MAIN_EDIT     101

BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwFileSize;

        dwFileSize = GetFileSize(hFile, NULL);
        if(dwFileSize != 0xFFFFFFFF)
        {
            LPSTR pszFileText;
            pszFileText = GlobalAlloc(GPTR, dwFileSize + 1);

            if(pszFileText != NULL)
            {
                DWORD dwRead;

                if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
                   {
                       pszFileText[dwFileSize] = 0;
                       if(SetWindowText(hEdit, pszFileText))
                            bSuccess = TRUE;
                   }
                   GlobalFree(pszFileText);
            }
        }
    }
}

BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwTextLenght;
        dwTextLenght = GetWindowTextLength(hEdit); //unicode

        if(dwTextLenght > 0)
        {
            LPSTR pszText;
            DWORD dwBufferSize = dwTextLenght + 1;

            pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
            if(pszText != NULL)
            {
                if(GetWindowText(hEdit, pszText, dwBufferSize)) //unicode
                {
                    DWORD dwWritten;

                    if(WriteFile(hFile, pszText, dwTextLenght, &dwWritten, NULL))
                        bSuccess = TRUE;
                }
                GlobalFree(pszText);
            }
        }
        CloseHandle(hFile);
    }
    return bSuccess;
}

void DoFileOpen(HWND hwnd)
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if(GetOpenFileName(&ofn)) //unicode
    {
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        LoadTextFileToEdit(hEdit, szFileName);
    }
}

void DoFileSave(HWND hwnd)
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "txt";

    if(GetSaveFileName(&ofn)) //unicode
    {
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        SaveTextFileFromEdit(hEdit, szFileName);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
    case WM_CREATE: {
        HFONT hfDefault;
        HWND hEdit;

        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL |
                               ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE, 0, 0, 100, 100,
                               hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);

        if(hEdit == NULL)
            MessageBox(hwnd, "Could not create IDC_MAIN_EDIT", "Error", MB_RETRYCANCEL | MB_ICONSTOP);

        hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

        break;
    }
    case WM_SIZE: {
        HWND hEdit;
        RECT rcClient;

        GetClientRect(hwnd, &rcClient);

        hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
        break;
    }
    case WM_CLOSE: {
        ShowWindow( GetConsoleWindow(), SW_SHOW);
        DestroyWindow(hwnd);
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    case WM_COMMAND: {
        switch(LOWORD(wParam))
        {
            //FILE
            case ID_FILE_NEW:
                SetDlgItemText(hwnd, IDC_MAIN_EDIT, "");
                break;
            case ID_FILE_OPEN:
                DoFileOpen(hwnd);
                break;
            case ID_FILE_SAVEAS:
                DoFileSave(hwnd);
                break;
            case ID_FILE_EXIT:
                //MessageBox(hwnd, "Are you sure do you want to quit?", "Warning", MB_ICONWARNING | MB_YESNO);
                PostMessage(hwnd, WM_CLOSE, 0, 0);
                break;
            case ID_EDIT_CUT:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_CUT, 0, 0);
                break;
            case ID_EDIT_COPY:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_COPY, 0, 0);
                break;
            case ID_EDIT_PASTE:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_PASTE, 0, 0);
                break;
            case ID_EDIT_SELECTALL:
                //SendDlgItemMessage(hwnd, IDC_MAIN_EDIT,)
                break;
            case ID_EDIT_UNDO:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_UNDO, 0, 0);
                break;
            case ID_EDIT_REDO:
                //SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_)
                break;
            case ID_HELP_ABOUT:
                MessageBox(hwnd, "TextPad by Phroton, Version 0.1.0.2-alpha build 36", "About", MB_OK | MB_ICONINFORMATION);
                break;
            case ID_HELP_VIEWONGITHUB: {
                char linkGithub[35] = "https://github.com/PhrotonX/TextPad";
                ShellExecute(NULL, "open", linkGithub, NULL, NULL, SW_SHOWNORMAL);
                break;
            }
        }

        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize           =   sizeof(WNDCLASSEX);
    wc.style            =   0;
    wc.lpfnWndProc      =   WndProc;
    wc.cbClsExtra       =   0;
    wc.cbWndExtra       =   0;
    wc.hInstance        =   hInstance;
    wc.hIcon            =   LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor          =   LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    =   (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName     =   (HMENU)MAKEINTRESOURCE(IDR_MENU1);
    wc.lpszClassName    =   g_szClassName;
    wc.hIconSm          =   LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, g_szClassName, "TextPad", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 760, 420, NULL, NULL, hInstance, NULL);


    if(hwnd == NULL){
        MessageBox(NULL, "Error", "Window Creation Failed", MB_ICONHAND | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    ShowWindow( GetConsoleWindow(), SW_HIDE);
    UpdateWindow(hwnd);

    while(GetMessage(&Msg, NULL, 0, 0) > 0){
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
