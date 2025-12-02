#include "fetchbase.h"
#include <windows.h>
#include <cwchar>
#include <cstdio>

#define WINDOW_CLASS_NAME L"SystemInfoAppClassCPP"
#define UPDATE_TIMER_ID 1

HWND hOS, hCPU, hRAM, hHostname, hIP, hUptime;

void UpdateSystemInfo() {
    KDC_OSInfo os;
    wchar_t buffer[256];

    if (get_os_info(&os) == 0) {
        swprintf(buffer, 256, L"OS: %hs %hs (%hs)", os.os_name, os.os_version, os.arch);
        SetWindowTextW(hOS, buffer);
    }

    KDC_CPUInfo cpu;
    if (get_cpu_info(&cpu) == 0) {
        // CHANGED: Divided by 1,000,000 and changed string to MHz
        // Used %llu because frequency_hz is a 64-bit integer
        swprintf(buffer, 256, L"CPU: %hs, Cores: %d, Frequency: %llu MHz", 
                 cpu.name, cpu.cores, cpu.frequency_hz / 1000000);
        SetWindowTextW(hCPU, buffer);
    }

    KDC_RAMInfo ram;
    if (get_ram_info(&ram) == 0) {
        swprintf(buffer, 256, L"RAM: Total=%llu MB, Used=%llu MB, Free=%llu MB",
                 ram.total_bytes / 1024 / 1024, ram.used_bytes / 1024 / 1024, ram.free_bytes / 1024 / 1024);
        SetWindowTextW(hRAM, buffer);
    }

    KDC_NetworkInfo net;
    if (get_network_info(&net) == 0) {
        swprintf(buffer, 256, L"Hostname: %hs", net.hostname);
        SetWindowTextW(hHostname, buffer);
        swprintf(buffer, 256, L"IP: %hs", net.ip_address);
        SetWindowTextW(hIP, buffer);
    }

    KDC_UptimeInfo up;
    if (get_uptime_info(&up) == 0) {
        swprintf(buffer, 64, L"Uptime: %llu seconds", up.uptime_seconds);
        SetWindowTextW(hUptime, buffer);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        // Increased Width to 650 (was 400)
        hOS = CreateWindowExW(0, L"STATIC", L"OS: ...", WS_CHILD | WS_VISIBLE, 20, 20, 650, 20, hwnd, NULL, NULL, NULL);
        hCPU = CreateWindowExW(0, L"STATIC", L"CPU: ...", WS_CHILD | WS_VISIBLE, 20, 45, 650, 20, hwnd, NULL, NULL, NULL);
        hRAM = CreateWindowExW(0, L"STATIC", L"RAM: ...", WS_CHILD | WS_VISIBLE, 20, 70, 650, 20, hwnd, NULL, NULL, NULL);
        hHostname = CreateWindowExW(0, L"STATIC", L"Hostname: ...", WS_CHILD | WS_VISIBLE, 20, 95, 650, 20, hwnd, NULL, NULL, NULL);
        hIP = CreateWindowExW(0, L"STATIC", L"IP: ...", WS_CHILD | WS_VISIBLE, 20, 120, 650, 20, hwnd, NULL, NULL, NULL);
        hUptime = CreateWindowExW(0, L"STATIC", L"Uptime: ...", WS_CHILD | WS_VISIBLE, 20, 145, 650, 20, hwnd, NULL, NULL, NULL);
        
        UpdateSystemInfo();
        SetTimer(hwnd, UPDATE_TIMER_ID, 5000, NULL);
        break;

    case WM_TIMER:
        if (wParam == UPDATE_TIMER_ID) UpdateSystemInfo();
        break;

    case WM_DESTROY:
        KillTimer(hwnd, UPDATE_TIMER_ID);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassW(&wc)) return 0;

    // Increased Window Width to 700 (was 500)
    HWND hwnd = CreateWindowExW(0, WINDOW_CLASS_NAME, L"System Information (C++)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 250, NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
