#ifndef HELPER_UTILS_H
#define HELPER_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <mqtt_client_interface.h>
#include <tuyalink_core.h>
#include <cJSON.h>
#include "system_info.h"

void saveToLogs(const char* text);
void check_actionCode(const tuyalink_message_t* msg);
cJSON* system_to_json(SystemInfo sys_info);
cJSON* network_to_json(NetworkInfo net_info);
void free_network_info(NetworkInfo* net_info);

#endif
