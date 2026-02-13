#include "mqtt_helper.h"
#include "system_info.h"
#include "signal_handler.h"
#include "init_daemon.h"
#include "argp_helper.h"

int main(int argc, char **argv) {
    struct arguments arguments = {NULL, NULL, NULL, 0};

    if (argc < 2)
    {
        syslog(LOG_ERR, "Tried running the program without any arguments");
        return 1;
    }

    // Parse command-line arguments
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.device_id || !arguments.device_secret || !arguments.product_id) {
        syslog(LOG_ERR, "Missing arguments!");
        return 1;
    }

    // Open syslog for logging
    openlog("tuyad", LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Starting Program...");
    syslog(LOG_INFO, "Device ID: %s", arguments.device_id);
    syslog(LOG_INFO, "Device Secret: %s", arguments.device_secret);
    syslog(LOG_INFO, "Product ID: %s", arguments.product_id);
    syslog(LOG_INFO, "Daemon mode: %s", arguments.daemon_mode ? "Enabled" : "Disabled");

    if (arguments.daemon_mode) {
        syslog(LOG_INFO, "Running in daemon mode.");
        init_daemon();
    }

    // Initialize MQTT client
    tuya_mqtt_context_t client;

    // Setup signal handling
    setup_signal_handler();

    if (init_mqtt_client(&client, &arguments) != 0) {
        syslog(LOG_ERR, "Failed to initialize MQTT client");
        closelog();
        return 1;
    }

    syslog(LOG_INFO, "MQTT client initialized successfully.");

    while (!exit_flag) {
        tuya_mqtt_loop(&client);
        send_information(&client);
        sleep(5);
    }

    syslog(LOG_INFO, "MQTT Daemon exiting...");
    syslog(LOG_INFO, "Cleaning up before exiting...");
    tuya_mqtt_disconnect(&client);
    tuya_mqtt_deinit(&client);

    closelog();
    return 0;
}
