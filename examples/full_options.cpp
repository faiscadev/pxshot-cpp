/// Full Options Example
/// Demonstrates all available screenshot options

#include <pxshot/pxshot.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>

int main() {
    const char* api_key = std::getenv("PXSHOT_API_KEY");
    if (!api_key) {
        std::cerr << "Error: PXSHOT_API_KEY environment variable not set\n";
        return 1;
    }
    
    try {
        // Create client with custom configuration
        pxshot::Client client(pxshot::ClientConfig{
            .api_key = api_key,
            .timeout_seconds = 120,
            .user_agent = "MyApp/1.0"
        });
        
        std::cout << "Taking full-page screenshot with all options...\n";
        
        // Take screenshot with all options
        auto result = client.screenshot({
            .url = "https://news.ycombinator.com",
            .format = pxshot::Format::JPEG,
            .quality = 85,
            .width = 1920,
            .height = 1080,
            .full_page = true,
            .wait_until = pxshot::WaitUntil::NetworkIdle,
            .wait_for_timeout = 1000,
            .device_scale_factor = 2.0
        });
        
        // Save to file
        const auto& bytes = result.bytes();
        std::ofstream file("full_page.jpg", std::ios::binary);
        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        
        std::cout << "Full-page screenshot saved to full_page.jpg (" 
                  << bytes.size() << " bytes)\n";
        
    } catch (const pxshot::Error& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
