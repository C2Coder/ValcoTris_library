#include "Logic.hpp"
#include "Platform.hpp"

#include <bitset>
#include <eventpp/callbacklist.h>
#include <eventpp/utilities/conditionalfunctor.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <functional>
#include <thread>

Logic::Logic() {
}

Logic& IRAM_ATTR Logic::singleton() {
    static Logic DRAM_ATTR instance;
    return instance;
}

void Logic::init() {
    gpio_config_t configData = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&configData);
}

StatusBar& Logic::statusBar() {
    return m_statusBar;
}

Display& Logic::display() {
    return m_display;
}

Nvs& Logic::nvs() {
    static Nvs instance("LogicLib");
    return instance;
}

Logic& logic = Logic::singleton();
Display& display = logic.display();
StatusBar& statusBar = logic.statusBar();

extern void logicMain();

extern "C" {
[[gnu::weak]] void app_main() {
    logic.init();

    // The SmartLeds Interrupt handler must be registered on different core than
    // the WiFi Interrupts are, otherwise SmartLeds can't feed RMT fast enough.
    // We can't feasibly change SmartLeds's core because it is initialized
    // via global constructors, but WiFi core is set to 1 via sdkconfig.* files.
    // CONFIG_ESP32_WIFI_TASK_PINNED_TO_CORE_1

    logicMain();

    while (true)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
}
}
