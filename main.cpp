
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

    int segmentsU = 20;
    int segmentsV = 8;

    float R = 5;
    float r = 2;

    void onFpsUpdate(float fps) override {
        cout << "FPS: " << fps << endl;
    }

    void onCreate(Graphics& gfx) override {

        grassTop = new Texture("../rsc/top.png", GL_NEAREST);
        grassSide = new Texture("../rsc/side.png", GL_NEAREST);
        grassBottom = new Texture("../rsc/bottom.png", GL_NEAREST);

        getGraphics()->getCamera()->setPosition({0, 0});

        addKeyBinding(GLFW_KEY_W, [this](float deltaTime) {
            glm::vec2 offset(0, 0);
            offset.y+= speed*deltaTime;
            getGraphics()->getCamera()->move(offset);
        });

        addKeyBinding(GLFW_KEY_S, [this](float deltaTime) {
            glm::vec2 offset(0, 0);
            offset.y-= speed*deltaTime;
            getGraphics()->getCamera()->move(offset);
        });

        addKeyBinding(GLFW_KEY_A, [this](float deltaTime) {
            glm::vec3 offset(0, 0, 0);
            offset.x-= speed*deltaTime;
            getGraphics()->getCamera()->move(offset);
        });

        addKeyBinding(GLFW_KEY_D, [this](float deltaTime) {
            glm::vec2 offset(0, 0);
            offset.x+= speed*deltaTime;
            getGraphics()->getCamera()->move(offset);
        });


        sphere = Shape::createCircle(100);
        sphere->translate(glm::vec2(0, 0));
        sphere->depth = 0;

        std::shared_ptr<LightSource> spotLight = std::make_shared<LightSource>(LightType::Ambient, glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
        spotLight->intensity = 0.5;


        auto spotlight = std::make_shared<LightSource>(LightType::Point, glm::vec2(200.f, 0.f), glm::vec2(-1.f, 0.f));
        spotlight->color = glm::vec3(1.f, 1.f, 0.8f);
        spotlight->intensity = 1.f;
        spotlight->cutoff = glm::cos(glm::radians(30.f));
        spotlight->radius = 500;
        spotlight->depth = 1;

        spotlight->initShadowResources(2048, 2048);



        gfx.addLight(spotlight);
        gfx.addLight(spotLight);
        gfx.addShape(sphere);

    }

    void onDraw(Graphics& gfx) override {

        gfx.clear(0, 0, 0, 1);

        gfx.render();

    }

    void onFrameDrawn(float deltaTime) override {
        //sphere->rotate(glm::vec3(0, 0, 0), 0, deltaTime*10);
        //cube.rotate(glm::vec3(0, 0, 0), deltaTime*speed*2, deltaTime*speed*2);
    };
};

int main() {
    Engine app(1000, 700, "smth");
    app.run(true);
}