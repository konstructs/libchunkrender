#include "gtest/gtest.h"

extern "C" {
#include "chunkrender.h"
}

char data[BLOCK_BUFFER_SIZE];

void write_block_type(uint32_t i, uint32_t block_type) {
    data[i * BLOCK_SIZE + BLOCKS_HEADER_SIZE] = block_type & 0xFF;
    data[i * BLOCK_SIZE + 1 + BLOCKS_HEADER_SIZE] = (block_type >> 8) & 0xFF;
}

void write_health_and_orientation(uint32_t i, uint16_t health, uint8_t direction, uint8_t rotation) {
    data[i * BLOCK_SIZE + 2 + BLOCKS_HEADER_SIZE] = (uint8_t)(health & 0xFF);
    data[i * BLOCK_SIZE + 3 + BLOCKS_HEADER_SIZE] = ((direction << 5) & 0xE0) + ((rotation << 3) & 0x18) + (uint8_t)((health >> 8) & 0x07);
}

void write_light(uint32_t i, uint8_t ambient, uint8_t red, uint8_t green, uint8_t blue, uint8_t light) {
    data[i * BLOCK_SIZE + 4 + BLOCKS_HEADER_SIZE] = (ambient & 0x0F) + ((red << 4) & 0xF0);
    data[i * BLOCK_SIZE + 5 + BLOCKS_HEADER_SIZE] = (green & 0x0F) + ((blue << 4) & 0xF0);
    data[i * BLOCK_SIZE + 6 + BLOCKS_HEADER_SIZE] = light & 0x0F;
}

void set(uint32_t x, uint32_t y, uint32_t z, uint32_t block_type, uint32_t health, uint8_t direction, uint8_t rotation, uint8_t ambient, uint8_t red, uint8_t green, uint8_t blue, uint8_t light) {
    uint32_t i = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
    write_block_type(i, block_type);
    write_health_and_orientation(i, health, direction, rotation);
    write_light(i, ambient, red, green, blue, light);
}

void clear() {
    memset(data, 0, BLOCK_BUFFER_SIZE);
    data[0] = 3; // Set chunk format version
}

typedef struct {
    uint8_t normal;
    uint8_t vertex;
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t du;
    uint8_t dv;
} vertex_t;

#define OFF_NORMAL 0
#define MASK_NORMAL 0x07
#define OFF_VERTEX 3
#define MASK_VERTEX 0x0F
#define OFF_X 7
#define OFF_Y 12
#define OFF_Z 17
#define MASK_POS 0x1F
#define MASK_VERTEX 0x0F
#define OFF_DU 0
#define OFF_DV 5
#define MASK_UV 0x1F

void validate_vertices(chunk_block_model_t model, vertex_t *vertices) {
    for(int i = 0; i < model.vertices; i++) {
        uint32_t d1 = model.data[i];
        uint32_t d2 = model.data[i+1];
        vertex_t vertex = vertices[i];

        EXPECT_EQ(vertex.normal, (d1 >> OFF_NORMAL) & MASK_NORMAL);
        EXPECT_EQ(vertex.vertex, (d1 >> OFF_VERTEX) & MASK_VERTEX);
        EXPECT_EQ(vertex.x, (d1 >> OFF_X) & MASK_POS);
        EXPECT_EQ(vertex.y, (d1 >> OFF_Y) & MASK_POS);
        EXPECT_EQ(vertex.z, (d1 >> OFF_Z) & MASK_POS);
        EXPECT_EQ(vertex.du, (d2 >> OFF_DU) & MASK_UV);
        EXPECT_EQ(vertex.dv, (d2 >> OFF_DV) & MASK_UV);
    }
}

TEST(ChunkRenderTest, EmptyChunkHasSizeZero) {
    clear();
    chunk_block_model_t model = render_chunk_blocks(data, NULL, NULL, NULL);
    EXPECT_EQ(model.vertices, 0);
}
