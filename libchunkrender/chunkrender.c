#include <stdlib.h>
#include "chunkrender.h"

chunk_block_model_t allocate_chunk_block_model() {
    size_t vertices = 36;
    chunk_block_model_t model = {vertices, (uint32_t*)malloc(2*sizeof(uint32_t)*vertices)};
    return model;
}

chunk_block_model_t reallocate_chunk_block_model(chunk_block_model_t old_model) {
    size_t vertices = old_model.vertices * 2;
    chunk_block_model_t model = {vertices, (uint32_t*)realloc(old_model.data, 2*sizeof(uint32_t)*vertices)};
    return model;
}

void free_chunk_block_model(chunk_block_model_t model) {
    free(model.data);
}

#define OFF_NORMAL 0
#define OFF_VERTEX 3
#define OFF_X 7
#define OFF_Y 12
#define OFF_Z 17
#define OFF_AO 22
#define OFF_DAMAGE_U 27
#define OFF_DAMAGE_V 31

#define OFF_DU 0
#define OFF_DV 5
#define OFF_AL 10
#define OFF_R 14
#define OFF_G 18
#define OFF_B 22
#define OFF_LIGHT 26


static uint8_t face_vertices[6][6] = {
    {2, 0, 1, 5, 2, 1}, // Left side
    {7, 6, 3, 4, 7, 3}, // Right side
    {2, 5, 7, 4, 2, 7}, // Top side
    {1, 0, 3, 6, 1, 3}, // Bottom side
    {4, 3, 0, 2, 4, 0}, // Front
    {5, 1, 6, 7, 5, 6}  // O side
};

static uint8_t uv[6][2] = {
    {0, 1},
    {0, 0},
    {1, 0},
    {1, 1},
    {0, 1},
    {1, 0}
};


chunk_block_model_t render_chunk_blocks(uint8_t *data, uint8_t *is_transparent, uint8_t *state,
                                        uint32_t block_texture[][6]) {
    size_t vertices = 0;
    chunk_block_model_t model = allocate_chunk_block_model();
    for(uint8_t face = 0; face < 6; face++) {
        for(uint8_t slice = 0; slice < CHUNK_SIZE; slice++) {
            for(uint8_t a = 0; a < CHUNK_SIZE; a++) {
                for(uint8_t b = 0; b < CHUNK_SIZE; b++) {

                    uint8_t x;
                    uint8_t y;
                    uint8_t z;
                    if(face == 0 || face == 1) {
                        x = slice;
                        y = b;
                        z = a;
                    } else if(face == 2 || face == 3) {
                        x = a;
                        y = slice;
                        z = b;
                    } else {
                        x = a;
                        y = b;
                        z = slice;
                    }
                    uint32_t i = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
                    uint32_t block_type = data[i*BLOCK_SIZE + BLOCKS_HEADER_SIZE];
                    if(state[block_type] != STATE_GAS) {
                        uint32_t texture = block_texture[block_type][face];
                        uint8_t du = texture % 16;
                        uint8_t dv = texture / 16;
                        for(uint8_t vertex = 0; vertex < 6; vertex++) {
                            model.data[vertices*2] =
                                (face << OFF_NORMAL) +
                                (face_vertices[face][vertex] << OFF_VERTEX) +
                                (x << OFF_X) + (y << OFF_Y) + (z << OFF_Z) +
                                (0 << OFF_AO) + (0 << OFF_DAMAGE_U) +
                                (0 << OFF_DAMAGE_V);

                            model.data[vertices*2 + 1] =
                                ((du + uv[vertex][0]) << OFF_DU) +
                                ((dv + uv[vertex][1]) << OFF_DV) +
                                (0x0F << OFF_AL) +
                                (0 << OFF_R) + (0 << OFF_G) +
                                (0 << OFF_B) + (0 << OFF_LIGHT);
                            vertices++;
                        }
                        if(vertices >= model.vertices) {
                            model = reallocate_chunk_block_model(model);
                        }
                    }
                }
            }
        }
    }
    model.vertices = vertices;
    return model;
}
