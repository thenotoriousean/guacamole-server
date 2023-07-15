#include "config.h"

#include "client.h"
#include "settings.h"
#include "kdrive.h"
#include "session.h"

#include <guacamole/client.h>
#include <guacamole/socket.h>
#include <guacamole/protocol.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <stdio.h>

void* guac_kdrive_client_thread(void* data) {

    guac_client* client = (guac_client*) data;
    guac_kdrive_client* kdrive_client = (guac_kdrive_client*) client->data;
    guac_kdrive_settings* settings = kdrive_client->settings;

    guac_kdrive_session* session = guac_kdrive_create_session(settings);
    session->client = client;
    kdrive_client->session = session;
    

    guac_client_log(session->client, GUAC_LOG_INFO, "compression: %s .",settings->compression);

    guac_client_end_frame(client);
    guac_protocol_send_size(client->socket, GUAC_DEFAULT_LAYER, settings->width, settings->height);
    guac_socket_flush(client->socket);

    //FILE *of = fopen("/home/testfiles/screentest","ab");
    while (client->state == GUAC_CLIENT_RUNNING) {
        if(!strcmp(settings->compression,"H264")){
            if(x2gokdrive_video_recv_loop(session))
                break;
        }
        else{
            if(kdrive_recv_loop(session))
                break;
        }
        
    }
    //fclose(of);

    guac_kdrive_free_session(session);
    return NULL;
}