import os
import wasm3
import RPi.GPIO as GPIO
import threading
import time
import selectors
import sys

LEFT_LED_PIN = 4        # Specify left indicator LED GPIO pin
RIGHT_LED_PIN = 3       # Specify Right Indicator LED GPIO pin
LEFT_BUTTON_PIN = 20    # Specify Left Push Button GPIO pin
RIGHT_BUTTON_PIN = 21   # Specify Right Push Button GPIO pin
PARKING_BUTTON_PIN = 16  # Specify Parking Push Button GPIO pin

GPIO.setwarnings(False)  # Ignore warning for now
# Use GPIO pin numbering not physical numbering, checkout pin config by running `pinout` interminal
GPIO.setmode(GPIO.BCM)

selector = selectors.DefaultSelector()
blinking_thread = None
current_indicator_status_code = 0
blinking_stop_event = threading.Event()

scriptpath = os.path.dirname(os.path.realpath(__file__))
wasm_file_path = os.path.join(scriptpath, "turn_indicator.wasm")


def pinMode(pin, mode):
    if mode == 0:
        GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    else:
        GPIO.setup(pin, GPIO.OUT, initial=GPIO.LOW)


def digitalWrite(pin, state):
    if state == 0:
        GPIO.output(pin, GPIO.LOW)
        # print("LED OFF ", pin)
    else:
        GPIO.output(pin, GPIO.HIGH)
        # print("LED ON ", pin)


def stop_blinking_thread():
    global blinking_stop_event, blinking_thread, current_indicator_status_code
    # print('stop is called')
    if blinking_thread is not None:
        blinking_stop_event.set()
        blinking_thread.join()
    blinking_stop_event.clear()
    current_indicator_status_code = 0


def blinkLED(interval, pin, stop_event):
    if interval == 0:
        if pin == 99:
            digitalWrite(LEFT_LED_PIN, GPIO.HIGH)
            digitalWrite(RIGHT_LED_PIN, GPIO.HIGH)
        else:
            digitalWrite(pin, GPIO.HIGH)
        return

    start_time = time.monotonic()
    while not stop_event.is_set():
        elapsed_time = time.monotonic() - start_time
        if pin == 99:
            if elapsed_time % (interval / 1000 * 2) < (interval / 1000):
                digitalWrite(LEFT_LED_PIN, GPIO.HIGH)
                digitalWrite(RIGHT_LED_PIN, GPIO.HIGH)
            else:
                digitalWrite(LEFT_LED_PIN, GPIO.LOW)
                digitalWrite(RIGHT_LED_PIN, GPIO.LOW)
        else:
            if elapsed_time % (interval / 1000 * 2) < (interval / 1000):
                digitalWrite(pin, GPIO.HIGH)
            else:
                digitalWrite(pin, GPIO.LOW)


def getLeftPushButtonState():
    return 0


def getRightPushButtonState():
    return 0


def updateIndicatorStatus(status, interval):
    global blinking_thread, blinking_stop_event, current_indicator_status_code
    if blinking_thread is not None:
        stop_blinking_thread()  # stop the previous thread, if any
    if status == 0:
        digitalWrite(LEFT_LED_PIN, GPIO.LOW)
        digitalWrite(RIGHT_LED_PIN, GPIO.LOW)
        current_indicator_status_code = 0
    elif status == 1:
        blinking_thread = threading.Thread(
            target=blinkLED, args=(interval, LEFT_LED_PIN, blinking_stop_event))
        blinking_thread.start()
        digitalWrite(RIGHT_LED_PIN, GPIO.LOW)
        current_indicator_status_code = 1
    elif status == 2:
        blinking_thread = threading.Thread(
            target=blinkLED, args=(interval, RIGHT_LED_PIN, blinking_stop_event))
        blinking_thread.start()
        digitalWrite(LEFT_LED_PIN, GPIO.LOW)
        current_indicator_status_code = 2
    elif status == 3:
        blinking_thread = threading.Thread(
            target=blinkLED, args=(interval, 99, blinking_stop_event))
        blinking_thread.start()
        current_indicator_status_code = 3
    print('current_indicator_status_code - ',
          current_indicator_status_code, flush=True)


def parse_wasm_file(wasm_file_path, env, rt):
    with open(wasm_file_path, "rb") as f:
        mod = env.parse_module(f.read())
        rt.load(mod)
        mod.link_function("env", "setIndicatorStatus",
                          "v(ii)", updateIndicatorStatus)
        mod.link_function("env", "getLeftPushButtonState",
                          "i()",  getLeftPushButtonState)
        mod.link_function("env", "getRightPushButtonState",
                          "i()",  getRightPushButtonState)
        print('WASM file loaded from path:', wasm_file_path, flush=True)


def reset_GPIO():
    try:
        GPIO.remove_event_detect(LEFT_BUTTON_PIN)
        GPIO.remove_event_detect(RIGHT_BUTTON_PIN)
        GPIO.remove_event_detect(PARKING_BUTTON_PIN)
    except:
        pass
    GPIO.cleanup()
    GPIO.setmode(GPIO.BCM)
    pinMode(LEFT_LED_PIN, 1)
    pinMode(RIGHT_LED_PIN, 1)
    pinMode(LEFT_BUTTON_PIN, 0)
    pinMode(RIGHT_BUTTON_PIN, 0)
    pinMode(PARKING_BUTTON_PIN, 0)


def watch_for_changes(path, wasm_env_callback):
    try:
        last_modified_time = os.path.getmtime(path)
        wasm_env_stop_event = threading.Event()
        wasm_env_thread = None
        wasm_env_thread = threading.Thread(
            target=wasm_env_callback, args=(path, wasm_env_stop_event,))
        wasm_env_thread.start()
    except:
        print(f'{path} File not found', flush=True)
        return
    try:
        while True:
            try:
                modified_time = os.path.getmtime(path)
                if modified_time != last_modified_time:
                    last_modified_time = modified_time
                    print(f"{path} has been modified!", flush=True)
                    if wasm_env_thread.is_alive:
                        if blinking_thread != None:
                            stop_blinking_thread()
                        reset_GPIO()
                        wasm_env_stop_event.set()
                        wasm_env_thread.join()
                    wasm_env_stop_event.clear()
                    wasm_env_thread = threading.Thread(
                        target=wasm_env_callback, args=(path, wasm_env_stop_event))
                    wasm_env_thread.start()
            except FileNotFoundError:
                # Handle case when the file is deleted and recreated
                last_modified_time = None
                while not last_modified_time:
                    try:
                        last_modified_time = os.path.getmtime(path)
                    except FileNotFoundError:
                        print('File not found or deleted', flush=True)
                        time.sleep(0.1)
            time.sleep(0.1)
    except KeyboardInterrupt:
        print('Recieved Keyboard Interrupt, Stopping the application...', flush=True)
        if blinking_thread != None:
            stop_blinking_thread()
        if wasm_env_thread.is_alive:
            wasm_env_stop_event.set()
            wasm_env_thread.join()
        wasm_env_stop_event.clear()
        GPIO.cleanup()
        sys.exit(0)


def wasm_env_callback(path, stop_event):
    global current_indicator_status_code
    env = wasm3.Environment()
    rt = env.new_runtime(4096)
    parse_wasm_file(wasm_file_path, env, rt)
    onLeftButtonPressed = rt.find_function("onLeftButtonPressed")
    onRightButtonPressed = rt.find_function("onRightButtonPressed")
    onParkingButtonPressed = rt.find_function("onParkingButtonPressed")
    # Checking push button events
    GPIO.add_event_detect(LEFT_BUTTON_PIN, GPIO.BOTH,
                          callback=lambda channel: onLeftButtonPressed(current_indicator_status_code), bouncetime=300)
    GPIO.add_event_detect(RIGHT_BUTTON_PIN, GPIO.BOTH,
                          callback=lambda channel: onRightButtonPressed(current_indicator_status_code), bouncetime=300)
    GPIO.add_event_detect(PARKING_BUTTON_PIN, GPIO.BOTH,
                          callback=lambda channel: onParkingButtonPressed(current_indicator_status_code), bouncetime=300)
    while not stop_event.is_set():
        # Wait for keyboard events for up to 0.5 second
        keyboard_events = selector.select(timeout=0.5)
        if keyboard_events:
            user_input = sys.stdin.readline().strip()
            print(f"You entered: {user_input}")
            if user_input in ('l', 'L'):
                onLeftButtonPressed(current_indicator_status_code)
            elif user_input in ('r', 'R'):
                onRightButtonPressed(current_indicator_status_code)
            elif user_input in ('p', 'P'):
                onParkingButtonPressed(current_indicator_status_code)
            else:
                print('Unrecognized input, Try again with l/L or r/R or p/P', flush=True)


def main():
    print('Reading the WASM binay from: ', wasm_file_path, flush=True)
    watch_for_changes(wasm_file_path, wasm_env_callback)


if __name__ == "__main__":
    reset_GPIO()
    selector.register(sys.stdin, selectors.EVENT_READ)
    main()
