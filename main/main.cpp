#include <cstdio>

#include "esp_console.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "cmd_nvs.h"
#include "cmd_system.h"
#include "cmd_wifi.h"

#include "http_requests.h"
#include "tsk_morse_output.h"

extern "C" void app_main() {
    // Initialize NVS and wifi
    initialize_nvs();
    initialize_wifi();
    initialize_morse();

    // Initialize the command line interface
    esp_console_repl_t* repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = ">";
    repl_config.max_cmdline_length = 128;

    esp_console_register_help_command();
    register_system_common();
    register_wifi();
    register_morse();

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));

    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    setup_server();

    // Spawn Tasks
    xTaskCreatePinnedToCore(task_morse_pin_output, "MORSE", 2048, nullptr, 10, nullptr, APP_CPU_NUM);
}
