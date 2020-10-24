EXTRA_INCLUDEPATH += $$[QT_INSTALL_HEADERS]
EXTRA_LIBDIR += $$[QT_INSTALL_LIBS] $$[QT_INSTALL_LIBS]/manual-link
!host_build|!cross_compile {
    QMAKE_LIBS_PRIVATE+=$$[QT_INSTALL_LIBS]/bz2.lib
    QMAKE_LIBS_PRIVATE+=$$[QT_INSTALL_LIBS]/libpng16.lib
}
QT_CPU_FEATURES.x86_64 = sse sse2
QT.global_private.enabled_features = sse2 alloca_malloc_h alloca avx2 dbus gui network relocatable sql system-zlib testlib widgets xml zstd
QT.global_private.disabled_features = alloca_h android-style-assets private_tests dbus-linked dlopen gc_binaries intelcet libudev posix_fallocate reduce_exports reduce_relocations release_tools stack-protector-strong
QT_COORD_TYPE = double
QMAKE_LIBS_ZLIB = $$[QT_INSTALL_LIBS]/zlib.lib
QMAKE_LIBS_ZSTD = -lzstd
CONFIG += sse2 aesni sse3 ssse3 sse4_1 sse4_2 avx avx2 avx512f avx512bw avx512cd avx512dq avx512er avx512ifma avx512pf avx512vbmi avx512vl compile_examples f16c largefile precompile_header rdrnd rdseed shani x86SimdAlways
QT_BUILD_PARTS += libs tools
QT_HOST_CFLAGS_DBUS += 
