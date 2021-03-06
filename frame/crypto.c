#include "frame/crypto.h"
#include "frame/meta.h"
#include <string.h>
#include <malloc.h>

static size_t gquic_frame_crypto_size(const void *const);
static int gquic_frame_crypto_serialize(const void *const, gquic_writer_str_t *const);
static int gquic_frame_crypto_deserialize(void *const, gquic_reader_str_t *const);
static int gquic_frame_crypto_init(void *const);
static int gquic_frame_crypto_dtor(void *const);

gquic_frame_crypto_t *gquic_frame_crypto_alloc() {
    gquic_frame_crypto_t *frame = gquic_frame_alloc(sizeof(gquic_frame_crypto_t));
    if (frame == NULL) {
        return NULL;
    }
    GQUIC_FRAME_META(frame).type = 0x06;
    GQUIC_FRAME_META(frame).deserialize_func = gquic_frame_crypto_deserialize;
    GQUIC_FRAME_META(frame).init_func = gquic_frame_crypto_init;
    GQUIC_FRAME_META(frame).dtor_func = gquic_frame_crypto_dtor;
    GQUIC_FRAME_META(frame).serialize_func = gquic_frame_crypto_serialize;
    GQUIC_FRAME_META(frame).size_func = gquic_frame_crypto_size;
    return frame;
}

static size_t gquic_frame_crypto_size(const void *const frame) {
    const gquic_frame_crypto_t *spec = frame;
    if (spec == NULL) {
        return 0;
    }
    return 1 + gquic_varint_size(&spec->len) + gquic_varint_size(&spec->off) + spec->len;
}

static int gquic_frame_crypto_serialize(const void *const frame, gquic_writer_str_t *const writer) {
    const gquic_frame_crypto_t *spec = frame;
    if (spec == NULL || writer == NULL) {
        return -1;
    }
    if (GQUIC_FRAME_SIZE(spec) > GQUIC_STR_SIZE(writer)) {
        return -2;
    }
    if (gquic_writer_str_write_byte(writer, GQUIC_FRAME_META(frame).type) != 0) {
        return -3;
    }
    const u_int64_t *vars[] = { &spec->off, &spec->len };
    int i;
    for (i = 0; i < 2; i++) {
        if (gquic_varint_serialize(vars[i], writer) != 0) {
            return -4;
        }
    }
    gquic_str_t data = { spec->len, spec->data };
    if (gquic_writer_str_write(writer, &data) != 0) {
        return -5;
    }
    return 0;
 }

static int gquic_frame_crypto_deserialize(void *const frame, gquic_reader_str_t *const reader) {
    gquic_frame_crypto_t *spec = frame;
    if (frame == NULL || reader == NULL) {
        return -1;
    }
    if (gquic_reader_str_read_byte(reader) != GQUIC_FRAME_META(frame).type) {
        return -2;
    }
    u_int64_t *vars[] = { &spec->off, &spec->len };
    int i;
    for (i = 0; i < 2; i++) {
        if (gquic_varint_deserialize(vars[i], reader) != 0) {
            return -3;
        }
    }
    if (spec->len > GQUIC_STR_SIZE(reader)) {
        return -4;
    }
    spec->data = malloc(spec->len);
    if (spec->data == NULL) {
        return -5;
    }
    gquic_str_t data = { spec->len, spec->data };
    if (gquic_reader_str_read(&data, reader) != 0) {
        return -6;
    }
    return 0;
}

static int gquic_frame_crypto_init(void *const frame) {
    gquic_frame_crypto_t *spec = frame;
    if (spec == NULL) {
        return -1;
    }
    spec->off = 0;
    spec->len = 0;
    spec->data = NULL;
    return 0;
}

static int gquic_frame_crypto_dtor(void *const frame) {
    gquic_frame_crypto_t *spec = frame;
    if (spec == NULL) {
        return -1;
    }
    if (spec->data != NULL) {
        free(spec->data);
    }
    return 0;
}
