#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Camera.h"
#include "Texture.h"

#include "../includes.h"

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    Vertex(glm::vec3 position, glm::vec4 color) : position(position), color(color) {};
};

class Obsidian;

class Graphics {
    friend class Obsidian;
    void updateProjection();

    glm::mat4 m_projection;
    int m_windowWidth = 800;
    int m_windowHeight = 600;

public:


    Graphics();

    ~Graphics();

    // Initialize OpenGL resources (call after GL context is ready)
    bool initialize(const Obsidian& engine);

    void setProjection(const glm::mat4& proj);
    void setWindowSize(int width, int height);

    GLuint getDefaultTexture();

    // Shader loading
    bool loadShader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& shaderName);
    void useShader(const std::string& shaderName);

    // Drawing
    void clear(float r, float g, float b, float a);
    void drawTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
    void fillTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);// simple 3 vertices, each x,y,z
    //void drawRectangle(float x, float y, float width, float height,const glm::vec4& c00, const glm::vec4& c10,const glm::vec4& c11, const glm::vec4& c01);
    void drawCircle(const Vertex& vertex, float radius, int segments);
    void fillCircle(const Vertex& vertex, float radius, int segments);

    // Draw polygon with automatically generated UVs based on bounding box
    void fillPolygon(const std::vector<Vertex>& positions );
    void drawPolygon(const std::vector<Vertex>& positions);

    // Draw textured quad from 4 corner points, binds the texture and draws polygon
    void drawTexture(Texture texture,
                     const glm::vec3& p0,
                     const glm::vec3& p1,
                     const glm::vec3& p2,
                     const glm::vec3& p3);

    // Set uniforms (e.g. MVP matrix)
    void setUniformMat4(const std::string& shaderName, const std::string& uniform, const glm::mat4& matrix);
    void bindTexture(GLuint tex);
    // Cleanup
    void cleanup();

    Camera* getCamera();

private:
    void resize(int width, int height);
    GLuint compileShader(GLenum type, const std::string& source);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);

    std::unordered_map<std::string, GLuint> shaderPrograms;

    Camera m_camera = Camera(glm::vec3(0, 0, 0));


    // For example, VAO/VBO for rectangle drawing
    GLuint rectVAO = 0;
    GLuint rectVBO = 0;

    GLuint currentProgram = 0;
    GLuint whiteTexture = 0;
};

#endif // GRAPHICS_H