#include "esp_http_server.h"
#pragma once

esp_err_t CssGet(httpd_req_t *req);
esp_err_t AlternatorDataGet(httpd_req_t *req);
esp_err_t AlternatorHtmlGet(httpd_req_t *req);
esp_err_t AlternatorResetPost(httpd_req_t *req);


