#ifndef _LIBGQUIC_HANDSHAKE_INITIAL_AEAD_H
#define _LIBGQUIC_HANDSHAKE_INITIAL_AEAD_H

#include "handshake/header_protector.h"
#include "handshake/aead.h"

int gquic_handshake_initial_aead_init(gquic_common_long_header_sealer_t *const sealer,
                                      gquic_common_long_header_opener_t *const opener,
                                      const gquic_str_t *const conn_id,
                                      int is_client);

#endif
