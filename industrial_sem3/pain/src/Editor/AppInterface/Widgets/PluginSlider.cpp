#include "../RenderTarget.hpp"
#include "Logic/PSliderDelegate.hpp"
#include "PluginSlider.hpp"

PluginSliderIntl::PluginSliderIntl(const Vec2i &sliderPos, int sliderSize, PluginSliderImpl *impl, Widget *parent) :
    Slider(sliderPos, sliderSize, parent), mImpl(impl)
{
    mDelegate = new PSliderDelegate(this);
    setDelegate(mDelegate);
}

PluginSliderIntl::~PluginSliderIntl()
{
    delete mDelegate;
}

PluginSliderImpl::PluginSliderImpl(const PUPPY::WBody &body, PUPPY::Widget *parent) :
    PluginWidgetImpl(body, false, parent)
{
    ::Widget *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = parentImpl->getWidget();
    }

    ::Slider *slider = new PluginSliderIntl(fromPluginVec(body.position), body.size.x, this, parentWidget);
    slider->setMaxValue(SLIDER_PREC);
    mWidget = slider;

    if (parent != nullptr)
        parent->add_child(this);
}

PUPPY::Vec2f PluginSliderImpl::get_fraction_2d()
{
    return PUPPY::Vec2f();
}

void PluginSliderImpl::set_fraction_2d(const PUPPY::Vec2f &frac)
{
}

float PluginSliderImpl::get_fraction()
{
    ::Slider *slider = static_cast<::Slider*>(mWidget);
    return (float)slider->getValue() / SLIDER_PREC;
}

void PluginSliderImpl::set_fraction(float frac)
{
    ::Slider *slider = static_cast<::Slider*>(mWidget);
    slider->setValue(frac * SLIDER_PREC);
}