Project (keylock-tray)
cmake_minimum_required (VERSION 2.6)

Set (PACKAGE "keylock-tray")
Set (VERSION "1.0")


# Default definitions
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wunused -Wshadow -Woverloaded-virtual -Wunreachable-code")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno -fno-exceptions")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
    if (NOT CMAKE_CXX_COMPILER MATCHES "clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
    endif (NOT CMAKE_CXX_COMPILER MATCHES "clang")
    set(CMAKE_LD_FLAGS "-fwhole-program")
endif (NOT MSVC)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -pg")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pg")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "-pg")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "-pg")
endif()

# directories
set(CMAKE_INSTALL_PREFIX  "/usr")
set(BINDIR  "bin"             CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game executable will be installed.")
set(PIXMAPDIR "share/pixmaps" CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game data files will be installed.")

If(NOT IS_ABSOLUTE "${BINDIR}")
    set(PROGRAM_EXECUTABLE_PATH ${CMAKE_INSTALL_PREFIX}/${BINDIR}/keylock-tray)
Else(NOT IS_ABSOLUTE "${BINDIR}")
    set(PROGRAM_EXECUTABLE_PATH ${BINDIR}/keylock-tray)
EndIf(NOT IS_ABSOLUTE "${BINDIR}")

If(NOT IS_ABSOLUTE "${PIXMAPDIR}")
    add_definitions(-DPIXMAP_DIR="${CMAKE_INSTALL_PREFIX}/${PIXMAPDIR}")
Else(NOT IS_ABSOLUTE "${PIXMAPDIR}")
    add_definitions(-DPIXMAP_DIR="${PIXMAPDIR}")
EndIf(NOT IS_ABSOLUTE "${PIXMAPDIR}")



# Detect missing dependencies


# libraries
Find_Package (GTK2 REQUIRED)
If (NOT GTK2_FOUND)
    Message (FATAL_ERROR "Couldn't find GTK development files.")
Else (NOT GTK2_FOUND)
    Include_Directories (${GTK2_INCLUDE_DIRS})
EndIf (NOT GTK2_FOUND)

Find_Package (X11 REQUIRED)
If (NOT X11_FOUND)
    Message (FATAL_ERROR "Couldn't find X11 development files.")
Else (NOT X11_FOUND)
    Include_Directories (${X11_INCLUDE_DIRS})
EndIf (NOT X11_FOUND)

# Sources

Set (PROGRAM_SOURCES
    ./main.c
    )

Add_Executable (keylock-tray ${PROGRAM_SOURCES})

Target_Link_Libraries (keylock-tray ${CMAKE_LD_FLAGS} ${GTK2_LIBRARIES} ${X11_LIBRARIES})


# installing to the proper places
install(PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/keylock-tray
    DESTINATION ${BINDIR})
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/keylock-tray-caps.svg"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/keylock-tray-num.svg"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)

