#include <iostream>
#include <stdexcept>
#include <memory>
#include <vector>

#include "Defines.h"
#include "Resource.h"

using namespace std;

namespace
{
  const std::vector<const char*> local_validation_layer_names =
  {
    "VK_LAYER_LUNARG_standard_validation"
  };
}

class TriangleApplication
{
public:
  void run()
  {
    initWindow();
    initVulkan();
    mainLoop();
  }

private:
  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
  {
    std::cerr << "validation layer: " << msg << std::endl;
    return VK_FALSE;
  }

  void createInstance()
  {
    // Optional setup
    VkApplicationInfo appInfo = {};
    VkInstanceCreateInfo createInfo = {};

    if (!checkValidationLayerSupport(createInfo))
    {
      throw runtime_error("validation layers requested, but not available.");
    }

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    std::vector<VkExtensionProperties> extensions;

    // check extensions
    uint32_t count = 0;
    bool use_glfw_ext = true;

    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
    if (count > 0)
    {
      VkExtensionProperties *extensions = new VkExtensionProperties[count];
      vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions);
      {
        std::cout << "available extensions:" << std::endl;
        for (uint32_t i = 0; i< count; ++i)
        {
          _extension_names.push_back((extensions + i)->extensionName);
          std::cout << "\t" << (extensions + i)->extensionName << std::endl;
        }
        createInfo.enabledExtensionCount = count;
        createInfo.ppEnabledExtensionNames = &_extension_names[0];
        use_glfw_ext = false;
      }
    }
    if (use_glfw_ext)
    {
      glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
      createInfo.enabledExtensionCount = glfwExtensionCount;
      createInfo.ppEnabledExtensionNames = glfwExtensions;
      cout << "No available extension is detected. The glfw extension is used." << endl;
    }

    vector<const char*> glfw_extensions;
    if(getRequiredExtension(glfw_extensions))
    {
      createInfo.enabledExtensionCount = static_cast<uint32_t>(glfw_extensions.size());
      createInfo.ppEnabledExtensionNames = glfw_extensions.data();
    }

   // createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create instance!");
    }
  }

  void initWindow()
  {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void initVulkan()
  {
    createInstance();
    setupCallBack();
  }

  bool checkValidationLayerSupport(VkInstanceCreateInfo &info)
  {
    uint32_t count = 0;
    info.enabledLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&count, nullptr);
    vector<VkLayerProperties> properties(count);
    vkEnumerateInstanceLayerProperties(&count, properties.data());
#if defined PRINT_VERBOS
    cout << "Available properties: " << count << endl;
#endif
    uint8_t found_layer = 0;
    for (const char *layer : local_validation_layer_names)
    {
      for (VkLayerProperties p : properties)
      {
#if defined PRINT_VERBOS
        cout << p.layerName << ": " << p.description << endl;
#endif // PRINT_VERBOS
        if (strcmp(p.layerName, layer) == 0)
        {
          ++found_layer;
          break;
        }
      }
    }
    if (found_layer == local_validation_layer_names.size())
    {
      info.ppEnabledLayerNames = local_validation_layer_names.data();
      info.enabledLayerCount = found_layer;
      return true;
    }
    return false;
  }

  bool getRequiredExtension(vector<const char*> &a_extensions)
  {
    a_extensions.clear();
    unsigned int glfw_ext_count = 0;
    const char **glfw_exts;
    glfw_exts = glfwGetRequiredInstanceExtensions(&glfw_ext_count);

    for (unsigned int idx = 0; idx < glfw_ext_count; ++idx)
    {
      a_extensions.push_back(glfw_exts[idx]);
    }

    if (g_validation_layers_enabled)
    {
      if(std::find(a_extensions.begin(), a_extensions.end(), VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == a_extensions.end())
      {
        a_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
      }
    }
    return !a_extensions.empty();
  }

  bool setupCallBack()
  {
    if (!g_validation_layers_enabled)
      return false;
    else
    {
      VkDebugReportCallbackCreateInfoEXT info = {};
      info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
      info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
      info.pfnCallback = debugCallback;

      if (debug::CreateDebugReportCallbackEXT(_instance, &info, nullptr, &_callback) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to setup debug callback.");
      }
    }
    return true;
  }

  void mainLoop()
  {
    while (!glfwWindowShouldClose(_window))
    {
      glfwPollEvents();
    }
  }

private:
  VkInstance _instance;
  GLFWwindow *_window;
  VkDebugReportCallbackEXT _callback;
  //Resource<VkInstance> _instance{vkDestroyInstance};
  vector<const char*> _extension_names;
  vector<VkExtensionProperties> _extensions;
};

int main()
{
  TriangleApplication app;

  try
  {
    app.run();
  }
  catch (const std::runtime_error& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}