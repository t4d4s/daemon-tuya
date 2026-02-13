#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_link.h>
#include <unistd.h>
#include <cJSON.h>
#include <syslog.h>
#include <sys/sysinfo.h>
#include <math.h>
#include "system_info.h"
#include "mqtt_helper.h"

SystemInfo get_system_info() {
    syslog(LOG_INFO, "Fetching system information...");
    SystemInfo info = {0};

    FILE *f = fopen("/proc/stat", "r");
    if (!f) {
        syslog(LOG_ERR, "Error opening /proc/stat");
        return info;
    }

    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    fscanf(f, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", 
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    fclose(f);

    static unsigned long long prev_total = 0, prev_idle = 0;
    unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
    unsigned long long idle_time = idle + iowait;

    if ((total - prev_total) > 0) {
        info.cpu_usage = ((total - prev_total - (idle_time - prev_idle)) * 100.0) / (total - prev_total);
    }
    prev_total = total;
    prev_idle = idle_time;
    info.cpu_usage = ((int)(info.cpu_usage * 100 + 0.5)) / 100.0;

    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0) {
        syslog(LOG_ERR, "Failed to retrieve system info");
        return info;
    }

    info.total_ram_gb = roundf(sys_info.totalram / (1024.0 * 1024.0 * 1024.0) * 100) / 100.0;
    info.free_ram_gb = roundf(sys_info.freeram / (1024.0 * 1024.0 * 1024.0) * 100) / 100.0;
    snprintf(info.system_uptime, sizeof(info.system_uptime), "%02ld:%02ld:%02ld", 
             sys_info.uptime / 3600, (sys_info.uptime % 3600) / 60, sys_info.uptime % 60);
    
    return info;
}

// https://man7.org/linux/man-pages/man7/netdevice.7.html
NetworkInfo get_network_info() {
    syslog(LOG_INFO, "Extracting network interface information...");
    NetworkInfo net_info = {NULL, 0};

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        syslog(LOG_ERR, "Error opening socket");
        return net_info;
    }

    struct if_nameindex *interface_indexes = if_nameindex();
    if (!interface_indexes) {
        syslog(LOG_ERR, "Error obtaining network interfaces");
        close(sockfd);
        return net_info;
    }

    size_t count = 0;
    for (struct if_nameindex *interface = interface_indexes; interface->if_index != 0 && interface->if_name; interface++) {
        if (strcmp(interface->if_name, "lo") == 0 || interface->if_name[0] == '\0') continue; // Skip loopback and invalid interfaces
        count++;
    }

    if (count == 0) {
        syslog(LOG_ERR, "No valid network interfaces found");
        if_freenameindex(interface_indexes);
        close(sockfd);
        return net_info;
    }

    net_info.interfaces = calloc(count, sizeof(NetworkInterfaceInfo));
    if (!net_info.interfaces) {
        syslog(LOG_ERR, "Memory allocation failed");
        if_freenameindex(interface_indexes);
        close(sockfd);
        return net_info;
    }

    size_t index = 0;
    for (struct if_nameindex *interface = interface_indexes; interface->if_index != 0 && interface->if_name; interface++) {
        if (strcmp(interface->if_name, "lo") == 0 || interface->if_name[0] == '\0') continue;

        struct ifreq ifr;
        strncpy(ifr.ifr_name, interface->if_name, IFNAMSIZ - 1);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';

        NetworkInterfaceInfo *iface_info = &net_info.interfaces[index];

        // Get IP address
        if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0) {
            strncpy(iface_info->ip_address, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr), INET_ADDRSTRLEN);
        }

        // Get netmask
        if (ioctl(sockfd, SIOCGIFNETMASK, &ifr) == 0) {
            strncpy(iface_info->net_mask, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_netmask)->sin_addr), INET_ADDRSTRLEN);
        }

        // Get TX and RX bytes from /proc/net/dev
        FILE *fd = fopen("/proc/net/dev", "r");
        if (fd) {
            char buffer[256];
            fgets(buffer, sizeof(buffer), fd); // Skip first line
            fgets(buffer, sizeof(buffer), fd); // Skip second line

            while (fgets(buffer, sizeof(buffer), fd)) {
                char interface_name[IFNAMSIZ];
                unsigned long long rx, tx;
                if (sscanf(buffer, " %255[^:]: %llu %*d %*d %*d %*d %*d %*d %*d %llu", 
                           interface_name, &rx, &tx) == 3) {
                    if (strcmp(interface_name, interface->if_name) == 0) {
                        iface_info->rx_bytes = rx;
                        iface_info->tx_bytes = tx;
                        break;
                    }
                }
            }
            fclose(fd);
        }

        // Only add interfaces with valid data
        if (strlen(iface_info->ip_address) > 0 && strlen(iface_info->net_mask) > 0) {
            index++;
        }
    }

    net_info.count = index;
    close(sockfd);
    if_freenameindex(interface_indexes);
    return net_info;
}

