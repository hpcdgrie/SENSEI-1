set(SENSEI_VTK_COMPONENTS vtkCommonDataModel)
if (ENABLE_VTK_MPI)
  list(APPEND SENSEI_VTK_COMPONENTS vtkParallelMPI)
endif()
if (ENABLE_VTK_IO)
  list(APPEND SENSEI_VTK_COMPONENTS vtkIOXML vtkIOLegacy)
  if (ENABLE_VTK_MPI)
    list(APPEND SENSEI_VTK_COMPONENTS vtkIOParallelXML)
  endif()
endif()
if (ENABLE_VTK_RENDERING)
  list(APPEND SENSEI_VTK_COMPONENTS vtkRenderingCore)
  if (TARGET vtkRenderingOpenGL2)
    list(APPEND SENSEI_VTK_COMPONENTS vtkRenderingOpenGL2)
  endif()
  if (TARGET vtkRenderingOpenGL)
    list(APPEND SENSEI_VTK_COMPONENTS vtkRenderingOpenGL)
  endif()
endif()

if (ENABLE_PYTHON)
  list(APPEND SENSEI_VTK_COMPONENTS vtkPython vtkWrappingPythonCore)
endif()
if (ENABLE_VTK_ACCELERATORS)
  list(APPEND SENSEI_VTK_COMPONENTS vtkAcceleratorsVTKm vtkIOLegacy
    vtkFiltersGeometry vtkImagingCore)
endif()

if (ENABLE_VTKM)
  list(APPEND SENSEI_VTK_COMPONENTS  vtkFiltersGeneral)
endif()

if (NOT ENABLE_CATALYST)
  add_library(sVTK INTERFACE)

  find_package(VTK QUIET COMPONENTS ${SENSEI_VTK_COMPONENTS})
  if (NOT VTK_FOUND)
    message(FATAL_ERROR "VTK (${SENSEI_VTK_COMPONENTS}) modules are required for "
      "Sensei core even when not using any infrastructures. Please set "
      "VTK_DIR to point to a directory containing `VTKConfig.cmake`.")
  endif()

  target_link_libraries(sVTK INTERFACE ${VTK_LIBRARIES})
  target_include_directories(sVTK SYSTEM INTERFACE ${VTK_INCLUDE_DIRS})
  target_compile_definitions(sVTK INTERFACE ${VTK_DEFINITIONS})

  install(TARGETS sVTK EXPORT sVTK)
  install(EXPORT sVTK DESTINATION lib/cmake EXPORT_LINK_INTERFACE_LIBRARIES)
endif()
