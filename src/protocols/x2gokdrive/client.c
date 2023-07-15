#include "config.h"

#include "client.h"
#include "user.h"
#include "kdrive.h"
#include "settings.h"

#include <guacamole/client.h>
#include <stdlib.h>

int guac_client_init(guac_client* client) {
    
    client->args = GUAC_KDRIVE_CLIENT_ARGS;

    guac_kdrive_client* kdrive_client = calloc(1, sizeof(guac_kdrive_client));
    client->data = kdrive_client;
    kdrive_client->client = client;

    client->join_handler = guac_kdrive_user_join_handler;
    client->leave_handler = guac_kdrive_user_leave_handler;
    client->free_handler = guac_kdrive_client_free_handler;

    return 0;
}

int guac_kdrive_client_free_handler(guac_client* client) {

    guac_kdrive_client* kdrive_client = (guac_kdrive_client*) client->data;
    guac_kdrive_settings* settings = kdrive_client->settings;
    
    guac_kdrive_settings_free(settings);
    free(client->data); 
    return 0;
}