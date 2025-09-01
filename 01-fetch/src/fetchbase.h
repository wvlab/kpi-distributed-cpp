#ifndef _M_DISTRIBUTED_CPP_FETCH_BASE_H
#define _M_DISTRIBUTED_CPP_FETCH_BASE_H

#define _M_DISTRIBUTED_CPP_FETCH_HOSTNAME_LENGTH 128

#include <stdint.h>

struct KDC_OSInfo {
        char os_name[64];
        char os_version[64];
        char arch[16];
};

struct KDC_CPUInfo {
        char name[64];
        int cores;
        uint64_t frequency_hz;
};

struct KDC_RAMInfo {
        uint64_t total_bytes;
        uint64_t used_bytes;
        uint64_t free_bytes;
};

struct KDC_NetworkInfo {
        char hostname[_M_DISTRIBUTED_CPP_FETCH_HOSTNAME_LENGTH];
        char ip_address[64];
};

struct KDC_UptimeInfo {
        uint64_t uptime_seconds;
};

// Функції
int8_t get_os_info(struct KDC_OSInfo *info);
int8_t get_cpu_info(struct KDC_CPUInfo *info);
int8_t get_ram_info(struct KDC_RAMInfo *info);
int8_t get_network_info(struct KDC_NetworkInfo *info);
int8_t get_uptime_info(struct KDC_UptimeInfo *info);

#endif // _M_DISTRIBUTED_CPP_FETCH_BASE_H
