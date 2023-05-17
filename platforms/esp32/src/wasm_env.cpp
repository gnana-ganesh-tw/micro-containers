#include <ArduinoJson.h>
#include <m3_env.h>
#include "SPIFFS.h"

#include "task.hpp"
#include "wasm_env.hpp"

////////////////////////
// macros
////////////////////////
#define LEFT_LED 21
#define RIGHT_LED 18
#define LEFT_PUSH_BUTTON 4
#define RIGHT_PUSH_BUTTON 5
#define HAZARD_PUSH_BUTTON 16
#define ESP32_ONBOARD_LED 2
#define WASM_STACK_SLOTS 1024
#define WASM_TASK_PRIORITY 1
#define FATAL(func, msg)                  \
    {                                     \
        Serial.print("Fatal: " func " "); \
        Serial.println(msg);              \
    }

IM3Environment env;

std::map<String, TaskHandle_t> taskHandles;
std::map<String, IM3Runtime> taskRuntimes;

std::map<String, PausedTaskInfo> pausedTasks;

int indicatorStatus;
int rightButtonState;
int leftButtonState;
int hazardButtonState;
int rightLedState;
int leftLedState;
int blinkRightLedLastTime;
int blinkLeftLedLastTime;
int blinkingMode;

void reset()
{
    indicatorStatus = 0;
    rightButtonState = digitalRead(RIGHT_PUSH_BUTTON);
    leftButtonState = digitalRead(LEFT_PUSH_BUTTON);
    hazardButtonState = digitalRead(HAZARD_PUSH_BUTTON);
    rightLedState = LOW;
    leftLedState = LOW;
    blinkRightLedLastTime = millis();
    blinkLeftLedLastTime = millis();
    blinkingMode = 0;
}

void createEnvironment()
{

    // create WASM3 environment for the interpreter
    env = m3_NewEnvironment();
    if (!env)
    {
        FATAL("NewEnvironment", "failed");
    }
}

////////////////////////
// binding functions
////////////////////////
m3ApiRawFunction(m3_arduino_millis)
{
    m3ApiReturnType(uint32_t)

        m3ApiReturn(millis());
}

m3ApiRawFunction(m3_arduino_updateState)
{
    switch (indicatorStatus)
    {
    case 0:
    {
        digitalWrite(RIGHT_LED, LOW);
        digitalWrite(LEFT_LED, LOW);
        break;
    }
    case 1:
    {
        digitalWrite(LEFT_LED, LOW);
        if (millis() - blinkRightLedLastTime > blinkingMode)
        {
            blinkRightLedLastTime = millis();
            rightLedState = !rightLedState;         // SET ledState TO THE OPPOSITE OF ledState
            digitalWrite(RIGHT_LED, rightLedState); // WRITE THE NEW ledState
        }
        else
        {

            digitalWrite(RIGHT_LED, rightLedState);
        }
        break;
    }
    case 2:
    {

        digitalWrite(RIGHT_LED, LOW);
        if (millis() - blinkLeftLedLastTime > blinkingMode)
        {
            blinkLeftLedLastTime = millis();
            leftLedState = !leftLedState;         // SET ledState TO THE OPPOSITE OF ledState
            digitalWrite(LEFT_LED, leftLedState); // WRITE THE NEW ledState
        }
        else
        {

            digitalWrite(LEFT_LED, leftLedState);
        }
        break;
    }
    case 3:
    {
        if (millis() - blinkLeftLedLastTime > blinkingMode)
        {
            blinkLeftLedLastTime = millis();
            leftLedState = !leftLedState;
            blinkRightLedLastTime = millis();
            rightLedState = !rightLedState;
            digitalWrite(RIGHT_LED, leftLedState);
            digitalWrite(LEFT_LED, leftLedState);
        }
        else
        {
            digitalWrite(RIGHT_LED, leftLedState);
            digitalWrite(LEFT_LED, leftLedState);
        }
        break;
    }
    }
    m3ApiSuccess();
}

uint8_t mapPinMode(uint8_t mode)
{
    switch (mode)
    {
    case 0:
        return INPUT;
    case 1:
        return OUTPUT;
    case 2:
        return INPUT_PULLUP;
    case 3:
        return INPUT_PULLDOWN;
    }
    return INPUT;
}

// m3ApiRawFunction(m3_arduino_pinMode)
// {
//     m3ApiGetArg(uint32_t, pin)
//         m3ApiGetArg(uint32_t, mode)

// #if !defined(PARTICLE)
//             typedef uint8_t PinMode;
// #endif
//     pinMode(pin, (PinMode)mapPinMode(mode));

//     Serial.println("pin mode: succes");
//     m3ApiSuccess();
// }

// m3ApiRawFunction(m3_arduino_setPinMode)
// {
//     #if !defined(PARTICLE)
//             typedef uint8_t PinMode;
//     #endif

//     pinMode(LEFT_PUSH_BUTTON, INPUT_PULLDOWN);
//     pinMode(LEFT_LED, OUTPUT);

//     pinMode(RIGHT_PUSH_BUTTON, INPUT_PULLDOWN);
//     pinMode(RIGHT_LED, OUTPUT);

//     Serial.println("pin mode: success");

//     m3ApiSuccess();
// }

m3ApiRawFunction(m3_arduino_getPinLED)
{
    Serial.println("Calling: analogRead()");
    m3ApiReturnType(uint32_t)

        m3ApiReturn(ESP32_ONBOARD_LED);
}

m3ApiRawFunction(m3_arduino_digitalWrite)
{
    m3ApiGetArg(uint32_t, pin)
        m3ApiGetArg(uint32_t, value)

            digitalWrite(pin, value);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_analogRead)
{
    m3ApiGetArg(uint32_t, pin);
    m3ApiReturnType(uint32_t)
        m3ApiReturn(analogRead(pin));
}

m3ApiRawFunction(m3_arduino_delay)
{
    m3ApiGetArg(uint32_t, ms)

        vTaskDelay(ms / portTICK_PERIOD_MS);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_print)
{
    m3ApiGetArgMem(const uint8_t *, buf)
        m3ApiGetArg(uint32_t, len)

            Serial.write(buf, len);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_printInt)
{
    m3ApiGetArg(int, num)

        Serial.print(num);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_setLeftIndicator)
{
    m3ApiGetArg(int, state)

        Serial.println("setLeftIndicator : ");
    Serial.print(state);

    if (state == 1)
        digitalWrite(LEFT_LED, HIGH);
    else
        digitalWrite(LEFT_LED, LOW);
    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_setRightIndicator)
{
    m3ApiGetArg(int, state)

        Serial.println("setRightIndicator : ");
    Serial.print(state);

    if (state == 1)
        digitalWrite(RIGHT_LED, HIGH);
    else
        digitalWrite(RIGHT_LED, LOW);
    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_setIndicatorStatus)
{
    m3ApiGetArg(int, state)
        m3ApiGetArg(int, mode)

            Serial.print("setIndicatorStatus : ");
    Serial.println(state);

    indicatorStatus = state;
    blinkingMode = mode;
    blinkLeftLedLastTime = millis();
    blinkRightLedLastTime = millis();
    leftLedState = HIGH;
    rightLedState = HIGH;
    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_getRightPushButtonState)
{
    m3ApiReturnType(uint32_t) int state = digitalRead(RIGHT_PUSH_BUTTON);
    int toggle = 0;
    if (state != rightButtonState)
    {
        rightButtonState = state;
        toggle = 1;
    }

    // Serial.println("Right Button");
    // Serial.println(state);

    m3ApiReturn(toggle);
}

m3ApiRawFunction(m3_arduino_getIndicatorStatus)
{
    m3ApiReturnType(uint32_t)
        // Serial.print("getIndicatorStatus : ");
        // Serial.println(indicatorStatus);

        m3ApiReturn(indicatorStatus);
}

m3ApiRawFunction(m3_arduino_getLeftPushButtonState)
{
    m3ApiReturnType(uint32_t) int state = digitalRead(LEFT_PUSH_BUTTON);
    // Serial.println("Left Button");
    // Serial.println(state);
    int toggle = 0;
    if (state != leftButtonState)
    {
        leftButtonState = state;
        toggle = 1;
    }
    m3ApiReturn(toggle);
}

m3ApiRawFunction(m3_arduino_getHazardPushButtonState)
{
    m3ApiReturnType(uint32_t) int state = digitalRead(HAZARD_PUSH_BUTTON);
    // Serial.print("Hazard Button: ");
    // Serial.println(state);
    int toggle = 0;
    if (state != hazardButtonState)
    {
        hazardButtonState = state;
        toggle = 1;
    }
    m3ApiReturn(toggle);
}

m3ApiRawFunction(m3_arduino_pauseTask)
{
    m3ApiGetArgMem(const uint8_t *, state)
        m3ApiGetArg(uint32_t, len)
            m3ApiGetArgMem(const char *, filename)

                PausedTaskInfo pausedTaskInfo;
    pausedTaskInfo.state = (uint8_t *)state;
    pausedTaskInfo.len = len;

    pausedTasks[filename] = pausedTaskInfo;

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_resumeTask)
{
    m3ApiReturnType(uint32_t)
        m3ApiGetArgMem(uint8_t *, state)
            m3ApiGetArg(uint32_t, len)
                m3ApiGetArgMem(const char *, filename)

                    uint32_t hadState = 0;
    if (pausedTasks.count(filename))
    {
        PausedTaskInfo pausedTaskInfo = pausedTasks[filename];
        memmove(state, pausedTaskInfo.state, min(len, pausedTaskInfo.len));
        pausedTasks.erase(filename);
        hadState = 1;
    }

    m3ApiReturn(hadState);
}

bool to_hex(char *dest, size_t dest_len, const uint8_t *values, size_t val_len)
{
    if (dest_len < (val_len * 2 + 1)) /* check that dest is large enough */
        return false;
    *dest = '\0'; /* in case val_len==0 */
    while (val_len--)
    {
        /* sprintf directly to where dest points */
        sprintf(dest, "%02X", *values);
        dest += 2;
        ++values;
    }
    return true;
}

////////////////////////
// WASM interpreter
////////////////////////
M3Result bindWasmFunctions(IM3Runtime runtime)
{
    IM3Module module = runtime->modules;
    const char *module_name = "env";

    m3_LinkRawFunction(module, module_name, "millis", "i()", &m3_arduino_millis);
    // m3_LinkRawFunction(module, module_name, "pinMode", "v(ii)", &m3_arduino_pinMode);
    // m3_LinkRawFunction(module, module_name, "setPinMode", "v()", &m3_arduino_setPinMode);
    m3_LinkRawFunction(module, module_name, "getPinLED", "i()", &m3_arduino_getPinLED);
    m3_LinkRawFunction(module, module_name, "digitalWrite", "v(ii)", &m3_arduino_digitalWrite);
    m3_LinkRawFunction(module, module_name, "analogRead", "i(i)", &m3_arduino_analogRead);
    m3_LinkRawFunction(module, module_name, "delay", "v(i)", &m3_arduino_delay);
    m3_LinkRawFunction(module, module_name, "print", "v(*i)", &m3_arduino_print);
    m3_LinkRawFunction(module, module_name, "printInt", "v(i)", &m3_arduino_printInt);
    m3_LinkRawFunction(module, module_name, "setLeftIndicator", "v(i)", &m3_arduino_setLeftIndicator);
    m3_LinkRawFunction(module, module_name, "setRightIndicator", "v(i)", &m3_arduino_setRightIndicator);
    m3_LinkRawFunction(module, module_name, "setIndicatorStatus", "v(ii)", &m3_arduino_setIndicatorStatus);
    m3_LinkRawFunction(module, module_name, "getLeftPushButtonState", "i()", &m3_arduino_getLeftPushButtonState);
    m3_LinkRawFunction(module, module_name, "getRightPushButtonState", "i()", &m3_arduino_getRightPushButtonState);
    m3_LinkRawFunction(module, module_name, "getHazardPushButtonState", "i()", &m3_arduino_getHazardPushButtonState);
    m3_LinkRawFunction(module, module_name, "getIndicatorStatus", "i()", &m3_arduino_getIndicatorStatus);
    m3_LinkRawFunction(module, module_name, "updateState", "v()", &m3_arduino_updateState);

    m3_LinkRawFunction(module, module_name, "pauseTask", "v(*i*)", &m3_arduino_pauseTask);
    m3_LinkRawFunction(module, module_name, "resumeTask", "i(*i*)", &m3_arduino_resumeTask);

    // Serial.println("Linked Arduino functions");
    return m3Err_none;
}

void callWasmFunction(IM3Runtime runtime, const char *functionName)
{
    M3Result result = m3Err_none;
    // start WASM program
    IM3Function f;
    result = m3_FindFunction(&f, runtime, "start");
    if (result)
        FATAL("FindFunction", result);

    result = m3_CallV(f);

    Serial.println("Function Called");

    uint32_t value = 0;
    result = m3_GetResultsV(f, &value);
    if (result)
        FATAL("m3_GetResults: %s", result);

    Serial.print("Result: ");
    Serial.println(value);

    // Getting here means we've encountered an error.
    // Let's print out some debugging information
    if (result)
    {
        M3ErrorInfo info;
        m3_GetErrorInfo(runtime, &info);
        Serial.print("Error: ");
        Serial.print(result);
        Serial.print(" (");
        Serial.print(info.message);
        Serial.println(")");
        if (info.file && strlen(info.file) && info.line)
        {
            Serial.print("At ");
            Serial.print(info.file);
            Serial.print(":");
            Serial.println(info.line);
        }
    }
}

void runWasmThread(void *parameters)
{
    reset();
    const char *filename = static_cast<const char *>(parameters);
    M3Result result = m3Err_none;

    IM3Runtime runtime = m3_NewRuntime(env, WASM_STACK_SLOTS, NULL);
    if (!runtime)
    {
        FATAL("NewRuntime", "failed");
        removeWasmTask(filename);
        return;
    }
    runtime->memoryLimit = getTaskDetails(filename).memoryLimit;

    // read WASM module from the filesystem into a uint8_t buffer
    if (!SPIFFS.exists(filename))
    {
        Serial.println("WASM file does not exist");
        Serial.print("File: ");
        Serial.println(filename);
        removeWasmTask(filename);
        return;
    }
    // Serial.println("Trying to open WASM file...");
    File file = SPIFFS.open(filename, "r");
    if (!file)
    {
        Serial.println("Failed to open WASM file for reading");
        removeWasmTask(filename);
        return;
    }
    // Serial.println("Opened WASM file");
    size_t app_wasm_len = file.size();
    uint8_t app_wasm[app_wasm_len];
    file.read(app_wasm, app_wasm_len);
    file.close();
    // Serial.println("Copied file contents into buffer");

    // parse the module using the environment
    IM3Module module;
    result = m3_ParseModule(env, &module, app_wasm, app_wasm_len);
    if (result)
    {
        FATAL("ParseModule", result);
        removeWasmTask(filename);
        return;
    }

    // Serial.println("Parsed WASM module");
    result = m3_LoadModule(runtime, module);
    if (result)
    {
        FATAL("LoadModule", result);
        removeWasmTask(filename);
        return;
    }
    // Serial.println("Loaded WASM module");

    // link created binding functions
    result = bindWasmFunctions(runtime);
    if (result)
    {
        FATAL("BindWasmFunctions", result);
        removeWasmTask(filename);
        return;
    }

    taskRuntimes[filename] = runtime;
    callWasmFunction(runtime, "fib");

    removeWasmTask(filename);
}

void runWasmPause(void *parameters)
{
    const char *filename = static_cast<const char *>(parameters);
    IM3Runtime runtime = taskRuntimes[filename];

    callWasmFunction(runtime, "_pause");

    vTaskDelete(taskHandles[filename]);
    taskHandles.erase(filename);

    vTaskDelete(NULL);
}

////////////////////////
// WASM task operations
////////////////////////

void createWasmTask(String filename)
{
    Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");
    Serial.print("Starting new task :");
    Serial.print(filename);
    Serial.println(getTaskDetails(filename).reservedStackSize);

    TaskHandle_t taskHandle = NULL;
    taskHandles[filename] = taskHandle;

    xTaskCreate(
        &runWasmThread,
        filename.c_str(),
        getTaskDetails(filename).reservedStackSize,
        (void *)taskHandles.find(filename)->first.c_str(), // get map key, to have a permanent reference
        WASM_TASK_PRIORITY,
        &taskHandles[filename]);
    Serial.println("...");
}

void removeWasmTask(String filename)
{
    Serial.print("Stopping task ");
    Serial.print(filename);
    Serial.println("...");

    taskRuntimes.erase(filename);
    pausedTasks.erase(filename);

    if (taskHandles.count(filename))
    {
        TaskHandle_t taskHandler = taskHandles[filename];
        taskHandles.erase(filename);
        vTaskDelete(taskHandler);
    }
}

bool isTaskActive(String filename)
{
    return taskRuntimes.count(filename);
}

void liveUpdateTask(String filename)
{
    if (isTaskActive(filename))
    {
        Serial.print("Performing live update for task ");
        Serial.print(filename);
        Serial.println("...");

        removeWasmTask(filename);
        createWasmTask(filename);
    }
}

void pauseTask(String filename)
{
    xTaskCreate(
        &runWasmPause,
        (filename + "_pause").c_str(),
        getTaskDetails(filename).reservedStackSize,
        (void *)taskRuntimes.find(filename)->first.c_str(), // get map key, to have a permanent reference
        WASM_TASK_PRIORITY,
        NULL);
}

bool isTaskPaused(String filename)
{
    return pausedTasks.count(filename);
}

String listActiveTasksJson()
{
    const size_t numActiveTasks = taskRuntimes.size();
    const size_t capacity = JSON_ARRAY_SIZE(numActiveTasks) + numActiveTasks * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(3);
    DynamicJsonDocument jsonDocument(capacity);

    JsonArray tasks = jsonDocument.createNestedArray("tasks");
    std::map<String, IM3Runtime>::iterator it;
    for (it = taskRuntimes.begin(); it != taskRuntimes.end(); it++)
    {
        JsonObject activeTaskObj = tasks.createNestedObject();
        activeTaskObj["filename"] = it->first.c_str();
        bool paused = isTaskPaused(it->first);
        activeTaskObj["paused"] = paused;
        if (!paused)
        {
            activeTaskObj["stack_high_water_mark"] = uxTaskGetStackHighWaterMark(taskHandles[it->first]);
        }
    }
    jsonDocument["espressif_num_threads"] = uxTaskGetNumberOfTasks();
    jsonDocument["status_code"] = 0;

    String res;
    serializeJson(jsonDocument, res);
    return res;
}