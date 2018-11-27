#pragma once

enum ObjectType
{
    BALL,
    CUSHION,
    POCKET
};

class Object
{
public:
    ObjectType type;
    Object (ObjectType);
    virtual ~Object();
};
