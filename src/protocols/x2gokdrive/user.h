#ifndef GUAC_KDRIVE_USER_H
#define GUAC_KDRIVE_USER_H

#include "config.h"

#include <guacamole/user.h>

/**
 * Handler for joining users.
 */
guac_user_join_handler guac_kdrive_user_join_handler;

/**
 * Handler for leaving users.
 */
guac_user_leave_handler guac_kdrive_user_leave_handler;

#endif