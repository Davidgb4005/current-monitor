#include "NetworkManager.hpp"

#include <cstring>
#include "esp_check.h"
#include "NetworkConfig.hpp"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

namespace network {
namespace {

const char* kLogTag = "NetworkManager";

void CopyWifiString(uint8_t* destination, size_t destination_size, const char* source) {
    if (source == nullptr || destination_size == 0) {
        return;
    }
    strncpy(reinterpret_cast<char*>(destination), source, destination_size);
    destination[destination_size - 1] = '\0';
}

}  // namespace

esp_err_t NetworkManager::InitializeNvs() {
    esp_err_t result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        result = nvs_flash_init();
    }
    return result;
}

esp_err_t NetworkManager::Init() {
    if (initialized_) {
        return ESP_OK;
    }

    esp_err_t result = InitializeNvs();
    if (result != ESP_OK) {
        return result;
    }

    result = esp_netif_init();
    if (result != ESP_OK && result != ESP_ERR_INVALID_STATE) {
        return result;
    }

    result = esp_event_loop_create_default();
    if (result != ESP_OK && result != ESP_ERR_INVALID_STATE) {
        return result;
    }

    initialized_ = true;
    return ESP_OK;
}

esp_err_t NetworkManager::InitializeWifiDriver() {
    if (wifi_driver_initialized_) {
        return ESP_OK;
    }

    wifi_init_config_t wifi_init = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t result = esp_wifi_init(&wifi_init);
    if (result == ESP_ERR_INVALID_STATE) {
        wifi_driver_initialized_ = true;
        return ESP_OK;
    }
    if (result != ESP_OK) {
        return result;
    }

    wifi_driver_initialized_ = true;
    return ESP_OK;
}

esp_err_t NetworkManager::Start(NetworkMode mode) {
    config_.startup_mode = mode;
    return Start(config_);
}

esp_err_t NetworkManager::Start(const NetworkConfig& config) {
    if (has_active_mode_ && config.startup_mode == current_mode_) {
        config_ = config;
        return ESP_OK;
    }

    config_ = config;
    if (config.startup_mode == NetworkMode::AccessPoint) {
        return StartAccessPoint(config.access_point);
    }
    return StartStation(config.station);
}

esp_err_t NetworkManager::StartAccessPoint(const AccessPointConfig& config) {
    ESP_RETURN_ON_ERROR(Init(), kLogTag, "Failed to initialize network stack");
    ESP_RETURN_ON_ERROR(InitializeWifiDriver(), kLogTag, "Failed to initialize Wi-Fi driver");

    if (wifi_started_) {
        ESP_RETURN_ON_ERROR(Stop(), kLogTag, "Failed to stop active Wi-Fi mode");
    }

    if (!ap_netif_created_) {
        esp_netif_create_default_wifi_ap();
        ap_netif_created_ = true;
    }

    wifi_config_t wifi_config = {};
    wifi_config.ap.ssid_len = strlen(config.ssid);
    wifi_config.ap.max_connection = config.max_connections;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    CopyWifiString(wifi_config.ap.ssid, sizeof(wifi_config.ap.ssid), config.ssid);
    CopyWifiString(wifi_config.ap.password, sizeof(wifi_config.ap.password), config.password);

    if (strlen(config.password) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_AP), kLogTag, "Failed to set AP mode");
    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_AP, &wifi_config), kLogTag, "Failed to configure AP");
    ESP_RETURN_ON_ERROR(esp_wifi_start(), kLogTag, "Failed to start AP");

    wifi_started_ = true;
    has_active_mode_ = true;
    current_mode_ = NetworkMode::AccessPoint;
    ESP_LOGI(kLogTag, "AP started. SSID:%s password:%s", config.ssid, config.password);
    return ESP_OK;
}

esp_err_t NetworkManager::StartStation(const StationConfig& config) {
    ESP_RETURN_ON_ERROR(Init(), kLogTag, "Failed to initialize network stack");
    ESP_RETURN_ON_ERROR(InitializeWifiDriver(), kLogTag, "Failed to initialize Wi-Fi driver");

    if (wifi_started_) {
        ESP_RETURN_ON_ERROR(Stop(), kLogTag, "Failed to stop active Wi-Fi mode");
    }

    if (!sta_netif_created_) {
        esp_netif_create_default_wifi_sta();
        sta_netif_created_ = true;
    }

    wifi_config_t wifi_config = {};
    CopyWifiString(wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), config.ssid);
    CopyWifiString(wifi_config.sta.password, sizeof(wifi_config.sta.password), config.password);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), kLogTag, "Failed to set station mode");
    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_STA, &wifi_config), kLogTag, "Failed to configure station mode");
    ESP_RETURN_ON_ERROR(esp_wifi_start(), kLogTag, "Failed to start station mode");
    ESP_RETURN_ON_ERROR(esp_wifi_connect(), kLogTag, "Failed to begin station connect");

    wifi_started_ = true;
    has_active_mode_ = true;
    current_mode_ = NetworkMode::Station;
    ESP_LOGI(kLogTag, "Station mode started. SSID:%s", config.ssid);
    return ESP_OK;
}

esp_err_t NetworkManager::Stop() {
    if (!wifi_started_) {
        return ESP_OK;
    }

    esp_err_t result = esp_wifi_stop();
    if (result == ESP_ERR_WIFI_NOT_INIT || result == ESP_ERR_WIFI_NOT_STARTED) {
        wifi_started_ = false;
        return ESP_OK;
    }
    if (result != ESP_OK) {
        return result;
    }

    wifi_started_ = false;
    return ESP_OK;
}

NetworkMode NetworkManager::CurrentMode() const {
    return current_mode_;
}

}  // namespace network
