#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "plugin_interface.h"
#include "abstract_widget.h"

#include "utils.h"

#include <cstdlib>
#include <ctime>
#include <cmath>

#ifndef isnan
#define isnan(x) !(x == x)
#endif

#include <functional>
#include <vector>

// ============================================================================ Info

const auto PTYPE = PUPPY::EXTENSION;

const char *PNAME    = "a_name";
const char *PVERSION = "a_vesion";
const char *PAUTHOR  = "an_author";
const char *PDESCR   = "a_description";

// ============================================================================ Resources

// ============================================================================ General

const MyPluginInterface PINTERFACE;

const PUPPY::PluginInfo PINFO =
{
    PUPPY::STD_VERSION, // std_version
    nullptr,            // reserved

    &PINTERFACE,

    PNAME,
    PVERSION,
    PAUTHOR,
    PDESCR,
    nullptr, // icon
    
    PTYPE
};

const PUPPY::AppInterface *APPI = nullptr;


extern "C" const PUPPY::PluginInterface *get_plugin_interface() {
    return &PINTERFACE;
}
 
// ============================================================================ Logic

double randdouble(double dmin, double dmax) {
    double x = (double) rand() / RAND_MAX;
    return dmin + x * (dmax - dmin);
}

class Tickable {
public:
    virtual void tick(double dt) = 0;
};


class Task : public Tickable {
    std::function<void()> action;
    double timer;

public:
    Task(const std::function<void()> &action, double timer):
    action(action),
    timer(timer)
    {}

    virtual ~Task() {}

    void tick(double dt) override {
        timer -= dt;
    }

    bool operator()() {
        if (timer < 0) {
            if (action) {
                action();
            }

            return true;
        }

        return false;
    }
};


class TaskManager : public Tickable {
    std::vector<Task*> tasks;
public:
    TaskManager() {}

    void add(Task* task) { if (!task) return; tasks.push_back(task); }

    void tick(double dt) override {
        for (size_t i = 0; i < tasks.size(); ) {
            tasks[i]->tick(dt);
            if ((*tasks[i])()) {
                delete tasks[i];
                std::swap(tasks[i], tasks[tasks.size() - 1]);
                tasks.pop_back();
            } else {
                ++i;
            }
        }
    }

};


struct ViewBody {
    Vec2f pos;
    Vec2f size;

    ViewBody(Vec2f pos, Vec2f size) :
    pos(pos),
    size(size)
    {}

    ViewBody(const PUPPY::WBody &body) :
    pos({body.position.x, body.position.y}),
    size({body.size.x, body.size.y})
    {}

    bool operator==(const ViewBody &other) const {
        return fabs((pos - other.pos).len()) < 0.1 && fabs((size - other.size).len()) < 0.1;
    }
};


class Animation;


class Unit : public AbstractWidget, public Tickable {
protected:
    Animation *animation;
    std::function<void(double)> ai;
    ViewBody unit_body;

    bool face_right;

public:

    Unit(const ViewBody &body, PUPPY::Widget *parent = nullptr) :
    AbstractWidget({{0, 0}, {body.size.x, body.size.y}}, nullptr),
    animation(nullptr),
    ai(nullptr),
    unit_body(body)
    {
        set_holder(APPI->factory.widget->abstract({{body.pos.x, body.pos.y}, {body.size.x, body.size.y}}, parent));
        
        set_parent(holder);
        holder->add_child(this);
    }

    virtual PUPPY::RenderTarget *get_texture() override;

    virtual void set_position(const PUPPY::Vec2f &position_) override { AbstractWidget::set_position(position_); unit_body.pos = {position_.x, position_.y}; }
    virtual void set_size(const PUPPY::Vec2f &size_) override { AbstractWidget::set_size(size_); unit_body.size = {size_.x, size_.y}; }
    virtual void set_body(const PUPPY::WBody &body_) override { AbstractWidget::set_body(body_); unit_body = body_; }

    const ViewBody &ubody() { return unit_body; }

    void set_ai(const std::function<void(double)> &new_ai) { ai = new_ai; }

    virtual void tick(double dt) override {
        ai(dt);
        focus();
    }
};


struct World : public Tickable {
    std::vector<ViewBody> windows;
    std::vector<Tickable*> ticking;
    std::vector<Unit*> units;
    TaskManager tasks;
    double time;
    
    PUPPY::Widget *root;

    World() :
    time(0),
    root(nullptr)
    {
        add(&tasks);
    }

    inline void add(Tickable *tickable) {
        ticking.push_back(tickable);
    }

    inline void add(Task *task) {
        tasks.add(task);
    }

    inline void add(Unit *unit) {
        units.push_back(unit);
        ticking.push_back(unit);
    }

    void tick(double dt) override {
        update();

        for (auto &t : ticking) {
            t->tick(dt);
        }
    }

    void update() {
        time = APPI->get_absolute_time();
        update_windows();
    }

    void update_windows() {
        auto bodies = APPI->get_windows();
        windows.clear();
        // printf("windows:\n");
        for (size_t i = 0; i < bodies.size(); ++i) {
            windows.push_back(bodies[i]);
            // printf("  %f %f %f %f\n", windows.back().pos.x, windows.back().pos.y, windows.back().size.x, windows.back().size.y);
        }
    }

} WORLD;


class Animation : public Tickable {
    std::vector<PUPPY::RenderTarget*> frames;
    size_t idx;

    double time;
    double frame_time;

public:
    Animation(const std::vector<PUPPY::RenderTarget*> &frames_, double frame_time) :
    frames(frames_),
    idx(0),
    time(0),
    frame_time(frame_time)
    {
        WORLD.add(this);
    }

    Animation(const std::vector<const char*> &frames_, double frame_time) :
    frames(),
    idx(0),
    time(0),
    frame_time(frame_time)
    {
        for (size_t i = 0; i < frames_.size(); ++i) {
            frames.push_back(APPI->factory.target->from_file(frames_[i]));
        }
        WORLD.add(this);
    }

    void tick(double dt) override {
        time += dt;
        while (time > frame_time) {
            time -= frame_time;
            idx = (idx + 1) % frames.size();
        }
    }

    PUPPY::RenderTarget *get_frame() {
        return frames[idx];
    }

    double get_length() const {
        return frame_time * frames.size();
    }

    void start() {
        time = 0;
        idx = 0;
    }
};


PUPPY::RenderTarget *Unit::get_texture() { if (animation) return animation->get_frame(); else return nullptr; }


class FixedPlaceAnimation : public Unit {
public:
    FixedPlaceAnimation(Animation *anm, double timer, const ViewBody &body, PUPPY::Widget *parent = nullptr) :
    Unit(body, parent)
    {
        animation = anm;
        anm->start();
        WORLD.add(new Task([this](){ set_to_delete(); }, std::min(timer, anm->get_length())));
    }
};


#define UNIT_AI(method_name) [this](double dt) {method_name(dt);}


class UFO : public Unit {
    Animation *anm_fly;
    Animation *anm_hide;
    Animation *anm_tp_disappear;
    Animation *anm_tp_appear;

    Animation *anm_scan_green;
    Animation *anm_boom;

    ViewBody target;
    Vec2f target_pos;
    float speed;
    bool on_ground;

    double idle_time;
    bool fixed_tpos = false;

public:
    UFO(const ViewBody &body, PUPPY::Widget *parent = nullptr) :
    Unit(body, parent),
    target(0, 0),
    target_pos(0),
    speed(200),
    on_ground(false),
    idle_time(0)
    {
        set_ai(UNIT_AI(ai_startup));

        anm_fly = new Animation({
                #define folder "./resources/ufolker/ufo/fly/"
                    folder "1.png",
                    folder "2.png",
                    folder "3.png",
                    folder "4.png"
                #undef folder
        }, 0.2);

        anm_hide = new Animation({
                #define folder "./resources/ufolker/ufo/hide/"
                    folder "1.png",
                    folder "2.png",
                    folder "3.png",
                    folder "4.png"
                #undef folder
        }, 0.2);

        anm_scan_green = new Animation(std::vector<const char *>{
                #define folder "./resources/ufolker/ufo/scan_green/"
                    folder "1.png",
                    folder "2.png",
                    folder "3.png",
                    folder "4.png",
                    folder "5.png",
                    folder "6.png",
                    folder "7.png"
                #undef folder
        }, 0.1);

        anm_boom = new Animation(std::vector<const char *>{
                #define folder "./resources/ufolker/ufo/boom/"
                    folder "1.png",
                    folder "2.png",
                    folder "3.png",
                    folder "4.png",
                    folder "5.png",
                    folder "6.png",
                    folder "7.png",
                    folder "8.png",
                    folder "9.png",
                    folder "10.png"
                #undef folder
        }, 0.1);
    }

    void gen_idle_time() {
        idle_time = rand() % 3 + 1.5;
    }

    void set_speed(float speed_) { speed = speed_; }

    void set_animation(std::string_view anm_name, Animation *anm) {
        if (anm_name == "fly") {
            anm_fly = anm;
        } else if (anm_name == "hide") {
            anm_hide = anm;
        } else if (anm_name == "tp_disappear") {
            anm_tp_disappear = anm;
        } else if (anm_name == "tp_appear") {
            anm_tp_appear = anm;
        } else if (anm_name == "scan_green") {
            anm_scan_green = anm;
        }
    }

    void choose_position_on_target() {
        for (int i = 0; i < 10 && is_near_target(true); ++i) {
            target_pos.x = target.pos.x + randdouble(unit_body.size.x / 2, target.size.x - unit_body.size.x / 2);
        }
    }

    bool target_remains_on_place() {
        for (const auto &window : WORLD.windows) {
            if (target == window) {
                return true;
            }
        }

        return false;
    }

    bool choose_target(bool force = false) {
        const auto &windows = WORLD.windows;
        if (!windows.size())
        {
            set_rand_target_pos();
            return true;
        }

        APPI->log("cur target %f %f %f %f", target.pos.x, target.pos.y, target.size.x, target.size.y);

        for (const auto &window : windows) {
            if (target == window && !force) {
                choose_position_on_target();
                return false;
            }
        }

        int idx = rand() % windows.size();
        target = windows[idx];

        target_pos = target.pos;
        target_pos.y -= unit_body.size.y;

        choose_position_on_target();
        APPI->log("target %f %f %f %f idx %d", target.pos.x, target.pos.y, target.size.x, target.size.y, idx);

        return true;
    }

    void move_to_target(double dt) {
        Vec2f vel = (target_pos - unit_body.pos).normal() * speed;
        Vec2f new_pos = unit_body.pos + vel * dt;
        set_position(PUPPY::Vec2f(new_pos.x, new_pos.y));

        APPI->log("vel %f %f", vel.x, vel.y);
    }

    bool is_near_target(bool widened = false) {
        if (!widened) return (target_pos - unit_body.pos).len() < 5;
        
        return (target_pos - unit_body.pos).len() < 20;
    }

    void set_rand_target_pos() {
        fixed_tpos = true;
        float x = (float) randdouble(0, WORLD.root->get_body().size.x - unit_body.size.x);
        float y = (float) randdouble(0, WORLD.root->get_body().size.y - unit_body.size.y);
        target_pos = {x, y};
    }

    virtual void on_mouse_press(const PUPPY::Event::MousePress &event) override {
        if (is_inside(event.position)) {
            set_ai(UNIT_AI(ai_hide));
        }
    }

// ---------------- ai

    void ai_startup(double) {
        set_ai(UNIT_AI(ai_choose_target));
    }

    void ai_hide(double) {
        fixed_tpos = true;
        on_ground = false;
        target = {0, 0};

        set_rand_target_pos();
        animation = anm_hide;
        set_ai(UNIT_AI(ai_move_to_target));

        new FixedPlaceAnimation(anm_boom, anm_boom->get_length(), unit_body, WORLD.root);
    }

    void ai_nothing(double) {}

    void ai_idle(double dt) {
        if (!isnan(dt)) {
            gen_idle_time();

            if (on_ground)
            switch (rand() % 3) {
                case 0:
                    set_ai(UNIT_AI(ai_scan_green));
                    return;

                default:
                    break;
            }
        }

        WORLD.add(new Task([this](){ ai_startup(0); }, idle_time));
        set_ai(UNIT_AI(ai_nothing));
    }

    void ai_scan_green(double) {
        idle_time = anm_scan_green->get_length() + randdouble(0.5, 1.5);
        new FixedPlaceAnimation(anm_scan_green, idle_time, unit_body, WORLD.root);
        ai_idle(NAN);
    }

    void ai_idle_arrived(double dt) {
        APPI->log("a nice window to explore!");
        ai_scan_green(dt);
    }

    void ai_idle_no_ground(double dt) {
        ai_idle(dt);
    }

    void ai_idle_near_ground(double dt) {
        ai_idle(dt);
    }

    void ai_ground_stolen(double dt) {
        APPI->log("give my window back you idiot");
        ai_idle(dt);
    }

    void ai_choose_target(double dt) {
        if (on_ground && !target_remains_on_place()) {
            ai_ground_stolen(dt);
            on_ground = false;
        }

        choose_target(false);
        animation = anm_fly;
        set_ai(UNIT_AI(ai_move_to_target));
    }

    void ai_choose_target_force(double) {
        choose_target(true);
    }

    void ai_move_to_target(double dt) {
        if (fixed_tpos) {
            move_to_target(dt);
            if (is_near_target()) {
                fixed_tpos = false;
                choose_target(true);
                set_ai(UNIT_AI(ai_idle));
            }
        } else if (target_remains_on_place()) {
            move_to_target(dt);

            if (is_near_target()) {
                if (!on_ground) {
                    on_ground = true;
                    set_ai(UNIT_AI(ai_idle_arrived));
                } else {
                    set_ai(UNIT_AI(ai_idle));
                }
            }

        } else {
            if (on_ground) {
                set_ai(UNIT_AI(ai_ground_stolen));
            } else {
                set_ai(UNIT_AI(ai_idle_no_ground));
            }
            on_ground = false;
        }
    }
};

// ============================================================================ Basics

PUPPY::Status MyPluginInterface::init(const PUPPY::AppInterface *app_interface) const {
    srand(time(NULL));

    APPI = app_interface;
    WORLD.root = APPI->get_root_widget();


    for (int i = 0; i < 5; i++)
    {
        auto ufo = new UFO({{500, 100}, 64}, WORLD.root);
        WORLD.add(ufo);
    }
    

    APPI->log("[plugin](%s) inited", PINFO.name);
    return PUPPY::OK;
}

PUPPY::Status MyPluginInterface::deinit() const {
    APPI->log("[plugin](%s) deinited | %s thanks you for using it", PINFO.name, PINFO.author);
    return PUPPY::OK;
}

void MyPluginInterface::show_settings() const {}

void MyPluginInterface::dump() const {
    APPI->log("[plugin](%s) is active", PINFO.name);
}

const PUPPY::PluginInfo *MyPluginInterface::get_info() const {
    return &PINFO;
}

void MyPluginInterface::on_tick(double dt) const {
    WORLD.tick(dt);
}

void MyPluginInterface::tool_on_press(const PUPPY::Vec2f &pos) const {
    draw(pos);
}

void MyPluginInterface::tool_on_move(const PUPPY::Vec2f &/*from*/, const PUPPY::Vec2f &to) const {
    draw(to);
}

void MyPluginInterface::tool_on_release(const PUPPY::Vec2f &/*pos*/) const {}

void MyPluginInterface::effect_apply() const {}

void MyPluginInterface::draw(const PUPPY::Vec2f &pos) const {
    float    size = APPI->get_size();
    PUPPY::RGBA color = APPI->get_color();

    auto preview = APPI->get_preview();

    preview->render_circle(pos, size, color, PUPPY::COPY);

    delete preview;
}