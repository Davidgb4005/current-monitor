#pragma once

#include <cstdint>

#include "NetworkTypes.hpp"

namespace network {

constexpr char kAccessPointSsid[] = "ESP32_AP";
constexpr char kAccessPointPassword[] = "12345678";
constexpr uint8_t kMaxStationConnections = 4;

constexpr char kStationSsid[] = "YOUR_WIFI_SSID";
constexpr char kStationPassword[] = "YOUR_WIFI_PASSWORD";

constexpr NetworkConfig kDefaultNetworkConfig = {
    NetworkMode::AccessPoint,
    {kAccessPointSsid, kAccessPointPassword, kMaxStationConnections},
    {kStationSsid, kStationPassword},
};

}  // namespace network
