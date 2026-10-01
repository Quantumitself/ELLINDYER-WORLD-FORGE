#pragma once

#include "App/ApplicationLifecycle.hpp"

namespace ellindyer::app
{

class Application
{
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) noexcept = delete;
    Application& operator=(Application&&) noexcept = delete;

    [[nodiscard]] int Execute(int argc, char** argv);

private:
    ApplicationLifecycle lifecycle_;
};

} // namespace ellindyer::app
