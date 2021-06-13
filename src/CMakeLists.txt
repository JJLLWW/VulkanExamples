# add every subdirectory that has a CMakeLists.txt, should this be a function?
file(GLOB CHILD_DIRS *)
foreach(CHILD_DIR ${CHILD_DIRS})
    if(IS_DIRECTORY ${CHILD_DIR} AND EXISTS ${CHILD_DIR}/CMakeLists.txt)
        add_subdirectory(${CHILD_DIR})
    endif()
endforeach()
    