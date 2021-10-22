if (NOT TARGET ttg)
  find_package(ttg CONFIG)
endif(NOT TARGET ttg)

if (TARGET ttg)
    message(STATUS "Found ttg CONFIG at ${ttg_CONFIG}")
else (TARGET ttg)

  include(FetchContent)
  FetchContent_Declare(
      ttg
      GIT_REPOSITORY      https://github.com/TESSEorg/ttg.git
      GIT_TAG             9f20ec02af70cd414d7454596b06270f734eb134
  )
  FetchContent_MakeAvailable(ttg)
  FetchContent_GetProperties(ttg
      SOURCE_DIR TTG_SOURCE_DIR
      BINARY_DIR TTG_BINARY_DIR
      )

endif(TARGET ttg)

# postcond check
if (NOT TARGET ttg)
message(FATAL_ERROR "FindOrFetchTTG could not make ttg target available")
endif(NOT TARGET ttg)

