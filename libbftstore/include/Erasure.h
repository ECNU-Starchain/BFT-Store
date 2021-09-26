#pragma once
#include <sys/time.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <string>

#include "../erasure-codes/liberasure.h"
#include "BlockCache.hpp"
#include "Hash.h"
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"

using namespace rocksdb;

#define BLOCKS_SIZE_BYTE 3
#define NodeAddr_Type ec::h512

namespace ec {
class Erasure : public std::enable_shared_from_this<Erasure> {
  public:
    Erasure(int64_t _k, int64_t _m, NodeAddr_Type &_nodeid,
            std::vector<NodeAddr_Type> _sealers, std::string _path,
            int _cache_capacity)
        : ec_k(_k), ec_m(_m), ec_nodeid(_nodeid), ec_sealers(_sealers),
          ec_DBPath(_path), cache_capacity(_cache_capacity) {}
    // init EC
    bool initErasure();

    // pre-process input、encode and decode
    std::pair<byte *, int64_t> preprocess(std::vector<bytes> const &blocks);
    std::pair<byte **, int64_t>
    encode(std::pair<byte *, int64_t> blocks_rlp_data);
    bytes decode(std::vector<bytes> const &data, std::vector<int> const &index,
                 int target_block_num);

    // array from one-dimensional  to two-dimensional
    void generate_ptrs(size_t data_size, uint8_t *data, erasure_bool *present,
                       uint8_t **ptrs);

    // write chunk
    void saveChunk(std::vector<bytes> const &blocks, int last_block_number);
    bool writeDB(unsigned int coding_epoch,
                 std::pair<unsigned int, byte *> chunk, int64_t chunk_size,
                 bool flag);

    // unpack chunk to block
    bytes unpackChunkToBlock(bytes const &data);
    // read chunk
    std::string readChunk(unsigned int coding_epoch, unsigned int chunk_pos);
    // read block
    bytes getBlockByNumber(unsigned int block_num);
    bytes localReadBlock(unsigned int coding_epoch, unsigned int chunk_pos);
    NodeAddr_Type remoteReadBlock(unsigned int block_num);

    // key's format in db
    std::string GetChunkDataKey(unsigned int coding_epoch, unsigned chunk_pos);

    // compute the set of nodes in current epoch
    // i-th chunk stored by chunk_set[i]-th node
    int *getDistinctChunkSet(unsigned int coding_epoch);

    NodeAddr_Type computeBlocksInWhichNode(int block_number);
    std::pair<int, int> computeChunkPosition(int block_number);

    int64_t findSequenceInSealers();

    int64_t getK() { return ec_k; }
    int64_t getM() { return ec_m; }

    unsigned int getPosition() { return ec_position_in_sealers; }
    std::vector<int> getChunkPosByNodeIdAndEpoch(int epoch);

  private:
    NodeAddr_Type ec_nodeid;                                // node ID
    erasure_encoder_flags ec_mode = ERASURE_FORCE_ADV_IMPL; // EC mode
    int64_t ec_k;                          // number of data chunks
    int64_t ec_m;                          // number of parity chunks
    std::vector<NodeAddr_Type> ec_sealers; // address list of all nodes
    unsigned int ec_position_in_sealers; // the index of node in the ec_sealers
    rocksdb::DB *ec_db;                  // DB handle
    std::string ec_DBPath;               // DB path
    std::shared_ptr<ec::LRUCache> ec_cache;
    int cache_capacity = 10;            // cache capacity
    int64_t complete_coding_epoch = -1; // completed ECepoch
};

} // namespace ec