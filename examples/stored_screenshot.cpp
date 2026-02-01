/// Stored Screenshot Example
/// Capture a screenshot and get a hosted URL

#include <pxshot/pxshot.hpp>
#include <iostream>
#include <cstdlib>

int main() {
    const char* api_key = std::getenv("PXSHOT_API_KEY");
    if (!api_key) {
        std::cerr << "Error: PXSHOT_API_KEY environment variable not set\n";
        return 1;
    }
    
    try {
        pxshot::Client client(api_key);
        
        std::cout << "Taking screenshot with storage...\n";
        
        // Take screenshot with storage enabled
        auto result = client.screenshot({
            .url = "https://example.com",
            .store = true
        });
        
        // Access stored screenshot info
        std::cout << "Screenshot stored!\n";
        std::cout << "  URL:        " << result.url() << "\n";
        std::cout << "  Expires:    " << result.expires_at() << "\n";
        std::cout << "  Dimensions: " << result.width() << "x" << result.height() << "\n";
        std::cout << "  Size:       " << result.size_bytes() << " bytes\n";
        
    } catch (const pxshot::Error& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
