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

Shape::Shape(const std::vector<Vertex>& verts,Texture texture, PrimitiveType drawType)
    : vertices(verts), type(drawType), texture(texture) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    updateBuffers();
}

void Shape::updateBuffers() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
}

void Shape::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shaderProgram) {
    if (!isVisible) return;

    glUseProgram(shaderProgram);

    glm::mat4 mvp = projection * view * modelMatrix;
    GLint loc = glGetUniformLocation(shaderProgram, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mvp[0][0]);


    glBindTexture(GL_TEXTURE_2D, texture.getData());

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
    // Bind simple shader that outputs white color only
    glUseProgram(shaderProgram);

    glm::mat4 mvp = projection * view * modelMatrix;
    GLint loc = glGetUniformLocation(shaderProgram, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mvp[0][0]);

    // Disable textures, colors, etc., or use a special shader that outputs constant white

    glBindVertexArray(vao);

    GLenum mode = GL_LINE_LOOP; // draw outlines
    switch (type) {
        case PrimitiveType::Triangles:
            mode = GL_LINE_LOOP; // could also do GL_LINES by indices if available
            break;
        case PrimitiveType::TriangleFan:
        case PrimitiveType::TriangleStrip:
        case PrimitiveType::Lines:
        case PrimitiveType::Points:
            mode = GL_LINE_LOOP; // fallback
            break;
    }

    glDrawArrays(mode, 0, (GLsizei)vertices.size());

    glBindVertexArray(0);
}


void Shape::translate(const glm::vec3 offset) {
    for (int i = 0; i < vertices.size(); i++) {
        vertices.at(i).position += offset;
    }
    updateBuffers();
}

void Shape::rotate(const glm::vec3 origin, float pitch, float yaw) {
    // Create a transformation matrix for rotation around origin
    glm::mat4 transform(1.0f);

    // 1. Translate so origin is at (0,0,0)
    transform = glm::translate(transform, -origin);

    // 2. Apply yaw (rotation around Y axis)
    transform = glm::rotate(transform, (float)glm::radians(yaw), glm::vec3(0, 1, 0));

    // 3. Apply pitch (rotation around X axis)
    transform = glm::rotate(transform, (float)glm::radians(pitch), glm::vec3(1, 0, 0));

    // 4. Translate back to original position
    transform = glm::translate(transform, origin);

    // Apply this transformation to all vertices
    for (auto& vert : vertices) {
        glm::vec4 pos = glm::vec4(vert.position, 1.0f);
        pos = transform * pos;
        vert.position = glm::vec3(pos);
    }

    // Update the GPU buffers
    updateBuffers();
}

void Shape::scale(const glm::vec3& factors, const glm::vec3& origin) {
    for (auto& vertex : vertices) {
        // Translate vertex so scaleOrigin is at origin
        glm::vec3 translated = vertex.position - origin;

        // Scale
        translated *= factors;

        // Translate back
        vertex.position = translated + origin;
    }

    updateBuffers();
}


std::shared_ptr<Shape> Shape::createSphere(float radius, int latitudeSegments, int longitudeSegments) {
    std::vector<Vertex> verts;

    for (int lat = 0; lat <= latitudeSegments; ++lat) {
        float theta = lat * glm::pi<float>() / latitudeSegments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longitudeSegments; ++lon) {
            float phi = lon * 2 * glm::pi<float>() / longitudeSegments;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            glm::vec3 pos(
                radius * sinTheta * cosPhi,
                radius * cosTheta,
                radius * sinTheta * sinPhi
            );

            glm::vec2 uv(
                1.0f - (float)lon / longitudeSegments,   // U goes from 1 to 0 left to right
                1.0f - (float)lat / latitudeSegments     // V goes from 1 to 0 bottom to top
            );

            verts.push_back(Vertex(pos, glm::vec4(1,1,1,1), uv));
        }
    }

    // Generate indices for triangle strips
    std::vector<Vertex> indexedVerts;
    for (int lat = 0; lat < latitudeSegments; ++lat) {
        for (int lon = 0; lon < longitudeSegments; ++lon) {
            int first = lat * (longitudeSegments + 1) + lon;
            int second = first + longitudeSegments + 1;

            // Two triangles per quad
            indexedVerts.push_back(verts[first]);
            indexedVerts.push_back(verts[second]);
            indexedVerts.push_back(verts[first + 1]);

            indexedVerts.push_back(verts[second]);
            indexedVerts.push_back(verts[second + 1]);
            indexedVerts.push_back(verts[first + 1]);
        }
    }

    return std::make_shared<Shape>(indexedVerts, generateWhiteTexture(),PrimitiveType::Triangles);
}

std::shared_ptr<Shape> Shape::createCuboid(const glm::vec3& size) {

    float hx = size.x * 0.5f;
    float hy = size.y * 0.5f;
    float hz = size.z * 0.5f;

    std::vector<Vertex> verts = {
        // Front face
        Vertex({-hx, -hy,  hz}, {1,1,1,1}, {0,0}),
        Vertex({ hx, -hy,  hz}, {1,1,1,1}, {1,0}),
        Vertex({ hx,  hy,  hz}, {1,1,1,1}, {1,1}),
        Vertex({-hx,  hy,  hz}, {1,1,1,1}, {0,1}),

        // Back face
        Vertex({ hx, -hy, -hz}, {1,1,1,1}, {0,0}),
        Vertex({-hx, -hy, -hz}, {1,1,1,1}, {1,0}),
        Vertex({-hx,  hy, -hz}, {1,1,1,1}, {1,1}),
        Vertex({ hx,  hy, -hz}, {1,1,1,1}, {0,1}),

        // Left face
        Vertex({-hx, -hy, -hz}, {1,1,1,1}, {0,0}),
        Vertex({-hx, -hy,  hz}, {1,1,1,1}, {1,0}),
        Vertex({-hx,  hy,  hz}, {1,1,1,1}, {1,1}),
        Vertex({-hx,  hy, -hz}, {1,1,1,1}, {0,1}),

        // Right face
        Vertex({ hx, -hy,  hz}, {1,1,1,1}, {0,0}),
        Vertex({ hx, -hy, -hz}, {1,1,1,1}, {1,0}),
        Vertex({ hx,  hy, -hz}, {1,1,1,1}, {1,1}),
        Vertex({ hx,  hy,  hz}, {1,1,1,1}, {0,1}),

        // Top face
        Vertex({-hx,  hy,  hz}, {1,1,1,1}, {0,0}),
        Vertex({ hx,  hy,  hz}, {1,1,1,1}, {1,0}),
        Vertex({ hx,  hy, -hz}, {1,1,1,1}, {1,1}),
        Vertex({-hx,  hy, -hz}, {1,1,1,1}, {0,1}),

        // Bottom face
        Vertex({-hx, -hy, -hz}, {1,1,1,1}, {0,0}),
        Vertex({ hx, -hy, -hz}, {1,1,1,1}, {1,0}),
        Vertex({ hx, -hy,  hz}, {1,1,1,1}, {1,1}),
        Vertex({-hx, -hy,  hz}, {1,1,1,1}, {0,1}),
    };

    // Indices for two triangles per face (6 faces)
    std::vector<Vertex> indexedVerts;
    const int faceCount = 6;
    for (int i = 0; i < faceCount; ++i) {
        int start = i * 4;
        // Triangle 1
        indexedVerts.push_back(verts[start]);
        indexedVerts.push_back(verts[start+1]);
        indexedVerts.push_back(verts[start+2]);
        // Triangle 2
        indexedVerts.push_back(verts[start]);
        indexedVerts.push_back(verts[start+2]);
        indexedVerts.push_back(verts[start+3]);
    }

    return std::make_shared<Shape>(indexedVerts, generateWhiteTexture(), PrimitiveType::Triangles);
}
std::shared_ptr<Shape> Shape::createPyramid(float baseWidth, float baseLength, float height ) {
    float hw = baseWidth * 0.5f;
    float hl = baseLength * 0.5f;

    glm::vec3 apex(0, 0, height);

    std::vector<Vertex> verts = {
        // Base (two triangles)
        Vertex({-hw, -hl, 0}, {1,1,1,1}, {0,0}),
        Vertex({ hw, -hl, 0}, {1,1,1,1}, {1,0}),
        Vertex({ hw,  hl, 0}, {1,1,1,1}, {1,1}),

        Vertex({-hw, -hl, 0}, {1,1,1,1}, {0,0}),
        Vertex({ hw,  hl, 0}, {1,1,1,1}, {1,1}),
        Vertex({-hw,  hl, 0}, {1,1,1,1}, {0,1}),

        // Sides (4 triangles)
        Vertex({-hw, -hl, 0}, {1,1,1,1}, {0,0}),
        Vertex({ hw, -hl, 0}, {1,1,1,1}, {1,0}),
        Vertex(apex, {1,1,1,1}, {0.5f, 1}),

        Vertex({ hw, -hl, 0}, {1,1,1,1}, {0,0}),
        Vertex({ hw,  hl, 0}, {1,1,1,1}, {1,0}),
        Vertex(apex, {1,1,1,1}, {0.5f, 1}),

        Vertex({ hw,  hl, 0}, {1,1,1,1}, {0,0}),
        Vertex({-hw,  hl, 0}, {1,1,1,1}, {1,0}),
        Vertex(apex, {1,1,1,1}, {0.5f, 1}),

        Vertex({-hw,  hl, 0}, {1,1,1,1}, {0,0}),
        Vertex({-hw, -hl, 0}, {1,1,1,1}, {1,0}),
        Vertex(apex, {1,1,1,1}, {0.5f, 1}),
    };

    return std::make_shared<Shape>(verts, generateWhiteTexture(), PrimitiveType::Triangles);
}

// 2D shapes with optional rotation around x and y axes
std::shared_ptr<Shape> Shape::createRectangle(float width, float height) {
    std::vector<Vertex> verts = {
        Vertex(glm::vec3(-width/2, -height/2, 0), glm::vec4(1,1,1,1), glm::vec2(0,0)),
        Vertex(glm::vec3( width/2, -height/2, 0), glm::vec4(1,1,1,1), glm::vec2(1,0)),
        Vertex(glm::vec3( width/2,  height/2, 0), glm::vec4(1,1,1,1), glm::vec2(1,1)),
        Vertex(glm::vec3(-width/2,  height/2, 0), glm::vec4(1,1,1,1), glm::vec2(0,1)),
    };
    return std::make_shared<Shape>(verts, generateWhiteTexture(), PrimitiveType::TriangleFan);
}

std::shared_ptr<Shape> Shape::createTriangle(Vertex v1, Vertex v2, Vertex v3) {
    std::vector<Vertex> verts = { v1, v2, v3 };
    return std::make_shared<Shape>(verts, generateWhiteTexture(), PrimitiveType::Triangles);
}
std::shared_ptr<Shape> Shape::createCircle(float radius, int segments) {

    std::vector<Vertex> verts;
    verts.reserve(segments + 2);

    // Center vertex
    verts.push_back(Vertex(glm::vec3(0, 0, 0), glm::vec4(1,1,1,1), glm::vec2(0.5f, 0.5f)));

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        verts.push_back(Vertex(glm::vec3(x, y, 0), glm::vec4(1,1,1,1), glm::vec2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle))));
    }

    return std::make_shared<Shape>(verts, generateWhiteTexture(), PrimitiveType::TriangleFan);
}
