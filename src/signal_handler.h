#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "mqtt_helper.h"

void handle_signal(int signal);
void setup_signal_handler();
void set_mqtt_client(tuya_mqtt_context_t* client);

extern volatile sig_atomic_t exit_flag;

#endif
