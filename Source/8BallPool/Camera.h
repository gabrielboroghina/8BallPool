#pragma once

#include <include/glm.h>
#include "ITargetObserver.h"
#include "include/math.h"
#include "UIConstants.h"

enum CameraType
{
    FirstPerson,
    ThirdPerson
};

/** Predefined camera layouts */
enum CameraLayout
{
    None,
    TopDown
};

class Camera : public ITargetObserver
{
    float distanceToTarget;
    glm::mat4 projectionMatrix;
    glm::vec3 forward, right, up;

public:
    glm::vec3 position;
    CameraType type;

    Camera(int fov_deg, float aspectRatio) : type(CameraType::FirstPerson)
    {
        forward = glm::normalize(glm::vec3(0, -1, -1));
        up = glm::vec3(0, 1, 0);
        right = glm::vec3(1, 0, 0);

        position = glm::vec3(0, 2, 5);

        // build Projection Matrix
        projectionMatrix = glm::perspective(RADIANS(fov_deg), aspectRatio, 0.01f, 200.0f);
    }

    ~Camera() { }

    glm::vec3 GetPosFromCameraSpace(glm::vec3 pos)
    {
        pos = glm::normalize(pos);
        return pos.x * forward + pos.y * up + pos.z * right;
    }

    void SetTarget(glm::vec3 targetPos, glm::vec3 dir, float distToTarget) override
    {
        distanceToTarget = distToTarget;

        this->forward = glm::normalize(-dir);
        this->right = glm::rotate(glm::mat4(1), RADIANS(-90), glm::vec3(0, 1, 0)) * glm::vec4(forward, 1);
        this->right.y = 0;
        right = glm::normalize(right);
        this->up = glm::normalize(glm::cross(right, forward));

        position = targetPos - forward * distanceToTarget / glm::length(forward);
    }

    /** Move the camera */
    void Update(float deltaTime, bool isWPressed, bool isAPressed, bool isSPressed, bool isDPressed, bool isQPressed, bool isEPressed)
    {
        const float distance = UIConstants::Camera::MOVE_SPEED * deltaTime;

        if (isWPressed) {
            // translate the camera forward
            TranslateForward(distance);
        }

        if (isAPressed) {
            // translate the camera to the left
            TranslateRight(-distance);
        }

        if (isSPressed) {
            // translate the camera backwards
            TranslateForward(-distance);
        }

        if (isDPressed) {
            // translate the camera to the right
            TranslateRight(distance);
        }

        if (isQPressed) {
            // translate the camera down
            TranslateUpword(-distance);
        }

        if (isEPressed) {
            // translate the camera up
            TranslateUpword(distance);
        }
    }

    /** Track the target in third person mode */
    void UpdatePos(glm::vec3 movement) override
    {
        if (type == ThirdPerson)
            position += movement;
    }

    /** Switch between First Person and Third Person camera modes */
    void SwitchMode(CameraType type, CameraLayout layout = None)
    {
        this->type = type;

        if (layout == TopDown) {
            position = glm::vec3(0, 5.5f, 0);

            forward = glm::normalize(glm::vec3(0, -1, 0));
            right = glm::vec3(1, 0, 0);
            up = glm::normalize(glm::cross(right, forward));
        }
    }

    void MoveForward(float distance)
    {
        glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
        position = position + glm::normalize(dir) * distance;
        // movement will keep the camera at the same height always
        // Example: If you rotate up/down your head and walk forward you will still keep the same relative distance (height) to the ground!
        // Translate the camera using the DIR vector computed from forward
    }

    void TranslateForward(float distance)
    {
        // Translate the camera using the "forward" vector
        position = position + glm::normalize(forward) * distance;
    }

    void TranslateUpword(float distance)
    {
        // Translate the camera using the up vector
        position = position + glm::normalize(up) * distance;
    }

    void TranslateRight(float distance)
    {
        // Translate the camera using the "right" vector
        // Usually translation using camera "right' is not very useful because if the camera is rotated around the "forward" vector 
        // translation over the right direction will have an undesired effect; the camera will get closer or farther from the ground
        // Using the projected right vector (onto the ground plane) makes more sense because we will keep the same distance from the ground plane
        glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
        position = position + glm::normalize(dir) * distance;
    }

    void RotateOX(float angle)
    {
        if (type == FirstPerson)
            RotateFirstPerson_OX(angle);
        else
            RotateThirdPerson_OX(angle);
    }

    void RotateOY(float angle)
    {
        if (type == FirstPerson)
            RotateFirstPerson_OY(angle);
        else
            RotateThirdPerson_OY(angle);
    }

    void RotateFirstPerson_OX(float angle)
    {
        // Compute the new "forward" and "up" vectors
        // Attention! Don't forget to normalize the vectors
        // Use glm::rotate()
        forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, right) * glm::vec4(forward, 1)));
        up = glm::normalize(glm::cross(right, forward));
    }

    void RotateFirstPerson_OY(float angle)
    {
        // Compute the new "forward", "up" and "right" vectors
        // Don't forget to normalize the vectors
        // Use glm::rotate()
        forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1)));
        right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1)));
        up = glm::normalize(glm::cross(right, forward));
    }

    void RotateFirstPerson_OZ(float angle)
    {
        // Compute the new Right and Up, Forward stays the same
        // Don't forget to normalize the vectors
        right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, forward) * glm::vec4(right, 1)));
        up = glm::normalize(glm::cross(right, forward));
    }

    void RotateThirdPerson_OX(float angle)
    {
        // Rotate the camera in Third Person mode - OX axis
        // Use distanceToTarget as translation distance
        TranslateForward(distanceToTarget);
        RotateFirstPerson_OX(angle);
        TranslateForward(-distanceToTarget);
    }

    void RotateThirdPerson_OY(float angle)
    {
        // Rotate the camera in Third Person mode - OY axis
        TranslateForward(distanceToTarget);
        RotateFirstPerson_OY(angle);
        TranslateForward(-distanceToTarget);
    }

    void RotateThirdPerson_OZ(float angle)
    {
        // Rotate the camera in Third Person mode - OZ axis
        TranslateForward(distanceToTarget);
        RotateFirstPerson_OZ(angle);
        TranslateForward(-distanceToTarget);
    }

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(position, position + forward, up);
    }

    glm::mat4 GetProjectionMatrix() const
    {
        return projectionMatrix;
    }

    glm::vec3 GetTargetPosition() const
    {
        return position + forward * distanceToTarget;
    }
};
