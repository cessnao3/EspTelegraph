#include "tsk_morse_output.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "argtable3/argtable3.h"
#include "esp_console.h"

#include <array>
#include <cstring>

#include "morse.h"

QueueHandle_t s_queue;

struct morse_msg {
    std::array<char, 256> msg;
    int millis_per_unit;
};

void initialize_morse() { s_queue = xQueueCreate(5, sizeof(morse_msg)); }

static std::array<const MorseCommand*, 2048> MORSE_BUFFER;

void task_morse_pin_output(void* pvParameters) {
    const gpio_num_t GPIO_MORSE_OUTPUT_PIN = GPIO_NUM_2;

    gpio_set_direction(GPIO_MORSE_OUTPUT_PIN, GPIO_MODE_OUTPUT);

    static morse_msg tmp_msg;

    while (true) {
        if (pdTRUE == xQueueReceive(s_queue, tmp_msg.msg.data(), pdMS_TO_TICKS(500))) {
            const int morse_len = construct_morse_commands(tmp_msg.msg.data(), MORSE_BUFFER.data(), MORSE_BUFFER.size());

            for (int i = 0; i < morse_len; ++i) {
                const MorseCommand* cmd = MORSE_BUFFER[i];
                if (cmd->units_enabled > 0) {
                    gpio_set_level(GPIO_MORSE_OUTPUT_PIN, 1);
                    vTaskDelay(pdMS_TO_TICKS(cmd->units_enabled * tmp_msg.millis_per_unit));
                    gpio_set_level(GPIO_MORSE_OUTPUT_PIN, 0);
                }
                vTaskDelay(pdMS_TO_TICKS(cmd->units_disabled * tmp_msg.millis_per_unit));
            }

            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

bool morse_queue_message(const char* msg, int millis_per_unit) {
    if (millis_per_unit <= 0) {
        millis_per_unit = DEFAULT_MILLIS_PER_UNIT;
    }

    static morse_msg tmp_msg{};
    if (strlen(msg) + 1 > tmp_msg.msg.size()) {
        return false;
    }

    strcpy(tmp_msg.msg.data(), msg);
    tmp_msg.millis_per_unit = millis_per_unit;

    printf("Trying to queue message \"%s\"\n", msg);

    return pdTRUE == xQueueSendToBack(s_queue, &tmp_msg, pdMS_TO_TICKS(500));
}

static int morse_queue_message_args(int argc, char** argv) {
    if (argc != 2) {
        return 1;
    } else if (!morse_queue_message(argv[1])) {
        return 1;
    } else {
        return 0;
    }
}

static struct {
    struct arg_str* message;
    struct arg_end* end;
} morse_args;

void register_morse() {
    morse_args.message = arg_str1(nullptr, nullptr, "<message>", "the message to print out");
    morse_args.end = arg_end(2);

    const esp_console_cmd_t message_cmd = {.command = "morse_msg",
                                           .help = "Prints a morse message over the GPIO 2",
                                           .hint = nullptr,
                                           .func = &morse_queue_message_args,
                                           .argtable = &morse_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&message_cmd));
}
