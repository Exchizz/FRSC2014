# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "mavlink: 1 messages, 0 services")

set(MSG_I_FLAGS "-Imavlink:/home/exchizz/ROS/Projects/UAV/mavlink/src/msg;-Istd_msgs:/opt/ros/indigo/share/std_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(genlisp REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(mavlink_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/exchizz/ROS/Projects/UAV/mavlink/src/msg/Mavlink.msg" NAME_WE)
add_custom_target(_mavlink_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "mavlink" "/home/exchizz/ROS/Projects/UAV/mavlink/src/msg/Mavlink.msg" "std_msgs/Header"
)

#
#  langs = gencpp;genlisp;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(mavlink
  "/home/exchizz/ROS/Projects/UAV/mavlink/src/msg/Mavlink.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/indigo/share/std_msgs/cmake/../msg/Header.msg"
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/mavlink
)

### Generating Services

### Generating Module File
_generate_module_cpp(mavlink
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/mavlink
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(mavlink_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(mavlink_generate_messages mavlink_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/exchizz/ROS/Projects/UAV/mavlink/src/msg/Mavlink.msg" NAME_WE)
add_dependencies(mavlink_generate_messages_cpp _mavlink_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(mavlink_gencpp)
add_dependencies(mavlink_gencpp mavlink_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS mavlink_generate_messages_cpp)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(mavlink
  "/home/exchizz/ROS/Projects/UAV/mavlink/src/msg/Mavlink.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/indigo/share/std_msgs/cmake/../msg/Header.msg"
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/mavlink
)

### Generating Services

### Generating Module File
_generate_module_lisp(mavlink
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/mavlink
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(mavlink_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(mavlink_generate_messages mavlink_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/exchizz/ROS/Projects/UAV/mavlink/src/msg/Mavlink.msg" NAME_WE)
add_dependencies(mavlink_generate_messages_lisp _mavlink_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(mavlink_genlisp)
add_dependencies(mavlink_genlisp mavlink_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS mavlink_generate_messages_lisp)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(mavlink
  "/home/exchizz/ROS/Projects/UAV/mavlink/src/msg/Mavlink.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/indigo/share/std_msgs/cmake/../msg/Header.msg"
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/mavlink
)

### Generating Services

### Generating Module File
_generate_module_py(mavlink
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/mavlink
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(mavlink_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(mavlink_generate_messages mavlink_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/exchizz/ROS/Projects/UAV/mavlink/src/msg/Mavlink.msg" NAME_WE)
add_dependencies(mavlink_generate_messages_py _mavlink_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(mavlink_genpy)
add_dependencies(mavlink_genpy mavlink_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS mavlink_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/mavlink)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/mavlink
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
add_dependencies(mavlink_generate_messages_cpp std_msgs_generate_messages_cpp)

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/mavlink)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/mavlink
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
add_dependencies(mavlink_generate_messages_lisp std_msgs_generate_messages_lisp)

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/mavlink)
  install(CODE "execute_process(COMMAND \"/usr/bin/python\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/mavlink\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/mavlink
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
add_dependencies(mavlink_generate_messages_py std_msgs_generate_messages_py)
