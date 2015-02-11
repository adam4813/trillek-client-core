#include "trillek-game.hpp"
#include <queue>
#include <thread>
#include "os.hpp"
#include "util/json-parser.hpp"
#include "systems/resource-system.hpp"
#include "systems/physics.hpp"
#include "systems/meta-engine-system.hpp"
#include "systems/sound-system.hpp"
#include "systems/lua-system.hpp"
#include "systems/gui.hpp"
#include <cstddef>
#include "systems/graphics.hpp"
#include <iostream>

#include "systems/vcomputer-system.hpp"

size_t gAllocatedSize = 0;

int main(int argCount, char **argValues) {
    trillek::game.Initialize();
    std::cout << "Starting Trillek client..." << std::endl;
    // create the window
    auto& os = trillek::game.GetOS();
#if __APPLE__
    os.InitializeWindow(800, 600, "Trillek Client Core", 3, 2);
#else
    os.InitializeWindow(1024, 768, "Trillek Client Core", 3, 0);
#endif
    glGetError(); // clear errors

    // Call each system's GetInstance to create the initial instance.
    trillek::resource::ResourceMap::GetInstance();

    // start the physics system, must be done before loading any components.
    trillek::game.GetPhysicsSystem().Start();

    trillek::util::JSONPasrser jparser;

    if (!jparser.Parse("common/assets/tests/sample.json")) {
        std::cerr << "Error loading JSON configuration file." << std::endl;
    }

    trillek::sound::System& soundsystem = trillek::game.GetSoundSystem();
    std::shared_ptr<trillek::sound::Sound> s1 = soundsystem.GetSound("music_track_1");
    // needs to be a mono sound for 3d effects to work
    if (s1) {
        s1->Play();
    }
    // start the graphic system
    trillek::game.GetGraphicSystem().Start(os.GetWindowWidth(), os.GetWindowHeight());

    auto &gui = trillek::game.GetGUISystem();
    gui.Start();

    // we register the systems in this queue
    std::queue<trillek::SystemBase*> systems;

    // register the fake system. Comment this to cancel
//  systems.push((trillek::SystemBase*)&trillek::game.GetFakeSystem());

    // register the engine system, i.e physics + scripts
    systems.push(&trillek::game.GetEngineSystem());

    systems.push(&trillek::game.GetGraphicSystem());

    // register the sound system
    systems.push(&trillek::game.GetSoundSystem());

    // Start Lua system.
    trillek::game.GetLuaSystem().Start();

    // Load a test file/main Lua file.
    trillek::game.GetLuaSystem().LoadFile("common/assets/scripts/test.lua");

    // Detach the window from the current thread
    os.DetachContext();

    systems.push(&trillek::game.GetVComputerSystem());

    // start the scheduler in another thread
    std::thread tp(
                   &trillek::TrillekScheduler::Initialize,
                   &trillek::game.GetScheduler(),
                   5,
                   std::ref(systems));

    // Start the client network layer
/*    trillek::game.GetNetworkClient().SetTCPHandler<trillek::network::CLIENT>();

    // Start the server network layer and connect the client to the server
    if(! trillek::game.GetNetworkClient().Connect("localhost", 7777, "my_login", "secret password")) {
        trillek::game.NotifyCloseWindow();
    }
*/
    while(!os.Closing()) {
        os.OSMessageLoop();
    }
    tp.join();

    // Terminating program
    os.MakeCurrent();
    os.Terminate();

    //jparser.Serialize("common/assets/tests/", "transforms.json", trillek::TransformMap::GetInstance());
    trillek::game.Terminate();
    std::cout << "Number of bytes not freed: " << gAllocatedSize << std::endl;
    return 0;
}
