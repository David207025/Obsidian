//
// Created by David Vacaroiu on 03.07.25.
//

#ifndef OBJECT_H
#define OBJECT_H

#include "../includes.h"

class Graphics;
class Shape;

class Object {
public:
    std::vector<std::shared_ptr<Shape>> shapes;

    void addShape(const std::shared_ptr<Shape>& shape);

    void translate(const glm::vec3& offset);

    void rotate(const glm::vec3 & origin, double angle);

    void scale(const glm::vec3& scaleFactor, const glm::vec3& scaleOrigin);

    void addToGraphics(Graphics& gfx);
};

#endif //OBJECT_H
