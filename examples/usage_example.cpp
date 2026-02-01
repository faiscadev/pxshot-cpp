/// Usage Example
/// Check API usage statistics

#include <pxshot/pxshot.hpp>
#include <iostream>
#include <iomanip>
#include <cstdlib>

int main() {
    const char* api_key = std::getenv("PXSHOT_API_KEY");
    if (!api_key) {
        std::cerr << "Error: PXSHOT_API_KEY environment variable not set\n";
        return 1;
    }
    
    try {
        pxshot::Client client(api_key);
        
        // Get usage statistics
        auto usage = client.usage();
        
        std::cout << "=== Pxshot Usage Statistics ===\n\n";
        
        std::cout << "Screenshots:\n";
        std::cout << "  Used:  " << usage.screenshots_taken << "\n";
        std::cout << "  Limit: " << usage.screenshots_limit << "\n";
        std::cout << "  Remaining: " << (usage.screenshots_limit - usage.screenshots_taken) << "\n\n";
        
        std::cout << "Storage:\n";
        std::cout << "  Used:  " << std::fixed << std::setprecision(2) 
                  << (usage.storage_bytes_used / 1024.0 / 1024.0) << " MB\n";
        std::cout << "  Limit: " << std::fixed << std::setprecision(2)
                  << (usage.storage_bytes_limit / 1024.0 / 1024.0) << " MB\n\n";
        
        std::cout << "Billing Period:\n";
        std::cout << "  Start: " << usage.period_start << "\n";
        std::cout << "  End:   " << usage.period_end << "\n";
        
    } catch (const pxshot::Error& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
