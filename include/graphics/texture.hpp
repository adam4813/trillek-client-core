#ifndef TEXTURE_HPP_INCLUDED
#define TEXTURE_HPP_INCLUDED

#include "resources/pixel-buffer.hpp"
#include "opengl.hpp"

namespace trillek {
namespace graphics {

class Texture : public resource::ResourceBase {
public:
    Texture() : texture_id(0) {}
    ~Texture();

    /**
     * \brief new texture instance from image
     */
    Texture(const resource::PixelBuffer &);

    // no copying (although it could be done)
    Texture(const Texture &) = delete;
    Texture& operator=(const Texture &) = delete;

    // Move textures
    Texture(Texture &&);
    Texture& operator=(Texture &&);

    /**
     * \brief delete the underlaying GL texture
     */
    void Destroy();

    /**
     * \brief get the ID of the texture
     * note: this method is not const, since GL can modify the texture with the ID
     * \return GLuint the GL texture ID
     */
    GLuint GetID() { return texture_id; }

    /**
     * \brief create a texture from an image
     */
    void Load(const resource::PixelBuffer &);

    /**
     * \brief create a blank texture RGB or RGBA format
     */
    void Generate(GLuint width, GLuint height, bool usealpha);

    bool operator==(const Texture & other) {
        return texture_id == other.texture_id;
    }
    bool Initialize(const std::vector<Property> &properties) { return true; }
protected:
    GLuint texture_id;

};

} // graphics

namespace reflection {

template <> inline const char* GetTypeName<graphics::Texture>() { return "texture"; }
template <> inline const unsigned int GetTypeID<graphics::Texture>() { return 2001; }

} // End of reflection
} // trillek

#endif