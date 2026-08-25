#include "AppController.hpp"

#include <cstring>

#include "Inputs.hpp"
#include "esp_log.h"

namespace {

const char* kLogTag = "AppController";

}  // namespace

AppController::AppController(LcdDisplay& lcd_display,
                             BatteryMonitorSensors& sensors,
                             network::NetworkManager& network_manager)
    : lcd_display_(lcd_display),
      sensors_(sensors),
      network_manager_(network_manager) {}

void AppController::Init() {
    ESP_ERROR_CHECK(network_manager_.Init());
    ESP_ERROR_CHECK(network_manager_.Start(active_mode_));
}

void AppController::Tick() {
    sensors_.Sample();
    HandleButtons();
    RenderCurrentPage();
}

void AppController::HandleButtons() {
    left_button.SetEdge();
    right_button.SetEdge();

    if ((left_button.Read() && right_button.ReadRisingEdge()) ||
        (right_button.Read() && left_button.ReadRisingEdge())) {
        lcd_display_.ToggleDisplay();
        return;
    }

    const uint8_t page = lcd_display_.GetPage();
    if (page == display::kNetworkPage) {
        if (left_button.ReadRisingEdge()) {
            pending_mode_ = pending_mode_ == network::NetworkMode::AccessPoint
                                ? network::NetworkMode::Station
                                : network::NetworkMode::AccessPoint;
        } else if (right_button.ReadRisingEdge()) {
            if (pending_mode_ == active_mode_ || network_manager_.Start(pending_mode_) == ESP_OK) {
                active_mode_ = pending_mode_;
            } else {
                ESP_LOGE(kLogTag, "Failed to apply network mode");
            }
            lcd_display_.NextPage();
        }
        return;
    }

    if (left_button.ReadRisingEdge()) {
        lcd_display_.PrevPage();
    } else if (right_button.ReadRisingEdge()) {
        lcd_display_.NextPage();
    }
}

void AppController::RenderCurrentPage() {
    const uint8_t page = lcd_display_.GetPage();
    switch (page) {
        case display::kAlternatorPage:
            RenderAlternatorPage(page);
            break;
        case display::kPassengerBatteryPage:
            RenderPassengerBatteryPage(page);
            break;
        case display::kDriverBatteryPage:
            RenderDriverBatteryPage(page);
            break;
        case display::kHeaterStatusPage:
            RenderHeaterStatusPage(page);
            break;
        case display::kPumpStatusPage:
            RenderPumpStatusPage(page);
            break;
        case display::kNetworkPage:
            RenderNetworkPage(page);
            break;
        default:
            break;
    }
}

void AppController::RenderAlternatorPage(uint8_t page) {
    const BatteryReadings& readings = sensors_.LatestReadings();
    lcd_display_.WriteLeftAlign(buffer_, display::EncodeFloat(buffer_, readings.alternator_voltage, 1), 2, 1, 6, page);
    lcd_display_.WriteLeftAlign(buffer_, display::EncodeFloat(buffer_, readings.alternator_current, 1), 1, 1, 6, page);
}

void AppController::RenderPassengerBatteryPage(uint8_t page) {
    const BatteryReadings& readings = sensors_.LatestReadings();
    lcd_display_.WriteLeftAlign(buffer_, display::EncodeFloat(buffer_, readings.passenger_voltage, 1), 1, 1, 6, page);
    lcd_display_.WriteLeftAlign(buffer_, display::EncodeFloat(buffer_, readings.passenger_current, 1), 2, 1, 6, page);
}

void AppController::RenderDriverBatteryPage(uint8_t page) {
    const BatteryReadings& readings = sensors_.LatestReadings();
    lcd_display_.WriteLeftAlign(buffer_, display::EncodeFloat(buffer_, readings.driver_voltage, 1), 1, 1, 6, page);
    lcd_display_.WriteLeftAlign(buffer_, display::EncodeFloat(buffer_, readings.driver_current, 1), 2, 1, 6, page);
}

void AppController::RenderHeaterStatusPage(uint8_t page) {
    if (!diesel_heater.Read()) {
        lcd_display_.WriteRightAlign("OFF", 3, 1, 19, 4, page);
    } else {
        lcd_display_.WriteRightAlign("ON", 2, 1, 19, 4, page);
    }

    if (!electric_heater.Read()) {
        lcd_display_.WriteRightAlign("OFF", 3, 2, 19, 4, page);
    } else {
        lcd_display_.WriteRightAlign("ON", 2, 2, 19, 4, page);
    }

    if (!engine_heater.Read()) {
        lcd_display_.WriteRightAlign("OFF", 3, 3, 19, 4, page);
    } else {
        lcd_display_.WriteRightAlign("ON", 2, 3, 19, 4, page);
    }
}

void AppController::RenderPumpStatusPage(uint8_t page) {
    if (!water_pump.Read()) {
        lcd_display_.WriteRightAlign("OFF", 3, 1, 19, 4, page);
    } else {
        lcd_display_.WriteRightAlign("ON", 2, 1, 19, 4, page);
    }

    if (!cabin_pump.Read()) {
        lcd_display_.WriteRightAlign("OFF", 3, 2, 19, 4, page);
    } else {
        lcd_display_.WriteRightAlign("ON", 2, 2, 19, 4, page);
    }
}

void AppController::RenderNetworkPage(uint8_t page) {
    lcd_display_.WriteLeftAlign(ModeLabel(active_mode_), static_cast<int8_t>(strlen(ModeLabel(active_mode_))), 1, 15, 5, page, false);
    lcd_display_.WriteLeftAlign(ModeLabel(pending_mode_), static_cast<int8_t>(strlen(ModeLabel(pending_mode_))), 2, 12, 8, page, false);
}

const char* AppController::ModeLabel(network::NetworkMode mode) const {
    return mode == network::NetworkMode::AccessPoint ? "AP" : "Station";
}
