#ifndef SHAPE_H
#define SHAPE_H

#include "Texture.h"
#include "Vertex.h"


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
    std::string shaderName = "default";
    GLuint vao = 0;
    GLuint vbo = 0;
    Texture texture = Texture(0);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    bool isVisible = true;
    GLuint queryID = 0;  // For occlusion queries
    float depth = 0;

    Shape(const std::vector<Vertex>& verts, Texture tex, PrimitiveType primType = PrimitiveType::Triangles);

    void updateBuffers();
    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shaderProgram);

    void initQuery();
    void deleteQuery();
    void drawOcclusionQuery(const glm::mat4& view, const glm::mat4& projection, GLuint shaderProgram);

    void translate(const glm::vec2& offset);
    void rotate(float degrees, const glm::vec2& origin = glm::vec2(0.0f));
    void scale(const glm::vec2& factors, const glm::vec2& origin = glm::vec2(0.0f));

    // Factory functions
    static std::shared_ptr<Shape> createRectangle(float width = 1.0f, float height = 1.0f);
    static std::shared_ptr<Shape> createTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3);
    static std::shared_ptr<Shape> createCircle(float radius = 1.0f, int segments = 32);
};

#endif // SHAPE_H