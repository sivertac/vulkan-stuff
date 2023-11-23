#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

std::string getMemoryTypePropertiesString(VkMemoryPropertyFlags propertyFlags) {
    std::string result;

    if (propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        result += "Device Local | ";
    if (propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        result += "Host Visible | ";
    if (propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        result += "Host Coherent | ";
    if (propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
        result += "Host Cached | ";
    if (propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
        result += "Lazily Allocated | ";

    // Remove the trailing " | " if any
    if (!result.empty() && result.back() == ' ')
        result.erase(result.end() - 3, result.end());

    return result;
}

void printMemoryInfo(const VkPhysicalDeviceMemoryProperties& memoryProperties) {
    std::cout << "Memory Heaps: " << memoryProperties.memoryHeapCount << std::endl;
    for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; ++i) {
        const VkMemoryHeap& heap = memoryProperties.memoryHeaps[i];
        std::cout << "  Heap " << i << ": Size = " << heap.size / (1024 * 1024) << " MB, Flags = " << heap.flags << std::endl;
    }

    std::cout << "Memory Types: " << memoryProperties.memoryTypeCount << std::endl;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        const VkMemoryType& type = memoryProperties.memoryTypes[i];
        std::cout << "  Type " << i << ": Heap Index = " << type.heapIndex << ", Property Flags = "
                  << getMemoryTypePropertiesString(type.propertyFlags) << std::endl;
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
    appInfo.apiVersion = VK_API_VERSION_1_0;

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
    std::cout << "Number of Vulkan-compatible GPUs: " << deviceCount << std::endl;

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

        // GPU memory information
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);
        std::cout << "  GPU Memory Information:" << std::endl;
        printMemoryInfo(memoryProperties);

        std::cout << "----------------------------------------" << std::endl;
    }

    // Clean up
    vkDestroyInstance(instance, nullptr);

    return EXIT_SUCCESS; 
}
