#pragma once

//includes 
#include <math.h>

class PositionClass
{
public:
    PositionClass();
    PositionClass(const PositionClass&);
    ~PositionClass();

    void SetFrameTime(float);
    void GetRotation(float&);
    void GetMovement(float&, float&);

    void MoveLeft(bool);
    void MoveRight(bool);
    void MoveForward(bool);
    void MoveBackward(bool);
private:
    float m_frameTime;
    float m_rotationY;
    float m_posX;
    float m_posY;
    float m_posZ;
    float m_leftTurnSpeed;
    float m_rightTurnSpeed;
    float m_movementSpeed;
};
