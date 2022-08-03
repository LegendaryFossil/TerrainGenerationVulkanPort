set(NAME "VulkanProject")

set(SRC
	"main.cpp"
	"vulkanDebugUtils.cpp"
	"vulkanDebugUtils.h"
	"vulkanDevice.cpp"
	"vulkanDevice.h"
	"vulkanSwapChain.cpp"
	"vulkanSwapChain.h"
	"vulkanUtils.cpp"
	"vulkanUtils.h"
	"windowDefs.h"
)

add_compile_options("/std:c++latest")

set(LIBRARIES
	"glfw"
	"glm::glm"
	"stb_image"
	"${EXTERNAL_LIB_PATH}/vulkan-sdk/1.3.216.0/lib/vulkan-1.lib"
)
    
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC})
source_group("" FILES ${SRC})

add_executable(${NAME} "")
target_sources(${NAME} PRIVATE ${SRC})

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${EXTERNAL_LIB_PATH}/vulkan-sdk/1.3.216.0/include)
target_link_libraries(${NAME} PUBLIC ${LIBRARIES})

install(TARGETS ${NAME} DESTINATION ${VULKAN_PROJECT_EXE_PATH})