#pragma once

void task_morse_pin_output(void* pvParameters);

void initialize_morse();

bool morse_queue_message(const char* msg, const int millis_per_unit = 0);

void register_morse();
