#ifndef __GUAC_KDRIVE_SETTINGS_H
#define __GUAC_KDRIVE_SETTINGS_H

#include "config.h"
#include "user.h"

typedef struct guac_kdrive_settings {

    char* hostname;
    int port;
    char* command;
    char* compression;
    int width, height;

} guac_kdrive_settings;


guac_kdrive_settings* guac_kdrive_parse_args(guac_user* user,
        int argc, const char** argv);

void guac_kdrive_settings_free(guac_kdrive_settings* settings);

    
extern const char* GUAC_KDRIVE_CLIENT_ARGS[];

#endif