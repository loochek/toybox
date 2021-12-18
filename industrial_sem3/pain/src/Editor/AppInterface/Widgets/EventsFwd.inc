#define EVENTS_FWD_HEADER() \
    virtual void onRedrawThis() override; \
    virtual void onUpdateThis(float elapsedTime) override; \
    virtual void onMouseClickedThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) override; \
    virtual void onMouseReleasedThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) override; \
    virtual void onMouseMoveThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) override; \
    virtual EventResult onKeyPressed(LGL::KeyboardKey key, LGL::InputModifier modifier) override; \
    virtual EventResult onKeyReleased(LGL::KeyboardKey key, LGL::InputModifier modifier) override; \
    virtual void onMouseScroll(int scrollDelta) override;

#define EVENTS_FWD(CLASSNAME, BASECLASSNAME) \
    void CLASSNAME::onRedrawThis() \
    { \
        PUPPY::Event::Render event; \
        event.force = true; \
        mImpl->on_render(event); \
        BASECLASSNAME::onRedrawThis(); \
    } \
    \
    void CLASSNAME::onUpdateThis(float elapsedTime) \
    { \
        PUPPY::Event::Tick event; \
        event.dt = elapsedTime; \
        mImpl->on_tick(event); \
        BASECLASSNAME::onUpdateThis(elapsedTime); \
    } \
    \
    void CLASSNAME::onMouseClickedThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) \
    { \
        PUPPY::Event::MousePress event; \
        event.button = PUPPY::MouseButton::left; \
        event.position = toPluginVec(Vec2f(localMousePos)); \
        mImpl->on_mouse_press(event); \
        BASECLASSNAME::onMouseClickedThis(localMousePos, globalMousePos); \
    } \
    \
    void CLASSNAME::onMouseReleasedThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) \
    { \
        PUPPY::Event::MouseRelease event; \
        event.button = PUPPY::MouseButton::left; \
        event.position = toPluginVec(Vec2f(localMousePos)); \
        mImpl->on_mouse_release(event); \
        BASECLASSNAME::onMouseReleasedThis(localMousePos, globalMousePos); \
    } \
    \
    void CLASSNAME::onMouseMoveThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) \
    { \
        PUPPY::Event::MouseMove event; \
        event.from = toPluginVec(Vec2f(localMousePos)); \
        event.to   = toPluginVec(Vec2f(localMousePos)); \
        mImpl->on_mouse_move(event); \
        BASECLASSNAME::onMouseMoveThis(localMousePos, globalMousePos); \
    } \
    \
    EventResult CLASSNAME::onKeyPressed(LGL::KeyboardKey key, LGL::InputModifier modifier) \
    { \
        PUPPY::Event::KeyDown event((PUPPY::Keyboard::Key)key); \
        mImpl->on_key_down(event); \
        return BASECLASSNAME::onKeyPressed(key, modifier); \
    } \
    \
    EventResult CLASSNAME::onKeyReleased(LGL::KeyboardKey key, LGL::InputModifier modifier) \
    { \
        PUPPY::Event::KeyUp event((PUPPY::Keyboard::Key)key); \
        mImpl->on_key_up(event); \
        return BASECLASSNAME::onKeyReleased(key, modifier); \
    } \
    \
    void CLASSNAME::onMouseScroll(int scrollDelta) \
    { \
        PUPPY::Event::Scroll event; \
        event.delta = scrollDelta; \
        BASECLASSNAME::onMouseScroll(scrollDelta); \
    } \

