#define _WIN32_WINNT 0x0501
#define WINVER 0x0501

#include <windows.h>
#include <ctype.h>
#include <vector>
#include <random>
#include <ctime>

// Variável global de controle
volatile bool g_running = true;

// Lista de teclas de comando para randomizar
std::vector<UINT> teclasComando = {
    VK_RETURN, VK_BACK, VK_TAB, VK_SHIFT, VK_CONTROL, VK_MENU, // Alt
    VK_LWIN, VK_RWIN, VK_ESCAPE,
    VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
    VK_DELETE, VK_INSERT, VK_HOME, VK_END, VK_PRIOR, VK_NEXT // Page Up/Down
};

// Inverte letras: A <-> Z, B <-> Y, etc.
char inverterLetra(char c) {
    if (isalpha((unsigned char)c)) {
        if (isupper((unsigned char)c)) return 'Z' - (c - 'A');
        else return 'z' - (c - 'a');
    }
    return c;
}

// Gera uma tecla de comando aleatória diferente da original
UINT teclaComandoAleatoria(UINT original) {
    static std::mt19937 rng((unsigned int)time(nullptr));
    std::vector<UINT> alternativas;

    for (UINT t : teclasComando) {
        if (t != original)
            alternativas.push_back(t);
    }

    if (alternativas.empty()) return original;

    std::uniform_int_distribution<int> dist(0, (int)alternativas.size() - 1);
    return alternativas[dist(rng)];
}

// Hook do teclado
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
            // ESC encerra tudo
            if (p->vkCode == VK_ESCAPE) {
                g_running = false; // Para a thread do mouse
                PostQuitMessage(0); // Encerra a thread do teclado
                return 1;
            }

            BYTE kbdState[256];
            if (!GetKeyboardState(kbdState)) return CallNextHookEx(NULL, nCode, wParam, lParam);

            WCHAR buffer[4] = {0};
            int result = ToUnicode(p->vkCode, p->scanCode, kbdState, buffer, 4, 0);

            // Letra comum: inverter
            if (result == 1 && isalpha(buffer[0])) {
                char original = (char)buffer[0];
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

            // Tecla de comando: randomiza
            for (UINT t : teclasComando) {
                if (p->vkCode == t) {
                    UINT aleatoria = teclaComandoAleatoria(p->vkCode);

                    INPUT in[2] = {};
                    in[0].type = INPUT_KEYBOARD;
                    in[0].ki.wVk = aleatoria;
                    in[0].ki.dwFlags = 0;

                    in[1] = in[0];
                    in[1].ki.dwFlags = KEYEVENTF_KEYUP;

                    SendInput(2, in, sizeof(INPUT));
                    return 1;
                }
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Inversão contínua do movimento do mouse
void MonitorMouse() {
    POINT p;
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    while (g_running) {
        if (GetCursorPos(&p)) {
            SetCursorPos(width - p.x, height - p.y);
        }
        Sleep(10);
    }
}

// Thread para o hook do teclado
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

// Thread para inverter o mouse
DWORD WINAPI MouseThread(LPVOID) {
    MonitorMouse();
    return 0;
}

// Entry point (WinMain evita console)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // Caminho completo do Chrome (altere se necessário)
    const char* chromePath = "C:\Program Files\Google\Chrome\Application";
    const char* url = "https://www.google.com";

    // Tenta abrir o Chrome com a URL
    ShellExecute(NULL, "open", chromePath, url, NULL, SW_SHOWNORMAL);

    // Inicia as threads do teclado e mouse
    HANDLE hKeyboardThread = CreateThread(NULL, 0, HookThread, NULL, 0, NULL);
    HANDLE hMouseThread = CreateThread(NULL, 0, MouseThread, NULL, 0, NULL);

    if (!hKeyboardThread || !hMouseThread) {
        return 1;
    }

    // Aguarda ambas as threads finalizarem
    WaitForSingleObject(hKeyboardThread, INFINITE);
    WaitForSingleObject(hMouseThread, INFINITE);

    // Libera os recursos
    CloseHandle(hKeyboardThread);
    CloseHandle(hMouseThread);

    return 0;
}

