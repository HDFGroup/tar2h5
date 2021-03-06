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

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    message("-- OPENSSL FOUND Version = " ${OPENSSL_VERSION})
    message("-- OPENSSL_INCLUDE_DIRS  = " ${OPENSSL_INCLUDE_DIRS})
    message("-- OPENSSL_LIBRARIES     = " ${OPENSSL_LIBRARIES})
endif (OPENSSL_FOUND)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(${MPI_INCLUDE_PATH} ${LibArchive_INCLUDE_PATH} ${HDF5_INCLUDE_DIRS})
link_directories(${MPI_LIBRARIES} ${LibArchive_LIBRARY} ${HDF5_LIBRARIES})

add_executable(archive_checker src/archive_checker.c)
target_link_libraries(archive_checker ${LibArchive_LIBRARIES})

add_executable(archive_checker_64k src/archive_checker_64k.c)
target_link_libraries(archive_checker_64k ${LibArchive_LIBRARIES})

add_executable(h5compactor src/h5compactor.c)
target_link_libraries(h5compactor ${MPI_LIBRARIES} ${LibArchive_LIBRARIES} ${HDF5_LIBRARIES})

add_executable(h5compactor-sha1 src/h5compactor-sha1.c)
target_link_libraries(h5compactor-sha1 ${MPI_LIBRARIES} ${LibArchive_LIBRARIES} ${HDF5_LIBRARIES} ${OPENSSL_LIBRARIES})

add_executable(h5shredder src/h5shredder.c)
target_link_libraries(h5shredder ${MPI_LIBRARIES} ${LibArchive_LIBRARIES} ${HDF5_LIBRARIES})

enable_testing()

add_test(run_archive_checker_demo ${PROJECT_SOURCE_DIR}/bin/archive_checker ${PROJECT_SOURCE_DIR}/demo/tar2h5-demo.tar)
add_test(run_archive_checker_64k_demo ${PROJECT_SOURCE_DIR}/bin/archive_checker_64k ${PROJECT_SOURCE_DIR}/demo/tar2h5-demo.tar)
add_test(run_h5compactor_demo ${PROJECT_SOURCE_DIR}/bin/h5compactor ${PROJECT_SOURCE_DIR}/demo/tar2h5-demo.tar)
add_test(run_h5compactor-sha1_demo ${PROJECT_SOURCE_DIR}/bin/h5compactor-sha1 ${PROJECT_SOURCE_DIR}/demo/tar2h5-demo.tar)
add_test(run_h5shredder_demo ${PROJECT_SOURCE_DIR}/bin/h5shredder ${PROJECT_SOURCE_DIR}/demo/tar2h5-demo.tar)