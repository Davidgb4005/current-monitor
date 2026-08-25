#pragma once

#include <cstdint>

namespace network {

enum class NetworkMode : uint8_t {
    AccessPoint,
    Station,
};

struct AccessPointConfig {
    const char* ssid;
    const char* password;
    uint8_t max_connections;
};

struct StationConfig {
    const char* ssid;
    const char* password;
};

struct NetworkConfig {
    NetworkMode startup_mode;
    AccessPointConfig access_point;
    StationConfig station;
};

}  // namespace network
