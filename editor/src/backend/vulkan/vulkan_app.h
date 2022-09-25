#pragma once

#include "backend/iapplication.h"
#include "layers/layer_stack.h"
#include "events/event.h"

#include "moth_ui/iimage_factory.h"
#include "moth_ui/ifont_factory.h"
#include "moth_ui/irenderer.h"

#include "vulkan_context.h"
//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.hpp>
//#include <backends/imgui_impl_glfw.h>
//#include <backends/imgui_impl_vulkan.h>

#include "vulkan_graphics.h"
#include "vulkan_swapchain.h"

namespace backend::vulkan {
    class Application : public IApplication, public moth_ui::EventListener {
    public:
        Application();
        virtual ~Application();

        int Run() override;
        void Stop() override { m_running = false; }

        bool OnEvent(moth_ui::Event const& event) override;

        void SetWindowTitle(std::string const& title) override;

        nlohmann::json& GetPersistentState() override { return m_persistentState; }

        backend::IGraphicsContext& GetGraphics() override { return *m_graphics; }

    protected:
        bool Initialise();

        void Update();
        void Draw();
        void Shutdown();

    private:
        static int constexpr INIT_WINDOW_WIDTH = 1280;
        static int constexpr INIT_WINDOW_HEIGHT = 960;

        moth_ui::IntVec2 m_windowPos = { -1, -1 };
        int m_windowWidth = 0;
        int m_windowHeight = 0;
        bool m_windowMaximized = false;

        bool m_running = false;
        bool m_paused = false;
        bool m_editorMode = false;
        std::chrono::milliseconds m_updateTicks;
        std::chrono::time_point<std::chrono::steady_clock> m_lastUpdateTicks;

        moth_ui::IntVec2 m_gameWindowPos;

        std::unique_ptr<LayerStack> m_layerStack;

        std::filesystem::path m_persistentFilePath;
        nlohmann::json m_persistentState;
        static char const* const PERSISTENCE_FILE;

        std::unique_ptr<moth_ui::IImageFactory> m_imageFactory;
        std::unique_ptr<moth_ui::IFontFactory> m_fontFactory;
        std::unique_ptr<moth_ui::IRenderer> m_uiRenderer;

        std::unique_ptr<backend::vulkan::Graphics> m_graphics;

        // glfw stuff
        GLFWwindow* m_glfwWindow = nullptr;
        //

        // vulkan stuff
        std::unique_ptr<Context> m_context;
        ImGui_ImplVulkanH_Window m_imWindowData;
        int m_imMinImageCount = 2;
        bool m_vkSwapChainrebuild = false;

        void InitVulkanWindow(VkSurfaceKHR vkSurface, int width, int height);
        void VulkanFrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* drawData);
        void VulkanFramePresent(ImGui_ImplVulkanH_Window* wd);
        //

        bool OnWindowSizeEvent(EventWindowSize const& event);
        bool OnQuitEvent(EventQuit const& event);

        VkSurfaceKHR m_customVkSurface;
        //VkSwapchainKHR m_customVkSwapchain;
        //std::vector<VkImage> m_customSwapchainImages;
        //VkFormat m_customSwapchainImageFormat;
        //VkExtent2D m_customSwapchainExtent;
        //std::vector<VkImageView> m_customSwapchainImageViews;
        //std::vector<VkFramebuffer> m_customSwapchainFramebuffers;
        //VkSemaphore m_customImageAvailableSemaphore;
        //VkSemaphore m_customRenderFinishedSemaphore;
        //VkFence m_customInFlightFence;

        std::shared_ptr<moth_ui::ITarget> m_testTarget;

        void ImGuiInit();

        void CustomInit();
        void CustomFrameRender();
    };
}