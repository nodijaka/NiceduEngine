#cmake_minimum_required(VERSION 3.5)
#project(MyProgram)
#
#add_executable(MyProgram main.cpp)


#cmake_minimum_required(VERSION 3.5)
#project(MyProgram)

# Create an executable target named FirstExecutable
#add_executable(FirstExecutable main.cpp)

# Set the output directory for the FirstExecutable target
#set_target_properties(FirstExecutable PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/FirstExecutableFolder"
#)

# Create another executable target named SecondExecutable
#add_executable(SecondExecutable main.cpp)

# Set the output directory for the SecondExecutable target
#set_target_properties(SecondExecutable PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/SecondExecutableFolder"
#)


cmake_minimum_required(VERSION 3.11)
project(MyProgram)

include(FetchContent)

# Use FetchContent to download and include SFML
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG master # 2.5.1 # Use a stable release tag instead of 'master'
)
FetchContent_MakeAvailable(SFML)

# Create an executable target named FirstExecutable
add_executable(FirstExecutable main.cpp)

# Set the output directory for the FirstExecutable target
set_target_properties(FirstExecutable PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/FirstExecutableFolder"
)

# Link FirstExecutable against SFML
target_link_libraries(FirstExecutable PRIVATE sfml-graphics sfml-window sfml-system)

# Create another executable target named SecondExecutable
add_executable(SecondExecutable main.cpp)

# Set the output directory for the SecondExecutable target
set_target_properties(SecondExecutable PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/SecondExecutableFolder"
)

# Link SecondExecutable against SFML
target_link_libraries(SecondExecutable PRIVATE sfml-graphics sfml-window sfml-system)
