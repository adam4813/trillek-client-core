#include "systems/resource-system.hpp"
#include "systems/graphics.hpp"
#include "trillek-game.hpp"

#include "resources/mesh.hpp"
#include "resources/md5anim.hpp"

#include "graphics/renderable.hpp"
#include "graphics/texture.hpp"
#include "graphics/shader.hpp"
#include "graphics/animation.hpp"

#include <sstream>

namespace trillek {
namespace graphics {

Renderable::Renderable() { }
Renderable::~Renderable() { }

/* TODO reimplement this elsewhere, and delete
 * Renderable::UpdateBufferGroups()
    for (size_t i = 0; i < this->mesh->GetMeshGroupCount(); ++i) {
        std::weak_ptr<resource::MeshGroup> mesh_group = this->mesh->GetMeshGroup(i);
        auto temp_meshgroup = mesh_group.lock();

        size_t texture_index = 0;
        for (std::string texture_name : temp_meshgroup->textures) {
            if(texture_name.length() > 0) {
                std::shared_ptr<Texture> texture = game.GetGraphicSystem().Get<Texture>(texture_name);
                if(texture_index < set_inst_textures.size()) {
                    if(set_inst_textures[texture_index]) {
                        std::vector<Property> props;
                        props.push_back(Property("filename", texture_name));
                        std::stringstream name;
                        name << this->entity_id << "%" << texture_index;
                        auto pixel_data = resource::ResourceMap::Create<resource::PixelBuffer>(name.str(), props);
                        if(pixel_data) {
                            inst_textures.push_back(pixel_data);
                            texture = std::make_shared<Texture>(pixel_data, true);
                            game.GetGraphicSystem().Add(name.str(), texture);
                }}}
                if(!texture) {
                    std::vector<Property> props;
                    props.push_back(Property("filename", texture_name));
                    auto pixel_data = resource::ResourceMap::Create<resource::PixelBuffer>(texture_name, props);
                    if (pixel_data) {
                        texture = std::make_shared<Texture>(pixel_data, false);
                        game.GetGraphicSystem().Add(texture_name, texture);
                }}
                if(texture) {
                    buffer_group->textures.push_back(texture);
                }
                texture_index++;
*/

void Renderable::SetMesh(std::shared_ptr<resource::Mesh> m) {
    this->mesh = m;
}

std::shared_ptr<resource::Mesh> Renderable::GetMesh() const {
    return this->mesh;
}

void Renderable::SetAnimation(std::shared_ptr<Animation> a) {
    this->animation = a;
}

std::shared_ptr<Animation> Renderable::GetAnimation() const {
    return this->animation;
}

bool Renderable::Initialize(const std::vector<Property> &properties) {
    std::string mesh_name;
    std::string shader_name;
    std::string animation_name;
    for (const Property& p : properties) {
        std::string name = p.GetName();
        if (name == "mesh") {
            mesh_name = p.Get<std::string>();
        }
        else if (name == "shader") {
            shader_name = p.Get<std::string>();
        }
        else if (name == "animation") {
            animation_name = p.Get<std::string>();
        }
        else if (name == "instanced_textures") {
            this->set_inst_textures = p.Get<std::vector<bool>>();
        }
        else if (name == "entity_id") {
            this->entity_id = p.Get<unsigned int>();
        }
    }

    this->mesh = resource::ResourceMap::Get<resource::Mesh>(mesh_name);
    if (!this->mesh) {
        return false;
    }

    auto animation_file = resource::ResourceMap::Get<resource::MD5Anim>(animation_name);
    if (animation_file) {
        // Make sure the mesh is valid for the animation file.
        if (animation_file->CheckMesh(this->mesh)) {
            this->animation = std::make_shared<Animation>();
            this->animation->SetAnimationFile(animation_file);
        }
        else {
            return false;
        }
    }

    return true;
}

} // End of graphics
} // End of trillek
