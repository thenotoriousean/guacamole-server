#include "session.h"

#include <arpa/inet.h>
#include <guacamole/client.h>
#include <guacamole/protocol.h>
#include <guacamole/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common/io.h"

static char *compression = "H264";
static int smp_num = 1;

int keysyms[] = {
    96,    126,   49,    33,    50,    64,    51,    35,    52,    36,    53,
    37,    54,    94,    55,    38,    56,    42,    57,    40,    48,    41,
    45,    96,    61,    43,    65288, 65289, 113,   81,    119,   87,    101,
    69,    114,   82,    116,   84,    121,   89,    117,   85,    105,   73,
    111,   79,    112,   80,    91,    123,   93,    125,   65293, 65509, 65,
    97,    83,    115,   68,    100,   70,    102,   71,    103,   72,    104,
    74,    106,   75,    107,   76,    108,   58,    59,    34,    39,    92,
    124,   65505, 90,    122,   88,    120,   67,    99,    86,    118,   66,
    98,    78,    110,   77,    109,   60,    44,    62,    46,    63,    47,
    65506, 65507, 65511, 65513, 32,    65027, 65512, 65383, 65508, 65307, 65470,
    65471, 65472, 65473, 65474, 65475, 65476, 65477, 65478, 65479, 65480, 65481,
    65482, 65483, 65484, 65386, 65360, 65365, 65535, 65367, 65366, 65362, 65361,
    65364, 65363, 65291, 65455, 65450, 65453, 65451, 65293, 65456, 65457, 65458,
    65459, 65460, 65461, 65462, 65463, 65464, 65465, 65454};

int keycodes[] = {
    49,  49,  10,  10,  11,  11,  12,  12,  13,  13,  14, 14,  15,  15,
    16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21, 21,  22,  23,
    24,  24,  25,  25,  26,  26,  27,  27,  28,  28,  29, 29,  30,  30,
    31,  31,  32,  32,  33,  33,  34,  34,  35,  35,  36, 66,  38,  38,
    39,  39,  40,  40,  41,  41,  42,  42,  43,  43,  44, 44,  45,  45,
    46,  46,  47,  47,  48,  48,  51,  51,  50,  52,  52, 53,  53,  54,
    54,  55,  55,  56,  56,  57,  57,  58,  58,  59,  59, 60,  60,  61,
    61,  62,  37,  133, 64,  65,  108, 134, 135, 105, 9,  67,  68,  69,
    70,  71,  72,  73,  74,  75,  76,  95,  96,  107, 78, 127, 118, 110,
    112, 119, 115, 117, 111, 113, 116, 114, 77,  106, 63, 82,  86,  104,
    90,  87,  88,  89,  83,  84,  85,  79,  80,  81,  91};

void kdrive_send_cookie(guac_kdrive_session *session, char *cookie) {
    guac_common_write(session->socket, cookie, 32);
}

void kdrive_send_geometry(guac_kdrive_session *session, int width, int height) {
    char message[EVENT_LENGTH] = {0};
    uint32_t event_type = GEOMETRY;
    memcpy(message, (char *)&event_type, 4);
    memcpy(message + 4, (char *)&width, 2);
    memcpy(message + 6, (char *)&height, 2);

    guac_common_write(session->socket, message, EVENT_LENGTH);
}

void kdrive_send_keepalive(guac_kdrive_session *session) {}

void kdrive_send_client_version(guac_kdrive_session *session) {
    char message[EVENT_LENGTH] = {0};
    uint32_t event_type = CLIENTVERSION;
    uint16_t version = FEATURE_VERSION;
    uint16_t os = OS_VERSION;
    memcpy(message, (char *)&event_type, 4);
    memcpy(message + 4, (char *)&version, 2);
    memcpy(message + 6, (char *)&os, 2);

    guac_common_write(session->socket, message, EVENT_LENGTH);
}

void kdrive_send_selchunk(guac_kdrive_session *session) {}

void read_data_header(guac_kdrive_session *session) {
    uint32_t data_type = *((uint32_t *)(session->message_buffer));
    guac_client_log(session->client, GUAC_LOG_INFO, "head_type %d", data_type);

    switch (data_type) {
        case FRAME:
            get_image_frame(session);
            break;

        case CURSOR:
            get_cursor(session);
            break;

        case DELETEDFRAMES:
            get_deleted_frames(session);
            break;

        case DELETEDCURSORS:
            get_deleted_cursors(session);
            break;

        case SELECTION:
            get_selection(session);
            break;

        case SERVER_VERSION:
            get_server_version(session);
            break;

        case DEMANDCLIENTSELECTION:
            get_client_selection(session);
            break;

        case H264STREAM:
            get_h264_stream(session);

        default:
            break;
    }

    free_message_buffer(session);
}

void get_frame_region(guac_kdrive_session *session) {
    guac_client_log(session->client, GUAC_LOG_INFO, "frame_region");

    uint32_t *buffer = (uint32_t *)(session->message_buffer);

    frame_region *region = malloc(sizeof(frame_region));
    region->source_crc = buffer[0];
    region->source_x = buffer[1];
    region->source_y = buffer[2];
    region->x = buffer[3];
    region->y = buffer[4];
    region->width = buffer[5];
    region->height = buffer[6];
    region->data_size = buffer[7];
    region->data = NULL;

    frame *current_frame = session->current_frame;
    current_frame->regions[current_frame->regions_size] = region;
    current_frame->regions_size++;

    if (region->source_crc == 0) {
        session->bytes_left_to_read = region->data_size;
        session->current_data_type = REGIONDATA;
        free_message_buffer(session);
    } else {
        if (current_frame->num_of_regions == current_frame->regions_size) {
            render_frame(session);
        } else {
            session->bytes_left_to_read = REGION_SIZE;
            session->current_data_type = FRAMEREGION;
        }
    }
}

void get_region_image(guac_kdrive_session *session) {
    guac_client_log(session->client, GUAC_LOG_INFO, "get_region_data");

    frame *current_frame = session->current_frame;
    frame_region *last_region =
        current_frame->regions[current_frame->regions_size - 1];
    last_region->data = (uint8_t *)(session->message_buffer);
    session->message_buffer = NULL;

    if (current_frame->num_of_regions == current_frame->regions_size) {
        render_frame(session);
    } else {
        session->bytes_left_to_read = REGION_SIZE;
        session->current_data_type = FRAMEREGION;
    }
}

void get_deleted_frames_list(guac_kdrive_session *session) {
    guac_client_log(session->client, GUAC_LOG_INFO, "deleted_frames_list");
    uint32_t *buffer = (uint32_t *)(session->message_buffer);

    for (int i = 0; i < session->deleted_frames_size; i++) {
        _del_image(session, TYPE_FRAME, *(buffer + i));
    }
    free_message_buffer(session);
}

void get_deleted_cursors_list(guac_kdrive_session *session) {
    // uint32_t *buffer = (uint32_t *)(session->message_buffer);

    // for (int i = 0; i < session->deleted_frames_size; i++) {
    //     _del_image(session, TYPE_CURSOR, *(buffer + i));
    // }
    free_message_buffer(session);
}

void get_cursor_image(guac_kdrive_session *session) {
    // cursor *current_cursor = session->current_cursor;
    // uint8_t *data = malloc(current_cursor->data_size);
    // guac_common_read(session->socket, data, current_cursor->data_size);
    // set_cursor(session);
    free_message_buffer(session);
}
void get_selection_buffer(guac_kdrive_session *session) {}

void get_image_frame(guac_kdrive_session *session) {
    guac_client_log(session->client, GUAC_LOG_INFO, "get_image_frame");

    if (session->current_frame) free_current_frame(session);
    frame *current_frame = session->current_frame = malloc(sizeof(frame));
    uint32_t *buffer = (uint32_t *)(session->message_buffer);
    current_frame->width = buffer[1];
    current_frame->height = buffer[2];
    current_frame->x = buffer[3];
    current_frame->y = buffer[4];
    current_frame->num_of_regions = buffer[5];
    current_frame->crc = buffer[6];
    current_frame->regions_size = 0;

    if (current_frame->num_of_regions == 0) {
        render_frame(session);
    } else {
        session->bytes_left_to_read = REGION_SIZE;
        session->current_data_type = FRAMEREGION;
    }
}

void get_cursor(guac_kdrive_session *session) {
    guac_client_log(session->client, GUAC_LOG_INFO, "get_cursor");

    uint16_t *buffer = (uint16_t *)(session->message_buffer);
    cursor *current_cursor = session->current_cursor;
    current_cursor->width = *(buffer + 5);
    current_cursor->height = *(buffer + 6);
    current_cursor->xhot = *(buffer + 7);
    current_cursor->yhot = *(buffer + 8);
    current_cursor->serial = *((uint32_t *)buffer + 5);
    current_cursor->data_size = *((uint32_t *)buffer + 6);
    free_message_buffer(session);

    if (current_cursor->data_size > 0) {
        session->current_data_type = CURSORDATA;
        session->bytes_left_to_read = current_cursor->data_size;
    } else {
        set_cursor(session);
    }
}

void get_deleted_frames(guac_kdrive_session *session) {
    guac_client_log(session->client, GUAC_LOG_INFO, "get_deleted_frames");

    session->deleted_frames_size = *((uint32_t *)(session->message_buffer) + 1);
    session->bytes_left_to_read =
        session->deleted_frames_size * sizeof(uint32_t);
    session->current_data_type = FRAMELIST;
    free_message_buffer(session);
}

void get_deleted_cursors(guac_kdrive_session *session) {
    guac_client_log(session->client, GUAC_LOG_INFO, "get_deleted_cursors");
    session->deleted_cursors_size =
        *((uint32_t *)(session->message_buffer) + 1);
    session->bytes_left_to_read =
        session->deleted_cursors_size * sizeof(uint32_t);
    session->current_data_type = CURSORLIST;
    free_message_buffer(session);
}

void get_selection(guac_kdrive_session *session) {}

void get_server_version(guac_kdrive_session *session) {
    session->server_version = *((uint16_t *)(session->message_buffer) + 2);
}
void get_client_selection(guac_kdrive_session *session) {}

void set_cursor(guac_kdrive_session *session) {}

void get_h264_stream(guac_kdrive_session *session){
    
}

void render_frame(guac_kdrive_session *session) {
    guac_client_log(session->client, GUAC_LOG_INFO, "render_frame");

    frame *current_frame = session->current_frame;
    if (current_frame->crc == 0) current_frame->x = current_frame->y = 0;

    if (current_frame->num_of_regions == 0) {
        _put_image(session, TYPE_FRAME, current_frame->crc, current_frame->x,
                   current_frame->y, 0, 0, current_frame->width,
                   current_frame->height);
    }

    for (int i = 0; i < current_frame->num_of_regions; i++) {
        frame_region *region = current_frame->regions[i];
        if (region->source_crc == 0) {
            _draw_image(session, region->data, region->data_size,
                        current_frame->x + region->x,
                        current_frame->y + region->y);
        } else {
            _put_image(session, TYPE_FRAME, region->source_crc,
                       current_frame->x + region->x - region->source_x,
                       current_frame->y + region->y - region->source_y,
                       region->source_x, region->source_y, region->width,
                       region->height);
        }
    }

    if (current_frame->crc != 0 && current_frame->num_of_regions != 0) {
        _get_image(session, TYPE_FRAME, current_frame->crc, current_frame->x,
                   current_frame->y, current_frame->width,
                   current_frame->height);
    }

    guac_client_end_frame(session->client);
    guac_socket_flush(session->client->socket);
}

void free_message_buffer(guac_kdrive_session *session) {
    if (session->message_buffer) free(session->message_buffer);
    session->message_buffer = NULL;
}

void free_current_frame(guac_kdrive_session *session) {
    frame *current_frame = session->current_frame;
    if (current_frame == NULL) return;
    for (int i = 0; i < current_frame->regions_size; i++) {
        if (current_frame->regions[i]->data != NULL)
            free(current_frame->regions[i]->data);
        free(current_frame->regions[i]);
    }
    free(current_frame);
    session->current_frame = NULL;
}

void _get_image(guac_kdrive_session *session, const char *type, uint32_t crc,
                int x, int y, int width, int height) {
    guac_socket *socket = session->client->socket;
    guac_protocol_get_image(socket, type, crc, x, y, width, height);

    // guac_client_log(session->client, GUAC_LOG_INFO, "getimage %s %d %d %d
    // %d",
    //                 type, x, y, width, height);
}

void _put_image(guac_kdrive_session *session, const char *type, uint32_t crc,
                int x, int y, int dirty_x, int dirty_y, int dirty_width,
                int dirty_height) {
    guac_socket *socket = session->client->socket;
    guac_protocol_put_image(socket, type, crc, x, y, dirty_x, dirty_y,
                            dirty_width, dirty_height);

    // guac_client_log(session->client, GUAC_LOG_INFO, "putimage %s %d %d %d %d
    // %d %d",
    //                 type, x, y, dirty_x, dirty_y, dirty_width, dirty_height);
}

void _del_image(guac_kdrive_session *session, const char *type, uint32_t crc) {
    guac_socket *socket = session->client->socket;
    guac_protocol_del_image(socket, type, crc);
}

void _draw_image(guac_kdrive_session *session, uint8_t *image,
                 uint32_t image_size, int x, int y) {
    guac_client *client = session->client;
    guac_socket *socket = client->socket;

    guac_stream *stream = guac_client_alloc_stream(client);
    guac_protocol_send_img(socket, stream, GUAC_COMP_OVER, GUAC_DEFAULT_LAYER,
                           "image/jpeg", x, y);

    guac_protocol_send_blob(socket, stream, image, image_size);

    guac_protocol_send_end(socket, stream);

    guac_client_free_stream(client, stream);

    guac_client_end_frame(session->client);
}

void kdrive_send_mouse(guac_kdrive_session *session, int x, int y, int mask) {
    // guac_client_log(session->client, GUAC_LOG_INFO, "mouse start %d %d %d", x, y, mask);
    mask = mask ? mask ^ (mask & (mask - 1)) : 0;
    int last_mask = session->last_mouse_mask;
    char message[EVENT_LENGTH] = {0};
    uint32_t *v32 = (uint32_t *)message;

    v32[0] = mask ? MOUSEPRESS : MOUSERELEASE;
    if (mask == last_mask) {
        v32[0] = MOUSEMOTION;
        v32[1] = x;
        v32[2] = y;
    } else {
        int btn = mask ^ last_mask;
        while (btn) {
            v32[2]++;
            btn >>= 1;
        }
    }

    session->last_mouse_mask = mask;
    // guac_client_log(session->client, GUAC_LOG_INFO, "mouse end %d %d %d", v32[0], v32[1], v32[2]);
    guac_common_write(session->socket, message, EVENT_LENGTH);
}

void kdrive_send_key(guac_kdrive_session *session, int keysym, int pressed) {
    uint32_t modifier = 0;
    if (keysym == XK_LeftShift || keysym == XK_RightShift)
        modifier = ShiftMask;
    else if (keysym == XK_LeftCtrl || keysym == XK_RightCtrl)
        modifier = ControlMask;
    else if (keysym == XK_LeftAlt || keysym == XK_RightAlt)
        modifier = Mod1Mask;
    else if (keysym == XK_LeftMeta || keysym == XK_RightMeta)
        modifier = Mod4Mask;

    if (pressed)
        session->key_modifier |= modifier;
    else
        session->key_modifier &= ~modifier;

    char message[EVENT_LENGTH] = {0};
    uint32_t *v32 = (uint32_t *)message;

    v32[0] = pressed ? KEYPRESS : KEYRELEASE;
    v32[1] = session->key_modifier;
    int size = sizeof(keysyms) / sizeof(int);
    for (int i = 0; i < size; i++) {
        if (keysyms[i] == keysym) {
            v32[2] = keycodes[i];
            break;
        }
    }
    // guac_client_log(session->client, GUAC_LOG_INFO, "key %d %d %d", v32[0], v32[1], v32[2]);
    guac_common_write(session->socket, message, EVENT_LENGTH);
}

guac_kdrive_session *guac_kdrive_create_session(
    guac_kdrive_settings *settings) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, settings->hostname, &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(settings->port);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        return NULL;
    }

    char request[100];
    memset(request, 0, sizeof(request));
    sprintf(request, "%s,%dx%d,%s", settings->command, settings->width,
            settings->height,compression);
    write(sock, request, strlen(request));

    char response[50];
    memset(response, 0, sizeof(response));
    read(sock, response, sizeof(response));
    close(sock);

    char cookie[33], port[6];
    memset(cookie, 0, sizeof(cookie));
    memset(port, 0, sizeof(port));
    strncpy(cookie, response, 32);
    strncpy(port, response + 33, strlen(response) - 33);

    // create session
    guac_kdrive_session *session =
        (guac_kdrive_session *)malloc(sizeof(guac_kdrive_session));
    session->bytes_left_to_read = session->current_data_type = 0;
    session->last_mouse_mask = 0;
    session->current_frame = NULL;
    session->current_cursor = malloc(sizeof(cursor));

    session->socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_port = htons(atoi(port));

    if (connect(session->socket, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) != 0) {
        return NULL;
    }

    kdrive_send_cookie(session, cookie);
    kdrive_send_geometry(session, settings->width, settings->height);

    return session;
}

void guac_kdrive_free_session(guac_kdrive_session *session) {
    close(session->socket);
    free(session);
}

int kdrive_recv_loop(guac_kdrive_session *session) {
    if (kdrive_wait(session, 1000) == 0) return 0;

    if (session->bytes_left_to_read == 0) {
        session->bytes_left_to_read = HEADER_SIZE;
        session->current_data_type = HEADER;
        free_message_buffer(session);
    }

    if (session->message_buffer == NULL)
        session->message_buffer = malloc(session->bytes_left_to_read);

    guac_common_read(session->socket, session->message_buffer,
                     session->bytes_left_to_read);
    guac_client_log(session->client, GUAC_LOG_INFO, "Receive %d bytes.",
                    session->bytes_left_to_read);
    session->bytes_left_to_read = 0;

    // guac_client_log(session->client, GUAC_LOG_INFO, "datatype %d", session->current_data_type);
    switch (session->current_data_type) {
        case HEADER:
            read_data_header(session);
            break;

        case FRAMEREGION:
            get_frame_region(session);
            break;

        case REGIONDATA:
            get_region_image(session);
            break;

        case FRAMELIST:
            get_deleted_frames_list(session);
            break;

        case CURSORLIST:
            get_deleted_cursors_list(session);
            break;

        case CURSORDATA:
            get_cursor_image(session);
            break;

        case SELECTIONBUFFER:
            get_selection_buffer(session);
            break;

        default:
            return 1;
    }
    return 0;
}

int x2gokdrive_video_recv_loop(guac_kdrive_session *session){
    if (kdrive_wait(session, 1000) == 0) return 0;
    u_int32_t length = 1024 * 16 * 4;

    if (session->message_buffer == NULL)
        session->message_buffer = malloc(length);


    u_int32_t bytesRead = read(session->socket, session->message_buffer, length);
    if(bytesRead == 0)
        return 0;
    guac_client_log(session->client, GUAC_LOG_INFO, "Receive %d bytes.",
                    bytesRead);

    // fseek(of, 0, SEEK_END);
    // fwrite(session->message_buffer, 1, bytesRead, of);

    // if(bytesRead == length){
    //     char filename[20];
    //     sprintf(filename, "/home/testfiles/samples/%d", smp_num-1);

    //     FILE *of = fopen(filename,"ab");
    //     fseek(of, 0, SEEK_END);

    //     fwrite(session->message_buffer, 1, bytesRead, of);
    //     fclose(of);

    //     return 0;
    // }

    char filename[20];
    sprintf(filename, "/home/testfiles/samples/%d", smp_num++);

    FILE *of = fopen(filename,"wb"); 

    fwrite(session->message_buffer, 1, bytesRead, of);
    fclose(of);

    return 0;

    
}

int kdrive_wait(guac_kdrive_session *session, int msec_timeout) {
    /* Initialize with single underlying file descriptor */
    struct pollfd fds[1] = {
        {.fd = session->socket, .events = POLLIN, .revents = 0}};

    /* No timeout if usec_timeout is negative */
    if (msec_timeout < 0) return poll(fds, 1, -1);

    /* Handle timeout if specified, rounding up to poll()'s granularity */
    return poll(fds, 1, msec_timeout);
}
