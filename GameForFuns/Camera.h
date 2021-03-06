//
//  Camera.h
//  GameForFuns
//
//  Created by Morten Omholt-Jensen on 19/04/2019.
//  Copyright © 2019 Morten Omholt-Jensen. All rights reserved.
//

#pragma once

#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 6.0f;
const GLfloat SENSITIVITY = 0.15f;
const GLfloat ZOOM = 45.0f; // FOV


class Camera {
public:
    Camera(
           glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = YAW,
           GLfloat pitch = PITCH
       ): front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
        
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateCameraVectors();
    }
    
    Camera(GLfloat posX,
           GLfloat posY,
           GLfloat posZ,
           GLfloat upX,
           GLfloat upY,
           GLfloat upZ,
           GLfloat yaw,
           GLfloat pitch
       ): front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
        
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateCameraVectors();
    }
    
    glm::mat4 getViewMatrix() {
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }
    
    void processKeyboard(Camera_Movement direction, GLfloat deltaTime) {
        GLfloat velocity = this->movementSpeed * deltaTime;
        if (direction == UP) {
            this->acceleration.y += 0.003f;
        }
        if (direction == FORWARD) {
            this->position += this->front * velocity;
        }
        if (direction == BACKWARD) {
            this->position -= this->front * velocity;
        }
        if (direction == LEFT) {
            this->position -= this->right * velocity;
        }
        if (direction == RIGHT) {
            this->position += this->right * velocity;
        }
    }
    
    void update(GLfloat deltaTime) {
        this->acceleration += glm::vec3(0.0f, -0.1f * deltaTime, 0.0f); // Gravity
        this->position += this->velocity;
        this->velocity += this->acceleration;
        this->acceleration = glm::vec3(0.0f);
    }
    
    void intersect() {
        if (this->position.y <= 0.0f) {
            this->position.y = 0.0f;
        }
    }
    
    void processMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true) {
        xOffset *= this->mouseSensitivity;
        yOffset *= this->mouseSensitivity;
        
        this->yaw += xOffset;
        this->pitch += yOffset;
        
        if (constrainPitch) {
            if (this->pitch > 89.0f) {
                this->pitch = 89.0f;
            }
            if (this->pitch < -89.0f) {
                this->pitch = -89.0f;
            }
        }
        this->updateCameraVectors();
    }
    
    void processMouseScroll(GLfloat yOffset) {
//        if (this->zoom >= 1.0f && this->zoom <= 45.0f) {
//            this->zoom -= yOffset;
//        }
//        if (this->zoom <= 1.0f) {
//            this->zoom = 1.0f;
//        }
//        if (this->zoom >= 45.0f) {
//            this->zoom = 45.0f;
//        }
    }
    
    GLfloat getZoom() {
        return this->zoom;
    }
    
    glm::vec3 getPosition() {
        return this->position;
    }
    
    void setPosition(glm::vec3 position) {
        this->position = position;
    }
    
    glm::vec3 getFront() {
        return this->front;
    }
    
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    GLfloat gravity = 0.9f;
    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    
    GLfloat yaw;
    GLfloat pitch;
    
    GLfloat movementSpeed;
    GLfloat mouseSensitivity;
    GLfloat zoom;
    
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        front.y = sin(glm::radians(this->pitch));
        front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        
        this->front = glm::normalize(front);
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
    
    
};






