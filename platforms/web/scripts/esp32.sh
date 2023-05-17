ip_addr=$1

echo "Stopping task if already running..."

curl -d "filename=turn_indicator.wasm" -X POST http://${ip_addr}/task/stop

echo "\n\nRemoving the old wasm binary..."
curl -d "filename=turn_indicator.wasm" -X POST http://${ip_addr}/task/delete

echo "\n\nUploading latest wasm binary to ESP32..."
curl -F "filename=@./public/turn_indicator.wasm" -X POST http://${ip_addr}/task/upload

echo "\n\nStarting the task from new wasm environment..."
curl -d "filename=turn_indicator.wasm" -X POST http://${ip_addr}/task/start