set(TEST_BINARY obc-firmware-tests)

set(TEST_DEPENDENCIES
    ${CMAKE_SOURCE_DIR}/obc/sys/time/obc_time_utils.c
    ${CMAKE_SOURCE_DIR}/obc/drivers/arducam/image_processing.c
    ${CMAKE_SOURCE_DIR}/obc/drivers/vn100/vn100_binary_parsing.c
    ${CMAKE_SOURCE_DIR}/interfaces/obc_gs_interface/common/obc_gs_crc.c
    ${CMAKE_SOURCE_DIR}/interfaces/data_pack_unpack/data_unpack_utils.c
    ${CMAKE_SOURCE_DIR}/obc/sys/persistent/obc_persistent.c
)

set(TEST_MOCKS
    ${CMAKE_SOURCE_DIR}/test/mocks/mock_logging.c
    ${CMAKE_SOURCE_DIR}/test/mocks/mock_fram.c
    ${CMAKE_SOURCE_DIR}/test/mocks/mock_crc.c
)

set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/main.cpp
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/test_obc_time_utils.cpp
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/test_image_processing.cpp
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/test_vn100_unpack.cpp
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/test_obc_persistent.cpp
)

set(TEST_SOURCES ${TEST_SOURCES} ${TEST_DEPENDENCIES} ${TEST_MOCKS})

add_executable(${TEST_BINARY} ${TEST_SOURCES})

target_include_directories(${TEST_BINARY}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/obc/sys
    ${CMAKE_SOURCE_DIR}/obc/sys/time
    ${CMAKE_SOURCE_DIR}/obc/sys/utils
    ${CMAKE_SOURCE_DIR}/obc/sys/logging
    ${CMAKE_SOURCE_DIR}/obc/sys/persistent
    ${CMAKE_SOURCE_DIR}/obc/drivers/ds3232
    ${CMAKE_SOURCE_DIR}/obc/drivers/arducam
    ${CMAKE_SOURCE_DIR}/obc/drivers/vn100
    ${CMAKE_SOURCE_DIR}/interfaces/obc_gs_interface/common
    ${CMAKE_SOURCE_DIR}/interfaces/data_pack_unpack
    ${CMAKE_SOURCE_DIR}/obc/drivers/fram
    ${CMAKE_SOURCE_DIR}/obc/reliance_edge/projects/freertos_rm46/host/ # redconf.h
    ${CMAKE_SOURCE_DIR}/obc/reliance_edge/include # redconf.h
    ${CMAKE_SOURCE_DIR}/obc/modules/alarm_mgr
    ${CMAKE_SOURCE_DIR}/obc/modules/command_mgr
    ${CMAKE_SOURCE_DIR}/interfaces/obc_gs_interface/commands
)

target_link_libraries(${TEST_BINARY}
    PRIVATE
    GTest::GTest
)

add_test(${TEST_BINARY} ${TEST_BINARY})
