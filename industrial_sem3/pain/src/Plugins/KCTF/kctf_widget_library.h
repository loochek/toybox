#include "abstract_widget.h"

class RadioButton : public AbstractWidget {
public:
    RadioButton(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) : AbstractWidget(body, parent) {}
    virtual ~RadioButton() {}

    virtual bool get_on() const = 0;
    virtual void set_on(bool flag) = 0;

    using HandlerType = std::function<void()>;

    virtual void set_handler(const HandlerType &handler_) = 0;
    virtual HandlerType &get_handler() = 0;
};

struct SuperWidgetFactory {
    virtual ~SuperWidgetFactory() {}
    virtual RadioButton *radio_button(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr, PUPPY::RGBA off = {200, 100, 100}, PUPPY::RGBA on = {100, 200, 100}) = 0;
};
