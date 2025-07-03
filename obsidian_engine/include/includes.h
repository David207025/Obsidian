#ifndef INCLUDES_H
#define INCLUDES_H

// Core GLM types must come first
#include "../third_party/glm/glm/glm.hpp"                    // ✅ for glm::mat4, vec3 etc.
#include "../third_party/glm/glm/gtc/type_ptr.hpp"           // for glm::value_ptr

// OpenGL & utility libraries
#include "../third_party/glad/include/glad/glad.h"
#include "../third_party/glfw/include/GLFW/glfw3.h"
#include "../third_party/stb/stb_image.h"
#include "../third_party/stb/stb_truetype.h"

// Engine includes
#include "./obsidian.h"
#include "./utils/Texture.h"
#include "./utils/Vertex.h"
#include "./utils/Font.h"
#include "./utils/Graphics.h"
#include "./utils/Object.h"

// STL includes
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <iostream>

#endif // INCLUDES_H