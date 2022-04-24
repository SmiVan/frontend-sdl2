#include "PresetSelectionGui.h"

#include "AnonymousProFont.h"
#include "ProjectMWrapper.h"
#include "SDLRenderingWindow.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include <Poco/Util/Application.h>

#include <cmath>

const char* PresetSelectionGui::name() const
{
    return "Preset Selection GUI";
}

void PresetSelectionGui::initialize(Poco::Util::Application& app)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    auto& renderingWindow = Poco::Util::Application::instance().getSubsystem<SDLRenderingWindow>();
    auto& projectMWrapper = Poco::Util::Application::instance().getSubsystem<ProjectMWrapper>();

    _sdlRenderingWindow = &renderingWindow;
    _projectMWrapper = &projectMWrapper;

    _renderingWindow = renderingWindow.GetRenderingWindow();
    _glContext = renderingWindow.GetGlContext();

    ImGui_ImplSDL2_InitForOpenGL(_renderingWindow, _glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    UpdateFontSize();
}

void PresetSelectionGui::uninitialize()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    _sdlRenderingWindow = nullptr;
    _projectMWrapper = nullptr;
    _renderingWindow = nullptr;
    _glContext = nullptr;
}

void PresetSelectionGui::UpdateFontSize()
{
    ImGuiIO& io = ImGui::GetIO();

    float dpi{96.0f};// Use default value of 96 DPI if SDL_GetDisplayDPI doesn't return a value!
    auto displayIndex = SDL_GetWindowDisplayIndex(_renderingWindow);
    if (displayIndex < 0)
    {
        poco_debug_f1(_logger, "Could not get display index for application window: %s", std::string(SDL_GetError()));
        return;
    }

    auto result = SDL_GetDisplayDPI(displayIndex, &dpi, nullptr, nullptr);
    if (result != 0)
    {
        poco_debug_f2(_logger, "Could not get DPI info for display %?d: %s", displayIndex, std::string(SDL_GetError()));
    }

    // Only interested in changes of > 1 DPI, really.
    if (static_cast<uint32_t>(dpi) == static_cast<uint32_t>(_dpi))
    {
        return;
    }

    poco_debug_f3(_logger, "DPI change for display %?d: %hf -> %hf", displayIndex, _dpi, dpi);

    _dpi = dpi;

    _font = io.Fonts->AddFontFromMemoryCompressedTTF(&AnonymousPro_compressed_data, AnonymousPro_compressed_size, floor(12.0f * (_dpi / 96.0f)));
    io.Fonts->Build();
    ImGui_ImplOpenGL3_CreateFontsTexture();

    ImGui::GetStyle().ScaleAllSizes(1.0);
}

void PresetSelectionGui::ProcessInput(const SDL_Event& event)
{
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void PresetSelectionGui::Draw()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        static char text[1024 * 16] =
            "/*\n"
            " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
            " the hexadecimal encoding of one offending instruction,\n"
            " more formally, the invalid operand with locked CMPXCHG8B\n"
            " instruction bug, is a design flaw in the majority of\n"
            " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
            " processors (all in the P5 microarchitecture).\n"
            "*/\n\n"
            "label:\n"
            "\tlock cmpxchg8b eax\n";

        ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_None);

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool PresetSelectionGui::WantsKeyboardInput()
{
    auto& io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

bool PresetSelectionGui::WantsMouseInput()
{
    auto& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}