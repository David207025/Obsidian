
#include <iostream>

#include "obsidian_engine/core.h"
#include "obsidian_engine/include/utils/Texture.h"

using namespace std;

class Engine : public Obsidian {
public:
    using Obsidian::Obsidian;
    float fps = 0;
    const float speed = 10;
    const float angleMulti = 10;
    Texture* texture = nullptr;


    float pitch = 0;
    float yaw = 90;

    void onFpsUpdate(float fps) override {
        this->fps = fps;
        cout << fps << endl;
    }

    void onCreate(Graphics& gfx) override {

        texture = new Texture("../rsc/test.png", GL_NEAREST);

        getGraphics()->getCamera()->setPosition({0, 0, 0});

        addKeyBinding(GLFW_KEY_W, [this]() {
            glm::vec3 offset(0, 0, 0);
            offset.z+= speed/fps;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_S, [this]() {
            glm::vec3 offset(0, 0, 0);
            offset.z-= speed/fps;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_A, [this]() {
            glm::vec3 offset(0, 0, 0);
            offset.x-= speed/fps;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_D, [this]() {
            glm::vec3 offset(0, 0, 0);
            offset.x+= speed/fps;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_SPACE, [this]() {
            glm::vec3 offset(0, 0, 0);
            offset.y+= speed/fps;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_LEFT_SHIFT, [this]() {
            glm::vec3 offset(0, 0, 0);
            offset.y-= speed/fps;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_RIGHT_SHIFT, [this]() {
            glm::vec3 offset(0, 0, 0);
            offset.y-= speed/fps;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_LEFT, [this]() {
            yaw -= angleMulti*speed/fps;
            if ( yaw < 0) {
                yaw += 360;
            }
            getGraphics()->getCamera()->setOrientation(pitch, yaw);
        });

        addKeyBinding(GLFW_KEY_RIGHT, [this]() {
            yaw += angleMulti*speed/fps;
            if ( yaw > 360) {
                yaw -= 360;
            }
            getGraphics()->getCamera()->setOrientation(pitch, yaw);
        });

        addKeyBinding(GLFW_KEY_DOWN, [this]() {
            pitch -= angleMulti*speed/fps;
            if ( pitch < 0) {
                pitch += 360;
            }
            getGraphics()->getCamera()->setOrientation(pitch, yaw);
        });

        addKeyBinding(GLFW_KEY_UP, [this]() {
            pitch += angleMulti*speed/fps;
            if ( pitch > 360) {
                pitch -= 360;
            }
            getGraphics()->getCamera()->setOrientation(pitch, yaw);
        });

    }

    void onDraw(Graphics& gfx) override {

        gfx.clear(0, 0, 0, 1);


        // // gfx.fillTriangle(Vertex(glm::vec3(-10, -10, -20), glm::vec4(1, 0, 0, 1)),
        // //                  Vertex(glm::vec3(10, -10, -50), glm::vec4(0, 1, 0, 1)),
        // //                  Vertex(glm::vec3(-10, 10, -50), glm::vec4(0, 0, 1, 1)));
        // //
        // //gfx.drawTexture(*texture, glm::vec3(-2, 2, 2), glm::vec3(2, 2, 2), glm::vec3(2, -2, -2), glm::vec3(-2, -2, -2));
        //
        // gfx.fillPolygon(std::vector<Vertex> {
        //     Vertex(glm::vec3(-1, -1,  1), glm::vec4(1, 0, 0, 1)),
        //     Vertex(glm::vec3( 1, -1,  1), glm::vec4(1, 0, 0, 1)),
        //     Vertex(glm::vec3( 1,  1,  1), glm::vec4(1, 0, 0, 1)),
        //     Vertex(glm::vec3(-1,  1,  1), glm::vec4(1, 0, 0, 1)),
        //
        // });
        //
        // gfx.fillPolygon(std::vector<Vertex> {
        //     Vertex(glm::vec3(-1, -1, -1), glm::vec4(1, 1, 0, 1)),
        //     Vertex(glm::vec3( 1, -1, -1), glm::vec4(1, 1, 0, 1)),
        //     Vertex(glm::vec3( 1,  1, -1), glm::vec4(1, 1, 0, 1)),
        //     Vertex(glm::vec3(-1,  1, -1), glm::vec4(1, 1, 0, 1))
        // });
        //
        // gfx.fillPolygon(std::vector<Vertex> {
        //     Vertex(glm::vec3(-1, -1,  1), glm::vec4(0, 1, 1, 1)),
        //     Vertex(glm::vec3(-1, -1, -1), glm::vec4(0, 1, 1, 1)),
        //     Vertex(glm::vec3(-1,  1, -1), glm::vec4(0, 1, 1, 1)),
        //     Vertex(glm::vec3(-1,  1,  1), glm::vec4(0, 1, 1, 1)),
        //
        // });
        //
        // gfx.fillPolygon(std::vector<Vertex> {
        //     Vertex(glm::vec3(1, -1,  1), glm::vec4(1, 0, 1, 1)),
        //     Vertex(glm::vec3(1, -1, -1), glm::vec4(1, 0, 1, 1)),
        //     Vertex(glm::vec3(1,  1, -1), glm::vec4(1, 0, 1, 1)),
        //     Vertex(glm::vec3(1,  1,  1), glm::vec4(1, 0, 1, 1)),
        // });
        //
        // gfx.fillPolygon(std::vector<Vertex> {
        //     Vertex(glm::vec3(-1, 1,  1), glm::vec4(0, 1, 0, 1)),
        //     Vertex(glm::vec3(-1, 1, -1), glm::vec4(0, 1, 0, 1)),
        //     Vertex(glm::vec3( 1, 1, -1), glm::vec4(0, 1, 0, 1)),
        //     Vertex(glm::vec3( 1, 1,  1), glm::vec4(0, 1, 0, 1)),
        //
        // });
        //
        // gfx.fillPolygon(std::vector<Vertex> {
        //     Vertex(glm::vec3(-1, -1,  1), glm::vec4(0, 0, 1, 1)),
        //     Vertex(glm::vec3(-1, -1, -1), glm::vec4(0, 0, 1, 1)),
        //     Vertex(glm::vec3( 1, -1, -1), glm::vec4(0, 0, 1, 1)),
        //     Vertex(glm::vec3( 1, -1,  1), glm::vec4(0, 0, 1, 1)),
        // });

    }
};

int main() {
    Engine app(1000, 700, "smth");
    app.run(true);
}