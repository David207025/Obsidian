//
// Created by David Vacaroiu on 02.07.25.
//

#ifndef FONT_H
#define FONT_H

struct Font {
    GLuint textureID = 0;
    stbtt_bakedchar cdata[96]; // ASCII 32..126
    int bitmapWidth = 512;
    int bitmapHeight = 512;
};

#endif //FONT_H
