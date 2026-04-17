file(GLOB_RECURSE SOURCES "*.cpp")

add_library(${PROJECT_NAME}_lib ${SOURCES})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)
