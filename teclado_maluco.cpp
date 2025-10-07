#define _WIN32_WINNT 0x0501
#define WINVER 0x0501

#include <windows.h>
#include <ctype.h>

// Inverte letras: A <-> Z, B <-> Y, etc.
char inverterLetra(char c) {
    if (isalpha((unsigned char)c)) {
        if (isupper((unsigned char)c)) return 'Z' - (c - 'A');
        else return 'z' - (c - 'a');
    }
    return c;
}

// Hook do teclado
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

        // Tecla ESC encerra
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && p->vkCode == VK_ESCAPE) {
            PostQuitMessage(0);
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        }

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            BYTE kbdState[256];
            if (!GetKeyboardState(kbdState)) return CallNextHookEx(NULL, nCode, wParam, lParam);

            WCHAR buffer[4] = {0};
            int result = ToUnicode(p->vkCode, p->scanCode, kbdState, buffer, 4, 0);

            if (result == 1) {
                char original = (char)buffer[0];
                if (isalpha((unsigned char)original)) {
                    char invertido = inverterLetra(original);

                    INPUT in[2] = {};
                    in[0].type = INPUT_KEYBOARD;
                    in[0].ki.wVk = 0;
                    in[0].ki.wScan = invertido;
                    in[0].ki.dwFlags = KEYEVENTF_UNICODE;

                    in[1] = in[0];
                    in[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

                    SendInput(2, in, sizeof(INPUT));
                    return 1;
                }
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Thread do hook
DWORD WINAPI HookThread(LPVOID) {
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (!hook) return 1;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hook);
    return 0;
}

// Entry point (WinMain evita console)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    HANDLE hThread = CreateThread(NULL, 0, HookThread, NULL, 0, NULL);
    if (!hThread) return 1;

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    return 0;
}

