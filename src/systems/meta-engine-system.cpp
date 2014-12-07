#include "trillek-game.hpp"
#include "systems/meta-engine-system.hpp"
#include "systems/physics.hpp"
#include "systems/graphics.hpp"
#include "systems/lua-system.hpp"

namespace trillek {
void MetaEngineSystem::ThreadInit() {
    game.GetLuaSystem().ThreadInit();
    game.GetPhysicsSystem().ThreadInit();
}

void MetaEngineSystem::RunBatch() const {
    game.GetLuaSystem().RunBatch();
    game.GetPhysicsSystem().RunBatch();
};

void MetaEngineSystem::HandleEvents(frame_tp timepoint) {
    game.GetLuaSystem().HandleEvents(timepoint);
    game.GetPhysicsSystem().HandleEvents(timepoint);
};

void MetaEngineSystem::Terminate() {
    game.GetLuaSystem().Terminate();
    game.GetPhysicsSystem().Terminate();
};
}
