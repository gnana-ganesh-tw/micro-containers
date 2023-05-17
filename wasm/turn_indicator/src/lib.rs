#![no_main]
#![no_std]

use core::panic::PanicInfo;

/* This enum can be used to choose the indicator mode or the way indicators
   behave when turned on. The following modes are available:
   - Continuous: The indicator is turned on until it is turned off again
   - BlinkingFast: The indicator is turned on for 500ms and then turned off
   for 500ms
   - BlinkingSlow: The indicator is turned on for 2s and then turned off for
   2s
*/
enum IndicatorMode {
    Continuous = 0,
    BlinkingFast = 500,  // 500ms
    BlinkingSlow = 2000, // 2s
}

/* Update this variable to change the indicator  mode. */
const CURRENT_INDICATOR_MODE: IndicatorMode = IndicatorMode::BlinkingFast;

/************************************************************************
    Functions implemented by the platform
    (these functions implement platform specific functionality)

    setLeftIndicator: Sets the left indicator state ON(1)/OFF(0)
                        and its mode (see mode def)
    setRightIndicator: Sets the right indicator state ON(1)/OFF(0)
                        and its mode (see mode def)
    setIndicatorStatus: Sets the indicator status
    status : 0 - Left and Right indicators OFF
             1 - Left indicator ON, Right indicator is OFF
             2 - Right indicator ON, Left indicator is OFF
             3 - Left and Right indicators ON (Parking mode is active)


*************************************************************************/
extern "C" {
    // #[link_name = "setLeftIndicator"]
    // fn setLeftIndicator(state: i32, indicatorMode: i32);

    // #[link_name = "setRightIndicator"]
    // fn setRightIndicator(state: i32, indicatorMode: i32);

    #[link_name = "getIndicatorStatus"]
    fn get_indicator_status() -> i32;

    #[link_name = "setIndicatorStatus"]
    fn setIndicatorStatus(indicator_status: i32, mode: i32);

    #[link_name = "getLeftPushButtonState"]
    fn getLeftPushButtonState() -> i32;

    #[link_name = "getRightPushButtonState"]
    fn getRightPushButtonState() -> i32;

    #[link_name = "getHazardPushButtonState"]
    fn get_hazard_push_button_state() -> i32;

    #[link_name = "updateState"]
    fn update_state();
}

/************************************************************************
    Functions implemented in Wasm (these functions implement the business
    logic)
*************************************************************************/

#[no_mangle]
unsafe fn onRightButtonPressed(indicator_status: i32) {
    if (indicator_status == 0) || (indicator_status == 1) {
        // If both indicators are off or only the left indicator is active,
        setIndicatorStatus(2, CURRENT_INDICATOR_MODE as i32);
    } else if indicator_status == 2 {
        // If the right indicator is active,
        setIndicatorStatus(0, CURRENT_INDICATOR_MODE as i32);
    }
    /* If parking indicator is active (indicator_status == 3), no action needed. */
}

#[no_mangle]
unsafe fn onLeftButtonPressed(indicator_status: i32) {
    if (indicator_status == 0) || (indicator_status == 2) {
        // If both indicators are off or only the right indicator is active,
        setIndicatorStatus(1, CURRENT_INDICATOR_MODE as i32);
    } else if indicator_status == 1 {
        // If the left indicator is active,
        setIndicatorStatus(0, CURRENT_INDICATOR_MODE as i32);
    }
    /* If parking indicator is active (indicator_status == 3), no action needed. */
}

#[no_mangle]
unsafe fn onParkingButtonPressed(indicator_status: i32) {
    if indicator_status == 3 {
        // If parking indicator is active,
        setIndicatorStatus(0, CURRENT_INDICATOR_MODE as i32);
    } else {
        // If parking indicator is not active,
        setIndicatorStatus(3, CURRENT_INDICATOR_MODE as i32);
    }
}

#[no_mangle]
unsafe fn start() {
    _setup();
    loop {
        _loop();
    }
}

unsafe fn _setup() {}

unsafe fn _loop() {
    // TODO: Update the loop as per new API
    update_state();
    let indicator_status = get_indicator_status();
    if getRightPushButtonState() == 1 {
        onRightButtonPressed(indicator_status);
    }
    if getLeftPushButtonState() == 1 {
        onLeftButtonPressed(indicator_status);
    }

    if get_hazard_push_button_state() == 1 {
        onParkingButtonPressed(indicator_status);
    }
    // }
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}
