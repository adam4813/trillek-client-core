#ifndef LUA_HPP_INCLUDED
#define LUA_HPP_INCLUDED

#include <string>
#include <map>
#include <list>
#include <mutex>

#include "systems/dispatcher.hpp"
#include "atomic-queue.hpp"
#include "os-event.hpp"
#include "trillek.hpp"
#include "systems/system-base.hpp"
#include "systems/ui-event.hpp"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <luawrapper/luawrapper.hpp>
#include <luawrapper/luawrapperutil.hpp>

template<>
struct luaU_Impl<std::string> {
    static std::string luaU_check(lua_State* L, int index) {
        unsigned x;
        const char * cs = lua_tolstring(L, index, &x);
        return std::string(cs, x);
    }
    static std::string luaU_to(lua_State* L, int index) {
        unsigned x;
        const char * cs = lua_tolstring(L, index, &x);
        return std::string(cs, x);
    }
    static void luaU_push(lua_State* L, const std::string& val) {
        lua_pushlstring(L, val.data(), val.length());
    }
    static void luaU_push(lua_State* L, std::string& val) {
        lua_pushlstring(L, val.data(), val.length());
    }
};

namespace trillek {
namespace script {

typedef int (LuaRegisterFunction)(lua_State*);

class LuaSystem final :
    public SystemBase,
    public UIEventHandler,
    public event::Subscriber<KeyboardEvent>,
    public event::Subscriber<MouseBtnEvent>,
    public event::Subscriber<MouseScrollEvent>,
    public event::Subscriber<MouseMoveEvent> {
public:
    LuaSystem();
    ~LuaSystem();
    /**
     * \brief Starts the Lua scripting system.
     *
     */
    void Start();

    void ThreadInit() override { }

    /**
     * \brief Causes an update in the system based on the change in time.
     *
     * Updates the state of the system based off how much time has elapsed since the last update.
     */
    void RunBatch() const override { };

    /**
     * \brief Calls a systems's register function with this systems lua_State.
     *
     * \param LuaRegisterFunction The function to call with this system's lua_State.
     */
    void RegisterSystem(LuaRegisterFunction func);

    /**
     * \brief Load Lua script from a file.
     *
     * \param const std::string fname The name of the file to load.
     */
    bool LoadFile(const std::string fname);

    /** \brief Handle incoming events to update data
     *
     * This function is called once every frame. It is the only
     * function that can write data. This function is in the critical
     * path, job done here must be simple.
     *
     * If event handling need some batch processing, a task list must be
     * prepared and stored temporarily to be retrieved by RunBatch().
     */
    void HandleEvents(frame_tp timepoint) override;

    /** \brief Save the data and terminate the system
     *
     * This function is called when the program is closing
     */
    void Terminate() override;

    /**
     * \brief Handles keyboard events.
     */
    void Notify(const KeyboardEvent* key_event);

    /**
     * \brief Handles mouse button events.
     */
    void Notify(const MouseBtnEvent* mousebtn_event);

    /** Handles mouse scroll events.
     */
    void Notify(const MouseScrollEvent*);

    /**
     * \brief Handles mouse move events.
     */
    void Notify(const MouseMoveEvent* mousemove_event);

    /**
     * \brief called when UI adds an event.
     */
    void AddUIEventType(uint32_t event_id, const std::string& event_class, const std::string& event_value);

    /**
     * \brief called when UI removes an event.
     */
    void RemoveUIEvent(uint32_t event_id);

    /**
     * \brief Handles UI events.
     */
    void UINotify(uint32_t event_id, const std::string& element_id);

    /**
     * \brief Registers a script event handler
     *
     * \param int eventType The type of event to subscribe to.
     * \param std::string the name of the script function to call.
     */
    void RegisterEventSubscriber(const int eventType, const std::string function) {
        this->event_handlers[eventType].push_back(function);
    }
private:
    /**
     * \brief Registers all systems, classes, etc with Lua.
     *
     * This is called internally after Lua is started. The types are registered
     * for this instance.
     *
     * This function is defined in a separate source file to reduce compile times.
     * Internally it calls the templated RegisterSomething functions.
     */
    void RegisterTypes();

    AtomicQueue<KeyboardEvent> event_key;
    AtomicQueue<MouseBtnEvent> event_mbtn;
    AtomicQueue<MouseScrollEvent> event_mscroll;
    AtomicQueue<MouseMoveEvent> event_mmove;
    lua_State* L;
    mutable std::mutex Lm;
    long long delta; // The time since the last HandleEvents was called.
    std::map<int, std::list<std::string>> event_handlers; // Mapping of event ID to script function.
};

} // End of script
} // End of trillek
#endif
