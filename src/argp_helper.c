#include "argp_helper.h"

static char doc[] = "Daemon program";
static char args_doc[] = "[DEVICE_ID] [DEVICE_SECRET] [PRODUCT_ID]";

static struct argp_option options[] = {
    {"device-id", 'i', "ID", 0, "Set the Device ID"},
    {"device-secret", 's', "SECRET", 0, "Set the Device Secret"},
    {"product-id", 'p', "PRODUCT", 0, "Set the Product ID"},
    {"daemon", 'd', 0, OPTION_ARG_OPTIONAL, "Run in daemon mode (optional)"},  // Fix
    {0}
};

error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'i': arguments->device_id = arg; break;
        case 's': arguments->device_secret = arg; break;
        case 'p': arguments->product_id = arg; break;
        case 'd': arguments->daemon_mode = 1; break;  // No need to check `arg`, it's a flag

        case ARGP_KEY_ARG:
            if (state->arg_num == 0)
                arguments->device_id = arg;
            else if (state->arg_num == 1)
                arguments->device_secret = arg;
            else if (state->arg_num == 2)
                arguments->product_id = arg;
            else
                argp_usage(state);
            break;

        case ARGP_KEY_END:
            if (!arguments->device_id || !arguments->device_secret || !arguments->product_id) {
                argp_usage(state);
            }
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

// Argp parser
struct argp argp = {
    options,      // Options list
    parse_opt,    // Parser function
    args_doc,     // Allow positional arguments
    doc           // Program documentation
};
