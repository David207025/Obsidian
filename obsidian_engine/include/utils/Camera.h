#ifndef CAMERA_H
#define CAMERA_H

#include "../includes.h"

class Camera {
    glm::mat4 m_viewMatrix = glm::mat4(1.0f);

    void updateViewMatrix() {
        // In 2D, camera position is x,y; no pitch/yaw
        // View matrix = translate(-position)
        m_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));
    }

public:
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    float zoom = 1.0f;  // Scale factor

    Camera(glm::vec2 startPos = glm::vec2(0.0f), float startZoom = 1.0f)
        : position(startPos), zoom(startZoom) {
        updateViewMatrix();
    }

    glm::mat4 getViewMatrix() const {
        // Apply zoom scaling
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, 1.0f));
        return scaleMat * m_viewMatrix;
    }

    void setPosition(const glm::vec2& pos) {
        position = pos;
        updateViewMatrix();
    }

    void move(const glm::vec2& delta) {
        position += delta;
        updateViewMatrix();
    }

    void setZoom(float newZoom) {
        zoom = newZoom;
        updateViewMatrix();
    }
};

#endif // CAMERA_H