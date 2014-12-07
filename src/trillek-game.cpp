#include "trillek-game.hpp"
#include "systems/gui.hpp"
#include "trillek-scheduler.hpp"
#include "os.hpp"
#include "systems/fake-system.hpp"
#include "systems/physics.hpp"
#include "systems/meta-engine-system.hpp"
#include "systems/sound-system.hpp"
#include "systems/graphics.hpp"
#include "systems/lua-system.hpp"
#include "systems/gui.hpp"
#include "components/shared-component.hpp"
#include "components/system-component.hpp"
#include "components/system-component-value.hpp"

namespace trillek {

TrillekGame game;

TrillekGame::TrillekGame() {
    close_window = false;
}
TrillekGame::~TrillekGame() {}

void TrillekGame::Initialize() {
    scheduler.reset(new TrillekScheduler);
    fake_system.reset(new FakeSystem);
    phys_sys.reset(new physics::PhysicsSystem);
    glfw_os.reset(new OS);
    shared_component.reset(new component::Shared);
    system_component.reset(new component::System);
    system_value_component.reset(new component::SystemValue);
    lua_sys.reset(new script::LuaSystem());
    gl_sys_ptr.reset(new graphics::RenderSystem());
    gl_sys_ptr->RegisterTypes();
    gui_system.reset(new gui::GuiSystem(*glfw_os.get(), *gl_sys_ptr.get()));
    close_window = false;
    engine_sys.reset(new MetaEngineSystem);
}

sound::System& TrillekGame::GetSoundSystem() {
    return *sound::System::GetInstance();
}

graphics::RenderSystem& TrillekGame::GetGraphicSystem() {
    return *gl_sys_ptr.get();
}

gui::GuiSystem& TrillekGame::GetGUISystem() {
    return *gui_system.get();
}

std::shared_ptr<graphics::RenderSystem> TrillekGame::GetGraphicsInstance() {
    return std::shared_ptr<graphics::RenderSystem>(gl_sys_ptr);
}

} // End of namespace trillek
