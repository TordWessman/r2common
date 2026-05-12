cmake_minimum_required(VERSION 3.14)
project(r2web3 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

# --- r2common dependency ---
# Use the subtree copy if present; otherwise fetch from GitHub.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/r2Common/Result.h")
    message(STATUS "r2web3: using local r2Common subtree")
    set(R2COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/r2Common")
    set(R2COMMON_SOURCES
        src/r2Common/R2Logger.cpp
        src/r2Common/Result.cpp
        src/r2Common/Network/CurlNetwork.cpp
        src/r2Common/Network/HttpRequest.cpp
    )
else()
    include(FetchContent)
    FetchContent_Declare(
        r2common
        GIT_REPOSITORY https://github.com/TordWessman/r2common.git
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(r2common)
    set(R2COMMON_INCLUDE_DIR "${r2common_SOURCE_DIR}")
    set(R2COMMON_SOURCES
        ${r2common_SOURCE_DIR}/R2Logger.cpp
        ${r2common_SOURCE_DIR}/Result.cpp
        ${r2common_SOURCE_DIR}/Network/CurlNetwork.cpp
        ${r2common_SOURCE_DIR}/Network/HttpRequest.cpp
    )
endif()

# --- Find libcurl (required for desktop builds) ---
find_package(CURL REQUIRED)

# --- Source files ---
set(BLOCKCHAIN_SOURCES
    src/Blockchain/ABIDecoder.cpp
    src/Blockchain/ABIEncoder.cpp
    src/Blockchain/Account.cpp
    src/Blockchain/Chain.cpp
    src/Blockchain/Contract.cpp
    src/Blockchain/EthereumSigner.cpp
    src/Blockchain/EthereumTransaction.cpp
    src/Blockchain/RlpEncoder.cpp
    src/Blockchain/Signer.cpp
    src/Blockchain/Internal/Chain_ethRequest.cpp
)

set(SHARED_SOURCES
    src/Shared/BigNumber.cpp
    src/Shared/Common.cpp
    src/Shared/cJSON.c
)

set(CRYPTO_SOURCES
    src/cryptography/bignum.c
    src/cryptography/ecdsa.c
    src/cryptography/hmac.c
    src/cryptography/memzero.c
    src/cryptography/rand.c
    src/cryptography/rfc6979.c
    src/cryptography/secp256k1.c
    src/cryptography/sha2.c
    src/cryptography/sha3.c
)

# --- Library target ---
add_library(r2web3
    ${BLOCKCHAIN_SOURCES}
    ${SHARED_SOURCES}
    ${CRYPTO_SOURCES}
    ${R2COMMON_SOURCES}
)

target_include_directories(r2web3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Shared
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Blockchain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cryptography
    ${R2COMMON_INCLUDE_DIR}
    ${R2COMMON_INCLUDE_DIR}/Network
)

target_link_libraries(r2web3 PUBLIC CURL::libcurl)
