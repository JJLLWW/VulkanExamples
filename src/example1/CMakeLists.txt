add_executable(example1 ${EXAMPLE1_SRC})
target_sources(example1 
PRIVATE
    main.cpp
)
target_link_libraries(example1 VKLInterface::VKLInterface)