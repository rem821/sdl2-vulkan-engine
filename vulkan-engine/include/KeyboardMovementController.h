//
// Created by Stanislav Svědiroh on 14.06.2022.
//
#pragma once

#include <SDL2/SDL.h>
#include "rendering/GameObject.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"

class KeyboardMovementController {
public:
    struct KeyMappings {
        int moveLeft = SDL_SCANCODE_A;
        int moveRight = SDL_SCANCODE_D;
        int moveForward = SDL_SCANCODE_W;
        int moveBackward = SDL_SCANCODE_S;
        int moveUp = SDL_SCANCODE_E;
        int moveDown = SDL_SCANCODE_Q;
        int lookLeft = SDL_SCANCODE_LEFT;
        int lookRight = SDL_SCANCODE_RIGHT;
        int lookUp = SDL_SCANCODE_UP;
        int lookDown = SDL_SCANCODE_DOWN;
        int sprint = SDL_SCANCODE_LSHIFT;
        int sneak = SDL_SCANCODE_LCTRL;
    };

    void moveInPlaneXZ(float dt, GameObject &gameObject);

    KeyMappings keys{};
    float moveSpeed{20.f};
    float lookSpeed{2.5f};
};