#include "../../Utils/Logger.hpp"
#include "../../EditorLogic/PaintController.hpp"
#include "../Canvas.hpp"
#include "PluginTypesProxy.hpp"
#include "RenderTarget.hpp"
#include "AppInterface.hpp"

AppInterfaceImpl::AppInterfaceImpl(PaintController *controller) : mController(controller)
{
    std_version = PSTD_VERSION;

    factory.target = new RenderTargetFactoryImpl();
    factory.widget = nullptr;
    factory.shader = nullptr;
}

AppInterfaceImpl::~AppInterfaceImpl()
{
    delete factory.target;
}

bool AppInterfaceImpl::ext_enable(const char *name) const
{
    // Ignore extensions now
    return false;
}

void *AppInterfaceImpl::ext_get_func(const char *extension, const char *func) const
{
    Logger::log(LogLevel::Warning, "Some plugin tried to access extensions, but it's not supported");
    return nullptr;
}

void *AppInterfaceImpl::ext_get_interface(const char *extension, const char *name) const
{
    Logger::log(LogLevel::Warning, "Some plugin tried to access extensions, but it's not supported");
    return nullptr;
}

void AppInterfaceImpl::log(const char *fmt, ...) const
{
    va_list args;
    va_start(args, fmt);
    Logger::log(LogLevel::Info, fmt, args);
    va_end(args);
}

double AppInterfaceImpl::get_absolute_time() const
{
    /// TODO:
    return 0;
}

P::RGBA AppInterfaceImpl::get_color() const
{
    return toPluginColor(mController->getCurrColor());
}

float AppInterfaceImpl::get_size() const
{
    return mController->getCurrSize();
}

void AppInterfaceImpl::set_color(const P::RGBA &color) const
{
    /// TODO:
    return;
}

void AppInterfaceImpl::set_size(float size) const
{
    /// TODO:
    return;
}

P::RenderTarget *AppInterfaceImpl::get_target() const
{
    Canvas *canvas = mController->getActiveCanvas();
    return new RenderTargetImpl(canvas->getLayer(canvas->getActiveLayer()), true);
}

P::RenderTarget *AppInterfaceImpl::get_preview() const
{
    Canvas *canvas = mController->getActiveCanvas();
    return new RenderTargetImpl(canvas->getPreviewLayer(), true);
}

void AppInterfaceImpl::flush_preview() const
{
    mController->getActiveCanvas()->flushPreview();
}