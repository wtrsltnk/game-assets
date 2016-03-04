#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : _position(glm::vec3(0.0f, 0.0f, 0.0f)),
      _up(glm::vec3(0.0f, 0.0f, 1.0f)),
      _rotation(glm::mat4(1.0f))
{ }

Camera::~Camera()
{ }

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->Position(), this->Target(), this->Up());
}

glm::vec3 Camera::Forward() const
{
    return glm::normalize(glm::mat3(this->_rotation)[1]);
}

glm::vec3 Camera::Left() const
{
    return glm::cross(this->Up(), this->Forward());
}

const glm::vec3& Camera::Up() const
{
    return this->_up;
}

void Camera::SetUp(const glm::vec3& up)
{
    this->_up = up;
}

const glm::vec3& Camera::Position() const
{
    return this->_position;
}

void Camera::SetPosition(const glm::vec3& pos)
{
    this->_position = pos;
}

glm::vec3 Camera::Target() const
{
    return this->Position() + this->Forward();
}

void Camera::MoveForward(float amount)
{
    this->_position += this->Forward() * amount;
}

void Camera::MoveLeft(float amount)
{
    this->_position += this->Left() * amount;
}

void Camera::RotateX(float angle)
{
    this->_rotation = glm::rotate(this->_rotation, angle, glm::vec3(1, 0, 0));
}

void Camera::RotateY(float angle)
{
    this->_rotation = glm::rotate(this->_rotation, angle, glm::vec3(0, 1, 0));
}

void Camera::RotateZ(float angle)
{
    this->_rotation = glm::rotate(this->_rotation, angle, glm::vec3(0, 0, 1));
}

