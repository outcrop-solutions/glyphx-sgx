CONFIG+= debug shared rtti no_plugin_manifest qpa
host_build {
    QT_ARCH = i386
    QT_TARGET_ARCH = i386
} else {
    QT_ARCH = i386
}
QT_EDITION = OpenSource
QT_CONFIG += minimal-config small-config medium-config large-config full-config debug_and_release build_all release debug shared zlib png freetype build_all accessibility opengl audio-backend wmf-backend native-gestures qpa concurrent
#versioning 
QT_VERSION = 5.2.1
QT_MAJOR_VERSION = 5
QT_MINOR_VERSION = 2
QT_PATCH_VERSION = 1
