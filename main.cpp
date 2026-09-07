#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstring>

#include <vulkan/vulkan.h>

int main()
{
    // =========================================================
    // 1. Vulkan Instance
    // =========================================================

    // Handle to the Vulkan instance.
    // VK_NULL_HANDLE means it has not been created yet.
    VkInstance instance = VK_NULL_HANDLE;


    // =========================================================
    // 2. Application Information
    // =========================================================

    // This structure describes our application.
    VkApplicationInfo appInfo{};

    // Specify the structure type.
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    // Application name.
    appInfo.pApplicationName = "My Vulkan App";

    // Application version.
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    // Engine name.
    // We are not using an engine.
    appInfo.pEngineName = "No Engine";

    // Engine version.
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    // Vulkan API version requested by our application.
    appInfo.apiVersion = VK_API_VERSION_1_0;


    // =========================================================
    // 3. Validation Layer & Instance Extensions
    // =========================================================

    const char* validationLayers[] =
    {
        "VK_LAYER_KHRONOS_validation"
    };

    // NEW: The swapchain requires these instance extensions to exist first.
    // VK_KHR_win32_surface is required because you are building an .exe on Windows.
    const char* instanceExtensions[] =
    {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };


    // =========================================================
    // 4. Create Vulkan Instance
    // =========================================================

    VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Enable validation layers
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(std::size(validationLayers));
    createInfo.ppEnabledLayerNames = validationLayers;

    // NEW: Enable the instance extensions
    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(std::size(instanceExtensions));
    createInfo.ppEnabledExtensionNames = instanceExtensions;

    // Create the Vulkan instance.
    VkResult result = vkCreateInstance(
        &createInfo,
        nullptr,
        &instance
    );

    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to create Vulkan instance."
                  << std::endl;

        return EXIT_FAILURE;
    }

    std::cout << "Vulkan instance created successfully."
              << std::endl;


    // =========================================================
    // 5. Find Physical Devices (GPUs)
    // =========================================================

    uint32_t deviceCount = 0;

    // First call:
    // Ask Vulkan how many physical devices are available.
    result = vkEnumeratePhysicalDevices(
        instance,
        &deviceCount,
        nullptr
    );

    if (result != VK_SUCCESS || deviceCount == 0)
    {
        std::cout << "Failed to find Vulkan physical device."
                  << std::endl;

        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }


    // Create a vector to store all physical devices.
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);


    // Second call:
    // Get the actual physical device handles.
    result = vkEnumeratePhysicalDevices(
        instance,
        &deviceCount,
        physicalDevices.data()
    );

    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to enumerate Vulkan devices."
                  << std::endl;

        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }


    std::cout << "Number of Vulkan devices: "
              << deviceCount
              << std::endl;


    // =========================================================
    // 6. Select Physical Device
    // =========================================================

    // For now, simply select the first GPU.
    //
    // Later, you can write code to select the best GPU
    // based on GPU type, memory, features, etc.
    VkPhysicalDevice physicalDevice =
        physicalDevices[0];


    // =========================================================
    // 7. Get Physical Device Properties
    // =========================================================

    VkPhysicalDeviceProperties deviceProperties{};

    // Get information about the selected GPU.
    vkGetPhysicalDeviceProperties(
        physicalDevice,
        &deviceProperties
    );


    // Print GPU name.
    std::cout << "Physical device: "
              << deviceProperties.deviceName
              << std::endl;

    // Print Vulkan API version supported by the GPU.
    std::cout << "Vulkan API version: "
              << VK_VERSION_MAJOR(deviceProperties.apiVersion)
              << "."
              << VK_VERSION_MINOR(deviceProperties.apiVersion)
              << "."
              << VK_VERSION_PATCH(deviceProperties.apiVersion)
              << std::endl;


    // =========================================================
    // 8. Check Physical Device Features
    // =========================================================

    // This structure contains optional features supported by
    // the physical device.
    VkPhysicalDeviceFeatures deviceFeatures{};

    vkGetPhysicalDeviceFeatures(
        physicalDevice,
        &deviceFeatures
    );


    // Check whether sampler anisotropy is supported.
    //
    // IMPORTANT:
    //
    // samplerAnisotropy is a FEATURE.
    //
    // It is NOT an extension.
    //
    // Therefore we check it separately from the swapchain
    // extension.
    if (deviceFeatures.samplerAnisotropy)
    {
        std::cout << "Sampler anisotropy: supported"
                  << std::endl;
    }
    else
    {
        std::cout << "Sampler anisotropy: not supported"
                  << std::endl;
    }


    // =========================================================
    // 9. Check Device Extensions
    // =========================================================

    // Extensions required by our application.
    //
    // VK_KHR_swapchain is normally required when creating
    // a presentation swapchain.
    const std::vector<const char*> deviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };


    // Get number of available device extensions.
    uint32_t extensionCount = 0;

    result = vkEnumerateDeviceExtensionProperties(
        physicalDevice,
        nullptr,
        &extensionCount,
        nullptr
    );

    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to get device extension count."
                  << std::endl;

        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }


    // Create vector to store available extensions.
    std::vector<VkExtensionProperties> availableExtensions(
        extensionCount
    );


    // Get all available extensions.
    result = vkEnumerateDeviceExtensionProperties(
        physicalDevice,
        nullptr,
        &extensionCount,
        availableExtensions.data()
    );

    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to enumerate device extensions."
                  << std::endl;

        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }


    // Assume all required extensions are supported.
    bool allExtensionsSupported = true;


    // Check every extension that our application requires.
    for (const char* requiredExtension : deviceExtensions)
    {
        bool extensionFound = false;

        // Compare required extension against every
        // extension supported by the physical device.
        for (const auto& availableExtension : availableExtensions)
        {
            if (std::strcmp(
                    requiredExtension,
                    availableExtension.extensionName) == 0)
            {
                extensionFound = true;
                break;
            }
        }


        // Extension was not found.
        if (!extensionFound)
        {
            std::cout << "Missing device extension: "
                      << requiredExtension
                      << std::endl;

            allExtensionsSupported = false;
        }
        else
        {
            std::cout << "Device extension supported: "
                      << requiredExtension
                      << std::endl;
        }
    }


    // If a required extension is missing, stop.
    if (!allExtensionsSupported)
    {
        std::cout << "Required device extensions are not supported."
                  << std::endl;

        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }


    // =========================================================
    // 10. Find Queue Family
    // =========================================================

    // Vulkan devices contain one or more queue families.
    //
    // We need a queue capable of graphics operations.
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        nullptr
    );


    // Store queue family information.
    std::vector<VkQueueFamilyProperties> queueFamilies(
        queueFamilyCount
    );


    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        queueFamilies.data()
    );


    // UINT32_MAX means that we haven't found a suitable
    // queue family yet.
    uint32_t graphicsQueueFamily = UINT32_MAX;


    // Search for a queue family supporting graphics.
    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueFamily = i;

            std::cout << "Graphics queue family found: "
                      << i
                      << std::endl;

            break;
        }
    }


    // Check whether a graphics queue was found.
    if (graphicsQueueFamily == UINT32_MAX)
    {
        std::cout << "Failed to find graphics queue family."
                  << std::endl;

        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }


    // =========================================================
    // 11. Create Queue
    // =========================================================

    // Queue priority.
    //
    // Range:
    // 0.0 = lowest priority
    // 1.0 = highest priority
    float queuePriority = 1.0f;


    // Information about the queue we want.
    VkDeviceQueueCreateInfo queueCreateInfo{};

    queueCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

    // Queue family that contains our graphics queue.
    queueCreateInfo.queueFamilyIndex =
        graphicsQueueFamily;

    // Number of queues we want from this family.
    queueCreateInfo.queueCount = 1;

    // Pointer to queue priority.
    queueCreateInfo.pQueuePriorities =
        &queuePriority;


    // =========================================================
    // 12. Create Logical Device
    // =========================================================

    VkDeviceCreateInfo deviceCreateInfo{};

    deviceCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;


    // Tell Vulkan which queue(s) we want.
    deviceCreateInfo.queueCreateInfoCount = 1;

    deviceCreateInfo.pQueueCreateInfos =
        &queueCreateInfo;


    // Enable the physical device features that we want.
    //
    // Here we enable sampler anisotropy only if the GPU
    // actually supports it.
    if (deviceFeatures.samplerAnisotropy)
    {
        deviceCreateInfo.pEnabledFeatures =
            &deviceFeatures;
    }
    else
    {
        // No optional features requested.
        deviceCreateInfo.pEnabledFeatures = nullptr;
    }


    // Enable required device extensions.
    deviceCreateInfo.enabledExtensionCount =
        static_cast<uint32_t>(deviceExtensions.size());

    deviceCreateInfo.ppEnabledExtensionNames =
        deviceExtensions.data();


    // =========================================================
    // 13. Create Logical Device
    // =========================================================

    VkDevice device = VK_NULL_HANDLE;

    result = vkCreateDevice(
        physicalDevice,
        &deviceCreateInfo,
        nullptr,
        &device
    );

    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to create logical device."
                  << std::endl;

        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }

    std::cout << "Logical device created successfully."
              << std::endl;


    // =========================================================
    // 14. Get Graphics Queue
    // =========================================================

    VkQueue graphicsQueue = VK_NULL_HANDLE;

    // Get the queue from the logical device.
    vkGetDeviceQueue(
        device,
        graphicsQueueFamily,
        0,
        &graphicsQueue
    );


    if (graphicsQueue == VK_NULL_HANDLE)
    {
        std::cout << "Failed to get graphics queue."
                  << std::endl;

        vkDestroyDevice(device, nullptr);
        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }

    std::cout << "Graphics queue obtained successfully."
              << std::endl;


    // =========================================================
    // 15. Create Vulkan Buffer
    // =========================================================

    // Size of our buffer.
    VkDeviceSize bufferSize = 1024;


    // Describe the buffer we want to create.
    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType =
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    // Size of the buffer.
    bufferInfo.size = bufferSize;

    // Tell Vulkan how we intend to use this buffer.
    //
    // Here we want to use it as a vertex buffer.
    bufferInfo.usage =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    // Only one queue family will access this buffer.
    bufferInfo.sharingMode =
        VK_SHARING_MODE_EXCLUSIVE;


    // Handle to the buffer.
    VkBuffer vertexBuffer = VK_NULL_HANDLE;


    // Create the buffer.
    result = vkCreateBuffer(
        device,
        &bufferInfo,
        nullptr,
        &vertexBuffer
    );


    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to create vertex buffer."
                  << std::endl;

        vkDestroyDevice(device, nullptr);
        vkDestroyInstance(instance, nullptr);

        return EXIT_FAILURE;
    }


    std::cout << "Successfully created vertex buffer."
              << std::endl;

    std::cout << "Buffer size: "
              << bufferSize
              << " bytes"
              << std::endl;


    // =========================================================
    // 16. Get Buffer Memory Requirements
    // =========================================================

    // IMPORTANT:
    //
    // vkCreateBuffer() creates the BUFFER OBJECT.
    //
    // It does NOT allocate GPU memory for the buffer.
    //
    // To actually use the buffer, we would normally call:
    //
    // vkGetBufferMemoryRequirements()
    // vkAllocateMemory()
    // vkBindBufferMemory()
    //
    // We are stopping before those steps here because this
    // example is focused on instance, device, extension,
    // queue and buffer creation.


    // =========================================================
    // 17. Destroy Buffer
    // =========================================================

    vkDestroyBuffer(
        device,
        vertexBuffer,
        nullptr
    );

    std::cout << "Vertex buffer destroyed."
              << std::endl;


    // =========================================================
    // 18. Destroy Logical Device
    // =========================================================

    // The logical device must be destroyed before the
    // Vulkan instance.
    vkDestroyDevice(
        device,
        nullptr
    );

    std::cout << "Logical device destroyed."
              << std::endl;


    // =========================================================
    // 19. Destroy Vulkan Instance
    // =========================================================

    vkDestroyInstance(
        instance,
        nullptr
    );

    std::cout << "Vulkan instance destroyed."
              << std::endl;


    // =========================================================
    // 20. Program Finished
    // =========================================================

    return EXIT_SUCCESS;
}
