
.. note::
   Try the quickstart!

   First create this ``CMakeLists.txt``:
   
   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.13)
      project(my-quickstart LANGUAGES CXX)
      include(FetchContent)
      FetchContent_Declare(ryml
          GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
          GIT_TAG v0.10.0
          GIT_SHALLOW FALSE  # ensure submodules are checked out
      )
      FetchContent_MakeAvailable(ryml)
      add_executable(my-quickstart ${ryml_SOURCE_DIR}/samples/quickstart.cpp)
      target_link_libraries(my-quickstart ryml::ryml)
      add_custom_target(run my-quickstart
          COMMAND $<TARGET_FILE:my-quickstart>
          DEPENDS my-quickstart)

   Now run the following commands in the same folder:
   
   .. code-block:: bash

      # configure the project
      cmake -S . -B build
      # build and run
      cmake --build build --target ryml-quickstart -j
      # optionally, open in your IDE
      cmake --open build
