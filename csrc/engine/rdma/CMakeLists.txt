add_library(
    _slime_rdma
    SHARED
    memory_pool.cpp
    rdma_transport.cpp
)

target_link_libraries(_slime_rdma PUBLIC _slime_utils ibverbs)
