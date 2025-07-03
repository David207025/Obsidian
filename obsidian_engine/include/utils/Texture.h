//
// Created by David Vacaroiu on 02.07.25.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "../includes.h"

class Texture {
    GLuint texture = 0;
public:

    Texture(const std::string& path, GLint filtering);
    Texture(GLuint texture);

    GLuint getData();
};

#endif //TEXTURE_H
