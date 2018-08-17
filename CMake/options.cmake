option(ENABLE_SENSEI "Enable Sensei infrastucture" ON)

cmake_dependent_option(ENABLE_PYTHON
  "Enable Python binding to Sensei infrastucture" OFF
  "ENABLE_SENSEI" OFF)

cmake_dependent_option(ENABLE_VTK_GENERIC_ARRAYS
  "VTK build has Generic arrays" OFF
  "ENABLE_SENSEI" OFF)

cmake_dependent_option(ENABLE_CATALYST
  "Enable analysis methods that use Catalyst" OFF
  "ENABLE_SENSEI" OFF)

cmake_dependent_option(ENABLE_CATALYST_PYTHON
  "Enable analysis methods that use Catalyst Python scripts" OFF
  "ENABLE_CATALYST" OFF)

cmake_dependent_option(ENABLE_ADIOS
  "Enable analysis methods that use ADIOS" OFF
  "ENABLE_SENSEI" OFF)

cmake_dependent_option(ENABLE_CONDUIT
  "Enable analysis methods that use Conduit" OFF
  "ENABLE_SENSEI" OFF)

cmake_dependent_option(ENABLE_LIBSIM
  "Enable analysis methods that use Libsim" OFF
  "ENABLE_SENSEI" OFF)

cmake_dependent_option(ENABLE_VTK_IO
  "Enable use of vtk I/O" OFF
  "ENABLE_SENSEI" OFF)

cmake_dependent_option(ENABLE_VTK_MPI
  "Enable use of parallel vtk" OFF
  "ENABLE_SENSEI" OFF)

cmake_dependent_option(ENABLE_VTK_RENDERING
  "Enable use of VTK's rendering libraries" ON
  "ENABLE_SENSEI" OFF)

cmake_dependent_option(ENABLE_VTK_ACCELERATORS
  "Enable analysis methods that use VTK-m via VTK's Accelerators module" OFF
  "ENABLE_SENSEI" OFF)

# Build with vtk-m support
# If ENABLE_VTKM is ON, then linking your library/executable to vtkm_cont
# will add include-directories and preprocessor definitions required by vtk-m.
# The sensei library will transitively link to vtkm_cont.
cmake_dependent_option(ENABLE_VTKM
  "Enable analysis methods that use VTK-m" OFF
  "ENABLE_SENSEI" OFF)

# If ENABLE_VTKM_RENDERING is ON, then linking your library/executable to
# vtkm_rendering will add include-directories and preprocessor definitions
# required by vtk-m.
# The sensei library will transitively link to vtkm_rendering.
cmake_dependent_option(ENABLE_VTKM_RENDERING
  "Enable analysis methods that use VTK-m's rendering library" OFF
  "ENABLE_VTKM" OFF)


option(ENABLE_PARALLEL3D "Enable Parallel3D miniapp" ON)
option(ENABLE_OSCILLATORS "Enable Oscillators miniapp" ON)
option(ENABLE_CONDUITTEST "Enable Conduit miniapp" OFF)

message(STATUS "ENABLE_SENSEI=${ENABLE_SENSEI}")
message(STATUS "ENABLE_PYTHON=${ENABLE_PYTHON}")
message(STATUS "ENABLE_VTK_GENERIC_ARRAYS=${ENABLE_VTK_GENERIC_ARRAYS}")
message(STATUS "ENABLE_CATALYST=${ENABLE_CATALYST}")
message(STATUS "ENABLE_CATALYST_PYTHON=${ENABLE_CATALYST}")
message(STATUS "ENABLE_ADIOS=${ENABLE_ADIOS}")
message(STATUS "ENABLE_CONDUIT=${ENABLE_CONDUIT}")
message(STATUS "ENABLE_LIBSIM=${ENABLE_LIBSIM}")
message(STATUS "ENABLE_VTK_IO=${ENABLE_VTK_IO}")
message(STATUS "ENABLE_VTK_MPI=${ENABLE_VTK_MPI}")
message(STATUS "ENABLE_VTK_RENDERING=${ENABLE_VTK_RENDERING}")
message(STATUS "ENABLE_VTK_ACCELERATORS=${ENABLE_VTK_ACCELERATORS}")
message(STATUS "ENABLE_VTKM=${ENABLE_VTKM}")
message(STATUS "ENABLE_VTKM_RENDERING=${ENABLE_VTKM_RENDERING}")
message(STATUS "ENABLE_PARALLEL3D=${ENABLE_PARALLEL3D}")
message(STATUS "ENABLE_OSCILLATORS=${ENABLE_OSCILLATORS}")
message(STATUS "ENABLE_CONDUITTEST=${ENABLE_CONDUITTEST}")
