//
// Created by David Vacaroiu on 03.07.25.
//

#ifndef SHAPE_H
#define SHAPE_H

#include "../utils/Texture.h"

struct Vertex;

enum class PrimitiveType {
    Triangles,
    TriangleFan,
    TriangleStrip,
    Lines,
    Points
};

class Shape {
public:
    std::vector<Vertex> vertices;
    PrimitiveType type = PrimitiveType::Triangles;
    std::string shaderName;
    GLuint vao = 0;
    GLuint vbo = 0;
    Texture texture = Texture(0);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    bool isVisible = true; // Set by frustum/occlusion culling
    GLuint queryID = 0;

    Shape(const std::vector<Vertex>& vertices, Texture texture, PrimitiveType type = PrimitiveType::Triangles);

    void updateBuffers();
    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shaderProgram);
    void initQuery();
    void deleteQuery();
    void drawOcclusionQuery(const glm::mat4& view, const glm::mat4& projection, GLuint shaderProgram);

    void translate(const glm::vec3 offset);
    void rotate(const glm::vec3 origin, float pitch, float yaw);
    void scale(const glm::vec3& factors, const glm::vec3& origin);

    static std::shared_ptr<Shape> createSphere(float radius = 1.0f, int latitudeSegments = 16, int longitudeSegments = 16);
    static std::shared_ptr<Shape> createCuboid(const glm::vec3& size = glm::vec3(1.0f));
    static std::shared_ptr<Shape> createPyramid(float baseWidth = 1.0f, float baseLength = 1.0f, float height = 1.0f);

    // 2D shapes with optional rotation around x and y axes
    static std::shared_ptr<Shape> createRectangle(float width = 1.0f, float height = 1.0f);
    static std::shared_ptr<Shape> createTriangle(Vertex v1, Vertex v2, Vertex v3);
    static std::shared_ptr<Shape> createCircle(float radius = 1.0f, int segments = 32);

};

#endif //SHAPE_H
