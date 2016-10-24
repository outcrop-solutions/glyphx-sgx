
# Script to do a one-step build of releases for all four apps.

# Make sure we're in the same path as this script since it expects that.
cd "${0%/*}"

# Build the three apps. Note we're only passing -c (clean build directories) for the first one,
# since doing it every time would be redundant and would make the build take forever.
./setup_osx.sh -bcirgj8
./setup_osx.sh -birmj8
./setup_osx.sh -birvj8
