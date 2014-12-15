#include "systems/lua-system.hpp"
#include <iostream>

namespace trillek {
namespace script {

int luaopen_Transform(lua_State*);
int luaopen_LuaSys(lua_State*);
int luaopen_PhysSys(lua_State*);
int luaopen_OSSys(lua_State*);
int luaopen_GuiSys(lua_State*);
int luaopen_Entity(lua_State*);

void LuaSystem::RegisterTypes() {
    // TODO: Move this into a location that makes more sense.
    RegisterSystem(&luaopen_Transform);
    RegisterSystem(&luaopen_LuaSys);
    RegisterSystem(&luaopen_PhysSys);
    RegisterSystem(&luaopen_OSSys);
    RegisterSystem(&luaopen_GuiSys);
    RegisterSystem(&luaopen_Entity);
}

} // End of script
} // End of trillek
