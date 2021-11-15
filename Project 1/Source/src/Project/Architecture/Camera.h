#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//struct Camera {
//
//	glm::vec3 position;
//	//glm::vec3 front;
//	glm::vec3 target;
//	glm::vec3 up;
//
//	glm::mat4 perspective;
//	float aspect;
//	float nearClip;
//	float farClip;
//
//	//const static float SPEED;
//
//};

struct Orthographic {
    float clipNear, clipFar;
    float clipRight, clipLeft;
    float clipBottom, clipTop;
};

struct Perspective {
    float clipNear, clipFar;
    float aspect, fov;
    float width, height;
};

struct Camera { // a light or camera common attributes
private:
    glm::mat4 _projection;
    enum { ORTHO, PERSP } pType;
    union {
        Perspective perspective;
        Orthographic orthographic;
    };

public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;

    glm::mat4 projection() { return _projection; }

    float getFOV() { return perspective.fov; }
    void setFOV(float fov) { setPerspective(fov, perspective.aspect, perspective.clipNear, perspective.clipFar); }

    float getAspect() { return perspective.aspect; }
    void setAspect(float aspect) { setPerspective(perspective.fov, perspective.aspect = aspect, perspective.clipNear, perspective.clipFar); }

    void getClipNearFar(float* near, float* far) { *near = perspective.clipNear, * far = perspective.clipFar; }
    void setClipNearFar(float near, float far) {
        if (pType == PERSP)
            setPerspective(perspective.fov, perspective.aspect, perspective.clipNear = near, perspective.clipFar = far);
        else if (pType == ORTHO)
            setOrtho(orthographic.clipLeft, orthographic.clipRight, orthographic.clipTop, orthographic.clipBottom, orthographic.clipNear = near, orthographic.clipFar = far);
    }

    void setPerspective(float fov, float aspect, float clipNear, float clipFar) {
        pType = PERSP;
        _projection = glm::perspective(perspective.fov = fov, perspective.aspect = aspect, perspective.clipNear = clipNear, perspective.clipFar = clipFar);
    }

    void setOrtho(float clipLeft, float clipRight, float clipBottom, float clipTop, float clipNear, float clipFar) {
        pType = ORTHO;
        _projection = glm::ortho(orthographic.clipLeft = clipLeft, orthographic.clipRight = clipRight, orthographic.clipTop = clipTop, orthographic.clipBottom = clipBottom, orthographic.clipNear = clipNear, orthographic.clipFar = clipFar);
    }
};

//const float Camera::SPEED = 0.5f;
