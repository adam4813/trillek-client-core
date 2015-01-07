#include "systems/lua-system.hpp"
#include "logging.hpp"
#include <iostream>

namespace trillek {
namespace script {

std::string lua_tostdstring(lua_State* L, int p) {
    size_t strl;
    const char * strc;
    if(!lua_isstring(L, p)) {
        return std::string();
    }
    strc = lua_tolstring(L, p, &strl);
    return std::string(strc, strl);
}

void lua_pushbool(lua_State* L, bool b) {
    lua_pushboolean(L, b?1:0);
}
bool lua_tobool(lua_State* L, int index) {
    return lua_toboolean(L, index) != 0;
}

int luaopen_Transform(lua_State*);
int luaopen_LuaSys(lua_State*);

LuaSystem::LuaSystem() {
    event::Dispatcher<KeyboardEvent>::GetInstance()->Subscribe(this);
    this->event_handlers[reflection::GetTypeID<KeyboardEvent>()];
    event::Dispatcher<MouseBtnEvent>::GetInstance()->Subscribe(this);
    this->event_handlers[reflection::GetTypeID<MouseBtnEvent>()];
    event::Dispatcher<MouseMoveEvent>::GetInstance()->Subscribe(this);
    this->event_handlers[reflection::GetTypeID<MouseMoveEvent>()];
}
LuaSystem::~LuaSystem() { }

void LuaSystem::Start() {
    this->L = luaL_newstate();
    luaL_openlibs(L);
    RegisterTypes();
}

void LuaSystem::RegisterSystem(LuaRegisterFunction func) {
    if (this->L) {
        func(this->L);
    }
}

bool LuaSystem::LoadFile(const std::string fname) {
    if (!this->L) {
        return false;
    }
    if (luaL_dofile(L, fname.c_str())) {
        // TODO: Ommit error string about the filename that failed to load.
        std::cerr << lua_tostring(L, -1) << std::endl;
        return false;
    }
    return true;
}

void LuaSystem::HandleEvents(frame_tp timepoint) {
    static frame_tp last_tp;
    this->delta = timepoint - last_tp;
    last_tp = timepoint;
    if (this->L) {
        // Call the registered event handlers in Lua.
        auto evh_itr = this->event_handlers.find(reflection::GetTypeID<KeyboardEvent>());
        if(evh_itr != this->event_handlers.end()) {
            for(auto& key_event : event_key.Poll()) {
                for(auto& handler : evh_itr->second) {
                    lua_getglobal(L, handler.c_str());
                    if(key_event.action == KeyboardEvent::KEY_DOWN) {
                        lua_pushstring(L, "Down");
                    }
                    else if(key_event.action == KeyboardEvent::KEY_UP) {
                        lua_pushstring(L, "Up");
                    }
                    else if(key_event.action == KeyboardEvent::KEY_REPEAT) {
                        lua_pushstring(L, "Repeat");
                    }
                    else {
                        lua_pushnil(L);
                    }
                    lua_pushnumber(L, key_event.key);
                    if(lua_pcall(L, 2, 0, 0)) {
                        size_t sl = 0;
                        const char *cs = lua_tolstring(L, -1, &sl);
                        LOGMSG(ERROR) << cs;
                    }
                }
            }
        }
        evh_itr = this->event_handlers.find(reflection::GetTypeID<MouseScrollEvent>());
        if(evh_itr != this->event_handlers.end()) {
            for(auto mousescroll : event_mscroll.Poll()) {
                for(auto& handler : evh_itr->second) {
                    lua_getglobal(L, handler.c_str());
                    lua_pushnumber(L, mousescroll.scroll_x);
                    lua_pushnumber(L, mousescroll.scroll_y);
                    if(lua_pcall(L, 2, 0, 0)) {
                        size_t sl = 0;
                        const char *cs = lua_tolstring(L, -1, &sl);
                        LOGMSG(ERROR) << cs;
                    }
                }
            }
        }
        evh_itr = this->event_handlers.find(reflection::GetTypeID<MouseMoveEvent>());
        if(evh_itr != this->event_handlers.end()) {
            for(auto mousemove : event_mmove.Poll()) {
                for(auto& handler : evh_itr->second) {
                    lua_getglobal(L, handler.c_str());
                    lua_pushnumber(L, mousemove.new_x);
                    lua_pushnumber(L, mousemove.new_y);
                    lua_pushnumber(L, mousemove.old_x);
                    lua_pushnumber(L, mousemove.old_y);
                    lua_pushnumber(L, mousemove.norm_x);
                    lua_pushnumber(L, mousemove.norm_y);
                    if(lua_pcall(L, 6, 0, 0)) {
                        size_t sl = 0;
                        const char *cs = lua_tolstring(L, -1, &sl);
                        LOGMSG(ERROR) << cs;
                    }
                }
            }
        }
        evh_itr = this->event_handlers.find(reflection::GetTypeID<MouseBtnEvent>());
        if(evh_itr != this->event_handlers.end()) {
            for(auto& mousebtn : event_mbtn.Poll()) {
                for (auto& handler : evh_itr->second) {
                    lua_getglobal(L, handler.c_str());
                    if(mousebtn.action == MouseBtnEvent::DOWN) {
                        lua_pushstring(L, "Down");
                    }
                    else if(mousebtn.action == MouseBtnEvent::UP) {
                        lua_pushstring(L, "Up");
                    }
                    else {
                        lua_pushnil(L);
                    }
                    switch(mousebtn.button) {
                    case MouseBtnEvent::LEFT:
                        lua_pushstring(L, "Left");
                        break;
                    case MouseBtnEvent::RIGHT:
                        lua_pushstring(L, "Right");
                        break;
                    case MouseBtnEvent::MIDDLE:
                        lua_pushstring(L, "Middle");
                        break;
                    case MouseBtnEvent::EX1:
                        lua_pushstring(L, "Button4");
                        break;
                    case MouseBtnEvent::EX2:
                        lua_pushstring(L, "Button5");
                        break;
                    default:
                        lua_pushnil(L);
                    }
                    if(lua_pcall(L, 2, 0, 0)) {
                        size_t sl = 0;
                        const char *cs = lua_tolstring(L, -1, &sl);
                        LOGMSG(ERROR) << cs;
                    }
                }
            }
        }
        // Call the registered update functions in Lua.
        evh_itr = this->event_handlers.find(1);
        if (evh_itr != this->event_handlers.end()) {
            std::unique_lock<std::mutex> locker(Lm);
            for(auto& handler : evh_itr->second) {
                lua_getglobal(L, handler.c_str());
                lua_pushnumber(L, delta * 1.0E-9);
                if(lua_pcall(L, 1, 0, 0)) {
                    size_t sl = 0;
                    const char *cs = lua_tolstring(L, -1, &sl);
                    LOGMSG(ERROR) << cs;
                }
            }
        }
    }
}

void LuaSystem::Terminate() {
    lua_close(L);
    L = nullptr;
}

void LuaSystem::AddUIEventType(uint32_t event_id, const std::string& event_class, const std::string& event_value) {
    std::unique_lock<std::mutex> locker(Lm);
    lua_getglobal(L, "_UI");
    int s = lua_gettop(L);
    lua_pushinteger(L, event_id);
    luaL_loadstring(L, event_value.c_str());
    lua_settable(L, s);
    lua_pop(L, 1);
}

void LuaSystem::RemoveUIEvent(uint32_t event_id) {
    if(!L) return;
    std::unique_lock<std::mutex> locker(Lm);
    lua_getglobal(L, "_UI");
    int s = lua_gettop(L);
    lua_pushinteger(L, event_id);
    lua_pushnil(L);
    lua_settable(L, s);
    lua_pop(L, 1);
}

void LuaSystem::UINotify(uint32_t event_id, const std::string& element_id) {
    std::unique_lock<std::mutex> locker(Lm);
    lua_getglobal(L, "_UI");
    int s = lua_gettop(L);
    lua_pushinteger(L, event_id);
    lua_gettable(L, s);
    if(lua_isfunction(L, lua_gettop(L))) {
        lua_pushstring(L, element_id.c_str());
        lua_pcall(L, 1, 0, 0);
    }
    lua_pop(L, 1);
}

void LuaSystem::Notify(const KeyboardEvent* key_event) {
    event_key.Push(*key_event);
}

void LuaSystem::Notify(const MouseBtnEvent* mousebtn_event) {
    event_mbtn.Push(*mousebtn_event);
}

void LuaSystem::Notify(const MouseScrollEvent* mousescroll) {
    event_mscroll.Push(*mousescroll);
}

void LuaSystem::Notify(const MouseMoveEvent* mousemove_event) {
    event_mmove.Push(*mousemove_event);
}

} // End of script
} // End of trillek

