#ifndef _LIBGQUIC_FRAME_STREAM_DATA_BLOCKED_H
#define _LIBGQUIC_FRAME_STREAM_DATA_BLOCKED_H

#include "util/varint.h"
#include "streams/type.h"

typedef struct gquic_frame_stream_data_blocked_s gquic_frame_stream_data_blocked_t;
struct gquic_frame_stream_data_blocked_s {
    u_int64_t id;
    u_int64_t limit;
};

gquic_frame_stream_data_blocked_t *gquic_frame_stream_data_blocked_alloc();

#endif
