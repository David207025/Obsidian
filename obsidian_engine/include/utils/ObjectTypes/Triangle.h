//
// Created by David Vacaroiu on 02.07.25.
//

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "../../includes.h"

class Graphics;

class Triangle : public RenderableObject {

    void drawShape(Graphics& g) {
        g.drawTriangle(v1, v2, v3);
    }

    void fillShape(Graphics& g) {
        g.fillTriangle(v1, v2, v3);
    }

public:
    Vertex v1, v2, v3;
    bool filled;

    Triangle(Vertex v1, Vertex v2, Vertex v3, bool filled) : v1(v1), v2(v2), v3(v3), filled(filled) {};

    void draw(Graphics& graphics) override {
        if (filled) {
            fillShape(graphics);
        } else {
            drawShape(graphics);
        }
    }

    void drawBoundingProxy(Graphics& graphics) override {
        // Draw as wireframe or box using bounding radius
    }
};

#endif //TRIANGLE_H
