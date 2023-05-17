#![no_std]

#[no_mangle]
pub fn add(left: i32, right: i32) -> i64 {
    left as i64 + right as i64
}

#[no_mangle]
pub fn sub(left: i32, right: i32) -> i32 {
    left - right
}

#[panic_handler]
fn handle_panic(_: &core::panic::PanicInfo) -> ! {
    unreachable!()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn add_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }

    #[test]
    fn sub_works() {
        let result = sub(4, 3);
        assert_eq!(result, 1);
    }
}
