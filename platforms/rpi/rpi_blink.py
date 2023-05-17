import os
import wasm3
import RPi.GPIO as GPIO 
from time import sleep

GPIO.setwarnings(False) # Ignore warning for now
GPIO.setmode(GPIO.BCM) # Use GPIO pin numbering not physical numbering

scriptpath = os.path.dirname(os.path.realpath(__file__))
wasm_fn = os.path.join(scriptpath, "blink-gpio-4.wasm")
print(wasm_fn)

env = wasm3.Environment()
rt = env.new_runtime(4096)

def pinMode(pin, mode):
    if mode == 0:
        GPIO.setup(pin, GPIO.IN, initial=GPIO.LOW)
    else:
        GPIO.setup(pin, GPIO.OUT, initial=GPIO.LOW)

def digitalWrite(pin, state):
    if state==0:
        GPIO.output(pin, GPIO.LOW)
        print("LED OFF ", pin)
    else:
        GPIO.output(pin, GPIO.HIGH)
        print("LED ON ", pin)

def delay(millis):
    sleep(millis/1000)

with open(wasm_fn, "rb") as f:
    mod = env.parse_module(f.read())
    rt.load(mod)
    mod.link_function("env", "pinMode",    "v(ii)",  pinMode)
    mod.link_function("env", "digitalWrite",    "v(ii)",  digitalWrite)
    mod.link_function("env", "delay",    "v(i)",  delay)

wasm_blink = rt.find_function("start")
wasm_blink()