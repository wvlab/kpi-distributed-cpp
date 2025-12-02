#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include "fetchbase.h"
#include <cstdio>
#include <cstring>
#include <iphlpapi.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

const char *get_arch_string(const DWORD arch)
{
        switch (arch) {
        case PROCESSOR_ARCHITECTURE_INTEL:
                return "x86";
        case PROCESSOR_ARCHITECTURE_AMD64:
                return "x64";
        case PROCESSOR_ARCHITECTURE_ARM:
                return "arm";
        case PROCESSOR_ARCHITECTURE_ARM64:
                return "arm64";
        default:
                return "unknown";
        }
}

int8_t get_os_info(KDC_OSInfo *info)
{
        if (!info)
                return -1;

        OSVERSIONINFOEX osvi;
        SYSTEM_INFO si;
        memset(&osvi, 0, sizeof(osvi));
        osvi.dwOSVersionInfoSize = sizeof(osvi);

        GetVersionEx((OSVERSIONINFO *)&osvi);
        GetNativeSystemInfo(&si);

        snprintf(info->os_name, sizeof(info->os_name), "Windows");
        snprintf(
            info->os_version, sizeof(info->os_version), "%lu.%lu (Build %lu)",
            osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber
        );

        const char *arch = get_arch_string(si.wProcessorArchitecture);
        snprintf(info->arch, sizeof(info->arch), "%s", arch);
        return 0;
}

int8_t get_cpu_info(KDC_CPUInfo *info)
{
        if (!info)
                return -1;

        SYSTEM_INFO si;
        GetNativeSystemInfo(&si);
        info->cores = si.dwNumberOfProcessors;

        // 2. Get Frequency and Name from Registry
        // Path:
        // HKEY_LOCAL_MACHINE\HARDWARE\DESCRIPTION\System\CentralProcessor\0
        HKEY hKey;
        long status = RegOpenKeyExA(
            HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ,
            &hKey
        );

        if (status == ERROR_SUCCESS) {
                // Read Frequency (~MHz)
                DWORD mhz = 0;
                DWORD dataSize = sizeof(mhz);
                if (RegQueryValueExA(
                        hKey, "~MHz", nullptr, nullptr, (LPBYTE)&mhz, &dataSize
                    ) == ERROR_SUCCESS) {
                        info->frequency_hz = static_cast<uint64_t>(mhz) *
                                             1000000; // Convert MHz to Hz
                } else {
                        info->frequency_hz = 0;
                }

                // Read Processor Name (ProcessorNameString)
                char nameBuffer[64] = {};
                dataSize = sizeof(nameBuffer) - 1;
                if (RegQueryValueExA(
                        hKey, "ProcessorNameString", nullptr, nullptr,
                        (LPBYTE)nameBuffer, &dataSize
                    ) == ERROR_SUCCESS) {
                        snprintf(
                            info->name, sizeof(info->name), "%s", nameBuffer
                        );
                } else {
                        snprintf(info->name, sizeof(info->name), "Unknown CPU");
                }

                RegCloseKey(hKey);
        } else {
                // Fallback if registry fails
                info->frequency_hz = 0;
                snprintf(info->name, sizeof(info->name), "Generic CPU");
        }

        return 0;
}

int8_t get_ram_info(KDC_RAMInfo *info)
{
        if (!info)
                return -1;
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        if (!GlobalMemoryStatusEx(&statex))
                return -1;

        info->total_bytes = statex.ullTotalPhys;
        info->free_bytes = statex.ullAvailPhys;
        info->used_bytes = info->total_bytes - info->free_bytes;
        return 0;
}

int8_t get_network_info(KDC_NetworkInfo *info)
{
        if (!info)
                return -1;

        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                return -1;
        }

        if (gethostname(info->hostname, sizeof(info->hostname)) != 0) {
                WSACleanup();
                return -1;
        }

        struct addrinfo hints = {};
        struct addrinfo *res = nullptr;

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        snprintf(info->ip_address, sizeof(info->ip_address), "127.0.0.1");

        if (getaddrinfo(info->hostname, nullptr, &hints, &res) == 0) {
                for (struct addrinfo *ptr = res; ptr != nullptr;
                     ptr = ptr->ai_next) {
                        struct sockaddr_in *sock_addr =
                            (struct sockaddr_in *)ptr->ai_addr;

                        // Convert binary IP to string
                        char ip_str[INET_ADDRSTRLEN];
                        inet_ntop(
                            AF_INET, &sock_addr->sin_addr, ip_str,
                            INET_ADDRSTRLEN
                        );

                        // Filter out loopback (127.0.0.1) if possible, unless
                        // it's the only one
                        if (strcmp(ip_str, "127.0.0.1") != 0) {
                                snprintf(
                                    info->ip_address, sizeof(info->ip_address),
                                    "%s", ip_str
                                );
                                break; // Stop at the first real IP found
                        }
                }
                freeaddrinfo(res);
        }

        WSACleanup();
        return 0;
}

int8_t get_uptime_info(KDC_UptimeInfo *info)
{
        if (!info)
                return -1;
        info->uptime_seconds = GetTickCount64() / 1000;
        return 0;
}
