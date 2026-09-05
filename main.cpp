#include <iostream>
#include <cstdlib>
#include <vulkan/vulkan.h>
#include <vector>

int main() {

    // Handle to the Vulkan instance.
    // VK_NULL_HANDLE means it is not initialized yet.
    VkInstance instance = VK_NULL_HANDLE;


    // ---------------------------------------------------------
    // 1. Application Information
    // ---------------------------------------------------------

    // Describes information about our application.
    // Vulkan uses this information internally and for debugging.
    VkApplicationInfo appInfo{};

    // Specifies that this structure is a VkApplicationInfo structure.
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    // Name of our application.
    appInfo.pApplicationName = "My Vulkan App";

    // Version of our application.
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    // Name of the game/graphics engine.
    // We are not using an engine, so we just specify "No Engine".
    appInfo.pEngineName = "No Engine";

    // Version of the engine.
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    // Vulkan API version that our application wants to use.
    appInfo.apiVersion = VK_API_VERSION_1_0;


    // ---------------------------------------------------------
    // 2. Create Vulkan Instance
    // ---------------------------------------------------------

    // Contains information required to create a Vulkan instance.
    VkInstanceCreateInfo createInfo{};

    // Specifies that this structure is a VkInstanceCreateInfo.
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    // Connect our application information to the instance creation info.
    createInfo.pApplicationInfo = &appInfo;

    // ---------------------------------------------------------
    // 10. Create Validation layer
    // ---------------------------------------------------------

    const char* validationLayer[] = {"VK_LAYER_KHRONOS_validation"};
    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = validationLayer;



    // Create the Vulkan instance.
    //
    // Parameters:
    // 1. createInfo  -> information about how to create the instance
    // 2. nullptr     -> no custom memory allocator
    // 3. &instance   -> Vulkan will store the created instance here
    //
    // vkCreateInstance returns VK_SUCCESS when creation succeeds.
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {

        std::cout << "Failed to create Vulkan instance." << std::endl;

        return EXIT_FAILURE;
    }


    // ---------------------------------------------------------
    // 3. Find Number of Physical Devices (GPUs)
    // ---------------------------------------------------------

    // This variable will contain the number of Vulkan-compatible
    // physical devices (GPUs) available on the system.
    uint32_t deviceCount = 0;


    // First call:
    // We don't provide an array of devices yet.
    // Vulkan only tells us how many physical devices are available.
    VkResult result = vkEnumeratePhysicalDevices(
        instance,       // Vulkan instance
        &deviceCount,   // Vulkan writes the number of devices here
        nullptr         // nullptr means: don't retrieve devices yet
    );


    // Check whether the function succeeded and whether
    // at least one Vulkan-compatible GPU was found.
    if (result != VK_SUCCESS || deviceCount == 0) {

        std::cout << "Failed to find Vulkan device." << std::endl;

        // Destroy the Vulkan instance before exiting.
        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }


    // ---------------------------------------------------------
    // 4. Get the Physical Devices
    // ---------------------------------------------------------

    // Now that we know how many GPUs exist,
    // create a vector large enough to store all of them.
    //
    // For example:
    // deviceCount = 2
    //
    // physicalDevices[0] -> GPU 0
    // physicalDevices[1] -> GPU 1
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);


    // Second call:
    // This time we provide an array where Vulkan can store
    // the physical device handles.
    result = vkEnumeratePhysicalDevices(
        instance,               // Vulkan instance
        &deviceCount,           // Number of devices
        physicalDevices.data()  // Array where devices are stored
    );


    // Check if retrieving the physical devices failed.
    if (result != VK_SUCCESS) {

        std::cout << "Failed to enumerate Vulkan devices." << std::endl;

        // Clean up the Vulkan instance.
        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }


    // ---------------------------------------------------------
    // 5. Select a Physical Device
    // ---------------------------------------------------------

    // For now, simply select the first GPU in the list.
    //
    // physicalDevices[0] is the first Vulkan-compatible GPU.
    // Later, we can add logic to choose the best GPU.
    VkPhysicalDevice physicalDevice = physicalDevices[0];


    // ---------------------------------------------------------
    // 6. Get Physical Device Properties
    // ---------------------------------------------------------

    // This structure will contain information about the GPU,
    // such as:
    //
    // - GPU name
    // - Vendor ID
    // - Device ID
    // - Driver version
    // - Supported Vulkan versions
    // - Various hardware limits
    VkPhysicalDeviceProperties deviceProperties{};


    // Ask Vulkan for the properties of our selected GPU.
    vkGetPhysicalDeviceProperties(
        physicalDevice,     // GPU we want information about
        &deviceProperties   // Vulkan writes the properties here
    );


    // Print the GPU name.
    std::cout << "Physical device: "
              << deviceProperties.deviceName
              << std::endl;

    // ---------------------------------------------------------
    //  7. Create Logical Device
    // ---------------------------------------------------------

    // We need at least one queue to create a logical device.
    // For simplicity, we just use the first available queue family.
    float queuePriorities = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount=1;
    queueCreateInfo.pQueuePriorities = &queuePriorities;

    //Describe the logical device we want to create
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;

    VkDevice device = VK_NULL_HANDLE;
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device)!= VK_SUCCESS) {
        std::cout << "Failed to create logical device." << std::endl;
        vkDestroyInstance(instance, nullptr);
        return EXIT_FAILURE;
    }

    // ---------------------------------------------------------
    // 8. Create and Destroy Vulkan Buffer
    // ---------------------------------------------------------

    // Define the buffer size (e.g., 1024 bytes)
    VkDeviceSize bufferSize = 1024;

    VkBufferCreateInfo bufferInfo={};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    if (vkCreateBuffer(device, &bufferInfo,nullptr,&vertexBuffer)!= VK_SUCCESS) {
        //handle buffer creation failure
        std::cout << "Failed to create vertex buffer." << std::endl;
    } else {
        std::cout << "Succesfully created vertex buffer of size: " << bufferSize << std::endl;
    }


    // Destroying a Vulkan buffer
    vkDestroyBuffer(device, vertexBuffer, nullptr);

    // ---------------------------------------------------------
    // 9. Clean up Device and Instance
    // ---------------------------------------------------------

    // Destroy logical device first
    vkDestroyDevice(device, nullptr);

    // Then destroy instance
    vkDestroyInstance(instance, nullptr);


    // Program finished successfully.
    return EXIT_SUCCESS;
}
