#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
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
        for (int i = 0; i< count; ++i)
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

    //createInfo.enabledExtensionCount = glfwExtensionCount;
    //createInfo.ppEnabledExtensionNames = glfwExtensions;

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