#include "config.h"

#include "client.h"
#include "settings.h"

#include <guacamole/user.h>
#include <stdlib.h>



const char* GUAC_KDRIVE_CLIENT_ARGS[] = {
    "hostname",
    "port",
    "command"
};

enum KDRIVE_ARGS_IDX {
    IDX_HOSTNAME,
    IDX_PORT,
    IDX_COMMAND,
    KDRIVE_ARGS_COUNT
};


guac_kdrive_settings* guac_kdrive_parse_args(guac_user* user,
        int argc, const char** argv) {

    if (argc != KDRIVE_ARGS_COUNT) {
        guac_user_log(user, GUAC_LOG_WARNING, "Incorrect number of connection "
                "parameters provided: expected %i, got %i.",
                KDRIVE_ARGS_COUNT, argc);
        return NULL;
    }

    guac_kdrive_settings* settings = calloc(1, sizeof(guac_kdrive_settings));

    settings->hostname =
        guac_user_parse_args_string(user, GUAC_KDRIVE_CLIENT_ARGS, argv,
                IDX_HOSTNAME, NULL);

    settings->port =
        guac_user_parse_args_int(user, GUAC_KDRIVE_CLIENT_ARGS, argv,
                IDX_PORT, 0);

    settings->command =
        guac_user_parse_args_string(user, GUAC_KDRIVE_CLIENT_ARGS, argv,
                IDX_COMMAND, NULL);

    settings->width = user->info.optimal_width;
    settings->height = user->info.optimal_height;

    settings->compression = "H264";

    return settings;
}


void guac_kdrive_settings_free(guac_kdrive_settings* setttings) {
    // free(setttings->hostname);
    // free(setttings->command);

    free(setttings);
}