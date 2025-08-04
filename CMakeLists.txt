cmake_minimum_required(VERSION 3.11)
project(SnakeRaylib)

set(CMAKE_CXX_STANDARD 20)

# FetchContent modülünü include et
include(FetchContent)

# Raylib'i otomatik olarak indir ve derle
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
)

FetchContent_MakeAvailable(raylib)

# Executable oluştur
add_executable(SnakeRaylib main.cpp)

# Raylib'i bağla
target_link_libraries(SnakeRaylib raylib)

# Graphics ve Sounds klasörlerini build dizinine kopyala
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Graphics
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Sounds
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Windows için ek ayarlar
if(WIN32)
    target_link_libraries(SnakeRaylib winmm)
endif()