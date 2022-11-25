#pragma once

#include "defines.h"
#include "vulkan_types.h"


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
                                                 VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                 void* user_data);


PINLINE bool create_instance(vulkan_types* vk_context)
{
        
        VkApplicationInfo info = {0};
        
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pNext = NULL;
        info.pApplicationName = "Petes engine";
        info.applicationVersion =VK_MAKE_VERSION(0,0,1);
        info.pEngineName = " ";
        info.engineVersion=VK_MAKE_VERSION(0,0,1);
	info.apiVersion=VK_API_VERSION_1_3;
        
        VkValidationFeatureEnableEXT validationFeaturesEnable[] = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT, VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
                VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
                VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
        };
	VkValidationFeaturesEXT validationFeatures = {
		.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
		.pNext = NULL,
		.enabledValidationFeatureCount = sizeof(validationFeaturesEnable) / sizeof(validationFeaturesEnable[0]),
		.pEnabledValidationFeatures = validationFeaturesEnable,
		.disabledValidationFeatureCount = 0,
		.pDisabledValidationFeatures = NULL
	};
        
        VkInstanceCreateInfo instance_info = {0};
        instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_info.pNext = &validationFeatures;
        instance_info.flags = 0;
        instance_info.pApplicationInfo = &info;
        
        
        const char layerList[][256] = {
                "VK_LAYER_KHRONOS_validation",
                "VK_LAYER_LUNARG_gfxreconstruct",
                /*"VK_LAYER_LUNARG_api_dump",*/
                
                
        };
        
        const char * const layers[] = {
                layerList[0],layerList[1]
        };
        
        instance_info.enabledLayerCount = ArraySize(layers);
        
        const char required_extension[][256] = {
                VK_KHR_SURFACE_EXTENSION_NAME,
                "VK_KHR_xcb_surface",
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                
                
                
                
                
        };
        
        const char * const extensions[] = {
                required_extension[0], required_extension[1], required_extension[2],
        };
        
        instance_info.enabledExtensionCount = ArraySize(extensions);
        instance_info.ppEnabledExtensionNames = extensions; 
        instance_info.ppEnabledLayerNames = layers;
        
        
        
        u32 available_extension_count = 0;
        vkEnumerateInstanceExtensionProperties(0, &available_extension_count, 0);
        
        VkExtensionProperties   available_extensions[20] = {0};
        
        vkEnumerateInstanceExtensionProperties(0, &available_extension_count, available_extensions);
        /*for(u32 i = 0;i < ArraySize(extensions); i++){ 
                printf("Required extensions: %s...\n", extensions[i]);
        };
        
        
        for(u32 i = 0; i < ArraySize(available_extensions); i++){
                
                printf("Available extensions: %s...\n", available_extensions[i].extensionName);
                
        };
        
        */
        // Verify required extensions are available.
        for (u32 i = 0; i < ArraySize(extensions); ++i) {
                bool extension_found = false;
                for (u32 j = 0; j < available_extension_count; ++j) {
                        
                        if (strcmp(extensions[i], available_extensions[j].extensionName) == 0) {
                                extension_found = true;
                                printf("Required extension found: %s...\n", extensions[i]);
                                break;
                        }
                }
                
                if (!extension_found) {
                        extension_found = false;
                        printf("Required extension not found: %s", extensions[i]);
                        return false;
                }
        }
        
        // Obtain a list of available validation layers
        u32 available_layer_count = 0;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
        VkLayerProperties available_layers[20];
        VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers));
        /*
        for(u32 i = 0; i < ArraySize(layers); i++){
                
                printf("Required validation layers: %s...\n", layers[i]);
                
        };
        
        for(u32 i = 0; i < ArraySize(available_layers); i++){
                
                printf("Available  validation layers: %s...\n", available_layers[i].layerName);
                
        };
        */
        // Verify all required layers are available.
        for (u32 i = 0; i < ArraySize(layers); i++) {
                bool layer_found = false;
                for (u32 j = 0; j < available_layer_count; j++) {
                        if (strcmp(layers[i] , available_layers[j].layerName) == 0) {
                                layer_found = true;
                                printf("Found validation layer: %s...\n", layers[i]);
                                break;
                        }
                }
                
                if (!layer_found) {
                        printf("Required validation layer is missing: %s", layers[i]);
                        return false;
                }
        }
        
        puts("All instance level layers and extensions found!\n");
        
        VK_CHECK(vkCreateInstance(&instance_info, 0,  &vk_context->instance));
        
        puts("Creating Vulkan debugger...");
        u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;  //|
        //    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        
        VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
        debug_create_info.messageSeverity = log_severity;
        debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debug_create_info.pfnUserCallback = vk_debug_callback;
        
        PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk_context->instance, "vkCreateDebugUtilsMessengerEXT");
        KASSERT_MSG(func, "meme");
        VK_CHECK(func(vk_context->instance, &debug_create_info, vk_context->allocator, &vk_context->debug_messenger));
        puts("Vulkan debugger created.");
        
        return true;
        
}


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
                                                 VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                 void* user_data) {
        switch (message_severity) {
                default:
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                puts(callback_data->pMessage);
                break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                puts(callback_data->pMessage);
                break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                puts(callback_data->pMessage);
                break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                puts(callback_data->pMessage);
                break;
        }
        return VK_FALSE;
}
