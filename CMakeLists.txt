cmake_minimum_required(VERSION 2.8.12)
project(roki)

find_package(zeda REQUIRED)
find_package(zm REQUIRED)
find_package(zeo REQUIRED)

add_definitions("-Wall -g -O3")

include_directories(include)

# Build
add_library(${PROJECT_NAME} SHARED
    src/rk_g3d.c
    src/rk_contact.c
    src/rk_motor.c
    src/rk_motor_none.c
    src/rk_motor_trq.c
    src/rk_motor_dc.c
    src/rk_force.c
    src/rk_body.c
    src/rk_joint.c
    src/rk_joint_fixed.c
    src/rk_joint_revol.c 
    src/rk_joint_prism.c 
    src/rk_joint_cylin.c
    src/rk_joint_hooke.c
    src/rk_joint_spher.c 
    src/rk_joint_float.c
    src/rk_joint_brfloat.c
    src/rk_link.c
    src/rk_chain.c
    src/rk_jacobi.c
    src/rk_ik_cell.c
    src/rk_ik.c
    src/rk_ik_seq.c
    src/rk_ik_imp.c
    src/rk_cd.c
    src/rk_abi.c)

# Install
INSTALL (
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*")

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    LIBRARY        DESTINATION lib
    INCLUDES       DESTINATION include
    PUBLIC_HEADER  DESTINATION include/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION share/cmake/${PROJECT_NAME}
    EXPORT_LINK_INTERFACE_LIBRARIES)