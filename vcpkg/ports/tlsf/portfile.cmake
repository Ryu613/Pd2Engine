vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO mattconte/tlsf
    REF deff9ab509341f264addbd3c8ada533678591905
    SHA512 0
    HEAD_REF master
)

vcpkg_cmake_install()