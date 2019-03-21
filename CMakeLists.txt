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
target_link_libraries(${PROJECT_NAME} zeda zm zeo)

add_executable(cad2zkc app/cad2zkc.c)
target_link_libraries(cad2zkc ${PROJECT_NAME})

add_executable(zkcconv app/zkcconv.c)
target_link_libraries(zkcconv ${PROJECT_NAME})

add_executable(z3d2zkc app/z3d2zkc.c)
target_link_libraries(z3d2zkc ${PROJECT_NAME})

add_executable(zkc2z3d app/zkc2z3d.c)
target_link_libraries(zkc2z3d ${PROJECT_NAME})

add_executable(z3d2gp app/z3d2gp.c)
target_link_libraries(z3d2gp ${PROJECT_NAME})

add_executable(zvs2zkci app/zvs2zkci.c)
target_link_libraries(zvs2zkci ${PROJECT_NAME})

add_executable(zvs2zkcs app/zvs2zkcs.c)
target_link_libraries(zvs2zkcs ${PROJECT_NAME})

add_executable(rk_ground app/rk_ground.c)
target_link_libraries(rk_ground ${PROJECT_NAME})

add_executable(rk_mp app/rk_mp.c)
target_link_libraries(rk_mp ${PROJECT_NAME})

add_executable(rk_fk app/rk_fk.c)
target_link_libraries(rk_fk ${PROJECT_NAME})

add_executable(rk_ik app/rk_ik.c)
target_link_libraries(rk_ik ${PROJECT_NAME})

add_executable(rk_id app/rk_id.c)
target_link_libraries(rk_id ${PROJECT_NAME})

add_executable(rk_ikcell_merge app/rk_ikcell_merge.c)
target_link_libraries(rk_ikcell_merge ${PROJECT_NAME})

# Install
INSTALL (
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*")

install (TARGETS 
    cad2zkc
    zkcconv
    z3d2zkc
    zkc2z3d
    z3d2gp
    zvs2zkci
    zvs2zkcs
    rk_ground 
    rk_mp 
    rk_fk 
    rk_ik 
    rk_id 
    rk_ikcell_merge
    RUNTIME DESTINATION bin)

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