#include "morse.h"

#include <cstring>
#include <cctype>

const int DEFAULT_MILLIS_PER_UNIT = 50;

static const char* morse_str_for_char(const char c) {
    switch (tolower(c)) {
    case 'a':
        return ".-";
    case 'b':
        return "-...";
    case 'c':
        return "-.-.";
    case 'd':
        return "-..";
    case 'e':
        return ".";
    case 'f':
        return "..-.";
    case 'g':
        return "--.";
    case 'h':
        return "....";
    case 'i':
        return "..";
    case 'j':
        return ".---";
    case 'k':
        return "-.-";
    case 'l':
        return ".-..";
    case 'm':
        return "--";
    case 'n':
        return "-.";
    case 'o':
        return "---";
    case 'p':
        return ".--.";
    case 'q':
        return "--.-";
    case 'r':
        return ".-.";
    case 's':
        return "...";
    case 't':
        return "-";
    case 'u':
        return "..-";
    case 'v':
        return "...-";
    case 'w':
        return ".--";
    case 'x':
        return "-..-";
    case 'y':
        return "-.--";
    case 'z':
        return "--..";
    case '1':
        return ".----";
    case '2':
        return "..---";
    case '3':
        return "...--";
    case '4':
        return "....-";
    case '5':
        return ".....";
    case '6':
        return "-....";
    case '7':
        return "--...";
    case '8':
        return "---..";
    case '9':
        return "----.";
    case '0':
        return "-----";
    default:
        return " "; // Return space by default
    }
}
static const MorseCommand CMD_DOT(1, 1);
static const MorseCommand CMD_DASH(3, 1);
static const MorseCommand CMD_SPACE(0, 7);
static const MorseCommand CMD_END_OF_CHAR(0, 3);

static void add_to_buffer(const size_t i, const MorseCommand* val, const MorseCommand** const buffer, const size_t buffer_len) {
    if (i < buffer_len) {
        buffer[i] = val;
    }
}

size_t construct_morse_commands(const char* msg, const MorseCommand** const buffer, const size_t buffer_len) {
    const size_t msg_len = strlen(msg);
    size_t current_index = 0;

    for (int i = 0; i < msg_len; ++i) {
        const char c = msg[i];

        const char* cmd = morse_str_for_char(c);
        const size_t cmd_len = strlen(cmd);

        for (int j = 0; j < cmd_len; ++j) {
            if (current_index >= buffer_len) {
                break;
            }

            const MorseCommand* val = nullptr;

            if (cmd[j] == '.') {
                val = &CMD_DOT;
            } else if (cmd[j] == '-') {
                val = &CMD_DASH;
            } else if (cmd[j] == ' ') {
                val = &CMD_SPACE;

                // Prevent duplicate spaces
                if (current_index > 0 && buffer[current_index - 1] == &CMD_SPACE) {
                    val = nullptr;
                }
            }

            if (val != nullptr) {
                add_to_buffer(current_index++, val, buffer, buffer_len);
            }
        }

        add_to_buffer(current_index++, &CMD_END_OF_CHAR, buffer, buffer_len);
    }

    return current_index;
}
