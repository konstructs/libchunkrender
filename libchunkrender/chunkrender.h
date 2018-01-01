#pragma once

#include <stdint.h>
#include <stddef.h>

#define VACUUM_TYPE 0
#define SOLID_TYPE 65535
#define BLOCK_TYPES 65536
#define MAX_HEALTH 2047

#define STATE_SOLID 0
#define STATE_LIQUID 1
#define STATE_GAS 2
#define STATE_PLASMA 3

#define BLOCK_SIZE 7
#define CHUNK_SIZE 32
#define BLOCK_BUFFER_SIZE (CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*BLOCK_SIZE)
#define BLOCKS_HEADER_SIZE 6

typedef struct {
    uint8_t a;
    uint8_t b;
    uint8_t width;
    uint8_t height;
    uint8_t *block_data;
} rectangle_t;

typedef struct {
    size_t size;
    rectangle_t *data;
} rectangle_list_t;

typedef struct {
    size_t vertices;
    uint32_t *data;
} chunk_block_model_t;

void generate_rectangles(uint8_t face,
                         uint8_t slice,
                         uint8_t *data,
                         uint8_t *is_transparent,
                         uint8_t *state,
                         uint32_t block_texture[][6],
                         rectangle_list_t *rectangles);

chunk_block_model_t render_chunk_blocks(uint8_t *data, uint8_t *is_transparent, uint8_t *state,
                                        uint32_t block_texture[][6]);
