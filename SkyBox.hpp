#ifndef SkyBox_hpp
#define SkyBox_hpp


#include "Shader.hpp"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <stdio.h>

namespace gps {
    class SkyBox
    {
    public:
        SkyBox();
        void Load(std::vector<const GLchar*> cubeMapFaces);
        void Draw(gps::Shader shader, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
        GLuint GetTextureId();
    private:
        GLuint skyboxVAO;
        GLuint skyboxVBO;
        GLuint cubemapTexture;
        GLuint LoadSkyBoxTextures(std::vector<const GLchar*> cubeMapFaces);
        void InitSkyBox();
    };
}

#endif /* SkyBox_hpp */
