/// Basic Screenshot Example
/// Capture a screenshot and save it to a file

#include <pxshot/pxshot.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>

int main() {
    // Get API key from environment
    const char* api_key = std::getenv("PXSHOT_API_KEY");
    if (!api_key) {
        std::cerr << "Error: PXSHOT_API_KEY environment variable not set\n";
        return 1;
    }
    
    try {
        // Create client
        pxshot::Client client(api_key);
        
        std::cout << "Taking screenshot of https://example.com...\n";
        
        // Take a basic screenshot
        auto result = client.screenshot({
            .url = "https://example.com"
        });
        
        // Save to file
        const auto& bytes = result.bytes();
        std::ofstream file("screenshot.png", std::ios::binary);
        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        
        std::cout << "Screenshot saved to screenshot.png (" 
                  << bytes.size() << " bytes)\n";
        
    } catch (const pxshot::ApiError& e) {
        std::cerr << "API Error [" << e.error_code << "]: " << e.what() << "\n";
        return 1;
    } catch (const pxshot::HttpError& e) {
        std::cerr << "HTTP Error (" << e.status_code << "): " << e.what() << "\n";
        return 1;
    } catch (const pxshot::Error& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
