#include "gtest/gtest.h"

extern "C" {
#include "chunkrender.h"
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
        uint32_t d1 = model.data[i * 2];
        uint32_t d2 = model.data[i * 2 + 1];
        vertex_t vertex = vertices[i];

        EXPECT_EQ(vertex.normal, (d1 >> OFF_NORMAL) & MASK_NORMAL) << "Normal of vertex " << i << " does not match.";
        EXPECT_EQ(vertex.vertex, (d1 >> OFF_VERTEX) & MASK_VERTEX) << "Vertex number of vertex " << i << " does not match.";
        EXPECT_EQ(vertex.x, (d1 >> OFF_X) & MASK_POS) << "X coordinate of vertex " << i << " does not match.";
        EXPECT_EQ(vertex.y, (d1 >> OFF_Y) & MASK_POS) << "Y coordinate of vertex " << i << " does not match.";
        EXPECT_EQ(vertex.z, (d1 >> OFF_Z) & MASK_POS) << "Z coordinate of vertex " << i << " does not match.";
        EXPECT_EQ(vertex.du, (d2 >> OFF_DU) & MASK_UV) << "U texture coordinate of vertex " << i << " does not match.";
        EXPECT_EQ(vertex.dv, (d2 >> OFF_DV) & MASK_UV) << "V texture coordinate of vertex " << i << " does not match.";
    }
}

uint8_t data[BLOCK_BUFFER_SIZE];

void write_block_type(uint32_t i, uint32_t block_type, uint8_t *data) {
    data[i * BLOCK_SIZE + BLOCKS_HEADER_SIZE] = block_type & 0xFF;
    data[i * BLOCK_SIZE + 1 + BLOCKS_HEADER_SIZE] = (block_type >> 8) & 0xFF;
}

void set(uint32_t x, uint32_t y, uint32_t z, uint32_t block_type, uint8_t *data) {
    uint32_t i = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
    write_block_type(i, block_type, data);
}

void clear(uint8_t *data) {
    memset(data, 0, BLOCK_BUFFER_SIZE);
    data[0] = 3; // Set chunk format version
}

// Type 0 is transparent, type 1 is opaque
static uint8_t is_transparent[] = {1, 0};

// Type 0 is gas, type 1 is solid
static uint8_t state[] = {STATE_GAS, STATE_SOLID};

// Type 0 has texture 0, type 1 has texture 0
static uint32_t texture[2][6] = {
    {0,0,0,0,0,0},
    {0,0,0,0,0,0}
};


TEST(ChunkRenderTest, EmptyChunkHasSizeZero) {
    clear(data);
    // Type 0 is gas, type 1 is solid
    uint8_t state[] = {STATE_GAS};
    chunk_block_model_t model = render_chunk_blocks(data, NULL, state, NULL);
    EXPECT_EQ(model.vertices, 0);
}

TEST(ChunkRenderTest, OneBlockHasValidVertices) {
    clear(data);

    // Create a single block on 0,0,0 with type 1
    set(0,0,0,1,data);

    // Create list with 36 vertices
    vertex_t vertices[] = {
        // norm, vertex, x, y, z, u, v
        {0, 2, 0, 0, 0, 0, 1}, // Left side
        {0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0},
        {0, 5, 0, 0, 0, 1, 1},
        {0, 2, 0, 0, 0, 0, 1},
        {0, 1, 0, 0, 0, 1, 0},

        {1, 7, 0, 0, 0, 0, 1}, // Right side
        {1, 6, 0, 0, 0, 0, 0},
        {1, 3, 0, 0, 0, 1, 0},
        {1, 4, 0, 0, 0, 1, 1},
        {1, 7, 0, 0, 0, 0, 1},
        {1, 3, 0, 0, 0, 1, 0},

        {2, 2, 0, 0, 0, 0, 1}, // Up side
        {2, 5, 0, 0, 0, 0, 0},
        {2, 7, 0, 0, 0, 1, 0},
        {2, 4, 0, 0, 0, 1, 1},
        {2, 2, 0, 0, 0, 0, 1},
        {2, 7, 0, 0, 0, 1, 0},

        {3, 1, 0, 0, 0, 0, 1}, // Down side
        {3, 0, 0, 0, 0, 0, 0},
        {3, 3, 0, 0, 0, 1, 0},
        {3, 6, 0, 0, 0, 1, 1},
        {3, 1, 0, 0, 0, 0, 1},
        {3, 3, 0, 0, 0, 1, 0},

        {4, 5, 0, 0, 0, 0, 1}, // Front side
        {4, 1, 0, 0, 0, 0, 0},
        {4, 6, 0, 0, 0, 1, 0},
        {4, 7, 0, 0, 0, 1, 1},
        {4, 5, 0, 0, 0, 0, 1},
        {4, 6, 0, 0, 0, 1, 0},

        {5, 4, 0, 0, 0, 0, 1}, // Back side
        {5, 3, 0, 0, 0, 0, 0},
        {5, 0, 0, 0, 0, 1, 0},
        {5, 2, 0, 0, 0, 1, 1},
        {5, 4, 0, 0, 0, 0, 1},
        {5, 0, 0, 0, 0, 1, 0}
    };

    chunk_block_model_t model = render_chunk_blocks(data, is_transparent, state, texture);
    EXPECT_EQ(model.vertices, 36)  << "The number of vertices does not match";
    validate_vertices(model, vertices);
}

TEST(ChunkRenderTest, TwoBlocksHasNoVerticesBetweenThem) {
    clear(data);

    // Create two blocks on 0,0,0 and 1,0,0 with type 1
    set(0,0,0,1,data);
    set(1,0,0,1,data);

    // Create list with 36 vertices
    vertex_t vertices[] = {
        // norm, vertex, x, y, z, u, v
        {0, 2, 0, 0, 0, 0, 1}, // Left side, first block
        {0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0},
        {0, 5, 0, 0, 0, 1, 1},
        {0, 2, 0, 0, 0, 0, 1},
        {0, 1, 0, 0, 0, 1, 0},

        {1, 7, 1, 0, 0, 0, 1}, // Right side, second block
        {1, 6, 1, 0, 0, 0, 0},
        {1, 3, 1, 0, 0, 1, 0},
        {1, 4, 1, 0, 0, 1, 1},
        {1, 7, 1, 0, 0, 0, 1},
        {1, 3, 1, 0, 0, 1, 0},

        {2, 2, 0, 0, 0, 0, 1}, // Up side, first block
        {2, 5, 0, 0, 0, 0, 0},
        {2, 7, 0, 0, 0, 1, 0},
        {2, 4, 0, 0, 0, 1, 1},
        {2, 2, 0, 0, 0, 0, 1},
        {2, 7, 0, 0, 0, 1, 0},

        {2, 2, 1, 0, 0, 0, 1}, // Up side, second block
        {2, 5, 1, 0, 0, 0, 0},
        {2, 7, 1, 0, 0, 1, 0},
        {2, 4, 1, 0, 0, 1, 1},
        {2, 2, 1, 0, 0, 0, 1},
        {2, 7, 1, 0, 0, 1, 0},

        {3, 1, 0, 0, 0, 0, 1}, // Down side, first block
        {3, 0, 0, 0, 0, 0, 0},
        {3, 3, 0, 0, 0, 1, 0},
        {3, 6, 0, 0, 0, 1, 1},
        {3, 1, 0, 0, 0, 0, 1},
        {3, 3, 0, 0, 0, 1, 0},

        {3, 1, 1, 0, 0, 0, 1}, // Down side, second block
        {3, 0, 1, 0, 0, 0, 0},
        {3, 3, 1, 0, 0, 1, 0},
        {3, 6, 1, 0, 0, 1, 1},
        {3, 1, 1, 0, 0, 0, 1},
        {3, 3, 1, 0, 0, 1, 0},

        {4, 5, 0, 0, 0, 0, 1}, // Front side, first block
        {4, 1, 0, 0, 0, 0, 0},
        {4, 6, 0, 0, 0, 1, 0},
        {4, 7, 0, 0, 0, 1, 1},
        {4, 5, 0, 0, 0, 0, 1},
        {4, 6, 0, 0, 0, 1, 0},

        {4, 5, 1, 0, 0, 0, 1}, // Front side, second block
        {4, 1, 1, 0, 0, 0, 0},
        {4, 6, 1, 0, 0, 1, 0},
        {4, 7, 1, 0, 0, 1, 1},
        {4, 5, 1, 0, 0, 0, 1},
        {4, 6, 1, 0, 0, 1, 0},

        {5, 4, 0, 0, 0, 0, 1}, // Back side, first block
        {5, 3, 0, 0, 0, 0, 0},
        {5, 0, 0, 0, 0, 1, 0},
        {5, 2, 0, 0, 0, 1, 1},
        {5, 4, 0, 0, 0, 0, 1},
        {5, 0, 0, 0, 0, 1, 0},

        {5, 4, 1, 0, 0, 0, 1}, // Back side, second block
        {5, 3, 1, 0, 0, 0, 0},
        {5, 0, 1, 0, 0, 1, 0},
        {5, 2, 1, 0, 0, 1, 1},
        {5, 4, 1, 0, 0, 0, 1},
        {5, 0, 1, 0, 0, 1, 0}
    };

    chunk_block_model_t model = render_chunk_blocks(data, is_transparent, state, texture);
    EXPECT_EQ(model.vertices, 60) << "The number of vertices does not match";
    validate_vertices(model, vertices);
}
