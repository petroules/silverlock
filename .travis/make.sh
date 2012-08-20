#!/bin/sh

# Build all variants
cd ..
make -C build-shared package
make -C build-static package
