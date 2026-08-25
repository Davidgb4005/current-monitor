#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_event.h"
#include "esp_spiffs.h"
#include <cstring> 

esp_err_t CssGet(httpd_req_t *req)
{
    FILE* f = fopen("/spiffs/style.css", "r");
    if (!f) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char buf[256];
    size_t read_bytes;
    httpd_resp_set_type(req, "text/css");

    while ((read_bytes = fread(buf, 1, sizeof(buf), f)) > 0) {
        httpd_resp_send_chunk(req, buf, read_bytes);
    }
    fclose(f);
    httpd_resp_send_chunk(req, NULL, 0); // signal end
    return ESP_OK;
}

esp_err_t AlternatorDataGet(httpd_req_t *req)
{
    const char *resp_str = static_cast<const char *>(*(static_cast<const char **>(req->user_ctx)));
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t AlternatorHtmlGet(httpd_req_t *req)
{
    FILE* f = fopen("/spiffs/alternator.html", "r");
    if (!f) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char buf[256];
    size_t read_bytes;
    httpd_resp_set_type(req, "text/html");

    while ((read_bytes = fread(buf, 1, sizeof(buf), f)) > 0) {
        httpd_resp_send_chunk(req, buf, read_bytes);
    }
    fclose(f);
    httpd_resp_send_chunk(req, NULL, 0); // signal end
    return ESP_OK;
}

esp_err_t AlternatorResetPost(httpd_req_t *req) {
    char buf[256];
    int ret;

    // Read the body of the request
    if ((ret = httpd_req_recv(req, buf, req->content_len)) <= 0) {
        return ESP_FAIL;
    }

    buf[ret] = '\0';  // Null-terminate for safe use as a string
    **(static_cast<char **>(req->user_ctx)) = buf[0];
    printf("Received: %s\n", buf);

    // Respond back
    const char* resp = "{\"status\":\"ok\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}