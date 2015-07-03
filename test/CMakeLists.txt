FILE (GLOB_RECURSE test_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
SET (test_LIBS gloc)
SET (test_BIN ${PROJECT_NAME}-unittests)
add_definitions (-DBOOST_TEST_DYN_LINK)

find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS} "../include")
#INCLUDE_DIRECTORIES(
  #"${gmock_SOURCE_DIR}/include"
  #                  "${gmock_SOURCE_DIR}"
 #                   "${gtest_SOURCE_DIR}/include"
 #                   # This directory is needed to build directly from Google
                    # Test sources.
                    #                   "${gtest_SOURCE_DIR}")
 #


SET (test_SRCS
  ${test_SRCS}
  # Any extra source files
  )

add_definitions(-DTEST_PATH="${PROJECT_SOURCE_DIR}/data/") 

SET (link_libs ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${test_LIBS})
#ADD_EXECUTABLE(${test_BIN} ${test_SRCS})
#TARGET_LINK_LIBRARIES(${test_BIN} ${test_LIBS} gtest gtest_main gmock gmock_main)

#ADD_CUSTOM_TARGET(check ALL "${EXECUTABLE_OUTPUT_PATH}/${test_BIN}" DEPENDS ${test_BIN} COMMENT "Executing unit tests..." VERBATIM SOURCES ${test_SRCS})
#ADD_CUSTOM_TARGET(testrun "${EXECUTABLE_OUTPUT_PATH}/${test_BIN}" DEPENDS ${test_BIN} COMMENT "Executing unit tests..." VERBATIM SOURCES ${test_SRCS})


file(GLOB TEST_SRCS RELATIVE ${MAINFOLDER}/test/ *.cc)

foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)
        message("test file ${testSrc}")
        #Add compile target
        add_executable(${testName} ${testSrc})

        TARGET_LINK_LIBRARIES(${testName} ${link_libs})
        #link to Boost libraries AND your targets and dependencies

        #I like to move testing binaries into a testBin directory
        set_target_properties(${testName} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY  ${BUILD_PATH}/test)
        add_test(NAME ${testName}
                WORKING_DIRECTORY ${BUILD_PATH}/test
                COMMAND ${BUILD_PATH}/test/${testName} )
endforeach(testSrc)
 
