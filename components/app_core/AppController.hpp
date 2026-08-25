#pragma once

#include "BatteryMonitorSensors.hpp"
#include "DisplayFormatter.hpp"
#include "DisplayPages.hpp"
#include "LcdDisplay.hpp"
#include "NetworkManager.hpp"
#include "NetworkTypes.hpp"

class AppController {
public:
    AppController(LcdDisplay& lcd_display,
                  BatteryMonitorSensors& sensors,
                  network::NetworkManager& network_manager);

    void Init();
    void Tick();

private:
    void HandleButtons();
    void RenderCurrentPage();
    void RenderAlternatorPage(uint8_t page);
    void RenderBatteryPage(uint8_t page);
    void RenderHeaterStatusPage(uint8_t page);
    void RenderPumpStatusPage(uint8_t page);
    void RenderNetworkPage(uint8_t page);
    const char* ModeLabel(network::NetworkMode mode) const;

    LcdDisplay& lcd_display_;
    BatteryMonitorSensors& sensors_;
    network::NetworkManager& network_manager_;
    network::NetworkMode active_mode_ = network::kDefaultNetworkConfig.startup_mode;
    network::NetworkMode pending_mode_ = network::kDefaultNetworkConfig.startup_mode;
    char buffer_[20] = {0};
};
