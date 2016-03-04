#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    virtual ~Camera();

    glm::mat4 GetViewMatrix();

    glm::vec3 Forward() const;
    glm::vec3 Left() const;
    const glm::vec3& Up() const;
    void SetUp(const glm::vec3& up);

    const glm::vec3& Position() const;
    void SetPosition(const glm::vec3& pos);
    glm::vec3 Target() const;

    void MoveForward(float amount);
    void MoveLeft(float amount);

    void RotateX(float angle);
    void RotateY(float angle);
    void RotateZ(float angle);

private:
    glm::vec3 _position;
    glm::vec3 _up;
    glm::mat4 _rotation;

};

#endif // CAMERA_H
