# BFT-Store: Storage Partition for Permissioned Blockchain via Erasure Coding

BFT-Store is a Byzantine fault-tolerant storage engine via EC for permissioned blockchain system, which reduces the storage complexity per block from O(n) to O(1) and improves the overall storage capability.

BFT-Store's library is able to use as a static library.

### Dependencies

------

- **Linux**
  - Upgrade your gcc to version at least 4.8 to get C++11 support.

### Using

------

The public interface is in `include/`. Callers should not include or rely on the details of any other header files in this package. Those internal APIs may be changed without warning.

Start with example in `example.cpp` and `CMakelists.txt`