
FILE (GLOB_RECURSE project_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)

# Add libraries here. Example: ${Boost_LIBRARIES}
SET (project_LIBS "")


ADD_LIBRARY(${PROJECT_NAME} SHARED ${project_SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${project_LIBS})

INSTALL(TARGETS ${project_name} DESTINATION lib)
