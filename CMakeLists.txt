cmake_minimum_required(VERSION 3.10)
project(tar2h5)

set(CMAKE_C_STANDARD 99)


find_package(MPI REQUIRED)
if (MPI_FOUND)
    message("-- MPI FOUND")
    message("-- MPI_INCLUDE_PATH = " ${MPI_INCLUDE_PATH})
    message("-- MPI_LIBRARIES    = " ${MPI_LIBRARIES})
endif (MPI_FOUND)

find_package(LibArchive REQUIRED)
if (LibArchive_FOUND)
    message("-- LibArchive FOUND Version = " ${LibArchive_VERSION})
    message("-- LibArchive_INCLUDE_DIRS  = " ${LibArchive_INCLUDE_DIRS})
    message("-- LibArchive_LIBRARIES     = " ${LibArchive_LIBRARIES})
endif (LibArchive_FOUND)

find_package(HDF5 REQUIRED)
if (HDF5_FOUND)
    message("-- HDF5 FOUND Version = " ${HDF5_VERSION})
    message("-- HDF5_INCLUDE_DIRS  = " ${HDF5_INCLUDE_DIRS})
    message("-- HDF5_LIBRARIES     = " ${HDF5_LIBRARIES})
endif (HDF5_FOUND)

include_directories(${MPI_INCLUDE_PATH} ${LibArchive_INCLUDE_PATH} ${HDF5_INCLUDE_DIRS})
link_directories(${MPI_LIBRARIES} ${LibArchive_LIBRARY} ${HDF5_LIBRARIES})

add_executable(h5compactor h5compactor.c)
target_link_libraries(h5compactor ${MPI_LIBRARIES} ${LibArchive_LIBRARIES} ${HDF5_LIBRARIES})

add_executable(h5shredder h5shredder.c)
target_link_libraries(h5shredder ${MPI_LIBRARIES} ${LibArchive_LIBRARIES} ${HDF5_LIBRARIES})
