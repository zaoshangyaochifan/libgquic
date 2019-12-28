#include "frame/new_token.h"
#include "frame/meta.h"
#include <malloc.h>
#include <string.h>

static size_t gquic_frame_new_token_size(const void *const);
static ssize_t gquic_frame_new_token_serialize(const void *const, void *, const size_t);
static ssize_t gquic_frame_new_token_deserialize(void *const, const void *, const size_t);
static int gquic_frame_new_token_init(void *const);
static int gquic_frame_new_token_dtor(void *const);

gquic_frame_new_token_t *gquic_frame_new_token_alloc() {
    gquic_frame_new_token_t *frame = gquic_frame_alloc(sizeof(gquic_frame_new_token_t));
    if (frame == NULL) {
        return NULL;
    }
    GQUIC_FRAME_META(frame).type = 0x07;
    GQUIC_FRAME_META(frame).deserialize_func = gquic_frame_new_token_deserialize;
    GQUIC_FRAME_META(frame).init_func = gquic_frame_new_token_init;
    GQUIC_FRAME_META(frame).dtor_func = gquic_frame_new_token_dtor;
    GQUIC_FRAME_META(frame).serialize_func = gquic_frame_new_token_serialize;
    GQUIC_FRAME_META(frame).size_func = gquic_frame_new_token_size;
    return frame;
}

static size_t gquic_frame_new_token_size(const void *const frame) {
    const gquic_frame_new_token_t *spec = frame;
    if (spec == NULL) {
        return 0;
    }
    return 1 + gquic_varint_size(&spec->len) + spec->len;
}

static ssize_t gquic_frame_new_token_serialize(const void *const frame, void *buf, const size_t size) {
    size_t off = 0;
    ssize_t serialize_len = 0;
    const gquic_frame_new_token_t *spec = frame;
    if (spec == NULL) {
        return -1;
    }
    if (buf == NULL) {
        return -2;
    }
    if (GQUIC_FRAME_SIZE(spec) > size) {
        return -3;
    }
    ((u_int8_t *) buf)[off++] = GQUIC_FRAME_META(spec).type;
    serialize_len = gquic_varint_serialize(&spec->len, buf + off, size - off);
    if (serialize_len <= 0) {
        return -4;
    }
    off += serialize_len;
    memcpy(buf + off, spec->token, spec->len);
    return off + spec->len;
}

static ssize_t gquic_frame_new_token_deserialize(void *const frame, const void *buf, const size_t size) {
    size_t off = 0;
    ssize_t deserialize_len = 0;
    gquic_frame_new_token_t *spec = frame;
    if (spec == NULL) {
        return -1;
    }
    if (buf == NULL) {
        return -2;
    }
    if (GQUIC_FRAME_META(spec).type != ((u_int8_t *) buf)[off++]) {
        return -3;
    }
    deserialize_len = gquic_varint_deserialize(&spec->len, buf + off, size - off);
    if (deserialize_len <= 0) {
        return -4;
    }
    if (spec->len > size - off) {
        return -4;
    }
    spec->token = malloc(spec->len);
    if (spec->token == NULL) {
        return -4;
    }
    memcpy(spec->token, buf + off, spec->len);
    return off + spec->len;
}

static int gquic_frame_new_token_init(void *const frame) {
    gquic_frame_new_token_t *spec = frame;
    if (spec == NULL) {
        return -1;
    }
    spec->len = 0;
    spec->token = NULL;
    return 0;
}

static int gquic_frame_new_token_dtor(void *const frame) {
    gquic_frame_new_token_t *spec = frame;
    if (spec == NULL) {
        return -1;
    }
    if (spec->token != NULL) {
        free(spec->token);
    }
    return 0;
}
