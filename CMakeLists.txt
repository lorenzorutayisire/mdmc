cmake_minimum_required (VERSION 3.8)

project ("mdmc")

# Loads the conan file and includes/links needed libraries.
include("conanbuildinfo.cmake")
conan_basic_setup()

# Puts all resources inside /bin/resources folder.
file(GLOB RESOURCES "resources/*")
file(COPY ${RESOURCES} DESTINATION "${CMAKE_BINARY_DIR}/bin/resources")

add_executable (
	mdmc
	"src/Main.cpp"
	
	"src/Camera.hpp"
	"src/Camera.cpp"

	#"src/voxelizer.hpp"
	#"src/voxelizer.cpp"
	
	"src/Phases/Phase.hpp"
	"src/Phases/PhaseManager.hpp"
	"src/Phases/PhaseManager.cpp"
	"src/Phases/LoadScenePhase.hpp"
	"src/Phases/LoadScenePhase.cpp"
	"src/Phases/VisualizeScenePhase.hpp"
	"src/Phases/VisualizeScenePhase.cpp"
	
	"src/Scene/Shader.hpp"
	"src/Scene/Mesh.hpp"
	"src/Scene/Mesh.cpp"
	"src/Scene/Scene.hpp"
	"src/Scene/Scene.cpp"
)
 target_link_libraries(mdmc ${CONAN_LIBS})
