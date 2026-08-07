# 1. Set the minimum required CMake version
cmake_minimum_required(VERSION 3.15)

# 2. Define the project name and set the language to C
project(GpsTrackerProject C)

# 3. Set the C standard (C11 is standard, change to C99 if needed)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 4. Define the executable name and list ALL source files (.c files)
# Note: Do not list the .h file here; CMake finds headers automatically if they are in the same folder.
add_executable(gps_tracker main.c gps.c)

# 5. Link the standard math library (m) to your executable target
target_link_libraries(gps_tracker PRIVATE m)


###### COMPILATION AND EXECUTION INSTRUCTIONS #####

    # 1. Create and enter a build directory
#   mkdir build
#   cd build

    # 2. Generate the build files
#   cmake ..

    # 3. Compile the program
#   cmake --build .

    # 4. Run your executable
#   ./gps_tracker
##############################################################