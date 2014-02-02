CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (Trillek)

#
# src/ includes all the necessary cpp files for this module, so what we do is
# to use CMake's GLOB instruction to find all the .cpp files and load them into
# the $CODE variable.
file ( GLOB_RECURSE CODE
 ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

# To use the module in our app we create a static library to be linked against.
# This library includes all the source files in our src/ directory, so we use
# the $CODE variable as the list of source files to include.
add_library ( core STATIC
	${CODE}
	)

#------------------------------------------------------------------------------
# Main executable !
add_executable ( Trillek ${CMAKE_CURRENT_SOURCE_DIR}/main/main.cpp )

#
# We need to link the executable to the modules that are used.
target_link_libraries ( Trillek 
	core 
	libSigmas 
	TR3200 
	)


#------------------------------------------------------------------------------
# Test stuff

#
# tests/ includes all the unit-tests for the module.
# Again, we use CMake's GLOB instruction to get all of them
# into the $TEST_CODE variable.
#file ( GLOB TESTS_CODE
# ${CMAKE_CURRENT_SOURCE_DIR}/tests/*
#)

#
# In order to run the tests, we need to create an executable file (our test runner)
# You'll understand more about this on the next article, that will focus on testing, for now
# it's enough that you understand that we include all the tests from $TEST_CODE into a single executable.
#add_executable ( TrillekCoreTests ${TESTS_CODE} )

#
# Then we need to link the test executable to the modules that are used.
# In this case, we're linking agains simple-test (the library needed to run the tests) and simplex-core,
# the module we're testing.
# This is usually the case, you link only to the test library and the module you're testing, but there may be
# exceptions where mocking is not an option.
#target_link_libraries ( TrillekCoreTests 
#	core 
#	libSigmas 
#	TR3200 
#	${TEST_LIBRARIES} 
#	)

#
# Finally, in order to be able to run "make tests" and run all the tests for the different modules,
# we have to add the executable file we just created to CMake's test list.
# We do that using add_tests
#add_test ( TrillekCoreTests TrillekCoreTests )


