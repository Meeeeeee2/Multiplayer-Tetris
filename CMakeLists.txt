# =========================================================
#  CMake Setup
# =========================================================

cmake_minimum_required(VERSION 3.16)

project(Tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
# Set this to ON when shipping the game
# This changes RESOURCES_PATH to local executable path
#! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
set(PRODUCTION_BUILD OFF CACHE BOOL "Production build")

# =========================================================
#  MSVC SETTINGS
# =========================================================

if(MSVC)

    # Static runtime
    # Debug   -> /MTd
    # Release -> /MT
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # SIMD optimizations
    add_compile_options(/arch:AVX2)

    # Disable annoying CRT warnings
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

endif()

# =========================================================
#  GLOBAL OPTIMIZATIONS
# =========================================================

# Link Time Optimization
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Build ALL libraries statically
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

# =========================================================
#  THIRD PARTY LIBRARIES
# =========================================================

# raylib
add_subdirectory(thirdparty/raylib-5.0)

# imgui docking branch
add_subdirectory(thirdparty/imgui-docking)

# rlImGui
add_subdirectory(thirdparty/rlImGui)

# ENet
add_subdirectory(thirdparty/enet)




# =========================================================
#  CLIENT
# =========================================================

# Automatically grab all client cpp files
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/client/src/*.cpp"
)



# Create executable
add_executable(TetrisClient)

target_compile_options(TetrisClient PRIVATE /showIncludes)

# Add source files
target_sources(TetrisClient PRIVATE ${CLIENT_SOURCES})

# Use C++17
set_property(TARGET TetrisClient PROPERTY CXX_STANDARD 17)

# Include directories
target_include_directories(TetrisClient PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/client/include/"
)

# =========================================================
#  CLIENT DEFINITIONS
# =========================================================

if(PRODUCTION_BUILD)

    # Production assets path
    target_compile_definitions(TetrisClient PUBLIC
        RESOURCES_PATH="./resources/"
        PRODUCTION_BUILD=1
    )

else()

    # Development assets path
    target_compile_definitions(TetrisClient PUBLIC
        RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/"
        PRODUCTION_BUILD=0
    )

endif()

# =========================================================
#  CLIENT LINKING
# =========================================================

target_link_libraries(TetrisClient PUBLIC

    # Graphics
    raylib_static
    imgui
    rlimgui

    # Networking
    enet
)



# =========================================================
#  SERVER
# =========================================================

# Automatically grab all server cpp files
file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/server/src/*.cpp"
)

# Create executable
add_executable(TetrisServer)

# Add source files
target_sources(TetrisServer PRIVATE ${SERVER_SOURCES})

# Use C++17
set_property(TARGET TetrisServer PROPERTY CXX_STANDARD 17)

# Include directories
target_include_directories(TetrisServer PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/server/include/"
)

# =========================================================
#  SERVER DEFINITIONS
# =========================================================

if(PRODUCTION_BUILD)

    target_compile_definitions(TetrisServer PUBLIC
        RESOURCES_PATH="./resources/"
        PRODUCTION_BUILD=1
    )

else()

    target_compile_definitions(TetrisServer PUBLIC
        RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/"
        PRODUCTION_BUILD=0
    )

endif()

# =========================================================
#  SERVER LINKING
# =========================================================

target_link_libraries(TetrisServer PUBLIC

   

    # Networking
    enet
)

# =========================================================
#  OPTIONAL:
#  Remove console window in production builds
# =========================================================

# if(MSVC AND PRODUCTION_BUILD)
#
#     set_target_properties(TetrisClient PROPERTIES
#         LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
#     )
#
# endif()