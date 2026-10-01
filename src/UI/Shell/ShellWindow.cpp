#include "UI/Shell/ShellWindow.hpp"

#include <cstdio>

namespace ellindyer::ui::shell
{

namespace
{

constexpr const char* kShellWindowName = "##WorldForgeShell";

constexpr ImGuiWindowFlags kShellWindowFlags =
    ImGuiWindowFlags_MenuBar
    | ImGuiWindowFlags_NoTitleBar
    | ImGuiWindowFlags_NoResize
    | ImGuiWindowFlags_NoMove
    | ImGuiWindowFlags_NoScrollbar
    | ImGuiWindowFlags_NoScrollWithMouse
    | ImGuiWindowFlags_NoCollapse
    | ImGuiWindowFlags_NoSavedSettings
    | ImGuiWindowFlags_NoBringToFrontOnFocus
    | ImGuiWindowFlags_NoNavFocus
    | ImGuiWindowFlags_NoBackground;

ImVec4 ColorPrimary()   { return ImVec4(1.00f, 1.00f, 1.00f, 1.00f); }
ImVec4 ColorSecondary() { return ImVec4(0.68f, 0.68f, 0.68f, 1.00f); }
ImVec4 ColorMuted()     { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }
ImVec4 ColorSeparator() { return ImVec4(0.24f, 0.24f, 0.24f, 1.00f); }

void RenderHorizontalLine(float height)
{
    const ImVec2 cursor = ImGui::GetCursorScreenPos();
    const float width = ImGui::GetContentRegionAvail().x;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(cursor,
                             ImVec2(cursor.x + width, cursor.y + height),
                             ImGui::GetColorU32(ColorSeparator()));
    ImGui::Dummy(ImVec2(width, height));
}

} // namespace

ShellWindow::ShellWindow() = default;

ShellWindow::~ShellWindow() = default;

void ShellWindow::Configure(const ShellWindowConfiguration& configuration)
{
    configuration_ = configuration;
    menu_bar_visible_ = configuration_.show_menu_bar;
}

void ShellWindow::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    const ImGuiIO& io = ImGui::GetIO();

    last_frame_time_ms_ = static_cast<float>(io.DeltaTime) * 1000.0f;
    last_frames_per_second_ = io.Framerate;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGui::Begin(kShellWindowName, nullptr, kShellWindowFlags);

    if (menu_bar_visible_)
    {
        menu_bar_.Render();
    }

    RenderHeader(fonts);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 20.0f));
    ImGui::BeginChild("##ShellBody",
                      ImVec2(0.0f, -28.0f),
                      false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

    RenderBody(fonts);

    ImGui::EndChild();
    ImGui::PopStyleVar();

    RenderStatusLine(fonts);

    ImGui::End();

    ImGui::PopStyleVar(3);
}

void ShellWindow::SetTitle(std::string title)
{
    configuration_.title = std::move(title);
}

void ShellWindow::SetSubtitle(std::string subtitle)
{
    configuration_.subtitle = std::move(subtitle);
}

const std::string& ShellWindow::GetTitle() const noexcept
{
    return configuration_.title;
}

const std::string& ShellWindow::GetSubtitle() const noexcept
{
    return configuration_.subtitle;
}

ellindyer::ui::menu::MenuBar& ShellWindow::GetMenuBar() noexcept
{
    return menu_bar_;
}

const ellindyer::ui::menu::MenuBar& ShellWindow::GetMenuBar() const noexcept
{
    return menu_bar_;
}

void ShellWindow::SetMenuBarVisible(bool visible) noexcept
{
    menu_bar_visible_ = visible;
}

bool ShellWindow::IsMenuBarVisible() const noexcept
{
    return menu_bar_visible_;
}

void ShellWindow::RenderHeader(const ellindyer::ui::fonts::FontSet& fonts)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 16.0f));
    ImGui::BeginChild("##ShellHeader",
                      ImVec2(0.0f, 72.0f),
                      false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

    if (fonts.heading_bold != nullptr)
    {
        ImGui::PushFont(fonts.heading_bold, fonts.heading_bold->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorPrimary());
    ImGui::TextUnformatted(configuration_.title.c_str());
    ImGui::PopStyleColor();

    if (fonts.heading_bold != nullptr)
    {
        ImGui::PopFont();
    }

    if (!configuration_.subtitle.empty())
    {
        if (fonts.small_regular != nullptr)
        {
            ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ColorSecondary());
        ImGui::TextUnformatted(configuration_.subtitle.c_str());
        ImGui::PopStyleColor();

        if (fonts.small_regular != nullptr)
        {
            ImGui::PopFont();
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();

    RenderHorizontalLine(1.0f);
}

void ShellWindow::RenderBody(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (fonts.large_regular != nullptr)
    {
        ImGui::PushFont(fonts.large_regular, fonts.large_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorPrimary());
    ImGui::TextUnformatted("Workspace");
    ImGui::PopStyleColor();

    if (fonts.large_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::Spacing();

    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorSecondary());
    ImGui::TextUnformatted("The main workspace will host the World Forge editing surfaces.");
    ImGui::TextUnformatted("Project Explorer, Inspector, and Workspace panels arrive in later phases.");
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
    ImGui::Text("Client area: %ux%u",
                static_cast<unsigned>(ImGui::GetIO().DisplaySize.x),
                static_cast<unsigned>(ImGui::GetIO().DisplaySize.y));
    ImGui::Text("Renderer: DirectX 11");
    ImGui::Text("UI framework: Dear ImGui");
    ImGui::PopStyleColor();

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }
}

void ShellWindow::RenderStatusLine(const ellindyer::ui::fonts::FontSet& fonts)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 4.0f));
    ImGui::BeginChild("##ShellStatusLine",
                      ImVec2(0.0f, 28.0f),
                      false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

    if (fonts.small_regular != nullptr)
    {
        ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
    ImGui::Text("Ready");
    ImGui::SameLine();
    ImGui::TextUnformatted(" | ");
    ImGui::SameLine();
    ImGui::Text("Frame: %.2f ms", static_cast<double>(last_frame_time_ms_));
    ImGui::SameLine();
    ImGui::TextUnformatted(" | ");
    ImGui::SameLine();
    ImGui::Text("FPS: %.0f", static_cast<double>(last_frames_per_second_));
    ImGui::PopStyleColor();

    if (fonts.small_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
}

} // namespace ellindyer::ui::shell
