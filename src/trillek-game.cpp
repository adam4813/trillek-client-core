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
#include "systems/vcomputer-system.hpp"
#include "components/shared-component.hpp"
#include "components/system-component.hpp"
#include "components/system-component-value.hpp"
#include "interaction.hpp"

namespace trillek {

TrillekGame game;

TrillekGame::TrillekGame() {
    close_window = false;
}
TrillekGame::~TrillekGame() {}

void TrillekGame::Terminate() {
    vcomputer_system.reset();
    engine_sys.reset();
    gui_system.reset();
    lua_sys.reset();
    gl_sys_ptr.reset();
    fake_system.reset();
    os_system.reset();
    shared_component.reset();
    system_component.reset();
    system_value_component.reset();
    phys_sys.reset();
    scheduler.reset();
}

void TrillekGame::Initialize() {
    ActionText::RegisterStatic();
    scheduler.reset(new TrillekScheduler);
    fake_system.reset(new FakeSystem);
    phys_sys.reset(new physics::PhysicsSystem);
    os_system.reset(new SystemSystem);
    shared_component.reset(new component::Shared);
    system_component.reset(new component::System);
    system_value_component.reset(new component::SystemValue);
    lua_sys.reset(new script::LuaSystem());
    gl_sys_ptr.reset(new graphics::RenderSystem());
    gl_sys_ptr->RegisterTypes();
    gui_system.reset(new gui::GuiSystem(GetOS(), *gl_sys_ptr.get()));
    close_window = false;
    engine_sys.reset(new MetaEngineSystem);
    vcomputer_system.reset(new VComputerSystem);
}

sound::System& TrillekGame::GetSoundSystem() {
    return *sound::System::GetInstance();
}

graphics::RenderSystem& TrillekGame::GetGraphicSystem() {
    return *gl_sys_ptr.get();
}

OS& TrillekGame::GetOS() {
    return *(os_system->glfw_os).get();
}

id_t TrillekGame::GetCameraEntity() {
    return gl_sys_ptr->GetActiveCameraID();
}

gui::GuiSystem& TrillekGame::GetGUISystem() {
    return *gui_system.get();
}

std::shared_ptr<graphics::RenderSystem> TrillekGame::GetGraphicsInstance() {
    return std::shared_ptr<graphics::RenderSystem>(gl_sys_ptr);
}

} // End of namespace trillek
