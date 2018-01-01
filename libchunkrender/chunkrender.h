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
    size_t vertices;
    uint32_t *data;
} chunk_block_model_t;

chunk_block_model_t render_chunk_blocks(uint8_t *data, uint8_t *is_transparent, uint8_t *state,
                                        uint32_t block_texture[][6]);
