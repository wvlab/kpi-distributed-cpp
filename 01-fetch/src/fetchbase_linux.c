#include "fetchbase.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

int8_t get_os_info(struct KDC_OSInfo *info)
{
        if (!info) {
                return -1;
        }

        struct utsname uts;
        if (uname(&uts) != 0) {
                return -1;
        }

        snprintf(info->os_name, sizeof(info->os_name), "%s", uts.sysname);
        snprintf(info->os_version, sizeof(info->os_version), "%s", uts.release);
#if defined(__x86_64__) || defined(_M_X64)
        snprintf(info->arch, sizeof(info->arch), "x64");
#else
        snprintf(info->arch, sizeof(info->arch), "x86");
#endif

        return 0;
}

int8_t get_cpu_info(struct KDC_CPUInfo *info)
{
        if (!info) {
                return -1;
        }

        FILE *f = fopen("/proc/cpuinfo", "r");
        if (!f) {
                return -1;
        }

        char line[256];
        int cores = 0;
        double freq_mhz = 0.0;
        char model_name[64] = {0};

        while (fgets(line, sizeof(line), f)) {
                if (strncmp(line, "processor", 9) == 0) {
                        cores++;
                } else if (strncmp(line, "model name", 10) == 0 &&
                           model_name[0] == '\0') {

                        char *colon = strchr(line, ':');

                        if (colon) {
                                while (*(++colon) == ' ') {
                                        strncpy(
                                            model_name, colon,
                                            sizeof(model_name) - 1
                                        );
                                }

                                size_t len = strlen(model_name);
                                if (len > 0 && model_name[len - 1] == '\n') {
                                        model_name[len - 1] = '\0';
                                }
                        }
                } else if (strncmp(line, "cpu MHz", 7) == 0 &&
                           freq_mhz == 0.0) {
                        char *colon = strchr(line, ':');
                        if (colon) {
                                freq_mhz = atof(colon + 1);
                        }
                }
        }
        fclose(f);

        strncpy(info->name, model_name, sizeof(info->name) - 1);
        info->name[sizeof(info->name) - 1] = '\0';
        info->cores = cores;
        info->frequency_hz = (uint64_t)(freq_mhz * 1e6);

        return 0;
}

int8_t get_ram_info(struct KDC_RAMInfo *info)
{
        if (!info) {
                return -1;
        }

        struct sysinfo si;
        if (sysinfo(&si) != 0) {
                return -1;
        }

        info->total_bytes = si.totalram * si.mem_unit;
        info->free_bytes =
            si.freeram * si.mem_unit - si.bufferram * si.mem_unit;
        info->used_bytes = info->total_bytes - info->free_bytes;
        return 0;
}

int8_t get_network_info(struct KDC_NetworkInfo *info)
{
        if (!info) {
                return -1;
        }

        if (gethostname(info->hostname, sizeof(info->hostname)) != 0) {
                return -1;
        }

        struct hostent *he = gethostbyname(info->hostname);
        if (he && he->h_addr_list[0]) {
                inet_ntop(
                    AF_INET, he->h_addr_list[0], info->ip_address,
                    sizeof(info->ip_address)
                );
        } else {
                snprintf(
                    info->ip_address, sizeof(info->ip_address), "127.0.0.1"
                );
        }

        return 0;
}

int8_t get_uptime_info(struct KDC_UptimeInfo *info)
{
        if (!info) {
                return -1;
        }

        struct sysinfo si;
        if (sysinfo(&si) != 0) {
                return -1;
        }

        info->uptime_seconds = si.uptime;
        return 0;
}
