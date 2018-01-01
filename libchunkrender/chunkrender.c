#include <stdlib.h>
#include "chunkrender.h"

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t z;
} point_t;

#define LEFT 0
#define RIGHT 1
#define TOP 2
#define BOTTOM 3
#define BACK 4
#define FRONT 5

static point_t point_in_slice(uint8_t face, uint8_t slice, uint8_t a, uint8_t b) {
    point_t point;
    if(face == LEFT || face == RIGHT) {
        point.x = slice;
        point.y = b;
        point.z = a;
    } else if(face == TOP || face == BOTTOM) {
        point.x = a;
        point.y = slice;
        point.z = b;
    } else {
        point.x = a;
        point.y = b;
        point.z = slice;
    }
    return point;
}

static int8_t face_direction(uint8_t face) {
    if(face == LEFT || face == BOTTOM || face == BACK) {
        return -1;
    } else {
        return 1;
    }
}

static uint32_t chunk_index(point_t point) {
    return point.x + point.y * CHUNK_SIZE + point.z * CHUNK_SIZE * CHUNK_SIZE;
}

static chunk_block_model_t allocate_chunk_block_model() {
    size_t vertices = 36;
    chunk_block_model_t model = {vertices, (uint32_t*)malloc(2*sizeof(uint32_t)*vertices)};
    return model;
}

static chunk_block_model_t reallocate_chunk_block_model(chunk_block_model_t old_model) {
    size_t vertices = old_model.vertices * 2;
    chunk_block_model_t model = {vertices, (uint32_t*)realloc(old_model.data, 2*sizeof(uint32_t)*vertices)};
    return model;
}

static void free_chunk_block_model(chunk_block_model_t model) {
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
    {5, 1, 6, 7, 5, 6}, // Back side
    {4, 3, 0, 2, 4, 0}  // Front side
};

static uint8_t uv[6][2] = {
    {0, 1},
    {0, 0},
    {1, 0},
    {1, 1},
    {0, 1},
    {1, 0}
};


static void render_face(point_t point, uint8_t face, uint32_t texture, uint32_t *data) {
    uint8_t du = texture % 16;
    uint8_t dv = texture / 16;
    for(uint8_t vertex = 0; vertex < 6; vertex++) {
        data[vertex*2] =
            (face << OFF_NORMAL) +
            (face_vertices[face][vertex] << OFF_VERTEX) +
            (point.x << OFF_X) +
            (point.y << OFF_Y) +
            (point.z << OFF_Z) +
            (0 << OFF_AO) +
            (0 << OFF_DAMAGE_U) +
            (0 << OFF_DAMAGE_V);

        data[vertex*2 + 1] =
            ((du + uv[vertex][0]) << OFF_DU) +
            ((dv + uv[vertex][1]) << OFF_DV) +
            (0x0F << OFF_AL) +
            (0 << OFF_R) + (0 << OFF_G) +
            (0 << OFF_B) + (0 << OFF_LIGHT);
    }
}

static uint8_t is_visible_through_next_slice(uint8_t face, uint8_t slice, uint8_t a, uint8_t b, int8_t direction,
        uint8_t *data, uint8_t *is_transparent) {
    int8_t next_slice = slice + direction;
    if(next_slice < 0 || next_slice >= CHUNK_SIZE) {
        // All blocks in other chunks are assumed transparent
        return 1;
    } else {
        // Otherwise check if the block type of the block in the next slice is transparent
        point_t point = point_in_slice(face, next_slice, a, b);
        uint32_t i = chunk_index(point);
        uint32_t block_type = data[i*BLOCK_SIZE + BLOCKS_HEADER_SIZE];
        return is_transparent[block_type];
    }
}

chunk_block_model_t render_chunk_blocks(uint8_t *data, uint8_t *is_transparent, uint8_t *state,
                                        uint32_t block_texture[][6]) {
    size_t vertices = 0;
    chunk_block_model_t model = allocate_chunk_block_model();
    for(uint8_t face = 0; face < 6; face++) {
        for(uint8_t slice = 0; slice < CHUNK_SIZE; slice++) {
            int direction = face_direction(face);
            for(uint8_t a = 0; a < CHUNK_SIZE; a++) {
                for(uint8_t b = 0; b < CHUNK_SIZE; b++) {
                    point_t point = point_in_slice(face, slice, a, b);
                    uint32_t i = chunk_index(point);
                    uint32_t block_type = data[i*BLOCK_SIZE + BLOCKS_HEADER_SIZE];
                    if(state[block_type] != STATE_GAS &&
                            is_visible_through_next_slice(face, slice, a, b, direction, data, is_transparent)) {
                        uint32_t texture = block_texture[block_type][face];
                        render_face(point, face, texture, model.data + vertices * 2);
                        vertices += 6;
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
