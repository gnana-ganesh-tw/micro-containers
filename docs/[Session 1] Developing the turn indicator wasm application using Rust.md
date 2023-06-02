# Microcontainers - Session 01

### Install Rust

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

### Creating your first rust project

```bash
cargo new turn_indicator
cd turn_indicator
```

### Running the rust project

```bash
cargo run 
```

### Disabling the Standard Library

```rust
#![no_std]

fn main() {
    println!("Hello, world!");
}
```

### Fixing `println!()` error

```rust
#![no_std]

fn main() {}
```

### Panic Implementation

```rust
use core::panic::PanicInfo;

/// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
```

`!` denotes this function does not return.

### Overwriting the Entry Point

```rust
#![no_std]
#![no_main]

// ...
```

### Writing a rust function

```rust
#[no_mangle]
unsafe fn start() {
    loop {}
}
```

### Defining Indicator mode

```rust

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
    BlinkingFast = 500,
    // 500ms
    BlinkingSlow = 2000, // 2s
}
```

### Setting the current Indicator mode

```rust
/* Update this variable to change the indicator  mode. */
const CURRENT_INDICATOR_MODE: IndicatorMode = IndicatorMode::BlinkingFast;
```

### Handling right button push

```rust
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
```

### Handling left button push

```rust

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
```

### Handling right button push

```rust

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
```

### Defining API from platform

```rust
/*
Functions implemented by the platform
(these functions implement platform specific functionality)

setIndicatorStatus: Sets the indicator status
status : 0 - Left and Right indicators OFF
         1 - Left indicator ON, Right indicator is OFF
         2 - Right indicator ON, Left indicator is OFF
         3 - Left and Right indicators ON (Parking mode is active)
*/
extern "C" {
    // Returns the value of current indicator status
    #[link_name = "getIndicatorStatus"]
    fn get_indicator_status() -> i32;

    #[link_name = "setIndicatorStatus"]
    fn setIndicatorStatus(indicator_status: i32, mode: i32);

    // Return 1 if the left button is pressed
    #[link_name = "getLeftPushButtonState"]
    fn getLeftPushButtonState() -> i32;

    // Return 1 if the right button is pressed
    #[link_name = "getRightPushButtonState"]
    fn getRightPushButtonState() -> i32;

    // Return 1 if the hazard button is pressed
    #[link_name = "getHazardPushButtonState"]
    fn get_hazard_push_button_state() -> i32;
}
```

## Buzz of "WASM"

WebAssembly (abbreviated Wasm) is a binary instruction format for a stack-based virtual machine. Wasm is designed as
a **portable compilation target** for programming languages.

### Why wasm ?

(referred from https://webassembly.org/ )

#### Efficient and fast

The Wasm stack machine is designed to be encoded in a size- and load-time-efficient binary format. WebAssembly aims to
execute at native speed by taking advantage of common hardware capabilities available on a wide range of platforms.

#### Safe

WebAssembly describes a memory-safe, sandboxed execution environment that may even be implemented inside existing
JavaScript virtual machines. When embedded in the web, WebAssembly will enforce the same-origin and permissions security
policies of the browser.


### Targets in Rust
(refer [Target Triplet](https://doc.rust-lang.org/nightly/rustc/platform-support.html))
- Rust supports wasm target

### Adding Rust target
```bash
rustup target add wasm32-wasi
```
### Compiling to wasm
```bash
cargo build --target wasm32-wasi
```
### Configuring the project to use a default target as wasm32-wasi
Create a file called `Config.toml` in `.cargo` folder with the following contents
```toml
[build]
target = "wasm32-wasi"
```
Now, `cargo build` can be used to generate a `turn_indicator.wasm` file


## Micro-controller

It is a small computer with memory, GPIO pins for input and outputs, which is capable of doing a specific task. For
example, engine control in automobile, washing machines, etc.

### Types of Microcontrollers

(refer here for more https://www.elprocus.com/microcontrollers-types-and-applications/)

#### Based on number of bits

- 8 bit
- 16 bit
- 32 bit
- 64 bit

### Problem with Microcontroller specific codebase

Different microcontroller usually has its own set of libraries and drivers. Which means the application that we write on
one platform

(more on this in the next session!!)


## Some useful rust resources to get you started

- [Rusltings](https://github.com/rust-lang/rustlings): 🦀 Small exercises to get you used to reading and writing Rust
  code!
- [The Rust Book](https://doc.rust-lang.org/book/): by Steve Klabnik and Carol Nichols, with contributions from the Rust
  Community

#### Embedded Rust

- [The Embedded Rust Book](https://docs.rust-embedded.org/book/): An introductory book about using the Rust Programming
  Language on "Bare Metal" embedded systems, such as Microcontrollers.
1