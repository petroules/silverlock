#!/bin/bash
set -e

# Build all variants
cd ..
make -C build-shared docs package package_source
make -C build-static docs package package_source
