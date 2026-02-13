#include "helper_utils.h"

void saveToLogs(const char* text)
{
    FILE *fptr;
    fptr = fopen("/tmp/tuya_action.log", "w");
    if (fptr == NULL) {
        syslog(LOG_ERR, "File doesn't exist!\n");
        exit(0);
    }
    else {
        syslog(LOG_INFO, "File created!\n");
    }
    if (strcmp(text, "") > 0) {
        fprintf(fptr, "Saved Parameter: %s\n", text);
        syslog(LOG_INFO, "Parameter saved!\n");
    }
    else {
        syslog(LOG_ERR, "Received parameter is empty!\n");
    }
    fclose(fptr);
}

void check_actionCode(const tuyalink_message_t* msg)
{
    //printf("%s\n", msg->data_string);
    
    cJSON *json = cJSON_Parse(msg->data_string);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            syslog(LOG_USER | LOG_INFO, "Could not parse JSON: %s", error_ptr);
        }
        return;
    }

    cJSON *inputParamsObj = cJSON_GetObjectItem(json, "inputParams");
    if (!cJSON_IsObject(inputParamsObj)) {
        cJSON_Delete(json);
        return;
    }

    cJSON *text = cJSON_GetObjectItem(inputParamsObj, "text");

    cJSON *action_code = cJSON_GetObjectItem(json, "actionCode");
    if (action_code && strcmp(action_code->valuestring, "logger") == 0 && text) {
        syslog(LOG_INFO, "Action Code: %s\n", action_code->valuestring);
        syslog(LOG_INFO, "Text: %s\n", text->valuestring);
        
        // Saving the text to logs
        saveToLogs(text->valuestring);
    } else {
        syslog(LOG_ERR, "The received message doesn't have actionCode 'logger' or text!\n");
    }

    // Cleaning up
    cJSON_Delete(json);
}


cJSON* system_to_json(SystemInfo sys_info) {
    cJSON *root = cJSON_CreateObject();
    if (!root) return NULL;

    cJSON_AddNumberToObject(root, "cpu_usage_percent", sys_info.cpu_usage);
    cJSON_AddNumberToObject(root, "total_ram_gb", sys_info.total_ram_gb);
    cJSON_AddNumberToObject(root, "free_ram_gb", sys_info.free_ram_gb);
    cJSON_AddStringToObject(root, "system_uptime", sys_info.system_uptime);

    return root;
}

cJSON* network_to_json(NetworkInfo net_info) {
    cJSON *root = cJSON_CreateObject();
    if (!root) return NULL;

    for (size_t i = 0; i < net_info.count; i++) {
        cJSON *interface_obj = cJSON_CreateObject();
        if (!interface_obj) continue;

        cJSON_AddStringToObject(interface_obj, "ip_address", net_info.interfaces[i].ip_address);
        cJSON_AddStringToObject(interface_obj, "net_mask", net_info.interfaces[i].net_mask);
        cJSON_AddNumberToObject(interface_obj, "rx_bytes", net_info.interfaces[i].rx_bytes);
        cJSON_AddNumberToObject(interface_obj, "tx_bytes", net_info.interfaces[i].tx_bytes);

        cJSON_AddItemToObject(root, net_info.interfaces[i].ip_address, interface_obj);
    }

    return root;
}

void free_network_info(NetworkInfo* net_info) {
    if (net_info->interfaces) {
        free(net_info->interfaces);
        net_info->interfaces = NULL;
    }
}
