#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        // Initialize camera properties
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;

        this->cameraUpDirection = cameraUp;

        // Calculate front and right directions
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
    }


    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO

        return glm::lookAt(cameraPosition, cameraTarget, this->cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    // MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT};
    void Camera::move(MOVE_DIRECTION direction, float speed) {

        switch (direction) {
        case MOVE_FORWARD:

            cameraPosition += cameraFrontDirection * speed;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            break;
        }

        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        //DONE
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        this->cameraFrontDirection = glm::normalize(direction);
        this->cameraTarget = cameraPosition + cameraFrontDirection;
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }
    void gps::Camera::moveTo(glm::vec3 newPosition, glm::vec3 newTarget) {
        // Update the camera position
        this->cameraPosition = newPosition;

        // Update the camera target
        this->cameraTarget = newTarget;

        // Recalculate the front direction
        this->cameraFrontDirection = glm::normalize(newTarget - newPosition);

        // Recalculate the right direction
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
    }

    // Get camera position
    glm::vec3 Camera::getPosition() const {
        return this->cameraPosition;
    }

    // Get camera target
    glm::vec3 Camera::getTarget() const {
        return this->cameraTarget;
    }

    // Get front direction
    glm::vec3 Camera::getFrontDirection() const {
        return this->cameraFrontDirection;
    }

    // Get right direction
    glm::vec3 Camera::getRightDirection() const {
        return this->cameraRightDirection;
    }

    // Get up direction
    glm::vec3 Camera::getUpDirection() const {
        return this->cameraUpDirection;
    }


}
