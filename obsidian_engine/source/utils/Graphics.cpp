#include "../../include/includes.h"

// 2D Vertex Shader with lighting support (no shadows)
static const char* defaultVertexShader = R"glsl(
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aUV;

uniform mat4 uMVP;
uniform mat4 uModel;

out vec4 vColor;
out vec2 vUV;
out vec3 vFragPos;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 0.0, 1.0);
    gl_Position = uMVP * worldPos;
    vColor = aColor;
    vUV = aUV;
    vFragPos = worldPos.xyz;
}
)glsl";

// Default Fragment Shader (without shadows)
static const char* defaultFragmentShader = R"glsl(
#version 330 core

out vec4 FragColor;

struct Light {
    int type;       // 0=ambient,1=directional,2=point/spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float cutoff;   // spotlight cutoff cosine
};

#define MAX_LIGHTS 8

in vec4 vColor;
in vec2 vUV;
in vec3 vFragPos;

uniform sampler2D uTexture;
uniform Light uLights[MAX_LIGHTS];
uniform int uNumLights;

void main() {
    vec3 norm = vec3(0.0, 0.0, 1.0);
    vec3 result = vec3(0.0);
    vec4 texColor = texture(uTexture, vUV);

    for (int i = 0; i < uNumLights; ++i) {
        Light light = uLights[i];
        vec3 lightColor = light.color * light.intensity;
        vec3 lightContribution = vec3(0.0);

        if (light.type == 0) {
            // Ambient light
            lightContribution = lightColor;
        } else if (light.type == 1) {
            // Directional light (simple diffuse)
            vec3 lightDir = normalize(-light.direction);
            float diff = max(dot(norm, lightDir), 0.0);
            lightContribution = diff * lightColor;
        } else if (light.type == 2) {
            // Point/Spot light (simple radial falloff, no cone here)
            vec3 lightDir = normalize(light.position - vFragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            float distance = length(light.position - vFragPos);
            float attenuation = 1.0 / (distance * distance + 0.01);
            lightContribution = diff * lightColor * attenuation;
        }

        result += lightContribution;
    }

    vec3 finalColor = result * vColor.rgb * texColor.rgb;
    FragColor = vec4(finalColor, vColor.a * texColor.a);
}
)glsl";

// Fullscreen Quad Vertex Shader (for spotlight glow)
static const char* fullscreenQuadVertexShader = R"glsl(
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uMVP;

out vec2 vUV;
out vec2 vFragPos;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 0.0, 1.0);
    gl_Position = uMVP * worldPos;
    vFragPos = worldPos.xy;
    vUV = aUV;
}
)glsl";

// Fullscreen Quad Fragment Shader (spotlight glow effect)
static const char* fullscreenQuadFragmentShader = R"glsl(
#version 330 core

in vec2 vFragPos;
out vec4 FragColor;

uniform vec2 uLightPos;     // in world coordinates (pixels)
uniform vec2 uLightDir;     // normalized direction in world coords
uniform float uCutoff;
uniform vec3 uLightColor;
uniform float uIntensity;
uniform float uRadius;

void main() {
    vec2 toLight = normalize(vFragPos - uLightPos);
    float dist = distance(vFragPos, uLightPos);

    float theta = dot(toLight, normalize(uLightDir));
    float coneMask = smoothstep(uCutoff, uCutoff + 0.1, theta);

    float radialFalloff = clamp(1.0 - dist / uRadius, 0.0, 1.0);
    float intensity = coneMask * radialFalloff * uIntensity;

    if (intensity < 0.01)
        discard;

    FragColor = vec4(uLightColor, intensity);
}
)glsl";

GLuint fullscreenQuadVAO = 0, fullscreenQuadVBO = 0;

void createFullscreenQuad(int width, int height) {
    if (fullscreenQuadVAO != 0) return;

    float halfW = width * 0.5f;
    float halfH = height * 0.5f;

    float quadVertices[] = {
        // positions (in pixels)      // UVs
        -halfW, -halfH,  0.0f, 0.0f,
         halfW, -halfH,  1.0f, 0.0f,
         halfW,  halfH,  1.0f, 1.0f,

        -halfW, -halfH,  0.0f, 0.0f,
         halfW,  halfH,  1.0f, 1.0f,
        -halfW,  halfH,  0.0f, 1.0f,
    };

    glGenVertexArrays(1, &fullscreenQuadVAO);
    glGenBuffers(1, &fullscreenQuadVBO);
    glBindVertexArray(fullscreenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

Graphics::Graphics() {
    updateProjection();
}

Graphics::~Graphics() {
    cleanup();
}

bool Graphics::initialize(const Obsidian& engine) {
    int width = engine.getWindowWidth();
    int height = engine.getWindowHeight();

    setWindowSize(width, height);

    glViewport(0, 0, width, height);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Default white texture
    unsigned char whitePixel[4] = { 255, 255, 255, 255 };
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (!loadShader(defaultVertexShader, defaultFragmentShader, "default")) return false;
    if (!loadShader(fullscreenQuadVertexShader, fullscreenQuadFragmentShader, "fullscreenQuad")) return false;

    createFullscreenQuad(width, height);

    return true;
}

void Graphics::updateProjection() {
    float halfWidth = static_cast<float>(m_windowWidth) * 0.5f;
    float halfHeight = static_cast<float>(m_windowHeight) * 0.5f;

    m_projection = glm::ortho(
        -halfWidth, halfWidth,
        -halfHeight, halfHeight,
        -1.0f, 1.0f
    );
}

void Graphics::resize(int width, int height) {
    setWindowSize(width, height);
    updateProjection();
}

void Graphics::setProjection(const glm::mat4& proj) {
    m_projection = proj;
}

void Graphics::setWindowSize(int width, int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    updateProjection();
}

void Graphics::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

bool Graphics::loadShader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& shaderName) {
    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    if (!vertShader) return false;
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!fragShader) {
        glDeleteShader(vertShader);
        return false;
    }
    GLuint program = linkProgram(vertShader, fragShader);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
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

    if (whiteTexture) {
        glDeleteTextures(1, &whiteTexture);
        whiteTexture = 0;
    }

    if (fullscreenQuadVBO) {
        glDeleteBuffers(1, &fullscreenQuadVBO);
        fullscreenQuadVBO = 0;
    }
    if (fullscreenQuadVAO) {
        glDeleteVertexArrays(1, &fullscreenQuadVAO);
        fullscreenQuadVAO = 0;
    }
}

Camera* Graphics::getCamera() {
    return &m_camera;
}

void Graphics::addShape(std::shared_ptr<Shape> shape) {
    shapes.push_back(shape);
}

void Graphics::addLight(std::shared_ptr<LightSource> source) {
    if (!source) return;

    for (const auto& existing : lights) {
        if (existing == source) return;
    }

    lights.push_back(source);
}

void Graphics::render() {
    glm::mat4 view = m_camera.getViewMatrix();
    glm::mat4 projection = m_projection;

    // Clear screen
    clear(0, 0, 0, 1);

    // Combine shapes and lights into one sorted list
    struct RenderItem {
        enum class Kind { Light, Shape } kind;
        float depth;
        std::shared_ptr<LightSource> light;
        std::shared_ptr<Shape> shape;
    };

    std::vector<RenderItem> items;

    for (const auto& light : lights) {
        if (light) {
            items.push_back({ RenderItem::Kind::Light, light->depth, light, nullptr });
        }
    }

    for (const auto& shape : shapes) {
        if (shape && shape->isVisible) {
            items.push_back({RenderItem::Kind::Shape, shape->depth, nullptr, shape});
        }
    }

    std::sort(items.begin(), items.end(), [](const RenderItem& a, const RenderItem& b) {
        if (a.depth != b.depth)
            return a.depth < b.depth;
        return a.kind == RenderItem::Kind::Light && b.kind == RenderItem::Kind::Shape;
    });

    // Shader handles
    GLuint quadShader = shaderPrograms["fullscreenQuad"];
    GLuint defaultShader = shaderPrograms["default"];

    // Set light uniforms once for shapes
    useShader("default");
    GLint numLightsLoc = glGetUniformLocation(defaultShader, "uNumLights");
    glUniform1i(numLightsLoc, static_cast<int>(lights.size()));

    for (int i = 0; i < lights.size(); ++i) {
        const auto& light = lights[i];
        std::string base = "uLights[" + std::to_string(i) + "]";
        glm::vec3 pos3 = glm::vec3(light->position, 0.0f);
        glm::vec3 dir3 = glm::vec3(light->direction, 0.0f);

        glUniform1i(glGetUniformLocation(defaultShader, (base + ".type").c_str()), static_cast<int>(light->type));
        glUniform3fv(glGetUniformLocation(defaultShader, (base + ".position").c_str()), 1, &pos3[0]);
        glUniform3fv(glGetUniformLocation(defaultShader, (base + ".direction").c_str()), 1, &dir3[0]);
        glUniform3fv(glGetUniformLocation(defaultShader, (base + ".color").c_str()), 1, &light->color[0]);
        glUniform1f(glGetUniformLocation(defaultShader, (base + ".intensity").c_str()), light->intensity);
        glUniform1f(glGetUniformLocation(defaultShader, (base + ".cutoff").c_str()), light->cutoff);
    }

    // Draw all items sorted by depth
    for (const auto& item : items) {
        if (item.kind == RenderItem::Kind::Light) {
            const auto& light = item.light;
            if (light->type != LightType::Directional && light->type != LightType::Point) continue;

            useShader("fullscreenQuad");

            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 mvp = projection * view * model;

            glUniformMatrix4fv(glGetUniformLocation(quadShader, "uModel"), 1, GL_FALSE, &model[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(quadShader, "uMVP"), 1, GL_FALSE, &mvp[0][0]);

            glUniform2fv(glGetUniformLocation(quadShader, "uLightPos"), 1, &light->position[0]);
            glUniform2fv(glGetUniformLocation(quadShader, "uLightDir"), 1, &light->direction[0]);
            glUniform1f(glGetUniformLocation(quadShader, "uCutoff"), light->cutoff);
            glUniform3fv(glGetUniformLocation(quadShader, "uLightColor"), 1, &light->color[0]);
            glUniform1f(glGetUniformLocation(quadShader, "uIntensity"), light->intensity);
            glUniform1f(glGetUniformLocation(quadShader, "uRadius"), light->radius);

            glBindVertexArray(fullscreenQuadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

        } else if (item.kind == RenderItem::Kind::Shape) {
            const auto& shape = item.shape;
            useShader("default");

            GLint modelLoc = glGetUniformLocation(defaultShader, "uModel");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &shape->modelMatrix[0][0]);

            shape->draw(view, projection, defaultShader);
        }
    }
}