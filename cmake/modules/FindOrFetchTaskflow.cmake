if (NOT TARGET Taskflow::Taskflow)
  # Taskflow registers its build tree with the package registry, which inhibits
  # portability of the build tree to a different machine (i.e. FETCHCONTENT_FULLY_DISCONNECTED=ON)
  find_package(Taskflow CONFIG NO_CMAKE_PACKAGE_REGISTRY)
endif(NOT TARGET Taskflow::Taskflow)

if (TARGET Taskflow::Taskflow)
    message(STATUS "Found Taskflow CONFIG at ${Taskflow_CONFIG}")
else (TARGET Taskflow::Taskflow)

  include(FetchContent)
  FetchContent_Declare(
      Taskflow
      GIT_REPOSITORY      https://github.com/taskflow/taskflow.git
      GIT_TAG             master
  )
  FetchContent_MakeAvailable(Taskflow)
  FetchContent_GetProperties(Taskflow
      SOURCE_DIR Taskflow_SOURCE_DIR
      BINARY_DIR Taskflow_BINARY_DIR
      )

  if (TARGET Taskflow AND NOT TARGET Taskflow::Taskflow)
    add_library(Taskflow::Taskflow ALIAS Taskflow)
  endif (TARGET Taskflow AND NOT TARGET Taskflow::Taskflow)

endif(TARGET Taskflow::Taskflow)

# postcond check
if (NOT TARGET Taskflow::Taskflow)
message(FATAL_ERROR "FindOrFetchTaskflow could not make Taskflow::Taskflow target available")
endif(NOT TARGET Taskflow::Taskflow)

