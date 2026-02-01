// Pxshot C++ SDK - Implementation

#include "pxshot/pxshot.hpp"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <sstream>
#include <algorithm>

namespace pxshot {

using json = nlohmann::json;

// =============================================================================
// Implementation Details
// =============================================================================

struct Client::Impl {
    ClientConfig config;
    std::unique_ptr<httplib::Client> http;
    
    explicit Impl(ClientConfig cfg) : config(std::move(cfg)) {
        // Parse base URL to extract host and scheme
        std::string url = config.base_url;
        
        // Create HTTP client
        http = std::make_unique<httplib::Client>(url);
        http->set_connection_timeout(config.timeout_seconds);
        http->set_read_timeout(config.timeout_seconds);
        http->set_write_timeout(config.timeout_seconds);
        
        // Enable following redirects
        http->set_follow_location(true);
    }
    
    [[nodiscard]] httplib::Headers make_headers(bool json_content = true) const {
        httplib::Headers headers;
        headers.emplace("Authorization", "Bearer " + config.api_key);
        
        if (json_content) {
            headers.emplace("Content-Type", "application/json");
        }
        
        std::string ua = config.user_agent.value_or(
            std::string("pxshot-cpp/") + VERSION
        );
        headers.emplace("User-Agent", ua);
        
        return headers;
    }
    
    void check_response(const httplib::Result& res, const std::string& context) {
        if (!res) {
            throw HttpError(0, context + ": " + httplib::to_string(res.error()));
        }
        
        if (res->status >= 400) {
            // Try to parse error response
            try {
                auto body = json::parse(res->body);
                std::string code = body.value("code", "unknown");
                std::string message = body.value("message", res->body);
                throw ApiError(code, message);
            } catch (const json::exception&) {
                throw HttpError(res->status, context + ": HTTP " + std::to_string(res->status));
            }
        }
    }
};

// =============================================================================
// Client Implementation
// =============================================================================

Client::Client(std::string_view api_key) 
    : Client(ClientConfig{std::string(api_key)}) {}

Client::Client(ClientConfig config) {
    if (config.api_key.empty()) {
        throw ValidationError("API key is required");
    }
    impl_ = std::make_unique<Impl>(std::move(config));
}

Client::~Client() = default;

Client::Client(Client&&) noexcept = default;
Client& Client::operator=(Client&&) noexcept = default;

ScreenshotResult Client::screenshot(const ScreenshotOptions& options) {
    // Validate
    if (options.url.empty()) {
        throw ValidationError("URL is required");
    }
    
    if (options.quality && (*options.quality < 0 || *options.quality > 100)) {
        throw ValidationError("Quality must be between 0 and 100");
    }
    
    if (options.width && *options.width <= 0) {
        throw ValidationError("Width must be positive");
    }
    
    if (options.height && *options.height <= 0) {
        throw ValidationError("Height must be positive");
    }
    
    // Build request body
    json body;
    body["url"] = options.url;
    
    if (options.format) {
        body["format"] = to_string(*options.format);
    }
    if (options.quality) {
        body["quality"] = *options.quality;
    }
    if (options.width) {
        body["width"] = *options.width;
    }
    if (options.height) {
        body["height"] = *options.height;
    }
    if (options.full_page) {
        body["full_page"] = *options.full_page;
    }
    if (options.wait_until) {
        body["wait_until"] = to_string(*options.wait_until);
    }
    if (options.wait_for_selector) {
        body["wait_for_selector"] = *options.wait_for_selector;
    }
    if (options.wait_for_timeout) {
        body["wait_for_timeout"] = *options.wait_for_timeout;
    }
    if (options.device_scale_factor) {
        body["device_scale_factor"] = *options.device_scale_factor;
    }
    if (options.store) {
        body["store"] = *options.store;
    }
    if (options.block_ads) {
        body["block_ads"] = *options.block_ads;
    }
    
    // Make request
    auto res = impl_->http->Post(
        "/v1/screenshot",
        impl_->make_headers(),
        body.dump(),
        "application/json"
    );
    
    impl_->check_response(res, "Screenshot request failed");
    
    // Check if response is JSON (stored) or binary (image bytes)
    bool store_mode = options.store.value_or(false);
    
    // Also check content-type header
    auto content_type = res->get_header_value("Content-Type");
    bool is_json = content_type.find("application/json") != std::string::npos;
    
    if (store_mode || is_json) {
        try {
            auto response = json::parse(res->body);
            
            StoredScreenshot stored;
            stored.url = response.at("url").get<std::string>();
            stored.expires_at = response.at("expires_at").get<std::string>();
            stored.width = response.at("width").get<int>();
            stored.height = response.at("height").get<int>();
            stored.size_bytes = response.at("size_bytes").get<int64_t>();
            
            return ScreenshotResult(std::move(stored));
        } catch (const json::exception& e) {
            throw Error(std::string("Failed to parse stored screenshot response: ") + e.what());
        }
    } else {
        // Binary image data
        std::vector<uint8_t> bytes(res->body.begin(), res->body.end());
        return ScreenshotResult(std::move(bytes));
    }
}

Usage Client::usage() {
    auto res = impl_->http->Get("/v1/usage", impl_->make_headers(false));
    
    impl_->check_response(res, "Usage request failed");
    
    try {
        auto response = json::parse(res->body);
        
        Usage usage;
        usage.screenshots_taken = response.at("screenshots_taken").get<int>();
        usage.screenshots_limit = response.at("screenshots_limit").get<int>();
        usage.storage_bytes_used = response.at("storage_bytes_used").get<int>();
        usage.storage_bytes_limit = response.at("storage_bytes_limit").get<int>();
        usage.period_start = response.at("period_start").get<std::string>();
        usage.period_end = response.at("period_end").get<std::string>();
        
        return usage;
    } catch (const json::exception& e) {
        throw Error(std::string("Failed to parse usage response: ") + e.what());
    }
}

std::string_view Client::base_url() const noexcept {
    return impl_->config.base_url;
}

} // namespace pxshot
