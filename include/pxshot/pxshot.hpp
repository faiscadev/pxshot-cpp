// Pxshot C++ SDK
// Official SDK for the Pxshot Screenshot API
// https://pxshot.com

#ifndef PXSHOT_HPP
#define PXSHOT_HPP

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <stdexcept>
#include <cstdint>

namespace pxshot {

// =============================================================================
// Version
// =============================================================================

constexpr const char* VERSION = "1.0.0";

// =============================================================================
// Exceptions
// =============================================================================

/// Base exception for all Pxshot errors
class Error : public std::runtime_error {
public:
    explicit Error(const std::string& message) : std::runtime_error(message) {}
    explicit Error(const char* message) : std::runtime_error(message) {}
};

/// HTTP/network related errors
class HttpError : public Error {
public:
    int status_code;
    
    HttpError(int code, const std::string& message)
        : Error(message), status_code(code) {}
};

/// API returned an error response
class ApiError : public Error {
public:
    std::string error_code;
    
    ApiError(const std::string& code, const std::string& message)
        : Error(message), error_code(code) {}
};

/// Invalid parameters
class ValidationError : public Error {
public:
    explicit ValidationError(const std::string& message) : Error(message) {}
};

// =============================================================================
// Types
// =============================================================================

/// Image format for screenshots
enum class Format {
    PNG,
    JPEG,
    WEBP
};

/// When to consider navigation complete
enum class WaitUntil {
    Load,           // window.onload event
    DOMContentLoaded,
    NetworkIdle,    // no network activity for 500ms
    Commit          // first network response
};

/// Screenshot request options
struct ScreenshotOptions {
    std::string url;                                    // Required: URL to capture
    std::optional<Format> format;                       // Image format (default: PNG)
    std::optional<int> quality;                         // JPEG/WEBP quality 0-100
    std::optional<int> width;                           // Viewport width
    std::optional<int> height;                          // Viewport height
    std::optional<bool> full_page;                      // Capture full scrollable page
    std::optional<WaitUntil> wait_until;                // Navigation wait condition
    std::optional<std::string> wait_for_selector;       // Wait for CSS selector
    std::optional<int> wait_for_timeout;                // Additional wait in ms
    std::optional<double> device_scale_factor;          // Device pixel ratio
    std::optional<bool> store;                          // Store and return URL
    std::optional<bool> block_ads;                      // Block ads and trackers
};

/// Result when store=true
struct StoredScreenshot {
    std::string url;            // URL to the stored screenshot
    std::string expires_at;     // ISO 8601 expiration timestamp
    int width;                  // Screenshot width in pixels
    int height;                 // Screenshot height in pixels
    int64_t size_bytes;         // File size in bytes
};

/// Screenshot result (either bytes or stored URL)
class ScreenshotResult {
public:
    /// Check if result contains stored screenshot (URL)
    [[nodiscard]] bool is_stored() const noexcept { return stored_.has_value(); }
    
    /// Check if result contains raw image bytes
    [[nodiscard]] bool is_bytes() const noexcept { return !bytes_.empty(); }
    
    /// Get stored screenshot info (throws if not stored)
    [[nodiscard]] const StoredScreenshot& stored() const {
        if (!stored_) {
            throw Error("Screenshot was not stored - use bytes() instead");
        }
        return *stored_;
    }
    
    /// Get raw image bytes (throws if stored)
    [[nodiscard]] const std::vector<uint8_t>& bytes() const {
        if (stored_) {
            throw Error("Screenshot was stored - use stored() instead");
        }
        return bytes_;
    }
    
    /// Get raw bytes, moving them out
    [[nodiscard]] std::vector<uint8_t> take_bytes() {
        if (stored_) {
            throw Error("Screenshot was stored - use stored() instead");
        }
        return std::move(bytes_);
    }
    
    // Convenience accessors for stored screenshots
    [[nodiscard]] const std::string& url() const { return stored().url; }
    [[nodiscard]] const std::string& expires_at() const { return stored().expires_at; }
    [[nodiscard]] int width() const { return stored().width; }
    [[nodiscard]] int height() const { return stored().height; }
    [[nodiscard]] int64_t size_bytes() const { return stored().size_bytes; }

private:
    friend class Client;
    
    std::vector<uint8_t> bytes_;
    std::optional<StoredScreenshot> stored_;
    
    explicit ScreenshotResult(std::vector<uint8_t> data) : bytes_(std::move(data)) {}
    explicit ScreenshotResult(StoredScreenshot info) : stored_(std::move(info)) {}
};

/// API usage statistics
struct Usage {
    int screenshots_taken;      // Total screenshots this period
    int screenshots_limit;      // Maximum allowed screenshots
    int storage_bytes_used;     // Storage used in bytes
    int storage_bytes_limit;    // Storage limit in bytes
    std::string period_start;   // ISO 8601 period start
    std::string period_end;     // ISO 8601 period end
};

// =============================================================================
// Client Configuration
// =============================================================================

struct ClientConfig {
    std::string api_key;                                // Required: API key
    std::string base_url = "https://api.pxshot.com";    // API base URL
    int timeout_seconds = 60;                           // Request timeout
    std::optional<std::string> user_agent;              // Custom User-Agent
};

// =============================================================================
// Client
// =============================================================================

/// Pxshot API client
class Client {
public:
    /// Construct client with API key
    explicit Client(std::string_view api_key);
    
    /// Construct client with full configuration
    explicit Client(ClientConfig config);
    
    /// Destructor
    ~Client();
    
    // Non-copyable, movable
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(Client&&) noexcept;
    Client& operator=(Client&&) noexcept;
    
    /// Capture a screenshot
    /// @param options Screenshot configuration
    /// @return ScreenshotResult containing either bytes or stored URL info
    /// @throws HttpError on network/HTTP errors
    /// @throws ApiError on API errors
    /// @throws ValidationError on invalid parameters
    [[nodiscard]] ScreenshotResult screenshot(const ScreenshotOptions& options);
    
    /// Get current usage statistics
    /// @return Usage information for current billing period
    /// @throws HttpError on network/HTTP errors
    /// @throws ApiError on API errors
    [[nodiscard]] Usage usage();
    
    /// Get the configured API base URL
    [[nodiscard]] std::string_view base_url() const noexcept;
    
    /// Get SDK version
    [[nodiscard]] static constexpr const char* version() noexcept { return VERSION; }

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// =============================================================================
// Utility Functions
// =============================================================================

/// Convert Format enum to string
[[nodiscard]] inline const char* to_string(Format f) noexcept {
    switch (f) {
        case Format::PNG: return "png";
        case Format::JPEG: return "jpeg";
        case Format::WEBP: return "webp";
    }
    return "png";
}

/// Convert WaitUntil enum to string
[[nodiscard]] inline const char* to_string(WaitUntil w) noexcept {
    switch (w) {
        case WaitUntil::Load: return "load";
        case WaitUntil::DOMContentLoaded: return "domcontentloaded";
        case WaitUntil::NetworkIdle: return "networkidle";
        case WaitUntil::Commit: return "commit";
    }
    return "load";
}

} // namespace pxshot

#endif // PXSHOT_HPP
