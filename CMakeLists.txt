cmake_minimum_required (VERSION 3.8)

project ("mdmc")

set (CMAKE_CXX_STANDARD 17)

# Loads the conan file and includes/links needed libraries.
include("conanbuildinfo.cmake")
conan_basic_setup()

include_directories("src")
include_directories("lib")

# Puts all resources inside /bin/resources folder.
file(GLOB RESOURCES "resources/*")
file(COPY ${RESOURCES} DESTINATION "${CMAKE_BINARY_DIR}/bin/resources")

file(GLOB_RECURSE
	SRC
	"src/*.hpp" "src/*.cpp"
	"lib/*.hpp" "lib/*.cpp"	
)

list(JOIN SRC "\n" SRC_DEBUG)
message(${SRC_DEBUG})

add_executable (mdmc ${SRC})

target_link_libraries(mdmc ${CONAN_LIBS})
