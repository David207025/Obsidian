//
// Created by David Vacaroiu on 03.07.25.
//

#ifndef OBJECT_H
#define OBJECT_H

class Object {
public:
    std::vector<std::shared_ptr<Shape>> shapes;

    void addShape(const std::shared_ptr<Shape>& shape);

    void translate(const glm::vec3& offset);

    void rotate(const glm::vec3& origin, double pitch, double yaw);

    void scale(const glm::vec3& scaleFactor, const glm::vec3& scaleOrigin);
};

#endif //OBJECT_H
