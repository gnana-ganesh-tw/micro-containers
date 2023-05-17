#!/bin/bash

ip_addr=$1
echo "Uploading latest wasm binary to Rasperberry PI..."
scp ./public/turn_indicator.wasm pi@${ip_addr}:/home/pi/wasm
echo "........"
echo "Upload Done!"
echo "New Wasm file loaded in the environment"
