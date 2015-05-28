project(apps)
ADD_SUBDIRECTORY(gloria)


#
# enable gpu build
#
set(BUILD_GPU Off CACHE BOOL "Build with GPU support")

if(BUILD_GPU)
	ADD_SUBDIRECTORY(gpu)
endif(BUILD_GPU)

#
# enable opencv based build
#
set(BUILD_OCV Off CACHE BOOL "Build with GPU support")

if(BUILD_OCV)
	ADD_SUBDIRECTORY(ocvapp)
endif(BUILD_OCV)

