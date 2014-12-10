#ifndef TRILLEKGAME_HPP_INCLUDED
#define TRILLEKGAME_HPP_INCLUDED

#include <memory>
#include <mutex>

namespace trillek {

namespace gui {
class GuiSystem;
}
class TrillekScheduler;
class MetaEngineSystem;
class FakeSystem;
class OS;

namespace graphics {
class RenderSystem;
}
namespace physics {
class PhysicsSystem;
}
namespace sound {
class System;
}
namespace script {
class LuaSystem;
}
namespace component {
class Shared;
class System;
class SystemValue;
}

class TrillekGame final {
public:

    TrillekGame();
    ~TrillekGame();
    void Initialize();
    void Terminate();

    /** \brief Return the scheduler
     *
     * \return TrillekScheduler& the scheduler
     */
    TrillekScheduler& GetScheduler() { return *scheduler.get(); }

    /** \brief Return the GLFW instance
     *
     * \return OS& the instance
     */
    OS& GetOS() { return *glfw_os.get(); };

    /** \brief Return the physics system instance
     *
     * \return physics::PhysicsSystem& the instance
     */
    physics::PhysicsSystem& GetPhysicsSystem() { return *phys_sys.get(); }

    /**
     * Get a reference to the GUI system
     */
    gui::GuiSystem& GetGUISystem();

    /** \brief Get the FakeSystem
     *
     * \return FakeSystem& the fake system
     */
    FakeSystem& GetFakeSystem() { return *fake_system.get(); }

    /** \brief Get the storage of shared components
     *
     * \return component::Shared& the storage
     *
     */
    component::Shared& GetSharedComponent() { return *shared_component.get(); };

    /** \brief Get the storage of system components stored by pointers
     *
     * \return component::System& the storage
     *
     */
    component::System& GetSystemComponent() { return *system_component.get(); };

    /** \brief Get the storage of system components stored by values
     *
     * \return component::SystemValue& the storage
     *
     */
    component::SystemValue& GetSystemValueComponent() { return *system_value_component.get(); };

    /** \brief Get the terminate flag
     *
     * The flag tells the world that the program will terminate
     *
     * \return bool true if we are about to terminate the program
     */
    bool GetTerminateFlag() { return close_window; };

    /** \brief Tells that the user tries to close the window
     *
     * This function is called by a callback set in GLFW
     */
    void NotifyCloseWindow() { close_window = true; };

    /** \brief Return the Lua system instance
    *
    * \return script::LuaSystem
    */
    script::LuaSystem& GetLuaSystem() { return *lua_sys.get(); };

    /** \brief Return the sound system instance
     *
     * \return sound::System&
     */
    sound::System& GetSoundSystem();

    /** \brief Return the graphic system instance
     *
     * \return graphics::System& the instance
     */
    graphics::RenderSystem& GetGraphicSystem();

    /** \brief Return the graphic system instance pointer
     *
     * \return std::shared_ptr<graphics::RenderSystem> the instance
     */
    std::shared_ptr<graphics::RenderSystem> GetGraphicsInstance();

    /** \brief Return the meta engine system instance
     *
     * This sytem wraps together some function calls of graphic and physics systems
     *
     * \return MetaEngineSystem& the instance
     */
    MetaEngineSystem& GetEngineSystem() { return *engine_sys.get(); };

    mutable std::mutex transforms_lock;
private:

    std::unique_ptr<TrillekScheduler> scheduler;
    std::unique_ptr<FakeSystem> fake_system;
    std::unique_ptr<physics::PhysicsSystem> phys_sys;
    std::unique_ptr<OS> glfw_os;
    std::unique_ptr<component::Shared> shared_component;
    std::unique_ptr<component::System> system_component;
    std::unique_ptr<component::SystemValue> system_value_component;
    bool close_window;

    std::once_flag once_graphics;
    std::shared_ptr<graphics::RenderSystem> gl_sys_ptr;
    std::unique_ptr<gui::GuiSystem> gui_system;
    std::unique_ptr<script::LuaSystem> lua_sys;
    std::unique_ptr<MetaEngineSystem> engine_sys;
};

extern TrillekGame game;
}

#endif // TRILLEKGAME_HPP_INCLUDED
