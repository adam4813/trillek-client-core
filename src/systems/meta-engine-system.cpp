#include "trillek-game.hpp"
#include "systems/meta-engine-system.hpp"
#include "systems/physics.hpp"
#include "systems/graphics.hpp"
#include "systems/lua-system.hpp"

namespace trillek {
void MetaEngineSystem::ThreadInit() {
    TrillekGame::GetLuaSystem().ThreadInit();
    TrillekGame::GetPhysicsSystem().ThreadInit();
}

void MetaEngineSystem::RunBatch() const {
    TrillekGame::GetLuaSystem().RunBatch();
    TrillekGame::GetPhysicsSystem().RunBatch();
};

void MetaEngineSystem::HandleEvents(frame_tp timepoint) {
    TrillekGame::GetLuaSystem().HandleEvents(timepoint);
    TrillekGame::GetPhysicsSystem().HandleEvents(timepoint);
};

void MetaEngineSystem::Terminate() {
    TrillekGame::GetLuaSystem().Terminate();
    TrillekGame::GetPhysicsSystem().Terminate();
};
}
