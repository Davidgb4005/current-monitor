#include "esp_http_server.h"
#include "UrlHandlerFunctions.hpp"
#include "UrlHandles.hpp"

const char * sent_msg_ptr = nullptr;
char recv_msg_buffer[256] = {'r'};
char * recv_msg_ptr = recv_msg_buffer;
httpd_uri_t css = {
    css.uri = "/style.css",
    css.method = HTTP_GET,
    css.handler = CssGet,
    css.user_ctx = NULL};

httpd_uri_t alternatorHtml = {
    alternatorHtml.uri = "/alternator_html",
    alternatorHtml.method = HTTP_GET,
    alternatorHtml.handler = AlternatorHtmlGet,
    alternatorHtml.user_ctx = NULL};

httpd_uri_t alternatorData = {
    alternatorData.uri = "/alternator_data",
    alternatorData.method = HTTP_GET,
    alternatorData.handler = AlternatorDataGet,
    alternatorData.user_ctx = const_cast<void*>(reinterpret_cast<const void*>(&sent_msg_ptr))};

httpd_uri_t alternatorReset = {
    alternatorReset.uri = "/alternator_reset",
    alternatorReset.method = HTTP_POST,
    alternatorReset.handler = AlternatorResetPost,
    alternatorReset.user_ctx = static_cast<void*>(&recv_msg_ptr)};

