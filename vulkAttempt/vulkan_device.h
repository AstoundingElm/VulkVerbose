#pragma once


PINLINE bool create_device(vulkan_types* context)
{
        u32 physical_device_index = 0;
        u32 physical_device_count = 0;
        VkPhysicalDevice  physical_devices[20];
        VK_CHECK(vkEnumeratePhysicalDevices(context->instance_context.instance, &physical_device_count, NULL));
        
        
        
        
        vkEnumeratePhysicalDevices(context->instance_context.instance, &physical_device_count, physical_devices);
        printf("The following physical devices (GPUs) are available to Vulkan:\n");
        for (uint32_t phys_i = 0; phys_i != physical_device_count; ++phys_i) {
                
                VkPhysicalDeviceFeatures features;
                vkGetPhysicalDeviceFeatures(physical_devices[phys_i], &features);
                
                VkPhysicalDeviceMemoryProperties memory;
                vkGetPhysicalDeviceMemoryProperties(physical_devices[phys_i], &memory);
                
                
                VkPhysicalDeviceProperties device_properties;
                vkGetPhysicalDeviceProperties(physical_devices[phys_i], &device_properties);
                printf("%u - %s%s\n", phys_i, device_properties.deviceName, (phys_i == physical_device_index) ? " (used)" : "");
                if (phys_i == physical_device_index) {
                        context->device_context.physical_device_properties = device_properties;
                }
                
                b8 supports_device_local_host_visible = false;
                for (u32 i = 0; i < memory.memoryTypeCount; ++i) {
                        // Check each memory type to see if its bit is set to 1.
                        if (
                            ((memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) &&
                            ((memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)) {
                                supports_device_local_host_visible = true;
                                
                                break;
                        }
                        
                }
                if(supports_device_local_host_visible){
                        
                        printf("Device %d supports device and local memory\n", phys_i);
                }
                
                
                switch(device_properties.deviceType)
                {
                        default:
                        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                        puts("GPU type is Unknown.");
                        break;
                        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                        puts("GPU type is Integrated.");
                        break;
                        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                        puts("GPU type is Descrete.");
                        break;
                        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                        puts("GPU type is Virtual.");
                        break;
                        case VK_PHYSICAL_DEVICE_TYPE_CPU:
                        puts("GPU type is CPU.");
                        break;
                        
                }
                
                u32 queue_family_count = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[phys_i], &queue_family_count, 0);
                
                const char* vendor = NULL;
                VkPhysicalDeviceDriverProperties driver_properties = {0};
                driver_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
                VkPhysicalDeviceProperties2 physical_device_properties_2 = {0};
                physical_device_properties_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
                physical_device_properties_2.pNext = &driver_properties;
                vkGetPhysicalDeviceProperties2(physical_devices[phys_i], &physical_device_properties_2);
                vendor = GetDeviceVendorFromDriverProperties (&driver_properties);
                printf("Vendor: %s\n", vendor);
                
                printf ("Driver: %s %s\n",
                        driver_properties.driverName, driver_properties.driverInfo);
                
                VkQueueFamilyProperties queue_families[32];
                vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[phys_i],&queue_family_count, queue_families);
                
                
                
                printf("%d queues families are on device %d\n", queue_family_count, phys_i);
                for (u32 i = 0; i <  queue_family_count; i++) {
                        
                        
                        
                        printf("Queue family %d on device %d has %d queues and supports:  \n", i, phys_i, queue_families[i].queueCount);
                        
                        if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                                printf("QUEUE_GRAPHICS |");
                        if(queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                                printf(" QUEUE_TRANSFER |");
                        if(queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                                printf(" QUEUE_COMPUTE |");
                        if(queue_families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
                                printf(" QUEUE_SPARSE_BINDING |");
                        VkBool32 supports_present = VK_FALSE;
                        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physical_devices[phys_i], i, context->surface_context.surface, &supports_present));
                        if(supports_present){
                                printf(" QUEUE_PRESENT");
                                
                        }else{return false;}
                        printf("\n");
                        
                        
                }
                
                printf(
                       "GPU Driver version: %d.%d.%d\n",
                       VK_VERSION_MAJOR(device_properties.driverVersion),
                       VK_VERSION_MINOR(device_properties.driverVersion),
                       VK_VERSION_PATCH(device_properties.driverVersion));
                
                // Vulkan API version.
                printf(
                       "Vulkan API version: %d.%d.%d\n",
                       VK_VERSION_MAJOR(device_properties.apiVersion),
                       VK_VERSION_MINOR(device_properties.apiVersion),
                       VK_VERSION_PATCH(device_properties.apiVersion));
                
                printf("VendorID: %u,  deviceID: %u\n", device_properties.vendorID, device_properties.deviceID);
                // Check if device supports local/host visible combo
                
                for (u32 j = 0; j < memory.memoryHeapCount; ++j) {
                        f32 memory_size_gib = (((f32)memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
                        if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                                printf("Local GPU memory: %.2f GiB\n", memory_size_gib);
                        } else {
                                printf("Shared System memory: %.2f GiB\n", memory_size_gib);
                        }
                }
                
                VkSurfaceCapabilitiesKHR capabilities;
                u32 format_count;
                
                VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_devices[phys_i], context->surface_context.surface, &capabilities));
                VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_devices[phys_i], context->surface_context.surface, &format_count,0));
                VkSurfaceFormatKHR *formats = NULL;
                //formats[format_count];
                VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_devices[phys_i], context->surface_context.surface, &format_count,formats));
                u32 present_mode_count;
                VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_devices[phys_i], context->surface_context.surface, &present_mode_count, 0));
                VkPresentModeKHR * present_modes = NULL;
                VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_devices[phys_i], context->surface_context.surface, &present_mode_count, present_modes));
                if(format_count < 1 || present_mode_count < 1){ printf("Required swapchain support not present, skipping device.");
                        return false;
                }
                
        }
        
        const char required_device_extension[][256] = {"VK_KHR_swapchain",/*"VK_KHR_portability_subset",*/
                VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
                VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
                VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME,
                "VK_EXT_descriptor_indexing",
                /*"VK_KHR_get_physical_device_properties2"*/
                /*"VK_KHR_maintenance3"*/
                /*VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME,*/
                
                
                
                
        };
        
        const char * const device_extension_names[] = {
                required_device_extension[0], required_device_extension[1],
                required_device_extension[2],
                required_device_extension[3],
                required_device_extension[4],
                /*required_device_extension[5],*/
                /*required_device_extension[6],*/
        };
        
        u32 available_device_extension_count = 0;
        VkExtensionProperties available_device_extensions[150] = {0};
        VK_CHECK(vkEnumerateDeviceExtensionProperties(physical_devices[0], 0, &available_device_extension_count, 0));
        
        VK_CHECK(vkEnumerateDeviceExtensionProperties(physical_devices[0], 0, &available_device_extension_count, available_device_extensions));
        
        u32 required_device_extension_count = ArraySize(device_extension_names);
        
        
        for (u32 i = 0; i < required_device_extension_count; ++i) {
                b8 found = false;
                for (u32 j = 0; j < available_device_extension_count; j++) {
                        if (strcmp(device_extension_names[i], available_device_extensions[j].extensionName) == 0) {
                                found = true;
                                break;
                        }
                }
                
                if (!found) {
                        printf("Required extension not found: '%s', skipping device.", device_extension_names[i]);
                        
                        return false;
                }
        }
        /*
        for(u32 i = 0; i < ArraySize(device_extension_names); i++)
        {
                printf("Required device extensions: %s\n\n", device_extension_names[i]);
        }
        
        for(u32 i = 0; i < ArraySize(available_device_extensions); i++)
        {
                printf("Available device extensions: %s\n", available_device_extensions[i].extensionName);
        }
        */
        printf("Transfer_family_index: %d\n Graphics_family_index: %d\n Compute_family_index: %d\nPresent_family_index: %d\n on gpu index: %d\n", context->queue_context.transfer_family_index, context->queue_context.graphics_family_index, context->queue_context.compute_family_index, context->queue_context.present_family_index,
               context->device_context.physical_device_index);
        
        context->device_context.gpu = physical_devices[physical_device_index];
        
        f32 queue_priority = 1.0f;
        
        VkDeviceQueueCreateInfo queue_create_info = {0};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.pNext = NULL;
        queue_create_info.flags = 0;
        queue_create_info.queueFamilyIndex = 0;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        
        VkPhysicalDeviceFeatures device_features = {0};
        
        VkDeviceCreateInfo device_create_info = {0};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.queueCreateInfoCount = 1;
        
        device_create_info.pQueueCreateInfos = &queue_create_info;
        device_create_info.pEnabledFeatures = &device_features;
        device_create_info.enabledExtensionCount = required_device_extension_count;
        device_create_info.ppEnabledExtensionNames = device_extension_names;
        
        // Deprecated and ignored, so pass nothing.
        device_create_info.enabledLayerCount = 0;
        device_create_info.ppEnabledLayerNames = 0;
        
        VkPhysicalDeviceFeatures vulkan_physical_device_features;
#if defined(VK_EXT_subgroup_size_control)
	VkPhysicalDeviceSubgroupProperties physical_device_subgroup_properties;
	memset (&physical_device_subgroup_properties, 0, sizeof (physical_device_subgroup_properties));
	VkPhysicalDeviceSubgroupSizeControlPropertiesEXT physical_device_subgroup_size_control_properties;
	memset (&physical_device_subgroup_size_control_properties, 0, sizeof (physical_device_subgroup_size_control_properties));
	VkPhysicalDeviceSubgroupSizeControlFeaturesEXT subgroup_size_control_features;
	memset (&subgroup_size_control_features, 0, sizeof (subgroup_size_control_features));
	if ( VK_EXT_subgroup_size_control)
	{
		memset (&physical_device_subgroup_size_control_properties, 0, sizeof (physical_device_subgroup_size_control_properties));
		physical_device_subgroup_size_control_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT;
		memset (&physical_device_subgroup_properties, 0, sizeof (physical_device_subgroup_properties));
		physical_device_subgroup_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;
		physical_device_subgroup_properties.pNext = &physical_device_subgroup_size_control_properties;
		VkPhysicalDeviceProperties2 physical_device_properties_2;
		memset (&physical_device_properties_2, 0, sizeof (physical_device_properties_2));
		physical_device_properties_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		physical_device_properties_2.pNext = &physical_device_subgroup_properties;
		vkGetPhysicalDeviceProperties2 (context->device_context.gpu, &physical_device_properties_2);
                
		memset (&subgroup_size_control_features, 0, sizeof (subgroup_size_control_features));
		subgroup_size_control_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT;
		VkPhysicalDeviceFeatures2 physical_device_features_2;
		memset (&physical_device_features_2, 0, sizeof (physical_device_features_2));
		physical_device_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		physical_device_features_2.pNext = &subgroup_size_control_features;
		vkGetPhysicalDeviceFeatures2 (context->device_context.gpu, &physical_device_features_2);
                
		//vulkan_physical_device_features = physical_device_features_2.features;
	}
	else
#endif
        vkGetPhysicalDeviceFeatures (context->device_context.gpu, &vulkan_physical_device_features);
        
#if defined(VK_EXT_subgroup_size_control)
	/*vulkan_globals.screen_effects_sops =
		vulkan_globals.vulkan_1_1_available && subgroup_size_control && subgroup_size_control_features.subgroupSizeControl &&
		subgroup_size_control_features.computeFullSubgroups && ((physical_device_subgroup_properties.supportedStages & VK_SHADER_STAGE_COMPUTE_BIT) != 0) &&
        ((physical_device_subgroup_properties.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_BIT) != 0)
		// Shader only supports subgroup sizes from 4 to 64. 128 can't be supported because Vulkan spec states that workgroup size
                // in x dimension must be a multiple of the subgroup size for VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT_EXT.
		&& (physical_device_subgroup_size_control_properties.minSubgroupSize >= 4) && (physical_device_subgroup_size_control_properties.maxSubgroupSize <= 64);
        
	if (vulkan_globals.screen_effects_sops)
		Con_Printf ("Using subgroup operations\n");&=*/
#endif
        
        // Create the device.
        VK_CHECK(vkCreateDevice(
                                context->device_context.gpu,
                                &device_create_info,
                                context->instance_context.allocator,
                                &context->device_context.logical_device));
        
        puts("Logical device created.\n");
        
        vkGetDeviceQueue(
                         context->device_context.logical_device,
                         context->queue_context.graphics_queue_index,
                         0,
                         &context->queue_context.graphics_queue);
        
        
        puts("Queues obtained.\n");
        return true;
        
        
}
