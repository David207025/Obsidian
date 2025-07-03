//
// Created by David Vacaroiu on 02.07.25.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "../includes.h"
class Camera {
    glm::mat4 m_view =calculateViewMatrix();

    glm::mat4 calculateViewMatrix() const {
        // Convert degrees to radians if needed
        float pitchRad = glm::radians(pitch);
        float yawRad = glm::radians(yaw);

        glm::vec3 direction;
        direction.x = cos(pitchRad) * cos(yawRad);
        direction.y = sin(pitchRad);
        direction.z = cos(pitchRad) * sin(yawRad);

        direction = glm::normalize(direction);

        glm::vec3 m_camera_center = position + direction;

        return glm::lookAt(
        position,
        m_camera_center,
        glm::vec3(0.0f, 1.0f, 0.0f)    // Up
        );
    }
public:
    glm::vec3 position;
    float pitch; // up/down rotation in degrees
    float yaw;   // left/right rotation in degrees

    Camera(glm::vec3 startPos, float startPitch = 0.f, float startYaw = 0.f)
        : position(startPos), pitch(startPitch), yaw(startYaw) {}

    glm::mat4 getViewMatrix() {
        return m_view;
    }
    glm::vec3 getDirection() const {
        float pitchRad = glm::radians(pitch);
        float yawRad = glm::radians(yaw);

        glm::vec3 dir;
        dir.x = cos(pitchRad) * cos(yawRad);
        dir.y = sin(pitchRad);
        dir.z = cos(pitchRad) * sin(yawRad);
        return glm::normalize(dir);
    }

    void setOrientation(float pitch, float yaw) {
        this->pitch = pitch;
        this->yaw = yaw;
        m_view = calculateViewMatrix();
    }

    void setPosition(const glm::vec3& pos) {
        position = pos;
        m_view = calculateViewMatrix();
    }

    void moveCamera(const glm::vec3& offset, bool yawOnly) {
        glm::vec3 forward;

        if (yawOnly) {
            // Only yaw affects movement
            float yawRad = glm::radians(yaw);
            forward = glm::normalize(glm::vec3(
                cos(yawRad),
                0.0f,
                sin(yawRad)
            ));
        } else {
            // Use full pitch + yaw for movement direction
            forward = getDirection();  // your existing function
        }

        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        glm::vec3 up = glm::vec3(0, 1, 0);

        position += right * offset.x;    // strafe right/left
        position += up * offset.y;       // move up/down
        position += forward * offset.z;  // move forward/backward

        m_view = calculateViewMatrix();  // recalculate view matrix
    }
};

#endif //CAMERA_H
