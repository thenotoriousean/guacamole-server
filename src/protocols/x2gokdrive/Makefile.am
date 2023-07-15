#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
# NOTE: Parts of this file (Makefile.am) are automatically transcluded verbatim
# into Makefile.in. Though the build system (GNU Autotools) automatically adds
# its own license boilerplate to the generated Makefile.in, that boilerplate
# does not apply to the transcluded portions of Makefile.am which are licensed
# to you by the ASF under the Apache License, Version 2.0, as described above.
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libguac-client-x2gokdrive.la

libguac_client_x2gokdrive_la_SOURCES =  \
    client.c                        \
    input.c                         \
    settings.c                      \
    session.c                       \
    kdrive.c                        \
    user.c

noinst_HEADERS =                \
    client.h                    \
    input.h                     \
    settings.h                  \
    session-priv.h              \
    session.h                   \
    kdrive.h                    \
    user.h


libguac_client_x2gokdrive_la_CFLAGS = \
    -Wall -Iinclude    \
    @COMMON_INCLUDE@           \
    @LIBGUAC_INCLUDE@          

libguac_client_x2gokdrive_la_LIBADD = \
    @COMMON_LTLIB@             \
    @LIBGUAC_LTLIB@            

libguac_client_x2gokdrive_la_LDFLAGS = \
    -version-info 0:0:0         \
    @PTHREAD_LIBS@              \
    @X2GOKDRIVE_LIBS@

