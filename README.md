# Pxshot C++ SDK

Official C++ SDK for the [Pxshot](https://pxshot.com) Screenshot API.

## Features

- 🚀 Modern C++17 with clean, idiomatic API
- 🔒 RAII resource management
- 📦 Header-only friendly (single include)
- ⚡ Fast HTTP via cpp-httplib
- 🛡️ Comprehensive error handling
- 📖 Fully documented

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+
- OpenSSL

## Installation

### CMake FetchContent (Recommended)

```cmake
include(FetchContent)

FetchContent_Declare(
    pxshot
    GIT_REPOSITORY https://github.com/pxshot/cpp-sdk.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(pxshot)

target_link_libraries(your_target PRIVATE pxshot::pxshot)
```

### Manual Build

```bash
git clone https://github.com/pxshot/cpp-sdk.git
cd cpp-sdk
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

Then in your CMakeLists.txt:

```cmake
find_package(pxshot REQUIRED)
target_link_libraries(your_target PRIVATE pxshot::pxshot)
```

## Quick Start

```cpp
#include <pxshot/pxshot.hpp>
#include <fstream>
#include <iostream>

int main() {
    // Create client with API key
    pxshot::Client client("px_your_api_key");
    
    // Take a screenshot (returns bytes)
    auto result = client.screenshot({
        .url = "https://example.com"
    });
    
    // Save to file
    const auto& bytes = result.bytes();
    std::ofstream file("screenshot.png", std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    
    std::cout << "Saved " << bytes.size() << " bytes\n";
    return 0;
}
```

## Usage

### Basic Screenshot

```cpp
pxshot::Client client("px_your_api_key");

// Simple screenshot
auto result = client.screenshot({.url = "https://example.com"});

// Access the raw bytes
const std::vector<uint8_t>& bytes = result.bytes();

// Or move them out
std::vector<uint8_t> data = result.take_bytes();
```

### Stored Screenshot (Get URL)

```cpp
auto result = client.screenshot({
    .url = "https://example.com",
    .store = true
});

// Access stored screenshot info
std::cout << "URL: " << result.url() << "\n";
std::cout << "Expires: " << result.expires_at() << "\n";
std::cout << "Size: " << result.width() << "x" << result.height() << "\n";
```

### Full Options

```cpp
auto result = client.screenshot({
    .url = "https://example.com",
    .format = pxshot::Format::JPEG,      // PNG, JPEG, or WEBP
    .quality = 85,                        // 0-100 for JPEG/WEBP
    .width = 1920,                        // Viewport width
    .height = 1080,                       // Viewport height
    .full_page = true,                    // Capture full scrollable page
    .wait_until = pxshot::WaitUntil::NetworkIdle,
    .wait_for_selector = ".content",      // Wait for element
    .wait_for_timeout = 2000,             // Additional wait (ms)
    .device_scale_factor = 2.0,           // Retina/HiDPI
    .store = false,                       // Get bytes vs URL
    .block_ads = true                     // Block ads
});
```

### Check Usage

```cpp
auto usage = client.usage();

std::cout << "Screenshots: " << usage.screenshots_taken 
          << "/" << usage.screenshots_limit << "\n";
std::cout << "Storage: " << usage.storage_bytes_used 
          << "/" << usage.storage_bytes_limit << " bytes\n";
```

### Custom Configuration

```cpp
pxshot::Client client(pxshot::ClientConfig{
    .api_key = "px_your_api_key",
    .base_url = "https://api.pxshot.com",  // Custom endpoint
    .timeout_seconds = 120,                 // Request timeout
    .user_agent = "MyApp/1.0"              // Custom User-Agent
});
```

## Error Handling

The SDK uses exceptions for error handling:

```cpp
try {
    auto result = client.screenshot({.url = "https://example.com"});
} catch (const pxshot::ValidationError& e) {
    // Invalid parameters (e.g., empty URL, quality out of range)
    std::cerr << "Validation error: " << e.what() << "\n";
} catch (const pxshot::ApiError& e) {
    // API returned an error
    std::cerr << "API error [" << e.error_code << "]: " << e.what() << "\n";
} catch (const pxshot::HttpError& e) {
    // Network/HTTP error
    std::cerr << "HTTP error (" << e.status_code << "): " << e.what() << "\n";
} catch (const pxshot::Error& e) {
    // Base class catches all pxshot errors
    std::cerr << "Error: " << e.what() << "\n";
}
```

## API Reference

### Types

#### `pxshot::Format`
- `PNG` - Lossless, best for graphics
- `JPEG` - Lossy, smaller files
- `WEBP` - Modern format, good compression

#### `pxshot::WaitUntil`
- `Load` - Wait for `window.onload`
- `DOMContentLoaded` - Wait for DOM ready
- `NetworkIdle` - Wait for network quiet
- `Commit` - Wait for first response

#### `pxshot::ScreenshotOptions`
| Field | Type | Description |
|-------|------|-------------|
| `url` | `string` | **Required.** URL to capture |
| `format` | `Format` | Image format (default: PNG) |
| `quality` | `int` | JPEG/WEBP quality 0-100 |
| `width` | `int` | Viewport width in pixels |
| `height` | `int` | Viewport height in pixels |
| `full_page` | `bool` | Capture entire scrollable page |
| `wait_until` | `WaitUntil` | Navigation completion condition |
| `wait_for_selector` | `string` | CSS selector to wait for |
| `wait_for_timeout` | `int` | Additional wait in milliseconds |
| `device_scale_factor` | `double` | Device pixel ratio |
| `block_ads` | `bool` | Block ads and trackers |
| `store` | `bool` | Return URL instead of bytes |

#### `pxshot::ScreenshotResult`
- `is_stored()` - Check if result is a stored URL
- `is_bytes()` - Check if result is raw bytes
- `bytes()` - Get raw image bytes (throws if stored)
- `take_bytes()` - Move bytes out
- `stored()` - Get `StoredScreenshot` info
- `url()`, `expires_at()`, `width()`, `height()`, `size_bytes()` - Convenience accessors

#### `pxshot::Usage`
| Field | Type | Description |
|-------|------|-------------|
| `screenshots_taken` | `int` | Screenshots used this period |
| `screenshots_limit` | `int` | Maximum allowed |
| `storage_bytes_used` | `int` | Storage used in bytes |
| `storage_bytes_limit` | `int` | Storage limit in bytes |
| `period_start` | `string` | ISO 8601 period start |
| `period_end` | `string` | ISO 8601 period end |

## Building Examples

```bash
mkdir build && cd build
cmake -DPXSHOT_BUILD_EXAMPLES=ON ..
cmake --build .

# Run examples (set API key first)
export PXSHOT_API_KEY="px_your_api_key"
./examples/basic_screenshot
./examples/stored_screenshot
./examples/full_options
./examples/usage_example
```

## License

MIT License - see [LICENSE](LICENSE) for details.

## Support

- 📧 Email: support@pxshot.com
- 📖 Docs: https://docs.pxshot.com
- 🐛 Issues: https://github.com/pxshot/cpp-sdk/issues
