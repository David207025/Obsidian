#ifndef OBSIDIAN_H
#define OBSIDIAN_H


#include "./utils/Graphics.h"
#include "includes.h"


typedef glm::vec4 Color;

class Obsidian {
public:
    Obsidian(int width = 800, int height = 600, const char* title = "Obsidian Window");
    virtual ~Obsidian();

    Graphics* getGraphics() {return &m_graphics;};

    int getWindowWidth() const { return m_width; }
    int getWindowHeight() const { return m_height; }

    void run(bool cappedFPS);

    void addKeyBinding(int key, std::function<void(float)> action);

    void removeKeyBinding(int key);

protected:
    virtual void onCreate(Graphics& graphics) {}
    virtual void onDraw(Graphics& graphics) {};
    virtual void onDestroy() {};
    virtual void onFpsUpdate(float fps) {};
    virtual void onFrameDrawn(float deltaTime) {};

    GLFWwindow* getWindow() const;

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);


    void onResize(int width, int height);

    int m_width;
    int m_height;
    const char* m_title;
    GLFWwindow* m_window;
    Graphics m_graphics;
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point m_lastTime;
    Clock::time_point m_fpsLastReport;
    int m_frameCount = 0;
    std::unordered_map<int, std::function<void(float)>> keyBindings;

};

#endif // OBSIDIAN_H