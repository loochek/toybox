#include "../../Utils/Logger.hpp"
#include "../../BaseGUI/Window.hpp"
#include "../../EditorLogic/PaintController.hpp"
#include "../../EditorWidgets/PaintMainWindow.hpp"
#include "../Canvas.hpp"
#include "Widgets/PluginWidget.hpp"
#include "PluginTypesProxy.hpp"
#include "RenderTarget.hpp"
#include "WidgetFactory.hpp"
#include "AppInterface.hpp"
#include "../Extensions/ExtensionManager.hpp"

AppInterfaceImpl::AppInterfaceImpl(Plugin *plugin, PaintController *controller) :
    mPlugin(plugin), mController(controller)
{
    std_version = PUPPY::STD_VERSION;

    factory.target = new RenderTargetFactoryImpl();
    factory.widget = new WidgetFactoryImpl(controller);
    factory.shader = nullptr;

    mRootWrap = new PluginWidgetImpl(mController->getRoot());
}

AppInterfaceImpl::~AppInterfaceImpl()
{
    delete factory.target;
    delete factory.widget;
    delete mRootWrap;
}

bool AppInterfaceImpl::ext_enable(const char *name) const
{
    return mController->getExtMgr()->isExtensionPresent(name);
}

void *AppInterfaceImpl::ext_get_func(const char *extension, const char *func) const
{
    return mController->getExtMgr()->getFunc(extension, func);
}

void *AppInterfaceImpl::ext_get_interface(const char *extension, const char *name) const
{
    return mController->getExtMgr()->getInterface(extension, name);
}

void AppInterfaceImpl::ext_register_as(const char *extension) const
{
    mController->getExtMgr()->registerExtension(extension, mPlugin);
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

PUPPY::RGBA AppInterfaceImpl::get_color() const
{
    return toPluginColor(mController->getCurrColor());
}

float AppInterfaceImpl::get_size() const
{
    return mController->getCurrSize();
}

void AppInterfaceImpl::set_color(const PUPPY::RGBA &color) const
{
    /// TODO:
    return;
}

void AppInterfaceImpl::set_size(float size) const
{
    /// TODO:
    return;
}

PUPPY::RenderTarget *AppInterfaceImpl::get_target() const
{
    Canvas *canvas = mController->getActiveCanvas();
    return new RenderTargetImpl(canvas->getLayer(canvas->getActiveLayer()), true);
}

PUPPY::RenderTarget *AppInterfaceImpl::get_preview() const
{
    Canvas *canvas = mController->getActiveCanvas();
    return new RenderTargetImpl(canvas->getPreviewLayer(), true);
}

void AppInterfaceImpl::flush_preview() const
{
    mController->getActiveCanvas()->flushPreview();
}

const std::vector<PUPPY::WBody> AppInterfaceImpl::get_windows() const
{
    std::vector<PUPPY::WBody> bodies;
    for (Widget *child : mController->getRoot()->getChildren())
    {
        if (dynamic_cast<Window*>(child) != nullptr && child->isEnabled())
            bodies.push_back(toPluginRect(child->getRect()));
    }
        
    return bodies;
}

PUPPY::Widget *AppInterfaceImpl::get_root_widget() const
{
    return mRootWrap;
}