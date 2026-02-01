# Pxshot Examples

# Basic screenshot example
add_executable(basic_screenshot basic_screenshot.cpp)
target_link_libraries(basic_screenshot PRIVATE pxshot::pxshot)

# Stored screenshot example
add_executable(stored_screenshot stored_screenshot.cpp)
target_link_libraries(stored_screenshot PRIVATE pxshot::pxshot)

# Full options example
add_executable(full_options full_options.cpp)
target_link_libraries(full_options PRIVATE pxshot::pxshot)

# Usage example
add_executable(usage_example usage_example.cpp)
target_link_libraries(usage_example PRIVATE pxshot::pxshot)
