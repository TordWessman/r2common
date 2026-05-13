/**
 * r2common Example: HTTP Request with Result and Logging
 *
 * Demonstrates the three core r2common components:
 *
 *   - R2Logger:       Unified logging (Serial on Arduino, cout on desktop)
 *   - Result<T>:      Rust-like error handling for embedded systems
 *   - ESPNetwork:     HTTP client abstraction (GET/POST requests)
 *   - HttpRequest:    Request builder using NetworkFacade
 *
 * This example connects to WiFi, then periodically fetches a public
 * JSON API endpoint and logs the result.
 */

#include <Arduino.h>
#include <WiFi.h>

#include "R2Logger.h"
#include "Result.h"
#include "Network/ESPNetwork.h"
#include "Network/HttpRequest.h"

using namespace r2common;

// ---- Configuration --------------------------------------------------------

const char *WIFI_SSID     = "your-ssid";
const char *WIFI_PASSWORD = "your-password";

// A public API that returns JSON (no auth required)
#define API_URL "http://httpbin.org/get"

// ---- Demonstrating Result<T> ----------------------------------------------

// Example function that returns a Result to demonstrate the pattern.
Result<int> parseSensorValue(const char *raw) {
    if (raw == nullptr) {
        return Result<int>::Err(-1, "null input");
    }

    int value = atoi(raw);
    if (value < 0 || value > 4095) {
        return Result<int>::Err(-2, "value out of ADC range");
    }

    return Result<int>(value);
}

// ---- Global instances -----------------------------------------------------

ESPNetwork *network = nullptr;

// ---- Arduino lifecycle ----------------------------------------------------

void setup() {
    Serial.begin(9600);
    delay(1000);

    // Configure logging level
    R2Logger::getInstance().SetLogLevel(R2Logger::LOG_TYPE_DEBUG);
    R2Logger::m("r2common HTTP Request example starting...");

    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    R2Logger::m("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    R2Logger::m("WiFi connected, IP: ", WiFi.localIP().toString().c_str());

    // Create network client (no TLS certificate needed for plain HTTP)
    network = new ESPNetwork();

    // --- Demonstrate Result<T> pattern ---
    R2Logger::m("--- Result<T> demo ---");

    Result<int> good = parseSensorValue("2048");
    if (good.HasValue()) {
        R2Logger::m("Parsed sensor value: ", good.Value());
    }

    Result<int> bad = parseSensorValue("9999");
    if (!bad.HasValue()) {
        R2Logger::e("Parse error: ", bad.ErrorMessage());
        R2Logger::e("Error code: ", bad.ErrorCode());
    }

    R2Logger::m("--- End Result<T> demo ---");
}

void loop() {
    static unsigned long lastRequest = 0;

    // Make an HTTP request every 30 seconds
    if (millis() - lastRequest < 30000 && lastRequest != 0) {
        return;
    }
    lastRequest = millis();

    R2Logger::m("Making HTTP GET request...");

    // Use HttpRequest builder with the network facade
    HttpRequest request(network);
    request.SetMethod("GET");
    request.SetBody(nullptr);

    HttpResponse response = request.SendRequest(API_URL);

    if (response.Success()) {
        R2Logger::m("Response OK, length: ", (uint32_t)response.responseLength);
        if (response.GetBody() != nullptr) {
            // Print first 200 chars of the response
            char preview[201];
            strncpy(preview, response.GetBody(), 200);
            preview[200] = '\0';
            R2Logger::m("Body preview: ", preview);
        }
    } else {
        R2Logger::e("HTTP request failed, status: ", (int)response.status);
    }
}
