
#include "graphics/render-layer.hpp"
#include "trillek-game.hpp"

namespace trillek {
namespace graphics {

RenderAttachment::RenderAttachment() {
    renderbuf = 0;
    this->attachtarget = GL_COLOR_ATTACHMENT0;
    this->multisample = false;
    this->clearonuse = false;
    this->multisample_texture = false;
    this->outputnumber = 0;
    this->clearstencil = 0;
    this->width = 0;
    this->height = 0;
    for(int i = 0; i < 4; i++) {
        this->clearvalues[i] = 0;
    }
}

RenderAttachment::~RenderAttachment() {
    Destroy();
}

RenderAttachment::RenderAttachment(RenderAttachment &&that) {
    this->renderbuf = that.renderbuf;
    this->texturename = std::move(that.texturename);
    this->attachtarget = that.attachtarget;
    this->multisample = that.multisample;
    this->multisample_texture = that.multisample_texture;
    this->clearonuse = that.clearonuse;
    this->outputnumber = that.outputnumber;
    this->clearstencil = that.clearstencil;
    this->width = that.width;
    this->height = that.height;
    for(int i = 0; i < 4; i++) {
        this->clearvalues[i] = that.clearvalues[i];
    }
    that.renderbuf = 0;
    this->texture = std::move(that.texture);
}

RenderAttachment& RenderAttachment::operator=(RenderAttachment &&that) {
    this->renderbuf = that.renderbuf;
    this->texturename = std::move(that.texturename);
    this->attachtarget = that.attachtarget;
    this->multisample = that.multisample;
    this->multisample_texture = that.multisample_texture;
    this->clearonuse = that.clearonuse;
    this->outputnumber = that.outputnumber;
    this->clearstencil = that.clearstencil;
    this->width = that.width;
    this->height = that.height;
    for(int i = 0; i < 4; i++) {
        this->clearvalues[i] = that.clearvalues[i];
    }
    that.renderbuf = 0;
    this->texture = std::move(that.texture);
    return *this;
}

bool RenderAttachment::SystemStart(const std::list<Property> &settings) {
    int width, height, samples;
    for(auto prop : settings) {
        if(prop.GetName() == "screen-width") {
            width = prop.Get<int>();
        }
        else if(prop.GetName() == "screen-height") {
            height = prop.Get<int>();
        }
        else if(prop.GetName() == "multisample") {
            this->multisample = prop.Get<bool>();
        }
        else if(prop.GetName() == "samples") {
            samples = prop.Get<int>();
        }
    }
    Generate(width, height, samples);
    return true;
}

bool RenderAttachment::SystemReset(const std::list<Property> &settings) {
    int samples;
    for(auto prop : settings) {
        if(prop.GetName() == "screen-width") {
            width = prop.Get<int>();
        }
        else if(prop.GetName() == "screen-height") {
            height = prop.Get<int>();
        }
        else if(prop.GetName() == "multisample") {
            this->multisample = prop.Get<bool>();
        }
        else if(prop.GetName() == "samples") {
            samples = prop.Get<int>();
        }
    }
    Destroy();
    Generate(width, height, samples);
    return true;
}

void RenderAttachment::Clear() {
    if(!this->clearonuse) {
        return; // return if we should not clear
    }
    switch(this->attachtarget) {
    case GL_COLOR_ATTACHMENT0:
        glDrawBuffer(GetAttach());
        glClearColor(clearvalues[0], clearvalues[1], clearvalues[2], clearvalues[3]);
        glClear(GL_COLOR_BUFFER_BIT);
        break;
    case GL_DEPTH_ATTACHMENT:
        glDrawBuffer(GL_DEPTH_ATTACHMENT);
        glClearDepth(clearvalues[0]);
        glClear(GL_DEPTH_BUFFER_BIT);
        break;
    case GL_STENCIL_ATTACHMENT:
        glDrawBuffer(GL_STENCIL_ATTACHMENT);
        glClearStencil(clearstencil);
        glClear(GL_STENCIL_BUFFER_BIT);
        break;
    case GL_DEPTH_STENCIL_ATTACHMENT:
        glDrawBuffer(GL_DEPTH_STENCIL_ATTACHMENT);
        glClearDepth(clearvalues[0]);
        glClearStencil(clearstencil);
        glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        break;
    }
}

bool RenderAttachment::Serialize(rapidjson::Document& document) {
    return false;
}

static std::string MakeString(const rapidjson::Value& v) {
    return std::string(v.GetString(), v.GetStringLength());
}

/*
 * Example json section
 *
  "attachment":{
    "coloroutput" : {
      "texture" : "newcolortexture",
      "target" : "color", // allowed are color, depth, stencil, depth-stencil
      "number" : 0, // only for color targets
      "clear" : [0, 0, 0, 0] // clear colors
    }
    "coloroutput2" : {
      "texture" : "newcolortexture2",
      "target" : "color",
      "number" : 0,
      "clear" : 0 // clear all colors
    }
    "depthstencil" : {
      "texture" : "newdepthtexture",
      "target" : "depth-stencil",
      "clear" : [0, 0] // clear depth
    }
  }
*/

bool RenderAttachment::Parse(const std::string &object_name, const rapidjson::Value& node) {
    if(!node.IsObject()) {
        // TODO use logger
        std::cerr << "[WARNING] Invalid attachment entry\n";
        return false;
    }
    for(auto attnode = node.MemberBegin(); attnode != node.MemberEnd(); attnode++) {
        std::string attribname = MakeString(attnode->name);
        if(attribname == "texture") {
            if(attnode->value.IsString()) {
                this->texturename = MakeString(attnode->value);
            }
            else {
                // TODO use logger
                std::cerr << "[ERROR] Attachment: Invalid texture name\n";
                return false;
            }
        }
        else if(attribname == "target") {
            if(attnode->value.IsString()) {
                std::string format = MakeString(attnode->value);
                if(format == "color" || format == "RGBA") {
                    this->attachtarget = GL_COLOR_ATTACHMENT0;
                }
                else if(format == "depth") {
                    this->attachtarget = GL_DEPTH_ATTACHMENT;
                }
                else if(format == "stencil") {
                    this->attachtarget = GL_STENCIL_ATTACHMENT;
                }
                else if(format == "depth-stencil") {
                    this->attachtarget = GL_DEPTH_STENCIL_ATTACHMENT;
                }
            }
            else {
                // TODO use logger
                std::cerr << "[ERROR] Attachment: Invalid target format\n";
                return false;
            }
        }
        else if(attribname == "number") {
            if(attnode->value.IsUint()) {
                this->outputnumber = attnode->value.GetUint();
            }
            else {
                // TODO use logger
                std::cerr << "[ERROR] Attachment: Invalid target\n";
                return false;
            }
        }
        else if(attribname == "clear") {
            if(attnode->value.IsArray()) {
                int index = 0;
                for(auto clearelem = attnode->value.Begin(); clearelem != attnode->value.End(); clearelem++) {
                    if(index > 3) {
                        std::cerr << "[WARNING] Attachment: Excess clearing value\n";
                        break;
                    }
                    if(clearelem->IsNumber()) {
                        this->clearvalues[index] = clearelem->GetDouble();
                        if(index <= 1 && clearelem->IsInt()) {
                            this->clearstencil = clearelem->GetInt();
                        }
                    }
                    else {
                        std::cerr << "[ERROR] Attachment: Invalid clearing value\n";
                        return false;
                    }
                    index++;
                }
                this->clearonuse = true;
            }
            else if(attnode->value.IsNumber()) {
                float clearval = attnode->value.GetDouble();
                if(attnode->value.IsInt()) {
                    this->clearstencil = attnode->value.GetInt();
                }
                for(int index = 0; index < 4; index++) {
                    this->clearvalues[index] = clearval;
                }
                this->clearonuse = true;
            }
            else {
                // TODO use logger
                std::cerr << "[ERROR] Attachment: Invalid clear\n";
                return false;
            }
        }
        else if(attribname == "multisample-texture") {
            if(attnode->value.IsBool()) {
                this->multisample_texture = attnode->value.GetBool();
            }
        }
    }
    return true;
}

void RenderAttachment::Generate(int width, int height, int samplecount) {
    if(multisample && !multisample_texture && renderbuf) {
        return;
    }
    if(texture) {
        if(texture->GetID()) return;
    }
    else {
        auto texptr = TrillekGame::GetGraphicSystem().Get<Texture>(texturename);
        if(texptr) {
            texture = std::move(texptr);
        }
        else {
            texture.reset(new Texture());
            TrillekGame::GetGraphicSystem().Add(texturename, texture);
        }
    }
    if(multisample) {
        if(multisample_texture) {
            switch(attachtarget) {
            case GL_DEPTH_ATTACHMENT:
                texture->GenerateMultisampleDepth(width, height, samplecount, false);
                break;
            case GL_STENCIL_ATTACHMENT:
                texture->GenerateMultisampleStencil(width, height, samplecount);
                break;
            case GL_DEPTH_STENCIL_ATTACHMENT:
                texture->GenerateMultisampleDepth(width, height, samplecount, true);
                break;
            default:
                texture->GenerateMultisample(width, height, samplecount);\
                break;
            }
        }
        else {
            glGenRenderbuffers(1, &renderbuf);
            switch(attachtarget) {
            case GL_DEPTH_ATTACHMENT:
                texture->GenerateDepth(width, height, false);
                break;
            case GL_STENCIL_ATTACHMENT:
                texture->GenerateStencil(width, height);
                break;
            case GL_DEPTH_STENCIL_ATTACHMENT:
                texture->GenerateDepth(width, height, true);
                break;
            default:
                texture->Generate(width, height, true);\
                break;
            }
        }
    }
    else {
        switch(attachtarget) {
        case GL_DEPTH_ATTACHMENT:
            texture->GenerateDepth(width, height, false);
            break;
        case GL_STENCIL_ATTACHMENT:
            texture->GenerateStencil(width, height);
            break;
        case GL_DEPTH_STENCIL_ATTACHMENT:
            texture->GenerateDepth(width, height, true);
            break;
        default:
            texture->Generate(width, height, true);\
            break;
        }
    }
}

void RenderAttachment::Destroy() {
    if(renderbuf) {
        glDeleteRenderbuffers(1, &renderbuf);
        renderbuf = 0;
    }
}

void RenderAttachment::BindTexture() {
    if(texture) {
        glBindTexture((multisample_texture ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), texture->GetID());
        CheckGLError();
    }
}

void RenderAttachment::AttachToFBO() {
    CheckGLError();
    if(renderbuf) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, this->attachtarget, GL_RENDERBUFFER, renderbuf);
        CheckGLError();
    }
    else {
        if(multisample_texture) {
            if(texture) {
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture->GetID());
                CheckGLError();
                glFramebufferTexture2D(GL_FRAMEBUFFER, this->attachtarget,
                        GL_TEXTURE_2D_MULTISAMPLE, this->texture->GetID(), 0);
                CheckGLError();
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                CheckGLError();
            }
        }
        else {
            if(texture) {
                glBindTexture(GL_TEXTURE_2D, texture->GetID());
                CheckGLError();
                glFramebufferTexture2D(GL_FRAMEBUFFER, this->attachtarget,
                        GL_TEXTURE_2D, this->texture->GetID(), 0);
                CheckGLError();
                glBindTexture(GL_TEXTURE_2D, 0);
                CheckGLError();
            }
        }
    }
}

RenderLayer::RenderLayer() {
    initialize_priority = 1; // start after all attachments
    width = 0;
    height = 0;
    fbo_id = 0;
}

RenderLayer::~RenderLayer() {
    Destroy();
}

static const char * GetFramebufferStatusMessage(GLuint status) {
    switch(status) {
    case GL_FRAMEBUFFER_COMPLETE:
        return "Complete";
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        return "Incomplete attachment";
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        return "Incomplete multisample buffer";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        return "Missing attachment";
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        return "Incomplete layer targets";
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        return "Incomplete draw buffer";
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        return "Incomplete read buffer";
    case GL_FRAMEBUFFER_UNSUPPORTED:
        return "Not supported (check graphics drivers)";
    }
    return "Unknown error";
}

bool RenderLayer::SystemStart(const std::list<Property> &settings) {
    this->attachments.clear();
    for(auto attachname : this->attachmentnames) {
        auto attachptr = TrillekGame::GetGraphicSystem().Get<RenderAttachment>(attachname);
        if(attachptr) {
            this->attachments.push_back(attachptr);
        }
        else {
            // TODO use logger
            std::cerr << "[ERROR] Layer: Attachment not found\n";
            this->attachments.clear();
            return false;
        }
    }
    Generate();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id); CheckGLError();
    for(auto attachptr : this->attachments) {
        attachptr->AttachToFBO();
    }

    GLuint status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER); CheckGLError();
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        // TODO Use logger
        std::cerr << "[ERROR] Framebuffer: " << GetFramebufferStatusMessage(status) << '\n';
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool RenderLayer::SystemReset(const std::list<Property> &settings) {
    return false;
}

bool RenderLayer::Serialize(rapidjson::Document& document) {
    return false;
}

void RenderLayer::Generate() {
    if(fbo_id) return;
    glGenFramebuffers(1, &fbo_id); CheckGLError();
}

void RenderLayer::Destroy() {
    if(fbo_id) {
        glDeleteFramebuffers(1, &fbo_id);
        fbo_id = 0;
    }
}

void RenderLayer::BindToRender() const {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id); CheckGLError();
    GLuint drawcount = this->attachments.size();
    GLenum *drawto = new GLenum[drawcount];
    int i = 0;
    for(auto attitr : this->attachments) {
        if(attitr) {
            attitr->Clear();
            drawto[i++] = attitr->GetAttach();
        }
        else {
            drawto[i++] = GL_COLOR_ATTACHMENT0;
        }
    }
    glDrawBuffers(drawcount, drawto); CheckGLError();
    delete drawto;
}

void RenderLayer::UnbindFromAll() const {
    glDrawBuffer(GL_BACK);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); CheckGLError();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); CheckGLError();
}

void RenderLayer::BindToRead() const {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_id); CheckGLError();
}

bool RenderLayer::Parse(const std::string &object_name, const rapidjson::Value& node) {
    if(!node.IsObject()) {
        // TODO use logger
        std::cerr << "[WARNING] Invalid render layer entry\n";
        return false;
    }
    for(auto attnode = node.MemberBegin(); attnode != node.MemberEnd(); attnode++) {
        std::string attribname = MakeString(attnode->name);
        if(attribname == "attach") {
            if(attnode->value.IsArray()) {
                for(auto attach = attnode->value.Begin(); attach != attnode->value.End(); attach++) {
                    if(attach->IsString()) {
                        this->attachmentnames.push_back(MakeString(*attach));
                    }
                    else {
                        // TODO use logger
                        std::cerr << "[ERROR] Layer: Invalid attachment\n";
                    }
                }
            }
        }
    }
    return true;
}

} // namespace graphics
} // namespace trillek