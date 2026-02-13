#ifndef ARGP_HELPER_H
#define ARGP_HELPER_H

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>

// Struct for arguments
struct arguments {
    char *device_id;
    char *device_secret;
    char *product_id;
    int daemon_mode;  // 1 if daemon mode is enabled
};

// Function to parse arguments
error_t parse_opt(int key, char *arg, struct argp_state *state);

// External declaration of the argp parser
extern struct argp argp;

#endif /* ARGP_HELPER_H */
