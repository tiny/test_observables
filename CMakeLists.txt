cmake_minimum_required(VERSION 3.10)
project(test_observables)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX OR APPLE)
    add_definitions(-DLINUX -DUNIX)
endif() 

include(FetchContent)

FetchContent_Declare(
    go_data
    GIT_REPOSITORY  https://github.com/tiny/go_data.git
    GIT_TAG         master
)

FetchContent_GetProperties(go_data)
if (NOT go_data_POPULATED)
    FetchContent_Populate(go_data)
    message(STATUS "libgo_data source dir: ${go_data_SOURCE_DIR}")
    add_subdirectory(${go_data_SOURCE_DIR} ${go_data_BINARY_DIR})
endif() 
FetchContent_MakeAvailable(go_data)

# Include directories
# include_directories(../go_data/include)
target_include_directories(libgo_data INTERFACE ${go_data_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} 
    src/main_observables.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE libgo_data)

