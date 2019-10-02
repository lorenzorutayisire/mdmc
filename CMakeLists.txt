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
	
	# Main
	"src/Main.cpp"
	
	"src/VoxelStage.hpp"

	"src/Voxelizer.hpp"
	"src/Voxelizer.cpp"
	
	"src/Phase.hpp"
	"src/PhaseManager.hpp"
	
	# Routine
	"src/Routine/LoadingPhase.hpp"
	"src/Routine/VisualizePhase.hpp"

	# GL
	"src/GL/Mesh.hpp"
	"src/GL/Mesh.cpp"

	"src/GL/Scene.hpp"
	"src/GL/Scene.cpp"
	
	"src/GL/Viewer.hpp"
	"src/GL/Viewer.cpp"

	"src/GL/Camera.hpp"
	"src/GL/Camera.cpp"

	"src/GL/Shader.hpp"
	"src/GL/Shader.cpp"

	"src/GL/Program.hpp"
	"src/GL/Program.cpp"

	#"src/Phases/LoadScenePhase.hpp"
	#"src/Phases/LoadScenePhase.cpp"
	#"src/Phases/VisualizeScenePhase.hpp"
	#"src/Phases/VisualizeScenePhase.cpp"
	#"src/Phases/VoxelizeScenePhase.cpp"
)
 target_link_libraries(mdmc ${CONAN_LIBS})
