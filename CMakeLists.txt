cmake_minimum_required(VERSION 3.10)

project(unboxing)

# Exports:
#   libraries: unboxing, testutils
#   executables: static_unboxer, unboxer, testunboxing

add_library(unboxing
    src/base/math_crc32.c
    src/base/math_crc64.c
    src/codecs/2dpam.c
    src/codecs/bchcodec.c
    src/codecs/cipher.c
    src/codecs/codecbase.c
    src/codecs/codecdispatcher.c
    src/codecs/crc32.c
    src/codecs/crc64.c
    src/codecs/ftfinterleaving.c
    src/codecs/interleaving.c
    src/codecs/ldpccodec.c
    src/codecs/modulator.c
    src/codecs/packetheader.c
    src/codecs/reedsolomon.c
    src/codecs/symbolconverter.c
    src/codecs/syncpointinserter.c
    src/config.c
    src/frame/2polinomialsampler.c
    src/frame/areasampler.c
    src/frame/sampler.c
    src/frame/trackergpf_1.c
    src/frame/trackergpf.c
    src/globals.c
    src/graphics/border.c
    src/graphics/calibrationbar.c
    src/graphics/component.c
    src/graphics/contentcontainer.c
    src/graphics/genericframe.c
    src/graphics/genericframefactory.c
    src/graphics/genericframegpf_1.c
    src/graphics/label.c
    src/graphics/metadatabar.c
    src/graphics/painter.c
    src/graphics/referencebar.c
    src/graphics/referencepoint.c
    src/image8.c
    src/log.c
    src/math/bitutils.c
    src/math/dsp.c
    src/math/math.c
    src/matrix.c
    src/metadata.c
    src/string.c
    src/unboxer.c
    src/unboxer_utility.c
    src/unboxer/cornermark.c
    src/unboxer/datapoints.c
    src/unboxer/filter.c
    src/unboxer/frametrackerutil.c
    src/unboxer/frameutil.c
    src/unboxer/histogramutils.c
    src/unboxer/horizontalmeasures.c
    src/unboxer/sampleutil.c
    src/unboxer/syncpoints.c
    src/unboxer/unboxerv1.c
    src/utils.c
    thirdparty/bch/bch.c
    thirdparty/glib/g_variant.c
    thirdparty/glib/ghash.c
    thirdparty/glib/glist.c
    thirdparty/glib/gvector.c
    thirdparty/LDPC-codes/alloc.c
    thirdparty/LDPC-codes/check.c
    thirdparty/LDPC-codes/dec.c
    thirdparty/LDPC-codes/distrib.c
    thirdparty/LDPC-codes/enc.c
    thirdparty/LDPC-codes/intio.c
    thirdparty/LDPC-codes/mod2convert.c
    thirdparty/LDPC-codes/mod2dense.c
    thirdparty/LDPC-codes/mod2sparse.c
    thirdparty/LDPC-codes/open.c
    thirdparty/LDPC-codes/rand.c
    thirdparty/LDPC-codes/rcode.c
    thirdparty/reedsolomon/galois_field.c
    thirdparty/reedsolomon/rs.c
)

target_link_libraries(unboxing m)

set_property(SOURCE src/unboxer.c APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/unboxer)

set_property(SOURCE src/codecs/bchcodec.c APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/bch)
set_property(SOURCE src/codecs/ldpccodec.c APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/LDPC-codes)
set_property(SOURCE src/codecs/reedsolomon.c APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/reedsolomon)

set_property(SOURCE src/codecs/codecbase.c APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/bch)
set_property(SOURCE src/codecs/codecbase.c APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/LDPC-codes)
set_property(SOURCE src/codecs/codecbase.c APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/reedsolomon)

set_property(SOURCE thirdparty/LDPC-codes/rand.c APPEND PROPERTY COMPILE_DEFINITIONS RAND_FILE="randfile")

target_include_directories(unboxing PUBLIC inc thirdparty/glib)


add_library(testutils tests/testutils/src/boxing_config.c)
target_include_directories(testutils PUBLIC tests/testutils/inc)

target_link_libraries(testutils unboxing)

if(PROJECT_IS_TOP_LEVEL)
    set(BUILD_TESTING ON)
endif()

if(BUILD_TESTING)
    enable_testing()
    add_executable(static_unboxer tests/static_unboxer/main.c)
    target_link_libraries(static_unboxer testutils)


    add_executable(unboxer tests/unboxer/main.c)
    target_link_libraries(unboxer testutils)


    add_executable(testunboxing
        tests/unboxingdata/configtests.c
        tests/unboxingdata/crctests.c
        tests/unboxingdata/image8tests.c
        tests/unboxingdata/mathtests.c
        tests/unboxingdata/matrixtests.c
        tests/unboxingdata/metadatatests.c
        tests/unboxingdata/stringtests.c
        tests/unboxingdata/testsmain.c
    )

    target_compile_definitions(testunboxing PRIVATE UNIT_TESTS_ENABLED)
    target_include_directories(testunboxing PRIVATE tests/testutils/inc)
    target_link_libraries(testunboxing unboxing -lcheck -lsubunit)

    add_test(NAME testunboxing COMMAND testunboxing)
    add_test(NAME static_unboxer COMMAND static_unboxer)
    add_test(NAME unboxer_4kv6 COMMAND unboxer -i ../tests/testdata/12288x6878_4kv6.image8 -s 12288 6878 -f 4kv6 -o out-4kv6.dat)
    add_test(NAME unboxer_4kv9 COMMAND unboxer -i ../tests/testdata/12288x6879_4kv9.image8 -s 12288 6879 -f 4kv9 -o out-4kv9.dat)
    add_test(NAME unboxer_4kv6_analog COMMAND unboxer -i ../tests/testdata/12288x6878_4kv6_analog.image8 -s 12288 6878 -f 4kv6 -o out-4kv6_analog.dat)
    add_test(NAME unboxer_4kv6_raw COMMAND unboxer -i ../tests/testdata/4kv6_*d.raw -f 4kv6 -s 4096 2160 -o out_4kv6_raw.dat -is_raw)
endif()
