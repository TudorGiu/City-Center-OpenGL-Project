#include "Camera.hpp"

namespace gps {

    
    
    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp)
    {


        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPosition);

        this->cameraFrontDirection = glm::normalize( cameraTarget-cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUp, cameraFrontDirection));
        this->cameraUpDirection = cameraUp;
       
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {

        return glm::lookAt(cameraPosition,cameraPosition + cameraFrontDirection, cameraUpDirection);
    }
    

   
    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed, CAMERA_MODE camera_mode) {

        switch (direction)
        {
        case gps::MOVE_FORWARD:
            this->cameraPosition += speed * cameraFrontDirection;
            break;
        case gps::MOVE_BACKWARD:
            this->cameraPosition -= speed * cameraFrontDirection;
            break;
        case gps::MOVE_RIGHT:
            this->cameraPosition += speed * cameraRightDirection;
            break;
        case gps::MOVE_LEFT:
            this->cameraPosition -= speed * cameraRightDirection;
            break;
        case gps::MOVE_MINUS_Z:
            this->cameraPosition.z -= speed;
            break;
        default:
            break;
        }

        if (camera_mode == FIRST_PERSON)
            this->cameraPosition.y = gps::FIRST_PERSON_Y;
    }
    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::setCameraPosition(glm::vec3 pos)
    {
        this->cameraPosition = pos;
    }

    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 newFrontDirection;
        newFrontDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFrontDirection.y = sin(glm::radians(pitch));
        newFrontDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        this->cameraFrontDirection = glm::normalize(newFrontDirection);

        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }

    glm::vec3 Camera::getCameraPosition() {
        return this->cameraPosition;
    }
}