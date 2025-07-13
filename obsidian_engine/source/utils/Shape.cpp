//
// Created by David Vacaroiu on 03.07.25.
//

#include "../../include/includes.h"

GLuint generateWhiteTexture() {
    GLuint whiteTexture;
    unsigned char whitePixel[4] = { 255, 255, 255, 255 };
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return whiteTexture;
}

Shape::Shape(const std::vector<Vertex>& verts, Texture texture, PrimitiveType drawType)
    : vertices(verts), type(drawType), texture(texture) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // No normal calculation in 2D
    updateBuffers();
}

void Shape::updateBuffers() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // position: vec2
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // color: vec4
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    // uv: vec2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
}

void Shape::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shaderProgram) {
    if (!isVisible) return;

    glm::mat4 model = modelMatrix;
    glm::mat4 mvp = projection * view * model;

    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uView"), 1, GL_FALSE, &view[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getData());
    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    glBindVertexArray(vao);

    GLenum mode = GL_TRIANGLES;
    switch (type) {
        case PrimitiveType::Triangles:    mode = GL_TRIANGLES;    break;
        case PrimitiveType::TriangleFan:  mode = GL_TRIANGLE_FAN; break;
        case PrimitiveType::TriangleStrip:mode = GL_TRIANGLE_STRIP; break;
        case PrimitiveType::Lines:        mode = GL_LINES;        break;
        case PrimitiveType::Points:       mode = GL_POINTS;       break;
    }

    glDrawArrays(mode, 0, (GLsizei)vertices.size());

    glBindVertexArray(0);
}

void Shape::initQuery() {
    if (queryID == 0)
        glGenQueries(1, &queryID);
}

void Shape::deleteQuery() {
    if (queryID != 0) {
        glDeleteQueries(1, &queryID);
        queryID = 0;
    }
}

void Shape::drawOcclusionQuery(const glm::mat4& view, const glm::mat4& projection, GLuint shaderProgram) {
    glUseProgram(shaderProgram);

    glm::mat4 mvp = projection * view * modelMatrix;
    GLint loc = glGetUniformLocation(shaderProgram, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(vao);

    // Using line loop for outlines
    GLenum mode = GL_LINE_LOOP;
    switch (type) {
        case PrimitiveType::Triangles:
        case PrimitiveType::TriangleFan:
        case PrimitiveType::TriangleStrip:
        case PrimitiveType::Lines:
        case PrimitiveType::Points:
            mode = GL_LINE_LOOP;
            break;
    }

    glDrawArrays(mode, 0, (GLsizei)vertices.size());

    glBindVertexArray(0);
}

void Shape::translate(const glm::vec2& offset) {
    for (auto& v : vertices) {
        v.position += offset;
    }
    updateBuffers();
}

void Shape::rotate(float degrees, const glm::vec2& origin) {
    float radians = glm::radians(degrees);
    float cosAngle = cos(radians);
    float sinAngle = sin(radians);

    for (auto& v : vertices) {
        glm::vec2 pos = v.position - origin;

        float xnew = pos.x * cosAngle - pos.y * sinAngle;
        float ynew = pos.x * sinAngle + pos.y * cosAngle;

        v.position = glm::vec2(xnew, ynew) + origin;
    }
    updateBuffers();
}

void Shape::scale(const glm::vec2& factors, const glm::vec2& origin) {
    for (auto& v : vertices) {
        glm::vec2 pos = v.position - origin;
        pos *= factors;
        v.position = pos + origin;
    }
    updateBuffers();
}

// 2D shapes factory functions

std::shared_ptr<Shape> Shape::createRectangle(float width, float height) {
    std::vector<Vertex> verts = {
        Vertex(glm::vec2(-width/2, -height/2), glm::vec4(1,1,1,1), glm::vec2(0,0)),
        Vertex(glm::vec2( width/2, -height/2), glm::vec4(1,1,1,1), glm::vec2(1,0)),
        Vertex(glm::vec2( width/2,  height/2), glm::vec4(1,1,1,1), glm::vec2(1,1)),
        Vertex(glm::vec2(-width/2,  height/2), glm::vec4(1,1,1,1), glm::vec2(0,1)),
    };
    return std::make_shared<Shape>(verts, Texture(generateWhiteTexture()), PrimitiveType::TriangleFan);
}

std::shared_ptr<Shape> Shape::createTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) {
    std::vector<Vertex> verts = {
        Vertex(p1, glm::vec4(1,1,1,1), glm::vec2(0,0)),
        Vertex(p2, glm::vec4(1,1,1,1), glm::vec2(0.5f,1)),
        Vertex(p3, glm::vec4(1,1,1,1), glm::vec2(1,0))
    };
    return std::make_shared<Shape>(verts, Texture(generateWhiteTexture()), PrimitiveType::Triangles);
}

std::shared_ptr<Shape> Shape::createCircle(float radius, int segments) {
    std::vector<Vertex> verts;
    verts.reserve(segments + 2);

    verts.push_back(Vertex(glm::vec2(0, 0), glm::vec4(1,1,1,1), glm::vec2(0.5f, 0.5f)));

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        verts.push_back(Vertex(glm::vec2(x, y), glm::vec4(1,1,1,1), glm::vec2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle))));
    }

    return std::make_shared<Shape>(verts, Texture(generateWhiteTexture()), PrimitiveType::TriangleFan);
}

