#include "fetchbase.h"
#include <iphlpapi.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winnt.h>

#pragma comment(lib, "iphlpapi.lib")

char *get_arch_string(const DWORD arch)
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
        case PROCESSOR_ARCHITECTURE_MIPS:
                return "mips";
        case PROCESSOR_ARCHITECTURE_ALPHA:
                return "mips";
        case PROCESSOR_ARCHITECTURE_PPC:
                return "powerpc";
        default:
                return "unknown";
        }
}

int8_t get_os_info(struct KDC_OSInfo *info)
{
        if (!info) {
                return -1;
        }

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

        char *arch = get_arch_string(si.wProcessorArchitecture);
        unsigned int archlen = strlen(arch);
        memcpy(
            info->arch, arch,
            ((sizeof(info->arch) - 1) < archlen) ? (sizeof(info->arch) - 1)
                                                 : archlen
        );
        return 0;
}

int8_t get_cpu_info(struct KDC_CPUInfo *info)
{
        if (!info) {
                return -1;
        }

        SYSTEM_INFO si;
        GetNativeSystemInfo(&si);
        snprintf(info->name, sizeof(info->name), "CPU");
        info->cores = si.dwNumberOfProcessors;
        info->frequency_hz = 0;
        return 0;
}

int8_t get_ram_info(struct KDC_RAMInfo *info)
{
        if (!info) {
                return -1;
        }

        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        if (!GlobalMemoryStatusEx(&statex)) {
                return -1;
        }

        info->total_bytes = statex.ullTotalPhys;
        info->free_bytes = statex.ullAvailPhys;
        info->used_bytes = info->total_bytes - info->free_bytes;
        return 0;
}

int8_t get_network_info(struct KDC_NetworkInfo *info)
{
        if (!info) {
                return -1;
        }

        DWORD size = sizeof(info->hostname);
        if (!GetComputerNameA(info->hostname, &size)) {
                return -1;
        }

        snprintf(info->ip_address, sizeof(info->ip_address), "127.0.0.1");
        return 0;
}

int8_t get_uptime_info(struct KDC_UptimeInfo *info)
{
        if (!info) {
                return -1;
        }

        info->uptime_seconds = GetTickCount64() / 1000;
        return 0;
}
