#include "../../include/utils/Graphics.h"
#include "../../include/obsidian.h"


// Vertex Shader
static const char* defaultVertexShader = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aUV;

uniform mat4 uMVP;

out vec4 vColor;
out vec2 vUV;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vColor = aColor;
    vUV = aUV;
}
)glsl";

// Fragment Shader
static const char* defaultFragmentShader = R"glsl(
#version 330 core
in vec4 vColor;
in vec2 vUV;
uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(uTexture, vUV);
    FragColor = texColor * vColor;
}
)glsl";

Graphics::Graphics() {
    updateProjection();
}

Graphics::~Graphics() {
    cleanup();
}

bool Graphics::initialize(const Obsidian &engine) {

    int width = engine.getWindowWidth();
    int height = engine.getWindowHeight();
    setWindowSize(width, height);

    // Corrected layout: 9 floats
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    resize(width, height);

    // Default 1x1 white texture
    unsigned char whitePixel[4] = { 255, 255, 255, 255 };
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return loadShader(defaultVertexShader, defaultFragmentShader, "default");
}

void Graphics::setProjection(const glm::mat4& proj) { m_projection = proj; }

void Graphics::setWindowSize(int width, int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    updateProjection();
}

void Graphics::resize(int width, int height) {
    setWindowSize(width, height);

   updateProjection();

}

void Graphics::updateProjection() {
    float aspect = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);
    float fov = glm::radians(45.0f); // Field of view in radians
    float near = 0.1f;               // Near clipping plane
    float far = 100.0f;              // Far clipping plane

    m_projection = glm::perspective(fov, aspect, near, far);
}

bool Graphics::loadShader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& shaderName) {
    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    if (!vertShader) return false;
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!fragShader) { glDeleteShader(vertShader); return false; }
    GLuint program = linkProgram(vertShader, fragShader);
    glDeleteShader(vertShader); glDeleteShader(fragShader);
    if (!program) return false;
    shaderPrograms[shaderName] = program;
    return true;
}

void Graphics::useShader(const std::string& shaderName) {
    auto it = shaderPrograms.find(shaderName);
    if (it != shaderPrograms.end()) {
        if (currentProgram != it->second) {
            glUseProgram(it->second);
            currentProgram = it->second;
        }
        // DO NOT bind whiteTexture here!
        // Also set sampler uniform here (optional)
        GLint loc = glGetUniformLocation(currentProgram, "uTexture");
        if (loc >= 0) glUniform1i(loc, 0);
    }
}

void Graphics::bindTexture(GLuint tex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << std::hex << err << std::dec << std::endl;
    }
    GLint loc = glGetUniformLocation(currentProgram, "uTexture");
    if (loc >= 0) glUniform1i(loc, 0);
}

void Graphics::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::drawTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    useShader("default");
    bindTexture(getDefaultTexture());

    float vertices[] = {
        // x, y, z,    r, g, b, a,     u, v
        v1.position.x, v1.position.y, v1.position.z, v1.color.r, v1.color.g, v1.color.b, v1.color.a, 0.0f, 0.0f,
        v2.position.x, v2.position.y, v2.position.z, v2.color.r, v2.color.g, v2.color.b, v2.color.a, 1.0f, 0.0f,
        v3.position.x, v3.position.y, v3.position.z, v3.color.r, v3.color.g, v3.color.b, v3.color.a, 0.0f, 1.0f,
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

    useShader("default");
    glm::mat4 mvp = m_projection * m_camera.getViewMatrix();
    setUniformMat4("default", "uMVP", mvp);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // reset

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Graphics::fillTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    useShader("default");
    bindTexture(getDefaultTexture());

    float vertices[] = {
        // x, y, z,    r, g, b, a,     u, v
        v1.position.x, v1.position.y, v1.position.z, v1.color.r, v1.color.g, v1.color.b, v1.color.a, 0.0f, 0.0f,
        v2.position.x, v2.position.y, v2.position.z, v2.color.r, v2.color.g, v2.color.b, v2.color.a, 1.0f, 0.0f,
        v3.position.x, v3.position.y, v3.position.z, v3.color.r, v3.color.g, v3.color.b, v3.color.a, 0.0f, 1.0f,
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

    useShader("default");
    glm::mat4 mvp = m_projection * m_camera.getViewMatrix();
    setUniformMat4("default", "uMVP", mvp);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Graphics::drawCircle(const Vertex& vertex, float radius, int segments) {
    useShader("default");
    bindTexture(getDefaultTexture());

    std::vector<float> vertices;
    vertices.reserve((segments + 2) * 9);

    // Center vertex (UV = 0.5, 0.5 - center of texture)
    vertices.push_back(vertex.position.x);
    vertices.push_back(vertex.position.y);
    vertices.push_back(0.0f);
    vertices.push_back(vertex.color.r);
    vertices.push_back(vertex.color.g);
    vertices.push_back(vertex.color.b);
    vertices.push_back(vertex.color.a);
    vertices.push_back(0.5f);
    vertices.push_back(0.5f);

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = vertex.position.x + radius * cos(angle);
        float y = vertex.position.y + radius * sin(angle);

        // UVs mapped from -radius..radius to 0..1 (circle centered at 0.5,0.5)
        float u = 0.5f + 0.5f * cos(angle);
        float v = 0.5f + 0.5f * sin(angle);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
        vertices.push_back(vertex.color.r);
        vertices.push_back(vertex.color.g);
        vertices.push_back(vertex.color.b);
        vertices.push_back(vertex.color.a);
        vertices.push_back(u);
        vertices.push_back(v);
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

    useShader("default");
    glm::mat4 mvp = m_projection * m_camera.getViewMatrix();
    setUniformMat4("default", "uMVP", mvp);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // reset

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Graphics::fillCircle(const Vertex& vertex, float radius, int segments) {
    useShader("default");
    bindTexture(getDefaultTexture());

    std::vector<float> vertices;
    vertices.reserve((segments + 2) * 9);

    // Center vertex (UV = 0.5, 0.5 - center of texture)
    vertices.push_back(vertex.position.x);
    vertices.push_back(vertex.position.y);
    vertices.push_back(0.0f);
    vertices.push_back(vertex.color.r);
    vertices.push_back(vertex.color.g);
    vertices.push_back(vertex.color.b);
    vertices.push_back(vertex.color.a);
    vertices.push_back(0.5f);
    vertices.push_back(0.5f);

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = vertex.position.x + radius * cos(angle);
        float y = vertex.position.y + radius * sin(angle);

        // UVs mapped from -radius..radius to 0..1 (circle centered at 0.5,0.5)
        float u = 0.5f + 0.5f * cos(angle);
        float v = 0.5f + 0.5f * sin(angle);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
        vertices.push_back(vertex.color.r);
        vertices.push_back(vertex.color.g);
        vertices.push_back(vertex.color.b);
        vertices.push_back(vertex.color.a);
        vertices.push_back(u);
        vertices.push_back(v);
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

    useShader("default");
    glm::mat4 mvp = m_projection * m_camera.getViewMatrix();
    setUniformMat4("default", "uMVP", mvp);

    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Graphics::drawPolygon(const std::vector<Vertex>& positions) {
    useShader("default");
    bindTexture(getDefaultTexture());
    if (positions.size() < 3) {
        std::cerr << "drawPolygon: need at least 3 vertices\n";
        return;
    }

    // Compute bounding box of polygon vertices
    float minX = positions[0].position.x, maxX = positions[0].position.x;
    float minY = positions[0].position.x, maxY = positions[0].position.y;

    for (const auto& pos : positions) {
        if (pos.position.x < minX) minX = pos.position.x;
        if (pos.position.x > maxX) maxX = pos.position.x;
        if (pos.position.y < minY) minY = pos.position.y;
        if (pos.position.y > maxY) maxY = pos.position.y;
    }

    std::vector<float> vertices;
    vertices.reserve(positions.size() * 9); // pos(3) + color(4) + uv(2)

    for (const auto& pos : positions) {
        float u = (pos.position.x - minX) / (maxX - minX);
        float v = (pos.position.y - minY) / (maxY - minY);

        vertices.push_back(pos.position.x);
        vertices.push_back(pos.position.y);
        vertices.push_back(pos.position.z);

        // White color (no tint)
        vertices.push_back(pos.color.r);
        vertices.push_back(pos.color.g);
        vertices.push_back(pos.color.b);
        vertices.push_back(pos.color.a);

        vertices.push_back(u);
        vertices.push_back(v);
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);

    // Color attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

    // UV attribute (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

    useShader("default");

    glm::mat4 mvp = m_projection * m_camera.getViewMatrix();
    setUniformMat4("default", "uMVP", mvp);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)positions.size());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // reset

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}


void Graphics::fillPolygon(const std::vector<Vertex>& positions) {
    useShader("default");
    bindTexture(getDefaultTexture());
    if (positions.size() < 3) {
        std::cerr << "drawPolygon: need at least 3 vertices\n";
        return;
    }

    // Compute bounding box of polygon vertices
    float minX = positions[0].position.x, maxX = positions[0].position.x;
    float minY = positions[0].position.x, maxY = positions[0].position.y;

    for (const auto& pos : positions) {
        if (pos.position.x < minX) minX = pos.position.x;
        if (pos.position.x > maxX) maxX = pos.position.x;
        if (pos.position.y < minY) minY = pos.position.y;
        if (pos.position.y > maxY) maxY = pos.position.y;
    }

    std::vector<float> vertices;
    vertices.reserve(positions.size() * 9); // pos(3) + color(4) + uv(2)

    for (const auto& pos : positions) {
        float u = (pos.position.x - minX) / (maxX - minX);
        float v = (pos.position.y - minY) / (maxY - minY);

        vertices.push_back(pos.position.x);
        vertices.push_back(pos.position.y);
        vertices.push_back(pos.position.z);

        // White color (no tint)
        vertices.push_back(pos.color.r);
        vertices.push_back(pos.color.g);
        vertices.push_back(pos.color.b);
        vertices.push_back(pos.color.a);

        vertices.push_back(u);
        vertices.push_back(v);
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);

    // Color attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

    // UV attribute (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

    useShader("default");

    glm::mat4 mvp = m_projection * m_camera.getViewMatrix();
    setUniformMat4("default", "uMVP", mvp);

    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)positions.size());

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

Camera* Graphics::getCamera() {
    return &m_camera;
}


void Graphics::drawTexture(Texture texture,
                           const glm::vec3& p0,
                           const glm::vec3& p1,
                           const glm::vec3& p2,
                           const glm::vec3& p3) {
    useShader("default");
    bindTexture(texture.getData());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Ensure the shader’s sampler uniform is set (assuming currentProgram is active)
    GLint loc = glGetUniformLocation(currentProgram, "uTexture");
    if (loc >= 0) glUniform1i(loc, 0);

    std::vector<glm::vec3> positions = { p0, p1, p2, p3 };
    std::vector<glm::vec2> uvs = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}
    };

    // Build vertex data manually here with colors = white (like drawPolygon)
    std::vector<float> vertices;
    vertices.reserve(4 * 9);

    for (size_t i = 0; i < 4; ++i) {
        vertices.push_back(positions[i].x);
        vertices.push_back(positions[i].y);
        vertices.push_back(positions[i].z);

        vertices.push_back(1.0f); // r
        vertices.push_back(1.0f); // g
        vertices.push_back(1.0f); // b
        vertices.push_back(1.0f); // a

        vertices.push_back(uvs[i].x);
        vertices.push_back(uvs[i].y);
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

    useShader("default");
    glm::mat4 mvp = m_projection * m_camera.getViewMatrix();
    setUniformMat4("default", "uMVP", mvp);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

bool Graphics::loadFont(const unsigned char* fontBuffer, int fontBufferSize, float pixelHeight) {
    // Allocate bitmap for baking font atlas
    unsigned char* bitmap = new unsigned char[m_font.bitmapWidth * m_font.bitmapHeight];
    memset(bitmap, 0, m_font.bitmapWidth * m_font.bitmapHeight);

    int bakeResult = stbtt_BakeFontBitmap(
        fontBuffer, 0, pixelHeight,
        bitmap, m_font.bitmapWidth, m_font.bitmapHeight,
        32, 96, m_font.cdata);

    if (bakeResult <= 0) {
        std::cerr << "Failed to bake font bitmap\n";
        delete[] bitmap;
        return false;
    }

    // Create OpenGL texture for the font atlas
    glGenTextures(1, &m_font.textureID);
    glBindTexture(GL_TEXTURE_2D, m_font.textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_font.bitmapWidth, m_font.bitmapHeight,
                 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] bitmap;
    return true;
}

void Graphics::renderText(const std::string& text, const glm::vec3& position, float scale,
                          glm::vec3 color, glm::vec3 rotationEuler) {
    if (m_font.textureID == 0) {
        std::cerr << "Font not loaded!\n";
        return;
    }

    useShader("text");

    glUniform3f(glGetUniformLocation(currentProgram, "textColor"), color.r, color.g, color.b);

    // Build model matrix: translate + rotate + scale (uniform scale for simplicity)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    if (rotationEuler != glm::vec3(0.0f)) {
        model = glm::rotate(model, glm::radians(rotationEuler.x), glm::vec3(1,0,0));
        model = glm::rotate(model, glm::radians(rotationEuler.y), glm::vec3(0,1,0));
        model = glm::rotate(model, glm::radians(rotationEuler.z), glm::vec3(0,0,1));
    }

    model = glm::scale(model, glm::vec3(scale));

    // Compose MVP = projection * view * model
    glm::mat4 mvp = m_projection * m_camera.getViewMatrix() * model;
    setUniformMat4("text", "uMVP", mvp);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_font.textureID);

    GLuint textVAO, textVBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    float x = 0.0f;  // Text quad coords start at (0,0) in local model space
    float y = 0.0f;

    for (char c : text) {
        if (c < 32 || c >= 128) continue;

        stbtt_bakedchar* b = &m_font.cdata[c - 32];

        float x0 = x + b->xoff;
        float y0 = y - b->yoff;
        float x1 = x0 + (b->x1 - b->x0);
        float y1 = y0 - (b->y1 - b->y0);

        float s0 = b->x0 / (float)m_font.bitmapWidth;
        float t0 = b->y0 / (float)m_font.bitmapHeight;
        float s1 = b->x1 / (float)m_font.bitmapWidth;
        float t1 = b->y1 / (float)m_font.bitmapHeight;

        float vertices[6][4] = {
            { x0, y0, s0, t0 },
            { x1, y0, s1, t0 },
            { x1, y1, s1, t1 },

            { x0, y0, s0, t0 },
            { x1, y1, s1, t1 },
            { x0, y1, s0, t1 }
        };

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += b->xadvance;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteBuffers(1, &textVBO);
    glDeleteVertexArrays(1, &textVAO);
}

void Graphics::setUniformMat4(const std::string& shaderName, const std::string& uniform, const glm::mat4& matrix) {
    auto it = shaderPrograms.find(shaderName);
    if (it == shaderPrograms.end()) {
        std::cerr << "Shader not found: " << shaderName << std::endl;
        return;
    }

    GLint location = glGetUniformLocation(it->second, uniform.c_str());
    if (location == -1) {
        std::cerr << "Uniform " << uniform << " not found in shader " << shaderName << std::endl;
        return;
    }

    glUseProgram(it->second);
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

GLuint Graphics::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint Graphics::linkProgram(GLuint vertShader, GLuint fragShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program link error:\n" << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

void Graphics::cleanup() {
    for (auto& [name, prog] : shaderPrograms) {
        glDeleteProgram(prog);
    }
    shaderPrograms.clear();
}

GLuint Graphics::getDefaultTexture() {
    return whiteTexture;
}

