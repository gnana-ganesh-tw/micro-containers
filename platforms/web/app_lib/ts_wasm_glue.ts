export function wasm_add(a: number, b: number) {
    return fetch("basic_ex.wasm").then(response =>
        response.arrayBuffer()
    ).then(bytes =>
        WebAssembly.instantiate(bytes)
    ).then(result => {
        const add = result.instance.exports.add as CallableFunction;
        return add(a, b);
    }
    );
}

export function wasm_sub(a: number, b: number) {
    return fetch("basic_ex.wasm").then(response =>
        response.arrayBuffer()
    ).then(bytes =>
        WebAssembly.instantiate(bytes)
    ).then(result => {
        const sub = result.instance.exports.sub as CallableFunction;
        return sub(a, b);
    }
    );
}

export class TurnIndicatorControllerInterface {
    wasmImportObject: any;

    constructor(indicatorStatusHandler: (status: number, mode: number) => void) {
        this.wasmImportObject = {
            env: {
                setIndicatorStatus: indicatorStatusHandler,
                getIndicatorStatus: () => 0,
                getRightPushButtonState: () => 0,
                getLeftPushButtonState: () => 0,
                getHazardPushButtonState: () => 0,
                updateState: () => {
                },
            }
        };
    }

    leftButtonPressed(indicatorStatus: number) {
        fetch("turn_indicator.wasm").then(response =>
            response.arrayBuffer()
        ).then(bytes =>
            WebAssembly.instantiate(bytes, this.wasmImportObject)
        ).then(result => {
            console.log("Raising leftButtonPressed event with indicatorStatus: " + indicatorStatus)
            const onLeftButtonPressed = result.instance.exports.onLeftButtonPressed as CallableFunction;
            onLeftButtonPressed(indicatorStatus);
        });
    }

    rightButtonPressed(indicatorStatus: number) {
        fetch("turn_indicator.wasm").then(response =>
            response.arrayBuffer()
        ).then(bytes =>
            WebAssembly.instantiate(bytes, this.wasmImportObject)
        ).then(result => {
            console.log("Raising rightButtonPressed event with indicatorStatus: " + indicatorStatus)
            const onRightButtonPressed = result.instance.exports.onRightButtonPressed as CallableFunction;
            onRightButtonPressed(indicatorStatus);
        });
    }

    parkingButtonPressed(indicatorStatus: number) {
        fetch("turn_indicator.wasm").then(response =>
            response.arrayBuffer()
        ).then(bytes =>
            WebAssembly.instantiate(bytes, this.wasmImportObject)
        ).then(result => {
            console.log("Raising parkingButtonPressed event with indicatorStatus: " + indicatorStatus)
            const onParkingButtonPressed = result.instance.exports.onParkingButtonPressed as CallableFunction;
            onParkingButtonPressed(indicatorStatus);
        });
    }
}
