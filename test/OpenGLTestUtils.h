//
// Created by Damian Stewart on 16/10/16.
//

#ifndef SKELLINGTON_OPENGLTESTUTILS_H_H
#define SKELLINGTON_OPENGLTESTUTILS_H_H

//
// Created by Damian Stewart on 18/07/16.
//

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <glm/glm.hpp>

#include <fstream>
#include <sstream>
#include <numeric>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::cerr;
using std::cout;
using std::endl;

static const vec4 COLOR_RED(1,0,0,1);
static const vec4 COLOR_GREEN(0,1,0,1);
static const vec4 COLOR_BLUE(0,0,1,1);

inline float RandomFloat(float min=0, float max=1) {
    return min + (max-min) * float(arc4random())/std::numeric_limits<uint32_t>::max();
}

inline vec3 RandomVec3() {
    return vec3(RandomFloat(), RandomFloat(), RandomFloat());
}

inline void glVertex3f(vec3 v) {
    glVertex3f(v.x, v.y, v.z);
}

inline void glTranslatef(vec3 v) {
    glTranslatef(v.x, v.y, v.z);
}

inline void glColor4f(const vec4 &c)
{
    glColor4f(c.r, c.g, c.b, c.a);
}

inline void DrawPoint(const vec3& v, const vec4 &color)
{
    glBegin(GL_POINTS);
    glColor4f(color);
    glVertex3f(v);
    glEnd();
}

inline void DrawLine(const vec3 &v1, const vec3 &v2, const vec4 &color)
{
    glBegin(GL_LINES);
    glColor4f(color);
    glVertex3f(v1);
    glVertex3f(v2);
    glEnd();
}

inline void DrawPolygon(const vector<vec3>& vertices, float r, float g, float b, bool filled ) {
    glColor4f(r, g, b, 1.0);
    glBegin(GL_LINE_LOOP);
    for (auto& v: vertices) {
        glVertex3f(v);
    }
    glEnd();

    if (filled) {
        glColor4f(r, g, b, 0.2);
        glBegin(GL_TRIANGLE_FAN);
        for (auto &v: vertices) {
            glVertex3f(v);
        }
        glEnd();
    }
}

inline void SetupOpenGLMatrices(GLFWwindow *window) {

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    //glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const float fov = 45;
    const float aspect = float(width)/height;
    const float zNear = 0.1f;
    const float zFar = 1000.0f;

    gluPerspective(fov, aspect, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

inline GLFWwindow* InitOpenGL(float width, float height) {

    if (!glfwInit()) {
        cerr << "Couldn't init GLFW" << endl;
        return nullptr;
    }

    auto window = glfwCreateWindow(width, height, "OpenGL Test Window", NULL, NULL);
    if (window == nullptr) {
        glfwTerminate();
        cerr << "Couldn't create window" << endl;
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    return window;
}

inline void OpenGLMainLoop(vec2 windowSize, std::function<void(void)> frameFunction, GLFWkeyfun keyFunction=nullptr) {

    auto window = InitOpenGL(windowSize.x, windowSize.y);

    if (keyFunction != nullptr) {
        glfwSetKeyCallback(window, keyFunction);
    }

    while (!glfwWindowShouldClose(window)) {
        SetupOpenGLMatrices(window);

        frameFunction();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
}

inline void OpenGLRotatingMainLoop(vec2 windowSize, vec3 camPos, float rotateSpeed, std::function<void(void)> frameFunction, GLFWkeyfun keyFunction = nullptr)
{
    float camAngle = 0;
    OpenGLMainLoop(windowSize, [&]() {
        camAngle += rotateSpeed;

        glTranslatef(camPos);

        glRotatef(camAngle, 0, 1, 0);
        frameFunction();
    }, keyFunction);
}

inline void OpenGLRotatingMainLoop(vec2 windowSize, float camDistance, float rotateSpeed, std::function<void(void)> frameFunction, GLFWkeyfun keyFunction=nullptr)
{
    OpenGLRotatingMainLoop(windowSize, vec3(0, 0, -camDistance), rotateSpeed, frameFunction, keyFunction);
}

inline std::ostream &operator<<(std::ostream &os, const vec3 &v)
{
    return os << "(" << v.x << "," << v.y << "," << v.z << ")";
}

inline std::ostream &operator<<(std::ostream &os, const vec4 &v)
{
    return os << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
}

inline std::ostream &operator<<(std::ostream& os, const mat4 &m)
{
    return os << "[" << m[0] << "," << m[1] << "," << m[2] << "," << m[3] << "]";
}

#endif //SKELLINGTON_OPENGLTESTUTILS_H_H
