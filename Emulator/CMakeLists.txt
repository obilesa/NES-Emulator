cmake_minimum_required(VERSION 3.16)
project(NES_Emulator)

set(CMAKE_CXX_STANDARD 20)
set(${CMAKE_CXX_FLAGS} -Wall -Wextra)
#[[include(FetchContent)]]
#[[if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")]]
#add_compile_options("-DEMCC_DEBUG")

set(USE_FLAGS "-s USE_SDL=2  -v  -s INITIAL_MEMORY=50mb -s ASSERTIONS -s STACK_SIZE=20mb -s NO_DISABLE_EXCEPTION_CATCHING -s -O3 -s ALLOW_MEMORY_GROWTH -s WASM_BIGINT -s EXPORTED_RUNTIME_METHODS=ccall -s ASYNCIFY -s EXPORTED_FUNCTIONS=_main,_load_game,_mount_FS -s FORCE_FILESYSTEM=1 -lidbfs.js")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
set(CMAKE_EXECUTABLE_SUFFIX .html)
#[[else()
    find_package(SDL2 REQUIRED)
endif()]]
#[[include_directories(${SDL2_INCLUDE_DIRS})]]
#FetchContent_Declare( googletest URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)

#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)
#enable_testing()

#file(COPY pacman.nes DESTINATION ${CMAKE_BINARY_DIR})
add_executable(NES_Emulator
        CPU.cpp
        CPU.h
        Opcode.cpp
        Opcode.h
        addressing_mode.h
        Bus.cpp
        Bus.h
        Mirroring.h
        ROM.cpp
        ROM.h
        main.cpp
        Input.cpp
        Input.h
        PPU.cpp
        PPU.h
        Mapper.cpp
        Mapper.h
        Mappers/mapper0.cpp
        Mappers/mapper0.h
        Interrupt_type.h
        nes_apu/blargg_common.h
        nes_apu/blargg_source.h
        nes_apu/apu_snapshot.cpp
        nes_apu/apu_snapshot.h
        nes_apu/Blip_Buffer.cpp
        nes_apu/Blip_Buffer.h
        nes_apu/Blip_Synth.h
        nes_apu/Multi_Buffer.cpp
        nes_apu/Multi_Buffer.h
        nes_apu/Nes_Apu.cpp
        nes_apu/Nes_Apu.h
        nes_apu/Nes_Namco.h
        nes_apu/Nes_Namco.cpp
        nes_apu/Nes_Oscs.h
        nes_apu/Nes_Oscs.cpp
        Simple_Apu.cpp
        Simple_Apu.h
        Sound_Queue.cpp
        Sound_Queue.h
        nes_apu/boost/config.hpp
        nes_apu/boost/cstdint.hpp
        nes_apu/boost/static_assert.hpp
        )
target_link_libraries(NES_Emulator ${SDL2_LIBRARIES})






#target_link_libraries(NES_Emulator gtest_main gtest)
#include(GoogleTest)
#gtest_discover_tests(NES_Emulator)
