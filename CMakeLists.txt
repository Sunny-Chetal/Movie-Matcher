cmake_minimum_required(VERSION 3.28)
project(Movie-Matcher)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# fetch Asio, which Crow depends on
FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-38-0
)
FetchContent_MakeAvailable(asio)

# tell Crow where Asio lives
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/include)

# fetch Crow
FetchContent_Declare(
  Crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG v1.3.1
)
FetchContent_MakeAvailable(Crow)

add_executable(Movie-Matcher
  src/main.cpp
  src/fetch_all_movies.cpp
  src/Movie.cpp
  src/merge_sort.cpp
  src/heap_sort.cpp
        src/main.cpp
)

# link Crow and Asio to our executable
target_include_directories(Movie-Matcher PRIVATE ${ASIO_INCLUDE_DIR})

if(WIN32)
  # if on Windows also link ws2_32 and mswsock for sockets support
  target_link_libraries(Movie-Matcher Crow::Crow ws2_32 mswsock)
else()
  target_link_libraries(Movie-Matcher Crow::Crow)
endif()
