#include <Arduino.h>
#include "setup.hpp"
#include "server.hpp"

#define LEFT_LED 21
#define RIGHT_LED 18
#define HAZARD_PUSH_BUTTON 16
#define LEFT_PUSH_BUTTON 4
#define RIGHT_PUSH_BUTTON 5

void setupHardware() {
    pinMode(LEFT_PUSH_BUTTON, INPUT_PULLUP);
    pinMode(LEFT_LED, OUTPUT);

    pinMode(RIGHT_PUSH_BUTTON, INPUT_PULLUP);
    pinMode(RIGHT_LED, OUTPUT);

    pinMode(HAZARD_PUSH_BUTTON, INPUT_PULLUP);
}

void setup()
{
    // put your setup code here, to run once:
    startSerial();
    // disableWDT();
    printGreetingMessage();
    setupHardware();
    startFilesystem();
    startWifi();
    initWasmEnvironment();
    startWebServer();
    setupTasks("/");
}

void loop()
{
    // put your main code here, to run repeatedly:

    handleServerRequests();
}