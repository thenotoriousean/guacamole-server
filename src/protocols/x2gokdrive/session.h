#ifndef __GUAC_KDRIVE_SESSION_H__
#define __GUAC_KDRIVE_SESSION_H__

#include "session-priv.h"
#include "settings.h"

#include <guacamole/protocol-constants.h>
#include <guacamole/client.h>
#include <stdio.h>

// frame region
typedef struct frame_region {
    uint32_t source_crc, width, height, data_size;
    int32_t source_x, source_y, x, y;
    uint8_t* data;
} frame_region;


// frame
#define MAX_REGION  10
typedef struct frame {
    uint32_t width, height, crc, num_of_regions, win_id;
    int32_t x, y;
    frame_region* regions[MAX_REGION];
    uint32_t regions_size;
} frame;

typedef struct cursor {
    uint16_t xhot, yhot, width, height;
    uint32_t serial, data_size;
} cursor;




// class session
typedef struct guac_kdrive_session {
    guac_client* client;

    int socket;

    int bytes_left_to_read;

    int current_data_type;

    char* message_buffer;

    char* video_stream;

    frame* current_frame;

    cursor* current_cursor;

    uint32_t deleted_frames_size;

    uint32_t deleted_cursors_size;

    uint16_t server_version;

    int last_mouse_mask;

    uint32_t key_modifier;

} guac_kdrive_session;

// constructor
guac_kdrive_session* guac_kdrive_create_session(guac_kdrive_settings* settings);
void guac_kdrive_free_session(guac_kdrive_session* session);

// method
void read_data_header(guac_kdrive_session *session);
void get_frame_region(guac_kdrive_session *session);
void get_region_image(guac_kdrive_session *session);
void get_deleted_frames_list(guac_kdrive_session *session);
void get_deleted_cursors_list(guac_kdrive_session *session);
void get_cursor_image(guac_kdrive_session *session);
void get_selection_buffer(guac_kdrive_session *session);

void get_image_frame(guac_kdrive_session *session);
void get_cursor(guac_kdrive_session *session);
void get_deleted_frames(guac_kdrive_session *session);
void get_deleted_cursors(guac_kdrive_session *session);
void get_selection(guac_kdrive_session *session);
void get_server_version(guac_kdrive_session *session);
void get_client_selection(guac_kdrive_session *session);

void set_cursor(guac_kdrive_session* session);
void render_frame(guac_kdrive_session* session);
void free_message_buffer(guac_kdrive_session* session);
void free_current_frame(guac_kdrive_session* session);

void _get_image(guac_kdrive_session* session, const char* type, uint32_t crc, int x, int y, int width, int height);
void _put_image(guac_kdrive_session* session, const char* type, uint32_t crc, int x, int y, int dirty_x, int dirty_y, int dirty_width, int dirty_height);
void _del_image(guac_kdrive_session* session, const char* type, uint32_t crc);
void _draw_image(guac_kdrive_session* session, uint8_t* image, uint32_t image_size, int x, int y);
// end of class session



// public method
void kdrive_send_cookie(guac_kdrive_session *session, char *cookie);
void kdrive_send_geometry(guac_kdrive_session *session, int width, int height);
void kdrive_send_keepalive(guac_kdrive_session *session);
void kdrive_send_client_version(guac_kdrive_session *session);
void kdrive_send_selchunk(guac_kdrive_session *session);
void kdrive_send_mouse(guac_kdrive_session* session, int x, int y, int mask);
void kdrive_send_key(guac_kdrive_session* session, int keysym, int pressed);
int kdrive_recv_loop(guac_kdrive_session* session);
int x2gokdrive_video_recv_loop(guac_kdrive_session *session);
int kdrive_wait(guac_kdrive_session* session, int msec_timeout);

#endif 