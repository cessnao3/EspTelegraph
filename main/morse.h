#pragma once

#include <cstddef>

struct MorseCommand {
    MorseCommand(const int units_enabled, const int units_disabled) : units_enabled(units_enabled), units_disabled(units_disabled) {}

    int units_enabled;
    int units_disabled;
};

extern const int DEFAULT_MILLIS_PER_UNIT;

size_t construct_morse_commands(const char* msg, const MorseCommand** const morse_buffer, const size_t buffer_len);
