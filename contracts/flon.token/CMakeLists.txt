add_contract(flon.token flon.token ${CMAKE_CURRENT_SOURCE_DIR}/src/flon.token.cpp)

target_include_directories(flon.token
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(flon.token
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/flon.token.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/flon.token.contracts.md @ONLY )

target_compile_options( flon.token PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
