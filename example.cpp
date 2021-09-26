#include "Erasure.h"

int main() {

    /*create Erasure*/
    int k = 4;
    int m = 2;
    ec::h512 nodeId = ec::h512(
        "6c4b3191f284c2faf6a0bab1f81af41d60f148ad5e0ba71c99f98264b2b765cb6"
        "55c8b5d33c4b64bb39e436f2f6fb85e4fd7bd83b4e9e7ba0f8ec05fc8db61fb");
    std::vector<ec::h512> sealerlist = {
        ec::h512(
            "6c4b3191f284c2faf6a0bab1f81af41d60f148ad5e0ba71c99f98264b2b765cb6"
            "55c8b5d33c4b64bb39e436f2f6fb85e4fd7bd83b4e9e7ba0f8ec05fc8db61fb"),
        ec::h512(
            "73d1affea7fe2a7e77d484753e34e05cc1321c8bd53725bc8a785f70c9e636da1"
            "9b96daef19c4237ecb82f3d2dbda19fd8256ec6ce8289a35ecf153ef71b9096"),
        ec::h512(
            "8f7d9665ecc68a419dac336e2e5401fe169e90f6ab5aae39d527808054b2cb4be"
            "3fdfa4062e12336ddfefb154bdcbc3b25502f90fe8ded85c0e195738df9a84e"),
        ec::h512(
            "9f39d729a955919cbb99527ea3868b20a8247be22bad4bcbd9a2fe277c095800e"
            "10cb84b580a91a58d7f25b6424a3c372f69f1bf177cfc564081ecd66a078760"),
        ec::h512(
            "bf2e09769f4aa671473d3751bc6a6c0f672989379ad8d60510caa8d6653a2385d"
            "6b0b335e734590c859e218f4200c3f1e486362ca9444e6956a4415fa739aed8"),
        ec::h512(
            "e9839f7d590588cfd8b13dd73670da4d6879837a6def7e3e46dfab977c8b09d790"
            "30af9f03442ba3beda12a4960fbd9a131c8c3ef32f43ca3189e83e19894590")};
    std::string dbpath = "./ecstorage/";
    unsigned capacity = 20;
    std::shared_ptr<ec::Erasure> erasure = std::make_shared<ec::Erasure>(
        k, m, nodeId, sealerlist, dbpath, capacity);

    erasure->initErasure();

    /*encode*/
    std::vector<ec::bytes> input_data(4, ec::bytes(1000, 1));
    unsigned epoch_max_block_number = 10;
    erasure->saveChunk(input_data, epoch_max_block_number);

    /*read*/
    unsigned read_block_number = 8;
    ec::bytes data = erasure->getBlockByNumber(read_block_number);
    if (data.size() != 0) {
        /// block stored locally or in cache
    } else {
        /// remote read

        // compute the block is stored in which node: target_node_id
        // compute the epoch and the position of the block: <epoch,chunk_pos>
        auto target_node_id = computeBlocksInWhichNode(read_block_number);
        auto position = erasure->computeChunkPosition(read_block_number);

        // request the block by Network
        // to do by user
        data = getChunkByNetwork(position, target_node_id);
        if (data is correct) {
            // unpack Chunk to Block data
            ec::bytes block_data = erasure->unpackChunkToBlock(data);
        } else {
            // decode
        }
    }

    /*decode when time-out or the chunk recieved from remote node is incorrect*/

    std::vector<bytes> receive_data;
    std::vector<int> index;
    auto target_node_id = computeBlocksInWhichNode(read_block_number);
    auto [target_epoch, chunk_pos] =
        erasure->computeChunkPosition(read_block_number);

    // node_id_to_chunk_pos means that the index of the node in the sealerlist
    // stored which chunk in the target_epoch.
    auto node_id_to_chunk_pos =
        m_erasure->getChunkPosByNodeIdAndEpoch(target_epoch);

    // request k chunks which the epoch including read_block_number from all
    // nodes
    for (int i = 0; i < sealerlist.size(); ++i) {
        // to do by user
        requestChunk(target_epoch, node_id_to_chunk_pos[i], sealerlist[i]);
    }

    // when recieved k correct chunks, start erasure code recovery
    ec::bytes decode_target_data =
        m_erasure->decode(receive_data, index, read_block_number);
    ec::bytes block_data = erasure->unpackChunkToBlock(decode_target_data);
    
    
    return 0;
}
