#include "Resource.h"

VkResult debug::CreateDebugReportCallbackEXT(VkInstance a_pInstance
  , const VkDebugReportCallbackCreateInfoEXT *a_pInfo
  , const VkAllocationCallbacks *a_pAllocator
  , VkDebugReportCallbackEXT *a_pCallback)
{
  auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(a_pInstance, "vkCreateDebugReportCallbackEXT");
  if (nullptr != func)
  {
    return func(a_pInstance, a_pInfo, a_pAllocator, a_pCallback);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}