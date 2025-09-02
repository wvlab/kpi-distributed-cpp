#include "fetchbase.h"
#include <wchar.h>
#include <windows.h>

#define WINDOW_CLASS_NAME L"SystemInfoAppClass"
#define UPDATE_TIMER_ID 1

HWND hOS, hCPU, hRAM, hHostname, hIP, hUptime;

void UpdateSystemInfo()
{
        struct KDC_OSInfo os;
        if (get_os_info(&os) == 0) {
                wchar_t buffer[256];
                swprintf(
                    buffer, 256, L"OS: %hs %hs (%hs)", os.os_name,
                    os.os_version, os.arch
                );
                SetWindowTextW(hOS, buffer);
        } else {
                SetWindowTextW(hOS, L"OS: Failed to get OS info");
        }

        struct KDC_CPUInfo cpu;
        if (get_cpu_info(&cpu) == 0) {
                wchar_t buffer[256];
                swprintf(
                    buffer, 256, L"CPU: %hs, Cores: %d, Frequency: %lu Hz",
                    cpu.name, cpu.cores, cpu.frequency_hz
                );
                SetWindowTextW(hCPU, buffer);
        } else {
                SetWindowTextW(hCPU, L"CPU: Failed to get CPU info");
        }

        // RAM Info
        struct KDC_RAMInfo ram;
        if (get_ram_info(&ram) == 0) {
                wchar_t buffer[256];
                swprintf(
                    buffer, 256, L"RAM: Total=%lu MB, Used=%lu MB, Free=%lu MB",
                    ram.total_bytes / 1024 / 1024, ram.used_bytes / 1024 / 1024,
                    ram.free_bytes / 1024 / 1024
                );
                SetWindowTextW(hRAM, buffer);
        } else {
                SetWindowTextW(hRAM, L"RAM: Failed to get RAM info");
        }

        // Network Info
        struct KDC_NetworkInfo net;
        if (get_network_info(&net) == 0) {
                wchar_t buffer_hostname[256];
                wchar_t buffer_ip[256];
                swprintf(buffer_hostname, 256, L"Hostname: %hs", net.hostname);
                swprintf(buffer_ip, 256, L"IP: %hs", net.ip_address);
                SetWindowTextW(hHostname, buffer_hostname);
                SetWindowTextW(hIP, buffer_ip);
        } else {
                SetWindowTextW(
                    hHostname, L"Hostname: Failed to get Network info"
                );
                SetWindowTextW(hIP, L"IP: Failed to get Network info");
        }

        // Uptime Info
        struct KDC_UptimeInfo up;
        if (get_uptime_info(&up) == 0) {
                wchar_t buffer[64];
                swprintf(buffer, 64, L"Uptime: %lu seconds", up.uptime_seconds);
                SetWindowTextW(hUptime, buffer);
        } else {
                SetWindowTextW(hUptime, L"Uptime: Failed to get Uptime info");
        }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        switch (uMsg) {
        case WM_CREATE:
                // Create static text controls with wide-character strings
                hOS = CreateWindowExW(
                    0, L"STATIC", L"OS: N/A", WS_CHILD | WS_VISIBLE, 20, 20,
                    400, 20, hwnd, NULL, NULL, NULL
                );
                hCPU = CreateWindowExW(
                    0, L"STATIC", L"CPU: N/A", WS_CHILD | WS_VISIBLE, 20, 45,
                    400, 20, hwnd, NULL, NULL, NULL
                );
                hRAM = CreateWindowExW(
                    0, L"STATIC", L"RAM: N/A", WS_CHILD | WS_VISIBLE, 20, 70,
                    400, 20, hwnd, NULL, NULL, NULL
                );
                hHostname = CreateWindowExW(
                    0, L"STATIC", L"Hostname: N/A", WS_CHILD | WS_VISIBLE, 20,
                    95, 400, 20, hwnd, NULL, NULL, NULL
                );
                hIP = CreateWindowExW(
                    0, L"STATIC", L"IP: N/A", WS_CHILD | WS_VISIBLE, 20, 120,
                    400, 20, hwnd, NULL, NULL, NULL
                );
                hUptime = CreateWindowExW(
                    0, L"STATIC", L"Uptime: N/A", WS_CHILD | WS_VISIBLE, 20,
                    145, 400, 20, hwnd, NULL, NULL, NULL
                );

                UpdateSystemInfo();
                SetTimer(hwnd, UPDATE_TIMER_ID, 5000, NULL);
                break;

        case WM_TIMER:
                if (wParam == UPDATE_TIMER_ID) {
                        UpdateSystemInfo();
                }
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

int WINAPI wWinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow
)
{
        WNDCLASSW wc = {0};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = WINDOW_CLASS_NAME;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        if (!RegisterClassW(&wc)) {
                MessageBoxW(
                    NULL, L"Window Registration Failed!", L"Error!",
                    MB_ICONEXCLAMATION | MB_OK
                );
                return 0;
        }

        HWND hwnd = CreateWindowExW(
            0, WINDOW_CLASS_NAME, L"System Information", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 500, 250, NULL, NULL, hInstance, NULL
        );

        if (hwnd == NULL) {
                MessageBoxW(
                    NULL, L"Window Creation Failed!", L"Error!",
                    MB_ICONEXCLAMATION | MB_OK
                );
                return 0;
        }

        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);

        MSG msg = {0};
        while (GetMessage(&msg, NULL, 0, 0)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }

        return (int)msg.wParam;
}
