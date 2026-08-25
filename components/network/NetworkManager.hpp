#pragma once

#include "NetworkConfig.hpp"
#include "NetworkTypes.hpp"
#include "esp_err.h"

namespace network {

class NetworkManager {
public:
    NetworkManager() = default;

    esp_err_t Init();
    esp_err_t Start(NetworkMode mode);
    esp_err_t Start(const NetworkConfig& config);
    esp_err_t StartAccessPoint(const AccessPointConfig& config);
    esp_err_t StartStation(const StationConfig& config);
    esp_err_t Stop();
    network::NetworkMode CurrentMode() const;

private:
    esp_err_t InitializeNvs();
    esp_err_t InitializeWifiDriver();

    bool initialized_ = false;
    bool wifi_started_ = false;
    bool wifi_driver_initialized_ = false;
    bool ap_netif_created_ = false;
    bool sta_netif_created_ = false;
    bool has_active_mode_ = false;
    NetworkMode current_mode_ = kDefaultNetworkConfig.startup_mode;
    NetworkConfig config_ = kDefaultNetworkConfig;
};

}  // namespace network
