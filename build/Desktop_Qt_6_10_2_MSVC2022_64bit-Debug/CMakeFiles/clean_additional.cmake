# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Raytracer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Raytracer_autogen.dir\\ParseCache.txt"
  "Raytracer_autogen"
  )
endif()
