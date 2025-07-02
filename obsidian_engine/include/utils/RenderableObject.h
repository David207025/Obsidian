//
// Created by David Vacaroiu on 02.07.25.
//

#ifndef RENDERABLEOBJECT_H
#define RENDERABLEOBJECT_H

class RenderableObject {
public:
    size_t id;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0f);

    virtual void draw(Graphics& graphics) = 0;
    virtual void drawBoundingProxy(Graphics& graphics) = 0;

    virtual ~RenderableObject() = default;
};

#endif //RENDERABLEOBJECT_H
