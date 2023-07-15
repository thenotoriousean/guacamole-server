#ifndef GUAC_KDRIVE_KDRIVE_H
#define GUAC_KDRIVE_KDRIVE_H

#include "config.h"

#include "settings.h"
#include "session.h"

#include <guacamole/client.h>

#include <pthread.h>

typedef struct guac_kdrive_client {

    pthread_t client_thread;

    guac_client* client;

    guac_kdrive_settings* settings;

    guac_kdrive_session* session;

} guac_kdrive_client;


void* guac_kdrive_client_thread(void* data);


#endif