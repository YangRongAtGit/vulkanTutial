#pragma once

template <typename T>
class Resource {
public:
  using ResRef = Resource<T>&;
  Resource() : Resource([](T) {}) {}

  Resource(std::function<void(T, VkAllocationCallbacks*)> deletef)
  {
    this->deleter = [=](T obj) { deletef(obj, nullptr); };
  }

  Resource(const ResRef instance, std::function<void(T, T, VkAllocationCallbacks*)> deletef)
  {
    this->deleter = [&instance, deletef](T obj) { deletef(instance, obj, nullptr); };
  }
  //Resource(const Resource<VkInstance>& instance, std::function<void(VkInstance, T, VkAllocationCallbacks*)> deletef) {
  //  this->deleter = [&instance, deletef](T obj) { deletef(instance, obj, nullptr); };
  //}

  //Resource(const Resource<VkDevice>& device, std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef) {
  //  this->deleter = [&device, deletef](T obj) { deletef(device, obj, nullptr); };
  //}

  ~Resource() {
    cleanup();
  }

  T* operator &() {
    cleanup();
    return &object;
  }

  operator T() const {
    return object;
  }

private:
  T object{ VK_NULL_HANDLE };
  std::function<void(T)> deleter;

  void cleanup() {
    if (object != VK_NULL_HANDLE) {
      deleter(object);
    }
    object = VK_NULL_HANDLE;
  }
};