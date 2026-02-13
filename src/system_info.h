#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include "mqtt_helper.h"
#include <netinet/in.h>

// Struct for getting system info
typedef struct {
    double cpu_usage;
    double total_ram_gb;
    double free_ram_gb;
    char system_uptime[20];
} SystemInfo;

// Structs for getting network information
typedef struct {
    char ip_address[INET_ADDRSTRLEN];
    char net_mask[INET_ADDRSTRLEN];
    unsigned long long rx_bytes;
    unsigned long long tx_bytes;
} NetworkInterfaceInfo;

typedef struct {
    NetworkInterfaceInfo *interfaces;
    size_t count;
} NetworkInfo;


// Function declarations
SystemInfo get_system_info();
NetworkInfo get_network_info();



#endif
