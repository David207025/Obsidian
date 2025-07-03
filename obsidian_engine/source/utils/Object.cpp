
#include "../../include/includes.h"

void Object::addShape(const std::shared_ptr<Shape>& shape) {
    shapes.push_back(shape);
}

void Object::translate(const glm::vec3& offset) {
    for (auto& shape : shapes) {
        shape->translate(offset);
    }
}

void Object::rotate(const glm::vec3& origin, double pitch, double yaw) {
    for (auto& shape : shapes) {
        shape->rotate(origin, pitch, yaw);
    }
}

void Object::scale(const glm::vec3& scaleFactor, const glm::vec3& scaleOrigin) {
    for (auto& shape : shapes) {
        shape->scale(scaleFactor, scaleOrigin);
    }
}