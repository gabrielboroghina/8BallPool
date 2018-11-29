#pragma once

#include "Objects/Ball.h"

class Player
{
public:
    int score, fouls;
    BallColor color;

    Player();
    ~Player();
};
