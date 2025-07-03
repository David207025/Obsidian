
#include <iostream>

#include "obsidian_engine/core.h"
#include "obsidian_engine/include/utils/Texture.h"

using namespace std;

class Engine : public Obsidian {
public:
    using Obsidian::Obsidian;
    const float speed = 10;
    const float angleMulti = 10;

    Texture* grassTop;
    Texture* grassBottom;
    Texture* grassSide;

    std::shared_ptr<Shape> sphere;

    float pitch = 0;
    float yaw = 0;

    void onFpsUpdate(float fps) override {
        cout << "FPS: " << fps << endl;
    }

    void onCreate(Graphics& gfx) override {

        grassTop = new Texture("../rsc/top.png", GL_NEAREST);
        grassSide = new Texture("../rsc/side.png", GL_NEAREST);
        grassBottom = new Texture("../rsc/bottom.png", GL_NEAREST);

        getGraphics()->getCamera()->setPosition({0, 0, 0});
        getGraphics()->getCamera()->setOrientation(0, 0);

        pitch = getGraphics()->getCamera()->pitch;
        yaw = getGraphics()->getCamera()->yaw;

        addKeyBinding(GLFW_KEY_W, [this](float deltaTime) {
            glm::vec3 offset(0, 0, 0);
            offset.z+= speed*deltaTime;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_S, [this](float deltaTime) {
            glm::vec3 offset(0, 0, 0);
            offset.z-= speed*deltaTime;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_A, [this](float deltaTime) {
            glm::vec3 offset(0, 0, 0);
            offset.x-= speed*deltaTime;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_D, [this](float deltaTime) {
            glm::vec3 offset(0, 0, 0);
            offset.x+= speed*deltaTime;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_SPACE, [this](float deltaTime) {
            glm::vec3 offset(0, 0, 0);
            offset.y+= speed*deltaTime;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_LEFT_SHIFT, [this](float deltaTime) {
            glm::vec3 offset(0, 0, 0);
            offset.y-= speed*deltaTime;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_RIGHT_SHIFT, [this](float deltaTime) {
            glm::vec3 offset(0, 0, 0);
            offset.y-= speed*deltaTime;
            getGraphics()->getCamera()->moveCamera(offset, true);
        });

        addKeyBinding(GLFW_KEY_LEFT, [this](float deltaTime) {
            yaw -= angleMulti*speed*deltaTime;
            if ( yaw < 0) {
                yaw += 360;
            }
            getGraphics()->getCamera()->setOrientation(pitch, yaw);
        });

        addKeyBinding(GLFW_KEY_RIGHT, [this](float deltaTime) {
            yaw += angleMulti*speed*deltaTime;
            if ( yaw > 360) {
                yaw -= 360;
            }
            getGraphics()->getCamera()->setOrientation(pitch, yaw);
        });

        addKeyBinding(GLFW_KEY_DOWN, [this](float deltaTime) {
            pitch -= angleMulti*speed*deltaTime;
            if ( pitch < 0) {
                pitch += 360;
            }
            getGraphics()->getCamera()->setOrientation(pitch, yaw);
        });

        addKeyBinding(GLFW_KEY_UP, [this](float deltaTime) {
            pitch += angleMulti*speed*deltaTime;
            if ( pitch > 360) {
                pitch -= 360;
            }
            getGraphics()->getCamera()->setOrientation(pitch, yaw);
        });

        glm::vec2 topLeft = glm::vec2(0, 1);
        glm::vec2 topRight = glm::vec2(1, 1);
        glm::vec2 bottomRight = glm::vec2(1, 0);
        glm::vec2 bottomLeft = glm::vec2(0, 0);

        auto side1 = std::make_shared<Shape>(
            std::vector<Vertex>{
                Vertex(glm::vec3(-1, -1, -1), glm::vec4(1, 1, 1, 1), bottomLeft ),
                Vertex(glm::vec3( 1, -1, -1), glm::vec4(1, 1, 1, 1), bottomRight),
                Vertex(glm::vec3( 1,  1, -1), glm::vec4(1, 1, 1, 1), topRight   ),
                Vertex(glm::vec3(-1,  1, -1), glm::vec4(1, 1, 1, 1), topLeft    ),
            },
            *grassSide,
            PrimitiveType::TriangleFan
        );

        auto side2 = std::make_shared<Shape>(
            std::vector<Vertex>{
                Vertex(glm::vec3(-1, -1, 1), glm::vec4(1, 1, 1, 1), bottomLeft ),
                Vertex(glm::vec3( 1, -1, 1), glm::vec4(1, 1, 1, 1), bottomRight),
                Vertex(glm::vec3( 1,  1, 1), glm::vec4(1, 1, 1, 1), topRight   ),
                Vertex(glm::vec3(-1,  1, 1), glm::vec4(1, 1, 1, 1), topLeft    ),
            },
            *grassSide,
            PrimitiveType::TriangleFan
        );

        auto side3 = std::make_shared<Shape>(
            std::vector<Vertex>{
                Vertex(glm::vec3(-1, -1, -1), glm::vec4(1, 1, 1, 1), bottomLeft ),
                Vertex(glm::vec3(-1, -1,  1), glm::vec4(1, 1, 1, 1), bottomRight),
                Vertex(glm::vec3(-1,  1,  1), glm::vec4(1, 1, 1, 1), topRight   ),
                Vertex(glm::vec3(-1,  1, -1), glm::vec4(1, 1, 1, 1), topLeft    ),
            },
            *grassSide,
            PrimitiveType::TriangleFan
        );

        auto side4 = std::make_shared<Shape>(
            std::vector<Vertex>{
                Vertex(glm::vec3( 1, -1, -1), glm::vec4(1, 1, 1, 1), bottomLeft ),
                Vertex(glm::vec3( 1, -1,  1), glm::vec4(1, 1, 1, 1), bottomRight),
                Vertex(glm::vec3( 1,  1,  1), glm::vec4(1, 1, 1, 1), topRight   ),
                Vertex(glm::vec3( 1,  1, -1), glm::vec4(1, 1, 1, 1), topLeft    ),
            },
            *grassSide,
            PrimitiveType::TriangleFan
        );

        auto top = std::make_shared<Shape>(
            std::vector<Vertex>{
                Vertex(glm::vec3(-1,  1, -1), glm::vec4(1, 1, 1, 1), bottomLeft ),
                Vertex(glm::vec3( 1,  1, -1), glm::vec4(1, 1, 1, 1), bottomRight),
                Vertex(glm::vec3( 1,  1,  1), glm::vec4(1, 1, 1, 1), topRight   ),
                Vertex(glm::vec3(-1,  1,  1), glm::vec4(1, 1, 1, 1), topLeft    ),
            },
            *grassTop,
            PrimitiveType::TriangleFan
        );

        auto bottom = std::make_shared<Shape>(
            std::vector<Vertex>{
                Vertex(glm::vec3(-1,  -1, -1), glm::vec4(1, 1, 1, 1), bottomLeft ),
                Vertex(glm::vec3( 1,  -1, -1), glm::vec4(1, 1, 1, 1), bottomRight),
                Vertex(glm::vec3( 1,  -1,  1), glm::vec4(1, 1, 1, 1), topRight   ),
                Vertex(glm::vec3(-1,  -1,  1), glm::vec4(1, 1, 1, 1), topLeft    ),
            },
            *grassBottom,
            PrimitiveType::TriangleFan
        );


        sphere = Shape::createSphere(10);

        sphere->translate(glm::vec3(20, 0, 0));

        gfx.addShape(side1);
        gfx.addShape(side2);
        gfx.addShape(side3);
        gfx.addShape(side4);
        gfx.addShape(top);
        gfx.addShape(bottom);

        gfx.addShape(sphere);
    }

    void onDraw(Graphics& gfx) override {

        gfx.clear(0, 0, 0, 1);

        gfx.render();

    }

    void onFrameDrawn(float deltaTime) override {
        sphere->rotate(glm::vec3(0, 0, 0), 0, deltaTime*10);
    };
};

int main() {
    Engine app(1000, 700, "smth");
    app.run(true);
}