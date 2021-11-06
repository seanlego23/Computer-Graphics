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

#include "Camera.h"
#include "shader_s.h"
#include "renderer.h"
#include "Material.h"
#include "SceneGraph.h"

#include "BezierCurve.h"
#include "QuadRenderer.h"
#include "CubeRenderer.h"
#include "TorusModel.h"
#include "Car.h"
#include "Road.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

glm::mat4 pMat; // perspective matrix
glm::mat4 vMat; // view matrix

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

unsigned int texture;
unsigned int grass_texture;
unsigned int road_texture;

// image buffer used by raster drawing basics.cpp
extern unsigned char imageBuff[512][512][3];

int myTexture();

#pragma warning( disable : 26451 )

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void setupTextures()
{
    // create textures 
        // -------------------------
    glGenTextures(1, &texture);
    glGenTextures(1, &grass_texture);
    glGenTextures(1, &road_texture);

    // texture is a buffer we will be generating for pixel experiments
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load image, create texture and generate mipmaps
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, (const void*)imageBuff);
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
                    editShader = val;
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
        new Shader("data/vertex.glsl", "data/fragment.glsl", "ground");

        new Shader("data/vtorus.glsl", "data/ftorus.glsl", "torus");

        new Shader("data/vOscillate.glsl", "data/fOscillate.glsl", "oscillate");

        new Shader("data/vRoad.glsl", "data/fRoad.glsl", "road");

        new Shader("data/vBezier.glsl", "data/fBezier.glsl", "line");

        new Shader("data/vLit.glsl", "data/fLit.glsl", "material");
    }

    myTexture();
    setupTextures();

    {
        new Material(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f,
                     grass_texture, Shader::shaders["ground"], "grass");

        new Material(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, 
                     0, Shader::shaders["torus"], "torus");

        new Material(glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f), 0.0f, road_texture, Shader::shaders["road"], "road");

        new Material(glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f), 0.0f, 0, Shader::shaders["line"], "curve");

        new Material(glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f), 0.0f, 0, Shader::shaders["oscillate"], "moveTorus");

        new Material(glm::vec4(0.3f), glm::vec4(0.7f), glm::vec4(0.3f), 64.0f, 0, Shader::shaders["material"], "litMaterial");
    }

    Camera camera;
    camera.setPerspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
    camera.position = glm::vec3(-3.0f, -10.0f, 5.0f);
    camera.target = glm::vec3(0.0f);
    camera.up = glm::vec3(0.0f, 0.0f, 1.0f);

    DirectionalLight light(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.4f), glm::vec3(1.0f), glm::vec3(1.0f));

    SceneGraph scene(camera, &light);

    TorusModel torus(1.2f, 0.5f, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -6.0f, 0.0f)), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), Material::materials["torus"]);
    scene.addRenderer(&torus);

    TorusModel torus2(1.2f, 0.5f, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 5.0f, 0.0f)), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), Material::materials["litMaterial"]);
    scene.addRenderer(&torus2);

    glm::mat4 t3transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    t3transform = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 8.0f, 0.0f)) * t3transform;
    TorusModel torus3(2.0f, 0.25f, t3transform, Material::materials["moveTorus"]);
    scene.addRenderer(&torus3);

    glm::mat4 justAbove = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.001f));
    BezierCurve line(Material::materials["curve"], justAbove, glm::vec3(-4.0f, -6.0f, 0.0f), glm::vec3(3.0f, 5.0f, 0.0f), 2e-1, fourPointRectangle);
    BezierCurve line2(Material::materials["curve"], justAbove, glm::vec3(3.0f, 5.0f, 0.0f), glm::vec3(7.0f, 8.0f, 0.0f), 2e-2, fourPointRectangle);
    Line line3(Material::materials["curve"], justAbove, glm::vec3(7.0f, 8.0f, 0.0f), glm::vec3(7.0f, 13.0f, 0.0f));

    glm::vec3 up(0.0f, 0.0f, 1.0f);
    glm::vec3 left(-1.0f, 0.0f, 0.0f);
    Road road(Material::materials["road"], glm::mat4(1.0f), &line, up, left, left, 1.0f, 0.5f, true);
    scene.addRenderer(&road);

    Road road2(Material::materials["road"], glm::mat4(1.0f), &line2, up, left, left, 1.0f, 0.5f, false);
    scene.addRenderer(&road2);

    Road road3(Material::materials["road"], glm::mat4(1.0f), &line3, up, left, left, 1.0f, 0.5f, false);
    scene.addRenderer(&road3);

    glm::mat4 cTransform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f));
    cTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * cTransform;
    cTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), up) * cTransform;
    cTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) * cTransform;
    Car car(nullptr, cTransform, "data/car/Jeep_Renegade_2016.obj", 2.0f);
    scene.addRenderer(&car);

    // render loop
    // -----------

    /* Deffered Shading setup
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gColorSpec;

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // - normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // - color + specular color buffer
    glGenTextures(1, &gColorSpec);
    glBindTexture(GL_TEXTURE_2D, gColorSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);
    */

    double lastTime = glfwGetTime();
    
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        // just like in a game engine, it's useful to know the delta time
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // glfw: poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();

        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        

        // render background
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        scene.render(deltaTime);

        // draw imGui over the top
        drawIMGUI(&line, &scene);

        glfwSwapBuffers(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    for (auto [str, s] : Shader::shaders)
        delete s;

    for (auto [str, m] : Material::materials)
        delete m;
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);

    pMat = glm::perspective(1.0472f, (float)width / (float)height, 0.1f, 1000.0f);	//  1.0472 radians = 60 degrees
}
