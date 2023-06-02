#![no_std]
#![no_main]

use core::panic::PanicInfo;

// ()

/// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
fn start() {
    loop {}
}

#[no_mangle]
fn add(num_1: i32, num_2: i32) -> i32 {
    num_1 + num_2
}
