#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <stdio.h>
#include "mqtt_helper.h"
#include "helper_utils.h"
#include "tuya_cacert.h"
#include "system_info.h"

int init_mqtt_client(tuya_mqtt_context_t *client, struct arguments *args) {
    if (!args->device_id || !args->device_secret || !args->product_id ||
        !(*args->device_id) || !(*args->device_secret) || !(*args->product_id)) {
        syslog(LOG_ERR, "MQTT initialization failed: Missing or empty credentials");
        return -1;
    }

    syslog(LOG_INFO, "Initializing MQTT with device_id: %s, device_secret: %s, product_id: %s",
           args->device_id, args->device_secret, args->product_id);

    int ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t) {
        .host = "m2.tuyacn.com",
        .port = 8883,
        .cacert = (const uint8_t *)tuya_cacert_pem,
        .cacert_len = sizeof(tuya_cacert_pem),
        .device_id = args->device_id,
        .device_secret = args->device_secret,
        .keepalive = 60,
        .timeout_ms = 2000,
        .on_messages = on_messages
    });

    if (ret != 0) {
        syslog(LOG_ERR, "MQTT initialization failed: %d", ret);
        return ret;
    }

    ret = tuya_mqtt_connect(client);
    if (ret != 0) {
        syslog(LOG_ERR, "MQTT connection failed: %d", ret);
        return ret;
    }

    syslog(LOG_INFO, "MQTT connected successfully");
    return 0;
}

void report_mqtt_data(tuya_mqtt_context_t* context, cJSON* data) {
    if (!data || !context) {
        syslog(LOG_ERR, "Invalid parameters for report_mqtt_data");
        return;
    }
    char* message = cJSON_PrintUnformatted(data);
    if (message) {
        tuyalink_thing_property_report(context, NULL, message);
        free(message);
    } else {
        syslog(LOG_ERR, "Failed to create JSON message");
    }
    cJSON_Delete(data);
}

void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg)
{
    syslog(LOG_INFO, "on message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
    switch (msg->type) {
        case THING_TYPE_ACTION_EXECUTE:
            check_actionCode(msg);
            break;
        default:
            break;
    }
}

void send_information(tuya_mqtt_context_t* context) {
    SystemInfo sys_info = get_system_info();
    NetworkInfo net_info = get_network_info();

    cJSON *sys_json = system_to_json(sys_info);
    cJSON *net_json = network_to_json(net_info);

    if (!sys_json) {
        syslog(LOG_ERR, "Failed to convert system information to JSON");
        cJSON_Delete(sys_json);
        return;
    }

    if (!net_json) {
        syslog(LOG_ERR, "Failed to convert network information to JSON");
        cJSON_Delete(net_json);
        return;
    }

    report_mqtt_data(context, sys_json);
    report_mqtt_data(context, net_json);
    free_network_info(&net_info);
}
