#include "http_requests.h"

#include <array>
#include <cstring>

#include "esp_log.h"

#include "morse.h"
#include "tsk_morse_output.h"

static std::array<char, 1024> response_buffer{};
static std::array<char, 1024> query_buffer{};
static std::array<char, 512> message_buffer{};
static std::array<char, 64> arg_buffer{};

static const char* html_index = "<!DOCTYPE html><html><head><title>Morse Key</title></head><body>"
                                "<h1>Morse Key</h1>"
                                "<form action=\"/api\" target=\"_blank\" method=\"get\">"
                                "<label>Message:</label></br>"
                                "<input type=\"text\" name=\"msg\" required minlength=\"1\" maxlength=\"256\"><br>"
                                "<label>Millis per Unit:</label><br>"
                                "<input type=\"text\" name=\"unit_millis\" required value=\"0\"><br>"
                                "<input type=\"submit\" value=\"Submit\">"
                                "</form></body></html>";

static esp_err_t decode_url_string(char* buffer, size_t buffer_len) {
    int msg_len = strlen(buffer);
    const int CODED_LEN = 3;

    if (buffer_len == 0 || msg_len < CODED_LEN) {
        return ESP_OK;
    }

    int i = 0;
    while (i < msg_len && i < static_cast<int>(buffer_len)) {
        int incr = 1;
        if (buffer[i] == '%') {
            buffer[i] = '\0';

            if (i + CODED_LEN <= msg_len) {
                int offset = 0;

                if (buffer[i + 1] == '2' && buffer[i + 2] == '0') {
                    buffer[i] = ' ';
                    offset = 1;
                } else {
                    incr = 0;
                }

                strcpy(buffer + i + offset, buffer + i + CODED_LEN);
                msg_len -= CODED_LEN - offset;
            }
        } else if (buffer[i] == '+') {
            buffer[i] = ' ';
        }
        i += incr;
    }

    return ESP_OK;
}

static const char* create_generic_response(const char* title, const char* msg) {
    sprintf(response_buffer.data(), "<!DOCTYPE html><html><head><title>%s</title></head><body>%s</body></html>", title, msg);
    return response_buffer.data();
}

static esp_err_t http_index_page(httpd_req_t* req) { return httpd_resp_send(req, html_index, HTTPD_RESP_USE_STRLEN); }

static esp_err_t http_queue_morse_msg(httpd_req_t* req) {
    printf("Received Queue Request");

    if (ESP_OK == httpd_req_get_url_query_str(req, query_buffer.data(), query_buffer.size()) &&
        ESP_OK == httpd_query_key_value(query_buffer.data(), "msg", message_buffer.data(), message_buffer.size()) &&
        ESP_OK == decode_url_string(message_buffer.data(), message_buffer.size())) {

        int millis_per_unit = 0;
        if (ESP_OK == httpd_query_key_value(query_buffer.data(), "unit_millis", arg_buffer.data(), arg_buffer.size())) {
            millis_per_unit = strtol(arg_buffer.data(), nullptr, 10);
        }

        const bool success = morse_queue_message(message_buffer.data(), millis_per_unit);

        if (success) {
            return httpd_resp_send(req, create_generic_response("Success", message_buffer.data()), HTTPD_RESP_USE_STRLEN);
        } else {
            return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                                       create_generic_response("Error", "Unable to add message to queue - refresh to try again"));
        }
    } else {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                   create_generic_response("Error", "Unable to get requested message from uri</body></html>"));
    }
}

static const httpd_uri_t uri_base = {.uri = "/", .method = HTTP_GET, .handler = http_index_page, .user_ctx = nullptr};
static const httpd_uri_t uri_morse = {.uri = "/api", .method = HTTP_GET, .handler = http_queue_morse_msg, .user_ctx = nullptr};

httpd_handle_t setup_server() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = nullptr;

    ESP_ERROR_CHECK(httpd_start(&server, &config));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &uri_base));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &uri_morse));

    return server;
}
