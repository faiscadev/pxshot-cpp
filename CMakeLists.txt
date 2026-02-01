cmake_minimum_required(VERSION 3.16)

project(pxshot
    VERSION 1.0.0
    DESCRIPTION "Official C++ SDK for Pxshot Screenshot API"
    HOMEPAGE_URL "https://pxshot.com"
    LANGUAGES CXX
)

# =============================================================================
# Options
# =============================================================================

option(PXSHOT_BUILD_EXAMPLES "Build example programs" ON)
option(PXSHOT_BUILD_TESTS "Build unit tests" OFF)
option(PXSHOT_INSTALL "Generate install target" ON)

# =============================================================================
# C++ Standard
# =============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =============================================================================
# Dependencies
# =============================================================================

include(FetchContent)

# cpp-httplib (header-only HTTP library)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)
FetchContent_MakeAvailable(httplib)

# nlohmann/json (header-only JSON library)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# OpenSSL (required for HTTPS)
find_package(OpenSSL REQUIRED)

# =============================================================================
# Library Target
# =============================================================================

add_library(pxshot
    src/pxshot.cpp
)

add_library(pxshot::pxshot ALIAS pxshot)

target_include_directories(pxshot
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(pxshot
    PUBLIC
        nlohmann_json::nlohmann_json
    PRIVATE
        httplib::httplib
        OpenSSL::SSL
        OpenSSL::Crypto
)

target_compile_features(pxshot PUBLIC cxx_std_17)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(pxshot PRIVATE
        -Wall -Wextra -Wpedantic -Werror=return-type
    )
elseif(MSVC)
    target_compile_options(pxshot PRIVATE /W4)
endif()

# =============================================================================
# Examples
# =============================================================================

if(PXSHOT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# =============================================================================
# Installation
# =============================================================================

if(PXSHOT_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)
    
    # Install library
    install(TARGETS pxshot
        EXPORT pxshotTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    # Install headers
    install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    
    # Install CMake config
    install(EXPORT pxshotTargets
        FILE pxshotTargets.cmake
        NAMESPACE pxshot::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pxshot
    )
    
    # Generate config file
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/pxshotConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/pxshotConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pxshot
    )
    
    # Generate version file
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/pxshotConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    
    # Install config files
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/pxshotConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/pxshotConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pxshot
    )
endif()

# =============================================================================
# Package Info
# =============================================================================

message(STATUS "")
message(STATUS "Pxshot C++ SDK v${PROJECT_VERSION}")
message(STATUS "  Build examples: ${PXSHOT_BUILD_EXAMPLES}")
message(STATUS "  Build tests:    ${PXSHOT_BUILD_TESTS}")
message(STATUS "  Install:        ${PXSHOT_INSTALL}")
message(STATUS "")
