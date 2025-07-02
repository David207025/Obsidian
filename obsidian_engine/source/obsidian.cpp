#include "../include/obsidian.h"

Obsidian::Obsidian(int width, int height, const char *title)
    : m_width(width), m_height(height), m_title(title), m_window(nullptr), m_graphics() {
}

Obsidian::~Obsidian() {
    onDestroy();
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

void Obsidian::run(bool cappedFPS) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(cappedFPS ? 1 : 0); // 👈 disables V-Sync
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow *win, int w, int h) {
        auto *self = static_cast<Obsidian *>(glfwGetWindowUserPointer(win));
        self->onResize(w, h); // your custom handler
    });


    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }

    // Query the actual framebuffer size (important on Retina)
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
    m_width = fbWidth;
    m_height = fbHeight;

    // Set the viewport now with actual framebuffer size
    glViewport(0, 0, m_width, m_height);

    // Call resize on graphics BEFORE initialize so projection matches framebuffer size
    m_graphics.resize(m_width, m_height);

    m_lastTime = Clock::now();
    m_fpsLastReport = Clock::now();
    m_frameCount = 0;

    if (!m_graphics.initialize(*this)) {
        std::cerr << "Failed to initialize graphics\n";
        return;
    }

    // **Now tell Graphics about the real size BEFORE any draw calls:**
    onCreate(m_graphics);

    // std::cout << "Renderer: "   << glGetString(GL_RENDERER)   << std::endl;
    // std::cout << "Vendor: "     << glGetString(GL_VENDOR)     << std::endl;
    // std::cout << "Version: "    << glGetString(GL_VERSION)    << std::endl;
    // std::cout << "GLSL: "       << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;


    while (!glfwWindowShouldClose(m_window)) {
        onDraw(m_graphics);

        glfwSwapBuffers(m_window);
        glfwPollEvents();

        for (const auto& [key, action] : keyBindings) {
            if (glfwGetKey(m_window, key) == GLFW_PRESS) {
                action();  // Execute the lambda
            }
        }

        auto currentTime = Clock::now();
        std::chrono::duration<float> delta = currentTime - m_lastTime;
        m_lastTime = currentTime;

        // FPS tracking
        m_frameCount++;
        std::chrono::duration<float> fpsElapsed = currentTime - m_fpsLastReport;
        if (fpsElapsed.count() >= 1.0f) {
            float fps = m_frameCount / fpsElapsed.count();
            onFpsUpdate(fps); // optional: notify subclass
            m_fpsLastReport = currentTime;
            m_frameCount = 0;
        }

    }
}

void Obsidian::onResize(int width, int height) {
    glViewport(0, 0, width, height);
    m_graphics.resize(width, height); // ✅ allowed because Obsidian is a friend
}

void Obsidian::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Obsidian::addKeyBinding(int key, std::function<void()> action) {
    keyBindings[key] = std::move(action);
}

void Obsidian::removeKeyBinding(int key) {
    keyBindings.erase(key);
}


GLFWwindow *Obsidian::getWindow() const {
    return m_window;
}
