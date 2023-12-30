# CMakeList.txt: 3D_Room_VS 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("3D_Room_VS")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
include_directories("include")
link_libraries(
	"${PROJECT_SOURCE_DIR}/lib/glfw3.lib"
	"${PROJECT_SOURCE_DIR}/lib/assimp-vc143-mtd.lib"
)

# 将源代码添加到此项目的可执行文件。
add_executable (3D_Room_VS 
		"src/glad.c" 
		"src/main.cpp" 
		"src/Shader.cpp" 
		"src/Shader.h" 
		"src/Texture.cpp" 
		"src/Texture.h"
		"src/Mesh.h"
		"src/Mesh.cpp"
		"src/Model.h"
		"src/Model.cpp" )
