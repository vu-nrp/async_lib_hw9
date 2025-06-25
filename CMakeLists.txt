cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(async VERSION ${PROJECT_VESRION})

# main lib
add_library(${PROJECT_NAME}
    lib/types.h
    lib/async.h
    lib/async.cpp
    lib/commandparser.h
    lib/commandparser.cpp
)

# test executable
add_executable(${PROJECT_NAME}_test
    test/main.cpp
)

set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# uncomment for inplace debug
# add_compile_definitions(DEBUG_ON)

#
target_include_directories(${PROJECT_NAME}
PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_BINARY_DIR}
)

# test executable linking the lib
target_link_libraries(${PROJECT_NAME}_test
PRIVATE
    ${PROJECT_NAME}
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT vu-nrp@yandex.ru)
include(CPack)
