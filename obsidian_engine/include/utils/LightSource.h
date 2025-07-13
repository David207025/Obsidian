#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <iostream>
#include "../includes.h"

enum class LightType {
    Ambient,
    Directional,
    Point
};

struct LightSource {
    LightType type;
    glm::vec2 position;
    glm::vec2 direction; // Normalized
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float cutoff = glm::cos(glm::radians(12.5f)); // For spotlight if ever needed
    float radius = 100;

    // Shadow mapping resources
    GLuint shadowFBO = 0;
    GLuint shadowMap = 0;
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
    glm::mat4 lightProjection = glm::mat4(1.0f);

    LightSource(LightType type, glm::vec2 pos, glm::vec2 dir = glm::vec2(0.0f, -1.0f))
        : type(type), position(pos), direction(glm::normalize(dir)) {}

    void initShadowResources(int shadowWidth, int shadowHeight) {
        glGenFramebuffers(1, &shadowFBO);

        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                     shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = {1.f, 1.f, 1.f, 1.f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Shadow framebuffer not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void updateLightSpaceMatrix(int sceneWidth, int sceneHeight) {
        float near_plane = -1.0f;
        float far_plane = 1.0f;

        lightProjection = glm::ortho(
            0.0f, static_cast<float>(sceneWidth),
            0.0f, static_cast<float>(sceneHeight),
            near_plane, far_plane
        );

        glm::vec3 eye = glm::vec3(position, 1.0f);
        glm::vec3 center = eye + glm::vec3(direction, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 lightView = glm::lookAt(eye, center, up);
        lightSpaceMatrix = lightProjection * lightView;
    }

    void destroy() {
        if (shadowMap) {
            glDeleteTextures(1, &shadowMap);
            shadowMap = 0;
        }
        if (shadowFBO) {
            glDeleteFramebuffers(1, &shadowFBO);
            shadowFBO = 0;
        }
    }

    ~LightSource() {
        destroy();
    }
};

#endif // LIGHTSOURCE_H