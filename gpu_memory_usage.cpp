#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>
#include <cstring>

void printMemoryBudget(VkPhysicalDevice device) {
    VkPhysicalDeviceMemoryBudgetPropertiesEXT memoryBudgetProperties = {};
    memoryBudgetProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;

    VkPhysicalDeviceMemoryProperties2 deviceMemoryProperties2 = {};
    deviceMemoryProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    deviceMemoryProperties2.pNext = &memoryBudgetProperties;

    vkGetPhysicalDeviceMemoryProperties2(device, &deviceMemoryProperties2);

    // get number of heaps
    uint32_t heapCount = deviceMemoryProperties2.memoryProperties.memoryHeapCount;

    std::cout << "Memory Budget Information:" << std::endl;
    for (uint32_t i = 0; i < heapCount; ++i) {
        std::cout << "Heap " << i << ": " << std::endl;
        std::cout << "  Budget: " << memoryBudgetProperties.heapBudget[i] / (1024 * 1024) << " MB" << std::endl;
        std::cout << "  Usage: " << memoryBudgetProperties.heapUsage[i] / (1024 * 1024) << " MB" << std::endl;
    }
}

int main() {
    // Initialize Vulkan
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan GPU Enumeration";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance!" << std::endl;
        return EXIT_FAILURE;
    }

    // Enumerate physical devices (GPUs)
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        std::cerr << "No Vulkan-compatible GPUs found!" << std::endl;
        vkDestroyInstance(instance, nullptr);
        return EXIT_FAILURE;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Print information about each GPU
    std::cout << "Number of Vulkan-compatible GPUs found: " << deviceCount << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (const auto& device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        std::cout << "GPU Name: " << deviceProperties.deviceName << std::endl;
        std::cout << "  API Version: " << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
                  << VK_VERSION_MINOR(deviceProperties.apiVersion) << "."
                  << VK_VERSION_PATCH(deviceProperties.apiVersion) << std::endl;
        std::cout << "  Driver Version: " << deviceProperties.driverVersion << std::endl;
        std::cout << "  Vendor ID: " << deviceProperties.vendorID << std::endl;
        std::cout << "  Device ID: " << deviceProperties.deviceID << std::endl;

        // Check for memory budget extension
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        bool hasMemoryBudget = false;
        for (const auto& extension : availableExtensions) {
            if (strcmp(extension.extensionName, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME) == 0) {
                hasMemoryBudget = true;
                break;
            }
        }

        if (!hasMemoryBudget) {
            std::cout << "  Memory budget extension not supported!" << std::endl;
            continue;
        }

        // GPU memory budget information
        printMemoryBudget(device);

        std::cout << "----------------------------------------" << std::endl;
    }

    // Clean up
    vkDestroyInstance(instance, nullptr);

    return EXIT_SUCCESS;
}
