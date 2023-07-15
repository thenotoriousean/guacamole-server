/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <guacamole/user.h>
#include <guacamole/socket.h>
#include <guacamole/protocol.h>

#include "kdrive.h"
#include "session.h"

int guac_kdrive_user_mouse_handler(guac_user* user, int x, int y, int mask) {
    guac_client* client = user->client;
    guac_kdrive_client* kdrive_client = (guac_kdrive_client*)client->data;
    guac_kdrive_session* session = kdrive_client->session;

    /* Send KDRIVE event only if finished connecting */
    // guac_client_log(session->client, GUAC_LOG_INFO, "mouse %d %d", x, y);
    if (session != NULL) kdrive_send_mouse(session, x, y, mask);

    return 0;
}

int guac_kdrive_user_key_handler(guac_user* user, int keysym, int pressed) {
    guac_kdrive_client* kdrive_client = (guac_kdrive_client*)user->client->data;
    guac_kdrive_session* session = kdrive_client->session;
    
    if (session != NULL) kdrive_send_key(session, keysym, pressed);
    // guac_client_log(session->client, GUAC_LOG_INFO, "keysym %d", keysym);

    return 0;
}

int guac_kdrive_user_size_handler(guac_user* user, int width, int height) {
    // guac_kdrive_client* kdrive_client = (guac_kdrive_client*)user->client->data;
    // guac_kdrive_session* session = kdrive_client->session;

    // if (session != NULL) kdrive_send_geometry(session, width, height);

    guac_protocol_send_size(user->client->socket, GUAC_DEFAULT_LAYER, width, height);
    guac_socket_flush(user->client->socket);

    return 0;
}
