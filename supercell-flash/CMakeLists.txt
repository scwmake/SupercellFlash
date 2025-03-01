# Project: supercell-flash
# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp source/*.hpp)
file(GLOB SC2_SCHEMAS sc2_schemas/*.fbs)

# choosing build type
if(WK_BUILD_SHARED)
    # shared library
    set(TARGET supercell-flash-shared)
    add_library(${TARGET} SHARED)
    
else()
    # static library
    set(TARGET supercell-flash-static)
    add_library(${TARGET} STATIC)

endif()

add_library(supercell::flash ALIAS ${TARGET})

# add *.h and *.cpp files
target_sources(${TARGET} PRIVATE ${HEADERS} ${SOURCES})

# include directories
target_include_directories(${TARGET} PUBLIC source)

target_link_libraries(${TARGET} PUBLIC wk::core)
target_link_libraries(${TARGET} PUBLIC supercell::compression)
target_link_libraries(${TARGET} PUBLIC supercell::texture)

wk_flatbuffers_generate_headers(
    TARGET SC2_API
    INCLUDE_PREFIX flash/SC2
    SCHEMAS ${SC2_SCHEMAS}
    FLAGS --cpp-std=c++17
)

set_target_properties(${TARGET} PROPERTIES
    FOLDER SupercellSDK/core
)

target_link_libraries(${TARGET}
    PUBLIC SC2_API
)

# setup project
wk_project_setup(${TARGET})
wk_include_flatbuffers()
