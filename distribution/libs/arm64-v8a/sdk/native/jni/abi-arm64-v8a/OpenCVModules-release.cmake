#----------------------------------------------------------------
# Generated CMake target import file for configuration "RELEASE".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libcpufeatures" for configuration "RELEASE"
set_property(TARGET libcpufeatures APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libcpufeatures PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/libcpufeatures.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libcpufeatures )
list(APPEND _IMPORT_CHECK_FILES_FOR_libcpufeatures "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/libcpufeatures.a" )

# Import target "libjpeg-turbo" for configuration "RELEASE"
set_property(TARGET libjpeg-turbo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libjpeg-turbo PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibjpeg-turbo.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libjpeg-turbo )
list(APPEND _IMPORT_CHECK_FILES_FOR_libjpeg-turbo "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibjpeg-turbo.a" )

# Import target "libtiff" for configuration "RELEASE"
set_property(TARGET libtiff APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libtiff PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibtiff.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libtiff )
list(APPEND _IMPORT_CHECK_FILES_FOR_libtiff "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibtiff.a" )

# Import target "libopenjp2" for configuration "RELEASE"
set_property(TARGET libopenjp2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libopenjp2 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibopenjp2.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libopenjp2 )
list(APPEND _IMPORT_CHECK_FILES_FOR_libopenjp2 "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibopenjp2.a" )

# Import target "libpng" for configuration "RELEASE"
set_property(TARGET libpng APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libpng PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibpng.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libpng )
list(APPEND _IMPORT_CHECK_FILES_FOR_libpng "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibpng.a" )

# Import target "libprotobuf" for configuration "RELEASE"
set_property(TARGET libprotobuf APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libprotobuf PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibprotobuf.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libprotobuf )
list(APPEND _IMPORT_CHECK_FILES_FOR_libprotobuf "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/liblibprotobuf.a" )

# Import target "tegra_hal" for configuration "RELEASE"
set_property(TARGET tegra_hal APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tegra_hal PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/libtegra_hal.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS tegra_hal )
list(APPEND _IMPORT_CHECK_FILES_FOR_tegra_hal "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/libtegra_hal.a" )

# Import target "ade" for configuration "RELEASE"
set_property(TARGET ade APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ade PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/libade.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS ade )
list(APPEND _IMPORT_CHECK_FILES_FOR_ade "${_IMPORT_PREFIX}/sdk/native/3rdparty/libs/arm64-v8a/libade.a" )

# Import target "opencv_core" for configuration "RELEASE"
set_property(TARGET opencv_core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_core.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_core )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_core "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_core.a" )

# Import target "opencv_imgproc" for configuration "RELEASE"
set_property(TARGET opencv_imgproc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgproc PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_imgproc.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_imgproc )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_imgproc "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_imgproc.a" )

# Import target "opencv_dnn" for configuration "RELEASE"
set_property(TARGET opencv_dnn APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_dnn PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_dnn.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_dnn )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_dnn "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_dnn.a" )

# Import target "opencv_features2d" for configuration "RELEASE"
set_property(TARGET opencv_features2d APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_features2d PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_features2d.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_features2d )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_features2d "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_features2d.a" )

# Import target "opencv_imgcodecs" for configuration "RELEASE"
set_property(TARGET opencv_imgcodecs APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgcodecs PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_imgcodecs.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_imgcodecs )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_imgcodecs "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_imgcodecs.a" )

# Import target "opencv_video" for configuration "RELEASE"
set_property(TARGET opencv_video APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_video PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_video.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_video )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_video "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_video.a" )

# Import target "opencv_videoio" for configuration "RELEASE"
set_property(TARGET opencv_videoio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_videoio PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_videoio.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_videoio )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_videoio "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_videoio.a" )

# Import target "opencv_gapi" for configuration "RELEASE"
set_property(TARGET opencv_gapi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_gapi PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_gapi.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_gapi )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_gapi "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_gapi.a" )

# Import target "opencv_highgui" for configuration "RELEASE"
set_property(TARGET opencv_highgui APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_highgui PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_highgui.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_highgui )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_highgui "${_IMPORT_PREFIX}/sdk/native/staticlibs/arm64-v8a/libopencv_highgui.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
