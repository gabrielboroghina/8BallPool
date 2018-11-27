#pragma once

#include "Object.h"

class Cushion : public Object
{
public:
    int type;
    Cushion(int type);
    ~Cushion();
};
