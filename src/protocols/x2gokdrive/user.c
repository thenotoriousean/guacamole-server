#include "config.h"

#include "user.h"
#include "kdrive.h"
#include "input.h"


#include <guacamole/argv.h>
#include <guacamole/audio.h>
#include <guacamole/client.h>
#include <guacamole/socket.h>
#include <guacamole/user.h>

#include <pthread.h>

int guac_kdrive_user_join_handler(guac_user* user, int argc, char** argv) {

    guac_kdrive_client* kdrive_client = (guac_kdrive_client*) user->client->data;

    /* Parse provided arguments */
    guac_kdrive_settings* settings = guac_kdrive_parse_args(user,
            argc, (const char**) argv);

    /* Fail if settings cannot be parsed */
    if (settings == NULL) {
        guac_user_log(user, GUAC_LOG_INFO,
                "Badly formatted client arguments.");
        return 1;
    }

    /* Store settings at user level */
    user->data = settings;
    // user->size_handler = guac_kdrive_user_size_handler;
    user->mouse_handler = guac_kdrive_user_mouse_handler;
    user->key_handler = guac_kdrive_user_key_handler;

    kdrive_client->settings = settings;

    /* Start client thread */
    if (pthread_create(&kdrive_client->client_thread, NULL, guac_kdrive_client_thread, user->client)) {
        guac_user_log(user, GUAC_LOG_ERROR, "Unable to start KDRIVE client thread.");
        return 1;
    }

    return 0;

}

int guac_kdrive_user_leave_handler(guac_user* user) {

    // guac_kdrive_client* kdrive_client = (guac_kdrive_client*) user->client->data;

    /* Free settings if not owner (owner settings will be freed with client) */
    guac_kdrive_settings* settings = (guac_kdrive_settings*) user->data;
    guac_kdrive_settings_free(settings);

    return 0;
}

