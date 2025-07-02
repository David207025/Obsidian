//
// Created by David Vacaroiu on 02.07.25.
//

#ifndef POLYGON_H
#define POLYGON_H

#include "../../includes.h"

class Graphics;

class Polygon : public RenderableObject {

    void drawShape(Graphics& g) {
        g.drawPolygon(positions);
    }

    void fillShape(Graphics& g) {
        g.fillPolygon(positions);
    }

public:
    std::vector<Vertex>& positions;
    bool filled;

    Polygon(const std::vector<Vertex>& positions, bool filled) : positions(positions), filled(filled) {};

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

#endif //POLYGON_H
