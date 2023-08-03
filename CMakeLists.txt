cmake_minimum_required(VERSION 3.0)  # Minimum required CMake version

project(banking)  # Name of your project

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
# link_directories()

# Add the source files to your project
set(SOURCES
    src/banking.c
    src/bankingFunctions.c
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

ADD_CUSTOM_TARGET(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the debug mode.")

ADD_CUSTOM_TARGET(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the release mode.")

# Set the executable name and link the source files
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} sqlite3)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(bankingTest test/bankingTest.c)
target_link_libraries(bankingTest cunit)