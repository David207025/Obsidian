//
// Created by David Vacaroiu on 02.07.25.
//

#ifndef VERTEX_H
#define VERTEX_H

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    Vertex(glm::vec3 position, glm::vec4 color) : position(position), color(color) {};
};

#endif //VERTEX_H
