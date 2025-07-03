#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "./Camera.h"
#include "./Shape.h"
#include "./Font.h"
#include "../includes.h"

class Obsidian;

class Graphics {
    friend class Obsidian;

    glm::mat4 m_projection;
    int m_windowWidth = 800;
    int m_windowHeight = 600;

    void updateProjection();
    void resize(int width, int height);

public:
    Graphics();
    ~Graphics();

    bool initialize(const Obsidian& engine);

    void setProjection(const glm::mat4& proj);
    void setWindowSize(int width, int height);

    void clear(float r, float g, float b, float a);

    void addShape(std::shared_ptr<Shape> shape);
    void render();  // Renders all visible shapes

    // Shader / Texture / Uniform utilities
    bool loadShader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& shaderName);
    void useShader(const std::string& shaderName);
    void setUniformMat4(const std::string& shaderName, const std::string& uniform, const glm::mat4& matrix);
    void bindTexture(GLuint tex);

    GLuint loadTexture(const std::string& path, GLint filtering = GL_LINEAR);
    GLuint getDefaultTexture();

    // Text rendering
    bool loadFont(const unsigned char* fontBuffer, int fontBufferSize, float pixelHeight = 32.0f);
    void renderText(const std::string& text, const glm::vec3& position, float scale,
                    glm::vec3 color, glm::vec3 rotationEuler = glm::vec3(0.0f));

    void cleanup();

    Camera* getCamera();

private:
    GLuint compileShader(GLenum type, const std::string& source);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);

    std::unordered_map<std::string, GLuint> shaderPrograms;
    GLuint currentProgram = 0;

    GLuint whiteTexture = 0;
    GLuint VAO = 0, VBO = 0;

    Camera m_camera = Camera(glm::vec3(0, 0, 0));
    Font m_font;

    std::vector<std::shared_ptr<Shape>> shapes;  // Collection of shapes to render
};

#endif // GRAPHICS_H