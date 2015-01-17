#ifndef RENDERABLE_HPP_INCLUDED
#define RENDERABLE_HPP_INCLUDED

#include "opengl.hpp"
#include "type-id.hpp"
#include <memory>
#include <vector>
#include "components/component.hpp"

namespace trillek {
namespace resource {

class Mesh;
class PixelBuffer;

} // End of resource

namespace graphics {

class RenderSystem;
struct LoadStatus;
class Animation;

class Renderable final : public ComponentBase {
public:
    friend class RenderSystem;
    Renderable();
    ~Renderable();

    /**
     * Sets the mesh resource associated with this component.
     *
     * \param std::shared_ptr<resource::Mesh> m The mesh resource for this component.
     */
    void SetMesh(std::shared_ptr<resource::Mesh> m);

    /**
     * Gets the mesh resource associated with this component.
     *
     * \return std::shared_ptr<resource::Mesh> The mesh resource for this component.
     */
    std::shared_ptr<resource::Mesh> GetMesh() const;

    /**
     * \brief Sets the animation for this component.
     * \param[in] const std::shared_ptr<Animation> a The animation for this component.
     */
    void SetAnimation(std::shared_ptr<Animation> a);

    /**
     * Gets the animation for this component.
     * \return const std::shared_ptr<Animation> The animation for this component.
     */
    std::shared_ptr<Animation> GetAnimation() const;

    /**
     * \brief Gets an instance texture.
     * \return std::shared_ptr<PixelBuffer> The pixel buffer for the texture.
     */
    std::shared_ptr<resource::PixelBuffer> GetInstanceTexture(size_t index) const {
        if(index < inst_textures.size()) {
            return std::shared_ptr<resource::PixelBuffer>(inst_textures.at(index));
        }
        return std::shared_ptr<resource::PixelBuffer>();
    }
    /**
     * \brief Initializes the component with the provided properties
     *
     * Valid properties include mesh (the mesh resource name) and shader (the shader resource name).
     * \param[in] const std::vector<Property>& properties The creation properties for the component.
     * \return bool True if initialization finished with no errors.
     */
    bool Initialize(const std::vector<Property> &properties);
private:
    std::shared_ptr<LoadStatus> loadstatus;

    std::string shader_name;
    std::shared_ptr<resource::Mesh> mesh;
    std::shared_ptr<Animation> animation;

    std::vector<bool> set_inst_textures;
    std::vector<std::shared_ptr<resource::PixelBuffer>> inst_textures;

    id_t entity_id;
};

} // End of graphics

namespace reflection {
TRILLEK_MAKE_IDTYPE_NAME(graphics::Renderable, "renderable", 2000)
} // End of reflection

} // End of trillek

#endif
