cmake_minimum_required(VERSION 3.5)
project(pos_ctrl)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
# find_package(tf2_ros REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

# Get the user name
if(DEFINED ENV{USER})
    set(USER $ENV{USER})
else()
    message(FATAL_ERROR "Failed to retrieve user name")
endif()

set(PCANBASIC_ROOT "/home/${USER}/Downloads/peak-linux-driver-8.16.0/libpcanbasic/pcanbasic")
message("PCANBASIC_ROOT: ${PCANBASIC_ROOT}")

# Get PCAN version
set(PCAN_ROOT "${PCANBASIC_ROOT}/src/pcan")
message("PCAN_ROOT: ${PCAN_ROOT}")

# Add include directories
include_directories(
  include
  ${PCANBASIC_ROOT}/include
  ${PCAN_ROOT}/driver
)

link_directories( ${PCANBASIC_ROOT}/lib )

add_executable(pcan_write 
  src/ManualWrite.cpp
  src/main_manual_write.cpp 
)
target_link_libraries( pcan_write pcanbasic )

add_executable( pcan_write_timer
  src/TimerWrite.cpp
  src/main_timer_write.cpp 
)
target_link_libraries( pcan_write_timer pcanbasic pthread )

add_executable( pcan_read_write
  src/ManualReadWrite.cpp
  src/main.cpp 
)
target_link_libraries( pcan_read_write pcanbasic pthread )