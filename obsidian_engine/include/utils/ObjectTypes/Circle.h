//
// Created by David Vacaroiu on 02.07.25.
//

#ifndef CIRCLE_H
#define CIRCLE_H

#include "../../includes.h"

class Graphics;

class Circle : public RenderableObject {

    void drawShape(Graphics& g) {
        g.drawCircle(v, radius, segments);
    }

    void fillShape(Graphics& g) {
        g.fillCircle(v, radius, segments);
    }

public:
    Vertex v;
    float radius;
    int segments;
    bool filled;

    Circle(Vertex v, float radius, int segments, bool filled) : v(v), radius(radius), segments(segments), filled(filled) {};

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



#endif //CIRCLE_H
