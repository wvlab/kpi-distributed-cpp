#include "fetchbase.h"
#include <stdio.h>

int main(void)
{
        printf("=== SYSTEM INFO ===\n\n");

        struct KDC_OSInfo os;
        if (get_os_info(&os) == 0) {
                printf("OS: %s %s (%s)\n", os.os_name, os.os_version, os.arch);
        } else {
                printf("Failed to get OS info\n");
        }

        struct KDC_CPUInfo cpu;
        if (get_cpu_info(&cpu) == 0) {
                printf(
                    "CPU: %s, cores=%d, frequency=%lu Hz\n", cpu.name,
                    cpu.cores, cpu.frequency_hz
                );
        } else {
                printf("Failed to get CPU info\n");
        }

        struct KDC_RAMInfo ram;
        if (get_ram_info(&ram) == 0) {
                printf(
                    "RAM: total=%lu MB, used=%lu MB, free=%lu MB\n",
                    ram.total_bytes / 1024 / 1024, ram.used_bytes / 1024 / 1024,
                    ram.free_bytes / 1024 / 1024
                );
        } else {
                printf("Failed to get RAM info\n");
        }

        struct KDC_NetworkInfo net;
        if (get_network_info(&net) == 0) {
                printf("Hostname: %s\n", net.hostname);
                printf("IP: %s\n", net.ip_address);
        } else {
                printf("Failed to get Network info\n");
        }

        struct KDC_UptimeInfo up;
        if (get_uptime_info(&up) == 0) {
                printf("Uptime: %lu seconds\n", up.uptime_seconds);
        } else {
                printf("Failed to get Uptime info\n");
        }

        return 0;
}
