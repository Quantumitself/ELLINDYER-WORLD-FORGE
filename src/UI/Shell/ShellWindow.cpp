#include "UI/Shell/ShellWindow.hpp"

#include "UI/ImGui/ImGuiCompat.hpp"

namespace ellindyer::ui::shell
{

namespace
{

constexpr const char* kShellWindowName = "##WorldForgeShell";
constexpr float kHeaderHeight = 72.0f;

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
    toolbar_visible_  = configuration_.show_toolbar;
}

void ShellWindow::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    const ImGuiIO& io = ImGui::GetIO();

    const float menu_bar_height = menu_bar_visible_ ? ImGui::GetFrameHeight() : 0.0f;
    const float toolbar_height  = toolbar_visible_
        ? (toolbar_.GetHeight() + 4.0f)
        : 0.0f;
    const float total_height    = menu_bar_height + toolbar_height + kHeaderHeight;
    consumed_height_ = total_height;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, total_height));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGui::Begin(kShellWindowName, nullptr, kShellWindowFlags);

    if (menu_bar_visible_)
    {
        menu_bar_.Render();
    }

    if (toolbar_visible_)
    {
        toolbar_.Render(fonts);
        RenderHorizontalLine(1.0f);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 8.0f));
    ImGui::BeginChild("##ShellHeader",
                      ImVec2(0.0f, kHeaderHeight),
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
    ImGui::PopStyleVar();   // header padding

    ImGui::End();
    ImGui::PopStyleVar(3);
}

void ShellWindow::SetTitle(std::string title) { configuration_.title = std::move(title); }
void ShellWindow::SetSubtitle(std::string subtitle) { configuration_.subtitle = std::move(subtitle); }
const std::string& ShellWindow::GetTitle() const noexcept { return configuration_.title; }
const std::string& ShellWindow::GetSubtitle() const noexcept { return configuration_.subtitle; }

ellindyer::ui::menu::MenuBar& ShellWindow::GetMenuBar() noexcept { return menu_bar_; }
const ellindyer::ui::menu::MenuBar& ShellWindow::GetMenuBar() const noexcept { return menu_bar_; }

ellindyer::ui::toolbar::Toolbar& ShellWindow::GetToolbar() noexcept { return toolbar_; }
const ellindyer::ui::toolbar::Toolbar& ShellWindow::GetToolbar() const noexcept { return toolbar_; }

void ShellWindow::SetMenuBarVisible(bool visible) noexcept { menu_bar_visible_ = visible; }
bool ShellWindow::IsMenuBarVisible() const noexcept { return menu_bar_visible_; }

void ShellWindow::SetToolbarVisible(bool visible) noexcept { toolbar_visible_ = visible; }
bool ShellWindow::IsToolbarVisible() const noexcept { return toolbar_visible_; }

float ShellWindow::GetConsumedHeight() const noexcept { return consumed_height_; }

} // namespace ellindyer::ui::shell
