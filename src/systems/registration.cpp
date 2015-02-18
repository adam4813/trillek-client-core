#include "trillek-game.hpp"
#include "graphics/renderable.hpp"
#include "graphics/camera.hpp"
#include "graphics/six-dof-camera.hpp"
#include "graphics/shader.hpp"
#include "graphics/light.hpp"
#include "graphics/render-layer.hpp"
#include "graphics/render-list.hpp"
#include "systems/graphics.hpp"
#include "systems/gui.hpp"
#include "systems/sound-system.hpp"
#include "systems/lua-system.hpp"
#include "resources/md5anim.hpp"
#include "resources/pixel-buffer.hpp"
#include "hardware/cpu.hpp"
#include "interaction.hpp"

#include "components/component-templates.hpp"
#include "components/component-factory.hpp"
#include "physics/collidable.hpp"
#include "resources/md5mesh.hpp"
#include "resources/md5anim.hpp"
#include "resources/obj.hpp"
#include "resources/pixel-buffer.hpp"
#include "resources/text-file.hpp"

#include "systems/physics.hpp"
#include "systems/resource-system.hpp"
#include "util/json-parser.hpp"

namespace trillek {
/** \brief Type of component
 *
 * DYNAMIC: The component is passed to the system and then stored in the
 * ComponentFactory container
 * SYSTEM : The component is passed to SystemComponent and is stored there
 * SHARED : The component is passed to SharedComponent and is stored there
 *
 * system_value is like system, but for primitive values that don't need a pointer
 * to be passed as argument of a fonction (bool, uint32_t).
 *
 * Only SHARED components can be shared between systems in different threads.
 * By default, DYNAMIC components are registered.
 *
 * The preferred choice is SYSTEM.
 */

using component::Component;

void graphics::RenderSystem::RegisterTypes() {
    // textures are implicitly instanced most of the time
    RegisterClassGenParser<graphics::Texture>();
    RegisterClassGenParser<graphics::Shader>();
    RegisterClassGenParser<graphics::RenderAttachment>();
    RegisterClassGenParser<graphics::RenderLayer>();
    RegisterClassGenParser<graphics::RenderList>();
    RegisterStaticParsers();
    RegisterListResolvers();
}

void gui::GuiSystem::RegisterTypes() {
    RegisterHandler("lua", &game.GetLuaSystem());
}

void util::JSONParser::RegisterTypes() {
    RegisterParser(game.GetGraphicsInstance());
    RegisterParser(sound::System::GetInstance());
    RegisterParser(resource::ResourceMap::GetInstance());
}

void util::JSONParser::RegisterComponentTypes() {
    auto& shared = game.GetSharedComponent();
    auto& system = game.GetSystemComponent();
    auto& system_value = game.GetSystemValueComponent();
    RegisterComponentType(ComponentFactory<SYSTEM, Component::Collidable>(system));
    RegisterComponentType(ComponentFactory<SHARED, Component::Velocity, bool>(shared));
    RegisterComponentType(ComponentFactory<SHARED, Component::VelocityMax, bool>(shared));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::ReferenceFrame, id_t>(system_value));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::IsReferenceFrame, bool>(system_value));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::CombinedVelocity>(system));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::OxygenRate, float>(system_value));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::Health, uint32_t>(system_value));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::Movable, bool>(system_value));
    RegisterComponentType(ComponentFactory<SHARED, Component::GraphicTransform, bool>(shared));
    RegisterComponentType(ComponentFactory<SHARED, Component::GameTransform, bool>(shared));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::VComputer>(system));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::VDisplay>(system));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::VKeyboard>(system));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::Renderable>(system));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::Light>(system));
    RegisterComponentType(ComponentFactory<SYSTEM, Component::Camera>(system));
}

void resource::ResourceMap::RegisterTypes() {
    RegisterResourceType<resource::TextFile>();
    RegisterResourceType<resource::MD5Mesh>();
    RegisterResourceType<resource::OBJ>();
    RegisterResourceType<resource::PixelBuffer>();
    RegisterResourceType<resource::MD5Anim>();
}

} // End of trillek
