# BFT-Store: Storage Partition for Permissioned Blockchain via Erasure Coding

BFT-Store is a Byzantine fault-tolerant storage engine via EC for permissioned blockchain system, which reduces the storage complexity per block from O(n) to O(1) and improves the overall storage capability.

For more details:

[《 BFT-Store: Storage Partition for Permissioned Blockchain via Erasure Coding (ICDE2020) 》][bft-store]
[《 A Byzantine Fault Tolerant Storage for Permissioned Blockchain. (SIGMOD2021) 》][bft-store-sigmod]
[《 A Reliable Storage Partition for Permissioned Blockchain. (TKDE2021) 》][bft-store-tkde]

[bft-store]: https://ieeexplore.ieee.org/document/9101675
[bft-store-sigmod]: https://dl.acm.org/doi/10.1145/3448016.3452744
[bft-store-tkde]: https://ieeexplore.ieee.org/document/9152150/


BFT-Store's library is able to use as a static library.

### Dependencies

------

- **Linux**
  - Upgrade your gcc to version at least 4.8 to get C++11 support.

### Source code layout

------

The BFT-Store root directory contains the libbftstore directory,  the CMakeLists.txt, this README, and an example  for BFT-Store. 

Inside the libbftstore directory are the following important directories:

- `cmake`: contains libraries BFT-Store uses.
- `erasure-codes`, `gf-arithmetic`, `log-table-generator`, `lohi-generator`: contains the implementation of erasure codes.
- `include`: contains header files of BFT-Store.
- `src`: contains the implementation of BFT-Store.

### Using 

------

The public interface is in `include/`. Callers should not include or rely on the details of any other header files in this package. Those internal APIs may be changed without warning.

Start with example in `example.cpp` and `CMakelists.txt`

Note: Different from the project in the paper,the source code is an independent library code from the project, which including reading, encoding, decoding and recoverying without network. Users need to provide network modules to support BFT-Store.
