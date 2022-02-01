//
// Sandbox program for Computer Graphics For Games (G4G)
// created May 2021 by Eric Ameres for experimenting
// with OpenGL and various graphics algorithms
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <tuple>
#include <vector>
#include <filesystem>

#include "Architecture\Camera.h"
#include "Architecture\shader_s.h"
#include "Architecture\renderer.h"
#include "Architecture\Material.h"
#include "Architecture\SceneGraph.h"

#include "Architecture\Arc.h"
#include "Architecture\BezierCurve.h"
#include "Architecture\QuadRenderer.h"
#include "Architecture\CubeRenderer.h"
#include "Architecture\SphereModel.h"
#include "Architecture\TorusModel.h"
#include "Car.h"
#include "Road.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

glm::mat4 pMat; // perspective matrix
glm::mat4 vMat; // view matrix

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
unsigned int scr_width = SCR_WIDTH;
unsigned int scr_height = SCR_HEIGHT;

unsigned int texture;
unsigned int grass_texture;
unsigned int road_texture;
unsigned int litScene;

// image buffer used by raster drawing basics.cpp
extern float imageBuff[3840][2160][3];

int myTexture();

SceneGraph* globalScene;

#pragma warning(push)
#pragma warning( disable : 26451 )

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void setupTextures()
{
    // create textures 
        // -------------------------
    glGenTextures(1, &texture);
    glGenTextures(1, &grass_texture);
    glGenTextures(1, &road_texture);
    glGenTextures(1, &litScene);

    // texture is a buffer we will be generating for pixel experiments
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load image, create texture and generate mipmaps
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 3840, 2160, 0, GL_RGB, GL_FLOAT, (const void*)imageBuff);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, grass_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("data/grass.png", &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, road_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 

    data = stbi_load("data/RoadTexture.png", &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

}

void drawIMGUI(renderer* myRenderer, SceneGraph* sg) {
    // Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static bool instantiated = false;

        // used to get values from imGui to the model matrix
        static float axis[] = { 0.0f,0.0f,1.0f };
        static float angle = 0.0f;

        static float transVec[] = { 0.0f,0.0f,0.0f };
        static float scaleVec[] = { 1.0f,1.0f,1.0f };

        static float cam_loc[3];
        static float cam_target[3];
        static float cam_rotAxis[] = { 0.0f,0.0f,1.0f };
        static float cam_angle = 0.0f;
        static float cam_fov;
        static float cam_aspect;
        static float cam_clip[] = { 1.0f, 1000.0f };


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Graphics For Games");  // Create a window and append into it.

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        static Shader* editShader;
        static renderer* editRenderer;

        if (ImGui::BeginTabBar("Shaders")) {
            for (const auto& [key, val] : Shader::shaders) {
                if (ImGui::BeginTabItem(val->name.c_str())) {
                    editShader = val.get();
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();

            ImGui::Text("Vertex Shader");
            ImGui::SameLine();
            ImGui::Text(std::filesystem::absolute(editShader->vertexPath).u8string().c_str());
            ImGui::InputTextMultiline("Vertex Shader", editShader->vtext, IM_ARRAYSIZE(editShader->vtext), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

            ImGui::Text("Fragment Shader");
            ImGui::SameLine();
            ImGui::Text(std::filesystem::absolute(editShader->fragmentPath).u8string().c_str());
            ImGui::InputTextMultiline("Fragment Shader", editShader->ftext, IM_ARRAYSIZE(editShader->ftext), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

            if (ImGui::Button("reCompile Shaders"))
                editShader->reload();

            ImGui::SameLine();

            if (ImGui::Button("Save Shaders"))
                editShader->saveShaders();
        }

        if (!instantiated) {
            cam_loc[0] = sg->camera.position.x;
            cam_loc[1] = sg->camera.position.y;
            cam_loc[2] = sg->camera.position.z;

            cam_target[0] = sg->camera.target.x;
            cam_target[1] = sg->camera.target.y;
            cam_target[2] = sg->camera.target.z;

            cam_fov = sg->camera.getFOV();
            cam_aspect = sg->camera.getAspect();
            sg->camera.getClipNearFar(&cam_clip[0], &cam_clip[1]);
            instantiated = true;
        }

        ImGui::SliderFloat("Exposure", &sg->exposure, 0.01f, 10.0f);

        // values we'll use to derive a model matrix
        ImGui::Text("Model Matrix");
        ImGui::DragFloat3("Translate", transVec, .01f, -10.0f, 10.0f);
        ImGui::InputFloat3("Axis", axis, "%.2f");
        ImGui::SliderAngle("Angle", &angle, -90.0f, 90.0f);
        ImGui::DragFloat3("Scale", scaleVec, .01f, -10.0f, 10.0f);

        ImGui::Text("Camera Matrix");
        ImGui::DragFloat3("Camera Translate", cam_loc, .01f, -20.0f, 20.0f);
        ImGui::DragFloat3("Camera Target", cam_target, .01f, -20.0f, 20.0f);
        ImGui::InputFloat3("Camera Axis", cam_rotAxis, "%.2f");
        ImGui::SliderAngle("Camera Angle", &cam_angle, -180.0f, 180.0f);
        ImGui::SliderAngle("FOV", &cam_fov, 10.0f, 200.0f);
        ImGui::DragFloat("Aspect Ratio", &cam_aspect, 0.01f, -3.0f, 3.0f);
        ImGui::DragFloat2("Clip Planes", cam_clip, 0.01f, 0.01f, 1000.0f);

        // show the texture that we generated
        //ImGui::Image((void*)(intptr_t)texture, ImVec2(64, 64));

        //ImGui::ShowDemoWindow(); // easter egg!  show the ImGui demo window

        ImGui::End();

        // IMGUI Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // factor in the results of imgui tweaks for the next round...

        sg->camera.setPerspective(cam_fov, cam_aspect, cam_clip[0], cam_clip[1]);
        glm::mat4 view = glm::rotate(glm::mat4(1.0f), -cam_angle, glm::vec3(cam_rotAxis[0], cam_rotAxis[1], cam_rotAxis[2]));
        sg->camera.position = view * glm::vec4(cam_loc[0], cam_loc[1], cam_loc[2], 1.0f);
        sg->camera.target = glm::vec4(cam_target[0], cam_target[1], cam_target[2], 1.0f);
        sg->camera.front = glm::normalize(sg->camera.target - sg->camera.position);
        sg->camera.right = glm::normalize(glm::cross(sg->camera.front, glm::vec3(0.0f, 0.0f, 1.0f)));
        sg->camera.up = glm::normalize(glm::cross(sg->camera.right, sg->camera.front));
        
        myRenderer->setXForm(glm::mat4(1.0f));
        myRenderer->translate(transVec);
        myRenderer->rotate(axis, angle);
        myRenderer->scale(scaleVec);
    }
}

int main() {
    namespace fs = std::filesystem;
    std::cout << "Current path is " << fs::current_path() << '\n';

    fs::file_status s = fs::file_status{};

    if (fs::status_known(s) ? fs::exists(s) : fs::exists("../../../data")) {
        std::cout << "Found upper level data directory, therefor assuming we're in a development tree \n";
        fs::current_path("../../..");
    }

    std::cout << "Absolute path for shaders is " << std::filesystem::absolute("./data/") << '\n';

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Graphics4Games Fall 2021", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    {
        new Shader("data/vGeo.glsl", "data/fGeo.glsl", "geometry");

        new Shader("data/vLight.glsl", "data/fLight.glsl", "light");

        new Shader("data/vPost.glsl", "data/fPost.glsl", "post");

        new Shader("data/vtorus.glsl", "data/ftorus.glsl", "torus");

        new Shader("data/vOscillate.glsl", "data/fOscillate.glsl", "oscillate");

        new Shader("data/vTex.glsl", "data/fTex.glsl", "textured");

        new Shader("data/vBezier.glsl", "data/fBezier.glsl", "line");

        new Shader("data/vLit.glsl", "data/fLit.glsl", "lit");

        new Shader("data/vUniverse.glsl", "data/fUniverse.glsl", "universe");
    }

    myTexture();
    setupTextures();

    {

        new Material(glm::vec4(1.0f, glm::vec3(0.3f)), glm::vec4(0.7f), glm::vec4(0.9f), 64.0f, 
                     0, Shader::shaders["torus"], "torus");

        new Material(glm::vec4(1.0f, glm::vec3(0.3f)), glm::vec4(1.0f), glm::vec4(1.0f), 64.0f, road_texture, Shader::shaders["textured"], "road");

        new Material(glm::vec4(1.0f, glm::vec3(0.3f)), glm::vec4(1.0f), glm::vec4(1.0f), 64.0f, texture, Shader::shaders["universe"], "stars");

        new Material(glm::vec4(1.0f, glm::vec3(0.3f)), glm::vec4(1.0f), glm::vec4(1.0f), 64.0f, 0, Shader::shaders["line"], "curve");

        new Material(glm::vec4(1.0f, glm::vec3(0.3f)), glm::vec4(0.5f), glm::vec4(0.9f), 64.0f, 0, Shader::shaders["oscillate"], "moveTorus");

        new Material(glm::vec4(1.0f, glm::vec3(0.3f)), glm::vec4(0.7f), glm::vec4(0.9f), 64.0f, 0, Shader::shaders["lit"], "litMaterial");

        new Material(glm::vec4(1.0f, glm::vec3(0.3f)), glm::vec4(0.7f), glm::vec4(0.9f), 64.0f, litScene, Shader::shaders["post"], "postProcessing");
    }

    QuadRenderer lightQuad(Material::materials["litMaterial"], glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)), "lightQuad");
    QuadRenderer screenQuad(Material::materials["postProcessing"], glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)), "screenQuad");

    Camera camera;
    camera.setPerspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    camera.position = glm::vec3(10.0f, -10.0f, 4.0f);
    camera.target = glm::vec3(-5.0f, 5.0f, 0.0f);
    camera.up = glm::vec3(0.0f, 0.0f, 1.0f);

    new DirectionalLight("Sun", glm::vec3(0.0f, 1.0f, -2.0f));

    SceneGraph scene(camera);
    globalScene = &scene;

    TorusModel torus(Material::materials["torus"], glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -6.0f, 0.0f)), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), "Torus1", 1.2f, 0.5f);
    scene.addRenderer(&torus);

    TorusModel torus2(Material::materials["litMaterial"], glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 5.0f, 0.0f)), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), "Torus2", 1.2f, 0.5f);
    scene.addRenderer(&torus2);

    glm::mat4 t3transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    t3transform = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 8.0f, 0.0f)) * t3transform;
    TorusModel torus3(Material::materials["moveTorus"], t3transform, "Torus3", 2.0f, 0.25f);
    scene.addRenderer(&torus3);

    glm::mat4 justAbove = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.001f));
    BezierCurve line(Material::materials["curve"], justAbove, "Curve1", glm::vec3(-4.0f, -6.0f, 0.0f), glm::vec3(3.0f, 5.0f, 0.0f), 2e-2, fourPointRectangle);
    BezierCurve line2(Material::materials["curve"], justAbove, "Curve2", glm::vec3(3.0f, 5.0f, 0.0f), glm::vec3(7.0f, 8.0f, 0.0f), 2e-2, fourPointRectangle);
    Line line3(Material::materials["curve"], justAbove, "Curve3", glm::vec3(7.0f, 8.0f, 0.0f), glm::vec3(7.0f, 13.0f, 0.0f));
    Arc arc1(Material::materials["curve"], justAbove, "Arc1", glm::vec3(0.0f, 13.0f, 0.0f), glm::vec3(7.0f, 13.0f, 0.0f), glm::pi<float>(), 2e-2);
    Line line4(Material::materials["curve"], justAbove, "Curve4", glm::vec3(-7.0f, 13.0f, 0.0f), glm::vec3(-7.0f, 6.0f, 0.0f));
    BezierCurve line5(Material::materials["curve"], justAbove, "Curve5", glm::vec3(-7.0f, 6.0f, 0.0f), glm::vec3(-10.0f, -6.0f, 0.0f), 2e-2, fourPointRectangle);
    Arc arc2(Material::materials["curve"], justAbove, "Arc2", glm::vec3(-7.0f, -6.0f, 0.0f), glm::vec3(-10.0f, -6.0f, 0.0f), glm::pi<float>(), 2e-2);

    glm::vec3 up(0.0f, 0.0f, 1.0f);
    glm::vec3 left(-1.0f, 0.0f, 0.0f);
    glm::vec3 right(1.0f, 0.0f, 0.0f);
    glm::vec3 back(0.0f, -1.0f, 0.0f);
    glm::vec3 forward(0.0f, 1.0f, 0.0f);
    Road road(Material::materials["road"], glm::mat4(1.0f), "Road1", &line, up, left, left, 1.0f, 0.2f, false);
    Road road2(Material::materials["road"], glm::mat4(1.0f), "Road2", &line2, up, left, left, 1.0f, 0.2f, false);
    Road road3(Material::materials["road"], glm::mat4(1.0f), "Road3", &line3, up, left, left, 1.0f, 0.2f, false);
    Road road4(Material::materials["road"], glm::mat4(1.0f), "Road4", &arc1, up, left, right, 1.0f, 0.2f, false);
    Road road5(Material::materials["road"], glm::mat4(1.0f), "Road5", &line4, up, right, right, 1.0f, 0.2f, false);
    Road road6(Material::materials["road"], glm::mat4(1.0f), "Road6", &line5, up, right, right, 1.0f, 0.2f, false);
    Road road7(Material::materials["road"], glm::mat4(1.0f), "Road7", &arc2, up, right, left, 1.0f, 0.2f, false);
    scene.addRenderer(&road);
    scene.addRenderer(&road2);
    scene.addRenderer(&road3);
    scene.addRenderer(&road4);
    scene.addRenderer(&road5);
    scene.addRenderer(&road6);
    scene.addRenderer(&road7);

    glm::mat4 cTransform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f));
    cTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * cTransform;
    cTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), up) * cTransform;
    cTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) * cTransform;
    Car car(Material::materials["litMaterial"], cTransform, "Car", "data/car/Jeep_Renegade_2016.obj", 1.0f);
    scene.addRenderer(&car);
    car.attachModel(&line);
    car.attachModel(&line2);
    car.attachModel(&line3);
    car.attachModel(&arc1);
    car.attachModel(&line4);
    car.attachModel(&line5);
    car.attachModel(&arc2);

    SphereModel universe(Material::materials["stars"], glm::mat4(1.0f), "Universe", 10);
    universe.options.cullBackFace = false;

    // Deffered Shading setup
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec, gMask;

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // - normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // - color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    glGenTextures(1, &gMask);
    glBindTexture(GL_TEXTURE_2D, gMask);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gMask, 0);

    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

    // finally check if framebuffer is complete
    GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    unsigned int lBuffer;
    glGenFramebuffers(1, &lBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lBuffer);

    glBindTexture(GL_TEXTURE_2D, litScene);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, litScene, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    unsigned int lattachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, lattachments);

    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Lighting Framebuffer not complete";
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


    std::shared_ptr<Shader> lightPass = Shader::shaders["light"];
    lightPass->use();
    lightPass->setInt("gPosition", 0);
    lightPass->setInt("gNormal", 1);
    lightPass->setInt("gAlbedoSpec", 2);
    lightPass->setInt("gMask", 3);
    lightPass->setVec2("ScreenSize", SCR_WIDTH, SCR_HEIGHT);

    std::shared_ptr<Shader> postPass = Shader::shaders["post"];
    postPass->use();
    postPass->setInt("gMask", 1);

    // render loop
    // -----------

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        // just like in a game engine, it's useful to know the delta time
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // glfw: poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();

        car.update(deltaTime);

        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //Geometry render pass
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glClear(GL_DEPTH_BUFFER_BIT);

            //render sphere environment map
            scene.pass = SceneGraph::RenderPass::FORWARD;
            universe.render(glm::mat4(glm::mat3(glm::lookAt(scene.camera.position, scene.camera.target, scene.camera.up))), 
                            scene.camera.projection(), deltaTime, &scene);

            glEnable(GL_DEPTH_TEST);

            scene.render(deltaTime, SceneGraph::RenderPass::GEOMETRY);
            glDisable(GL_DEPTH_TEST);
        }

        //Lighting render pass
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lBuffer);
            glClear(GL_COLOR_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, gMask);

            scene.pass = SceneGraph::RenderPass::LIGHTING;
            lightQuad.render(glm::mat4(1.0f), glm::mat4(1.0f), deltaTime, &scene);
        }

        //Post-processing pass
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gMask);

            scene.pass = SceneGraph::RenderPass::POST;
            screenQuad.render(glm::mat4(1.0f), glm::mat4(1.0f), deltaTime, &scene);
        }

        // draw imGui over the top
        drawIMGUI(&line, &scene);

        glfwSwapBuffers(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions
    glViewport(0, 0, scr_width = width, scr_height = height);

    //globalScene->camera.setAspect((float)width / (float)height);
}

#pragma warning(pop)