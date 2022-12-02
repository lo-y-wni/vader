list( APPEND vader_testinput
  testinput/recipe_AirTemperature_A.yaml
  testinput/vader_AirTemperature_A.yaml
)

list( APPEND vader_testdata
  testdata/AirTemperature_A.nc
)

# Create data directory for test input and symlink all files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testinput)
foreach(FILENAME ${vader_testinput})
    execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink
           ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
           ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME} )
endforeach(FILENAME)
# Create data directory for test data and symlink all files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testdata)
foreach(FILENAME ${vader_testdata})
    execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink
           ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
           ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME} )
endforeach(FILENAME)

# Create test executables
ecbuild_add_executable( TARGET  test_recipe.x
                        SOURCES vader/TestRecipe.cc vader/TestUtils.cc
                        LIBS    vader
                       )
ecbuild_add_executable( TARGET  test_vader.x
                        SOURCES vader/TestVader.cc vader/TestUtils.cc
                        LIBS    vader
                       )

# Test recipes
ecbuild_add_test( TARGET  test_vader_recipe_AirTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_recipe.x
                  ARGS    "testinput/recipe_AirTemperature_A.yaml"
                  LIBS    vader )

# Test vader calling recipes
ecbuild_add_test( TARGET  test_vader_vader_AirTemperature_A
                  COMMAND ${CMAKE_BINARY_DIR}/bin/test_vader.x
                  ARGS    "testinput/vader_AirTemperature_A.yaml"
                  LIBS    vader )
