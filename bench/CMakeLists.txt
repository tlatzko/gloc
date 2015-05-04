FILE (GLOB_RECURSE bench_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
SET (bench_LIBS ${Boost_LIBRARIES})
SET (bench_BIN ${PROJECT_NAME}-bench)

INCLUDE_DIRECTORIES("${gmock_SOURCE_DIR}/include"
                    "${gmock_SOURCE_DIR}"
                    "${gtest_SOURCE_DIR}/include"
                    # This directory is needed to build directly from Google
                    # Test sources.
                    "${gtest_SOURCE_DIR}")


ADD_EXECUTABLE(${bench_BIN} ${bench_SRCS})
TARGET_LINK_LIBRARIES(${bench_BIN} ${bench_LIBS} gtest gtest_main gmock gmock_main)


file(GLOB bench_SRCS RELATIVE ${MAINFOLDER}/bench/ *.cc)

foreach(benchSrc ${bench_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(benchName ${benchSrc}_bench NAME_WE)
        message("bench file ${benchSrc}")
        #Add compile target
        add_executable(${benchName} ${benchSrc})

        TARGET_LINK_LIBRARIES(${benchName} ${bench_LIBS} gtest gtest_main gmock gmock_main)
        #link to Boost libraries AND your targets and dependencies


endforeach(benchSrc)
 
