#pragma once

#include "vulkan_context.h"
#include "vulkan_buffer.h"

namespace backend::vulkan {
    class Image {
    public:
        static std::unique_ptr<Image> FromFile(Context& context, std::filesystem::path const& path);
        static std::unique_ptr<Image> FromRGBA(Context& context, int width, int height, unsigned char const* pixels);
        Image(Context& context);
        Image(Context& context, VkImage image, VkImageView view, VkExtent2D extent, VkFormat format, bool owning = true);
        Image(Context& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, bool owning = true);
        ~Image();

        uint32_t GetId() const { return m_id; }

        VkImage GetVkImage() const { return m_vkImage; }
        VkExtent2D GetVkExtent() const { return m_vkExtent; }
        VkFormat GetVkFormat() const { return m_vkFormat; }
        VkImageView GetVkView() const { return m_vkView; }
        VkSampler GetVkSampler() const { return m_vkSampler; }
        VkDescriptorSet GetDescriptorSet();

        Image(Image const&) = delete;
        Image& operator=(Image const&) = delete;

    protected:
        uint32_t m_id;
        Context& m_context;
        VkExtent2D m_vkExtent;
        VkFormat m_vkFormat;

        VkImage m_vkImage = VK_NULL_HANDLE;
        VmaAllocation m_vmaAllocation = VK_NULL_HANDLE;
        VkImageView m_vkView = VK_NULL_HANDLE;
        VkSampler m_vkSampler = VK_NULL_HANDLE;
        VkDescriptorSet m_vkDescriptorSet = VK_NULL_HANDLE;

        bool m_owningImage = true;

        void CreateResource(VkImageTiling tiling, VkImageUsageFlags usage);
        void CreateView();
        void CreateDefaultSampler();
    };
}
