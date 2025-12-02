#include "fetchbase.h"
#include <arpa/inet.h>
#include <cstdio>
#include <fstream>
#include <netdb.h>
#include <string>
#include <string_view>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

std::string_view trim(std::string_view sv)
{
        while (!sv.empty() && std::isspace(sv.front())) {
                sv.remove_prefix(1);
        }

        while (!sv.empty() && std::isspace(sv.back())) {
                sv.remove_suffix(1);
        }

        return sv;
}

int8_t get_os_info(KDC_OSInfo *info)
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
        // TODO: add more platforms
#if defined(__x86_64__) || defined(_M_X64)
        snprintf(info->arch, sizeof(info->arch), "x64");
#else
        snprintf(info->arch, sizeof(info->arch), "x86");
#endif
        return 0;
}

int8_t get_cpu_info(KDC_CPUInfo *info)
{
        if (!info) {
                return -1;
        }

        std::ifstream file("/proc/cpuinfo");
        if (!file.is_open())
                return -1;

        std::string line;
        int cores = 0;
        double freq_mhz = 0.0;
        std::string model_name;

        while (std::getline(file, line)) {
                std::string_view sv(line);

                if (sv.substr(0, 9) == "processor") {
                        cores++;
                        continue;
                }

                auto colon_pos = sv.find(':');
                if (colon_pos == std::string_view::npos)
                        continue;

                std::string_view key = trim(sv.substr(0, colon_pos));
                std::string_view value = trim(sv.substr(colon_pos + 1));

                if (key == "model name" && model_name.empty()) {
                        model_name = std::string(value);
                } else if (key == "cpu MHz" && freq_mhz == 0.0) {
                        try {
                                freq_mhz = std::stod(std::string(value));
                        } catch (...) {
                                freq_mhz = 0;
                        }
                }
        }

        snprintf(info->name, sizeof(info->name), "%s", model_name.c_str());
        info->cores = cores;
        info->frequency_hz = static_cast<uint64_t>(freq_mhz * 1e6);

        return 0;
}

int8_t get_ram_info(KDC_RAMInfo *info)
{
        if (!info)
                return -1;

        struct sysinfo si;
        if (sysinfo(&si) != 0)
                return -1;

        info->total_bytes = si.totalram * si.mem_unit;
        info->free_bytes =
            si.freeram * si.mem_unit - si.bufferram * si.mem_unit;
        info->used_bytes = info->total_bytes - info->free_bytes;
        return 0;
}

int8_t get_network_info(KDC_NetworkInfo *info)
{
        if (!info)
                return -1;

        if (gethostname(info->hostname, sizeof(info->hostname)) != 0)
                return -1;

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

int8_t get_uptime_info(KDC_UptimeInfo *info)
{
        if (!info)
                return -1;
        struct sysinfo si;
        if (sysinfo(&si) != 0)
                return -1;
        info->uptime_seconds = si.uptime;
        return 0;
}
