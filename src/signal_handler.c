#include <syslog.h>
#include "signal_handler.h"
#include "mqtt_helper.h"


volatile sig_atomic_t exit_flag = 0;

// Signal handler function
void handle_signal(int signal) {
    switch (signal) {
        case SIGINT:
             syslog(LOG_ERR, "Caught SIGINT (Interrupt). Exiting...");
            break;
        case SIGTERM:
             syslog(LOG_ERR, "Caught SIGTERM (Terminate). Exiting...");
            break;
        case SIGQUIT:
             syslog(LOG_ERR, "Caught SIGQUIT (Quit). Exiting...");
            break;
        default:
             syslog(LOG_ERR, "Caught unknown signal (%d). Exiting...", signal);
            break;
    }
    exit_flag = 1;
}

// Function to set up signal handling
void setup_signal_handler() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    // Register signals
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    syslog(LOG_INFO, "Signal handler setup complete.");
}