CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(PROJECT_NAME sfml-oculus)

SET(OCULUS_SDK_DIR /home/gergondet/devel-src/oculus/OculusSDK)
INCLUDE_DIRECTORIES(${OCULUS_SDK_DIR}/LibOVR/Include)
LINK_DIRECTORIES(${OCULUS_SDK_DIR}/LibOVR/Lib/Linux/Release/i386)

SET(${PROJECT_NAME}_SRC
OVRWrapper.cpp
SFMLScreen.cpp
PLYMesh.cpp
main.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ovr sfml-graphics sfml-window sfml-system GL)

SET(SHADERS
${CMAKE_SOURCE_DIR}/fragment_shader.frag
${CMAKE_SOURCE_DIR}/ply_shader.frag
${CMAKE_SOURCE_DIR}/ply_shader.vert
${CMAKE_SOURCE_DIR}/sfmlscreen_shader.frag
${CMAKE_SOURCE_DIR}/sfmlscreen_shader.vert
${CMAKE_SOURCE_DIR}/vertex_shader.vert
)

SET(FONT ${CMAKE_SOURCE_DIR}/arial.ttf)

INSTALL(FILES ${SHADERS} DESTINATION ${CMAKE_BINARY_DIR})
INSTALL(FILES ${FONT} DESTINATION ${CMAKE_BINARY_DIR})
