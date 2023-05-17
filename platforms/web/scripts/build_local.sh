#!/bin/bash

RET=$(git rev-parse --show-toplevel)
echo "Repository directory: ${RET}"
cd $RET/wasm/turn_indicator
echo "Wasm app directory: $(pwd)"
echo "Removing old build..."
rm -rf target
echo "Building wasm app..."
RUSTFLAGS='-C opt-level=z -C linker-plugin-lto -C link-arg=-zstack-size=4096 -C link-arg=--initial-memory=65536 -C link-arg=--max-memory=65536' cargo build --release --target wasm32-wasi
# verify if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Removing old wasm app from web app..."
    rm -rf ../../web/next_app/public/turn_indicator.wasm
    echo "Copying wasm app to web app..."
    cp target/wasm32-wasi/release/turn_indicator.wasm ../../web/next_app/public
    echo "Done!"
else
    echo "Build failed! Please see console output for more information."
fi
