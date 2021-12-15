#include "../../Editor/EditorPluginAPI/plugin_std.hpp"


struct MyPluginInterface : public PUPPY::PluginInterface {
    bool  ext_enable        (const char */*name*/)                            const override;
    void *ext_get_func      (const char */*extension*/, const char */*func*/) const override;
    void *ext_get_interface (const char */*extension*/, const char */*name*/) const override;

    const PUPPY::PluginInfo *get_info() const override;

    PUPPY::Status init   (const PUPPY::AppInterface*) const override;
    PUPPY::Status deinit ()                       const override;
    void      dump   ()                       const override;

    void on_tick(double dt)   const override;

    void effect_apply() const override;

    void tool_on_press  (const PUPPY::Vec2f &position)                 const override;
    void tool_on_release(const PUPPY::Vec2f &position)                 const override;
    void tool_on_move   (const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const override;

// settings
    void show_settings() const override;

//additional
    void draw(const PUPPY::Vec2f &position) const;

    PUPPY::Extensioner exts;
};

bool  MyPluginInterface::ext_enable        (const char */*name*/)                        const { return false;   }
void *MyPluginInterface::ext_get_func      (const char */*extension*/, const char *func) const { return exts.get(func); }
void *MyPluginInterface::ext_get_interface (const char */*extension*/, const char *name) const { return exts.get(name); }

