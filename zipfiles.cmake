 file(COPY ${EIGEN3_INCLUDE_DIRS} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
 file(COPY ${DOC_DIR}/MSSAMex.m DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

 file(ARCHIVE_CREATE OUTPUT ${CMAKE_RELEASE_LOCATION}/MSSAMex.zip
    PATHS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    FORMAT zip
)

#message("${EIGEN3_INCLUDE_DIRS}")

