#include "gtest/gtest.h"

extern "C" {
#include "chunkrender.h"
}

char data[BLOCK_BUFFER_SIZE];

TEST(ChunkRenderTest, EmptyChunkHasSizeZero) {
    memset(data, 0, BLOCK_BUFFER_SIZE);
    chunk_block_model_t model = render_chunk_blocks(data, NULL, NULL, NULL);
    EXPECT_EQ(model.size, 0);
}
