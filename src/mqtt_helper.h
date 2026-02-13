#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <mqtt_client_interface.h>
#include <tuyalink_core.h>
#include "argp_helper.h"

int init_mqtt_client(tuya_mqtt_context_t *client, struct arguments *args);
void report_mqtt_data(tuya_mqtt_context_t *client, cJSON *data);
void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg);
void send_information(tuya_mqtt_context_t* context);

#endif

