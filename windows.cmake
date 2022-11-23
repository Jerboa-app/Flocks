set(CMAKE_SYSTEM_NAME Windows)
set(SFML_OS_WINDOWS TRUE)
set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

# cross compilers to use for C, C++ and Fortran
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_Fortran_COMPILER ${TOOLCHAIN_PREFIX}-gfortran)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# target environment on the build host system
set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
set(FREETYPE_LIBRARY ${PROJECT_SOURCE_DIR}/extlibs/libs-mingw/x86/libfreetype.a)
set(OPENAL_LIBRARY ${PROJECT_SOURCE_DIR}/extlibs/libs-mingw/x86/libopenal32.a)
set(VORBISFILE_LIBRARY ${PROJECT_SOURCE_DIR}/extlibs/libs-mingw/x86/libvorbisfile.a)
set(VORBISENC_LIBRARY ${PROJECT_SOURCE_DIR}/extlibs/libs-mingw/x86/libvorbisenc.a)
set(VORBIS_LIBRARY ${PROJECT_SOURCE_DIR}/extlibs/libs-mingw/x86/libvorbis.a)
set(OGG_LIBRARY ${PROJECT_SOURCE_DIR}/extlibs/libs-mingw/x86/libogg.a)
set(FLAC_LIBRARY ${PROJECT_SOURCE_DIR}/extlibs/libs-mingw/x86/libFLAC.a)

set(FREETYPE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/headers/freetype2)
set(OPENAL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/headers/AL)
set(VORBIS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/headers/vorbis)
set(OGG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/headers/ogg)
set(FLAC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/headers/FLAC)
