//
// Created by David Vacaroiu on 02.07.25.
//

#ifndef VERTEX_H
#define VERTEX_H

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
    Vertex(glm::vec3 position, glm::vec4 color, glm::vec2 uv) : position(position), color(color), uv(uv) {};

};

#endif //VERTEX_H

