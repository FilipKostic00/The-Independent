#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void renderQuad();

unsigned int loadCubemap(vector<std::string> faces);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 islandModelPosition = glm::vec3(0.0f);
    glm::vec3 lighthouseModelPosition = glm::vec3(0.0f);
    glm::vec3 shedModelPosition = glm::vec3(0.0f);
    glm::vec3 picnicTableModelPosition = glm::vec3(0.0f);
    glm::vec3 treeModelPosition = glm::vec3(0.0f);
    glm::vec3 roundTableModelPosition = glm::vec3(0.0f);
    glm::vec3 candleModelPosition = roundTableModelPosition;
    glm::vec3 firewoodModelPosition = glm::vec3(0.0f);
    float islandModelScale = 0.1f;
    float eyeModelScale = 1.0f;
    float lighthouseModelScale = 0.3f;
    float shedModelScale = 0.3f;
    float picnicTableModelScale = 1.0f;
    float treeModelScale = 1.0f;
    float roundTableModelScale = 1.0f;
    float candleModelScale = 0.1f;
    float firewoodModelScale = 0.1f;
    float exposure = 0.6f;
    float gamma = 1.255f;
    bool blinn = false;
    bool isCamSpotLightEnabled = false;
    bool hdr = true;
    bool bloom = true;
    PointLight eyePointLight1;
    PointLight eyePointLight2;
    PointLight candlePointLight;
    SpotLight cameraSpotLight;
    DirLight dirLight;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 20.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n'
        << lighthouseModelPosition.x << '\n'
        << lighthouseModelPosition.y << '\n'
        << lighthouseModelPosition.z << '\n'
        << lighthouseModelScale << '\n'
        << shedModelPosition.x << '\n'
        << shedModelPosition.y << '\n'
        << shedModelPosition.z << '\n'
        << shedModelScale << '\n'
        << picnicTableModelPosition.x << '\n'
        << picnicTableModelPosition.y << '\n'
        << picnicTableModelPosition.z << '\n'
        << picnicTableModelScale << '\n'
        << treeModelPosition.x << '\n'
        << treeModelPosition.y << '\n'
        << treeModelPosition.z << '\n'
        << treeModelScale << '\n'
        << roundTableModelPosition.x << '\n'
        << roundTableModelPosition.y << '\n'
        << roundTableModelPosition.z << '\n'
        << roundTableModelScale << '\n'
        << candleModelPosition.x << '\n'
        << candleModelPosition.y << '\n'
        << candleModelPosition.z << '\n'
        << candleModelScale << '\n'
        << firewoodModelPosition.x << '\n'
        << firewoodModelPosition.y << '\n'
        << firewoodModelPosition.z << '\n'
        << firewoodModelScale << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z
           >> lighthouseModelPosition.x
           >> lighthouseModelPosition.y
           >> lighthouseModelPosition.z
           >> lighthouseModelScale
           >> shedModelPosition.x
           >> shedModelPosition.y
           >> shedModelPosition.z
           >> shedModelScale
           >> picnicTableModelPosition.x
           >> picnicTableModelPosition.y
           >> picnicTableModelPosition.z
           >> picnicTableModelScale
           >> treeModelPosition.x
           >> treeModelPosition.y
           >> treeModelPosition.z
           >> treeModelScale
           >> roundTableModelPosition.x
           >> roundTableModelPosition.y
           >> roundTableModelPosition.z
           >> roundTableModelScale
           >> candleModelPosition.x
           >> candleModelPosition.y
           >> candleModelPosition.z
           >> candleModelScale
           >> firewoodModelPosition.x
           >> firewoodModelPosition.y
           >> firewoodModelPosition.z
           >> firewoodModelScale;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;



    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader hdrShader("resources/shaders/hdr.vs", "resources/shaders/hdr.fs");
    Shader blurShader("resources/shaders/blur.vs", "resources/shaders/blur.fs");

    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }



    // load models
    // -----------
    Model islandModel("resources/objects/island/island.obj");
    islandModel.SetShaderTextureNamePrefix("material.");

    Model eyeModel1("resources/objects/eyeball/eyeball.obj");
    eyeModel1.SetShaderTextureNamePrefix("material.");

    Model eyeModel2("resources/objects/eyeball/eyeball.obj");
    eyeModel2.SetShaderTextureNamePrefix("material.");

    Model lighthouseModel("resources/objects/lighthouse/lighthouse.obj");
    lighthouseModel.SetShaderTextureNamePrefix("material.");

    Model shedModel("resources/objects/shed/shed.obj");
    shedModel.SetShaderTextureNamePrefix("material.");

    Model picnicTableModel("resources/objects/picnic table/picnic_table.obj");
    picnicTableModel.SetShaderTextureNamePrefix("material.");

    Model treeModel("resources/objects/tree/tree.obj");
    treeModel.SetShaderTextureNamePrefix("material.");

    Model roundTableModel("resources/objects/round-table/round_table.obj");
    roundTableModel.SetShaderTextureNamePrefix("material.");

    Model candleModel("resources/objects/candle/candle.obj");
    candleModel.SetShaderTextureNamePrefix("material.");

    Model firewoodModel("resources/objects/firewood/firewood.obj");
    firewoodModel.SetShaderTextureNamePrefix("material.");

    //Eye point light 1
    PointLight& eyePointLight1 = programState->eyePointLight1;
    eyePointLight1.position = glm::vec3(4.0f, 4.0, 0.0);
    eyePointLight1.ambient = glm::vec3(0.735f, 0.735f, 0.735f);
    eyePointLight1.diffuse = glm::vec3(0.8, 0.8, 0.8);
    eyePointLight1.specular = glm::vec3(1.0, 1.0, 1.0);

    eyePointLight1.constant = 0.505f;
    eyePointLight1.linear = 0.025f;
    eyePointLight1.quadratic = 0.015f;

    //Eye point light 2
    PointLight& eyePointLight2 = programState->eyePointLight2;
    eyePointLight2.position = glm::vec3(4.0f, 4.0, 0.0);
    eyePointLight2.ambient = glm::vec3(0.735f, 0.735f, 0.735f);
    eyePointLight2.diffuse = glm::vec3(0.8, 0.8, 0.8);
    eyePointLight2.specular = glm::vec3(1.0, 1.0, 1.0);

    eyePointLight2.constant = 0.505f;
    eyePointLight2.linear = 0.025f;
    eyePointLight2.quadratic = 0.015f;

    //Camera Spotlight
    SpotLight& cameraSpotlight = programState->cameraSpotLight;
    cameraSpotlight.direction = programState->camera.Front;
    cameraSpotlight.position = programState->camera.Position;
    cameraSpotlight.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraSpotlight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    cameraSpotlight.specular = glm::vec3(1.0, 1.0, 1.0);
    cameraSpotlight.cutOff = glm::cos(glm::radians(12.5f));
    cameraSpotlight.outerCutOff = glm::cos(glm::radians(15.0f));

    cameraSpotlight.constant = 1.0f;
    cameraSpotlight.linear = 0.09f;
    cameraSpotlight.quadratic = 0.032f;

    //Dir light
    DirLight& dirLight = programState->dirLight;
    dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight.ambient = glm::vec3(0.02f, 0.02f, 0.02f);
    dirLight.diffuse = glm::vec3(0.01f, 0.01f, 0.01f);
    dirLight.specular = glm::vec3(0.01f, 0.01f, 0.01f);


    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };


    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // load textures
    // -------------
    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox/skybox_right.png"),
                    FileSystem::getPath("resources/textures/skybox/skybox_left.png"),
                    FileSystem::getPath("resources/textures/skybox/skybox_bottom.png"),
                    FileSystem::getPath("resources/textures/skybox/skybox_top.png"),
                    FileSystem::getPath("resources/textures/skybox/skybox_front.png"),
                    FileSystem::getPath("resources/textures/skybox/skybox_back.png")
            };
    unsigned int cubemapTexture = loadCubemap(faces);

    hdrShader.use();
    hdrShader.setInt("scene", 0);
    hdrShader.setInt("blumBloor",0);
    blurShader.use();
    blurShader.setInt("image", 0);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        ourShader.setVec3("viewPosition", programState->camera.Position);
        //Dir Light
        ourShader.setVec3("dirLight.direction", dirLight.direction);
        ourShader.setVec3("dirLight.ambient", dirLight.ambient);
        ourShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        ourShader.setVec3("dirLight.specular", dirLight.specular);

        //Eye point light 1
        eyePointLight1.position = glm::vec3(20.0 * cos(currentFrame / 2), 10.0 * sin(currentFrame / 2), 10.0 * sin(currentFrame / 2));
        ourShader.setVec3("eyePointLight1.position", eyePointLight1.position);
        ourShader.setVec3("eyePointLight1.ambient", eyePointLight1.ambient);
        ourShader.setVec3("eyePointLight1.diffuse", eyePointLight1.diffuse);
        ourShader.setVec3("eyePointLight1.specular", eyePointLight1.specular);
        ourShader.setFloat("eyePointLight1.constant", eyePointLight1.constant);
        ourShader.setFloat("eyePointLight1.linear", eyePointLight1.linear);
        ourShader.setFloat("eyePointLight1.quadratic", eyePointLight1.quadratic);

        //Eye point light 2
        eyePointLight2.position = glm::vec3(-10.0 * sin(currentFrame / 2), -10.0 * sin(currentFrame / 2), -20.0 * cos(currentFrame / 2));
        ourShader.setVec3("eyePointLight2.position", eyePointLight2.position);
        ourShader.setVec3("eyePointLight2.ambient", eyePointLight2.ambient);
        ourShader.setVec3("eyePointLight2.diffuse", eyePointLight2.diffuse);
        ourShader.setVec3("eyePointLight2.specular", eyePointLight2.specular);
        ourShader.setFloat("eyePointLight2.constant", eyePointLight2.constant);
        ourShader.setFloat("eyePointLight2.linear", eyePointLight2.linear);
        ourShader.setFloat("eyePointLight2.quadratic", eyePointLight2.quadratic);

        //Camera spotlight

        cameraSpotlight.position = programState->camera.Position;
        cameraSpotlight.direction = programState->camera.Front;
        ourShader.setVec3("cameraSpotLight.position", cameraSpotlight.position);
        ourShader.setVec3("cameraSpotLight.direction", cameraSpotlight.direction);
        ourShader.setVec3("cameraSpotLight.ambient", cameraSpotlight.ambient);
        ourShader.setVec3("cameraSpotLight.diffuse", cameraSpotlight.diffuse);
        ourShader.setVec3("cameraSpotLight.specular", cameraSpotlight.specular);
        ourShader.setFloat("cameraSpotLight.constant", cameraSpotlight.constant);
        ourShader.setFloat("cameraSpotLight.linear", cameraSpotlight.linear);
        ourShader.setFloat("cameraSpotLight.quadratic", cameraSpotlight.quadratic);\
        ourShader.setFloat("cameraSpotLight.cutOff", cameraSpotlight.cutOff);
        ourShader.setFloat("cameraSpotLight.outerCutOff", cameraSpotlight.outerCutOff);

        ourShader.setBool("blinn", programState->blinn);
        ourShader.setFloat("isCamSpotLightEnabled", programState->isCamSpotLightEnabled);

        // render the island model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               programState->islandModelPosition); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(programState->islandModelScale));    // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        islandModel.Draw(ourShader);

        // render eye model 1
        glm::mat4 eyeball1 = glm::mat4(1.0f);
        eyeball1 = glm::translate(eyeball1, eyePointLight1.position); //Eye moves like light

        // Calculate the direction from the eye position to the target point (shed position)
        glm::vec3 targetPosition = glm::vec3(programState->shedModelPosition);
        glm::vec3 direction = glm::normalize(targetPosition - eyePointLight1.position);

        // Calculate the angles to rotate the eye to align it with the direction
        float yaw = atan2(direction.z, direction.x);
        float pitch = asin(direction.y);
        eyeball1 = glm::rotate(eyeball1, -yaw, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around y-axis (yaw)
        eyeball1 = glm::rotate(eyeball1, pitch, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around z-axis (pitch)
        eyeball1 = glm::scale(eyeball1, glm::vec3(programState->eyeModelScale));
        ourShader.setMat4("model", eyeball1);
        eyeModel1.Draw(ourShader);

        // render eye model 2
        glm::mat4 eyeball2 = glm::mat4(1.0f);
        eyeball2 = glm::translate(eyeball2, eyePointLight2.position); //Eye moves like light

        direction = glm::normalize(targetPosition - eyePointLight2.position);
        yaw = atan2(direction.z, direction.x);
        pitch = asin(direction.y);

        eyeball2 = glm::rotate(eyeball2, -yaw, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around y-axis (yaw)
        eyeball2 = glm::rotate(eyeball2, pitch, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around z-axis (pitch)
        eyeball2 = glm::scale(eyeball2, glm::vec3(programState->eyeModelScale));
        ourShader.setMat4("model", eyeball2);
        eyeModel2.Draw(ourShader);

        // render the lighthouse model
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glm::mat4 lighthouse = glm::mat4(1.0f);
        lighthouse = glm::translate(lighthouse, programState->lighthouseModelPosition);
        lighthouse = glm::scale(lighthouse, glm::vec3(programState->lighthouseModelScale));
        ourShader.setMat4("model", lighthouse);
        lighthouseModel.Draw(ourShader);
        glDisable(GL_CULL_FACE);

        // render shed model
        glm::mat4 shed = glm::mat4(1.0f);
        shed = glm::translate(shed, programState->shedModelPosition);
        shed = glm::scale(shed, glm::vec3(programState->shedModelScale));
        ourShader.setMat4("model", shed);
        shedModel.Draw(ourShader);

        // render picnic table model
        glm::mat4 picnicTable = glm::mat4(1.0f);
        picnicTable = glm::translate(picnicTable, programState->picnicTableModelPosition);
        picnicTable = glm::scale(picnicTable, glm::vec3(programState->picnicTableModelScale));
        ourShader.setMat4("model", picnicTable);
        picnicTableModel.Draw(ourShader);

        // render tree model
        glm::mat4 tree = glm::mat4 (1.0f);
        tree = glm::translate(tree, programState->treeModelPosition);
        tree = glm::scale(tree, glm::vec3(programState->treeModelScale));
        ourShader.setMat4("model", tree);
        treeModel.Draw(ourShader);

        // render round table model
        glm::mat4 roundTable = glm::mat4(1.0f);
        roundTable = glm::translate(roundTable, programState->roundTableModelPosition);
        roundTable = glm::scale(roundTable, glm::vec3(programState->roundTableModelScale));
        ourShader.setMat4("model", roundTable);
        roundTableModel.Draw(ourShader);

        // render candle model
        glm::mat4 candle = glm::mat4(1.0f);
        candle = glm::translate(candle, programState->candleModelPosition);
        candle = glm::scale(candle, glm::vec3(programState->candleModelScale));
        ourShader.setMat4("model", candle);
        candleModel.Draw(ourShader);

        // render firewood model
        glm::mat4 firewood = glm::mat4(1.0f);
        firewood = glm::translate(firewood, programState->firewoodModelPosition);
        firewood = glm::scale(firewood, glm::vec3(programState->firewoodModelScale));
        ourShader.setMat4("model", firewood);
        firewoodModel.Draw(ourShader);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(programState->camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        blurShader.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        hdrShader.setInt("hdr", programState->hdr);
        hdrShader.setInt("bloom", programState->bloom);
        hdrShader.setFloat("exposure", programState->exposure);
        hdrShader.setFloat("gamma", programState->gamma);
        renderQuad();

        if (programState->ImGuiEnabled)
            DrawImGui(programState);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// Load skybox texture
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}
//TO DO: Tidy up gui
void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
   // ImGui::ShowDemoWindow();


    {
        static float f = 0.0f;
        ImGui::Begin("Controls");
        ImGui::Text(programState->hdr ? "HDR ON" : "HDR OFF");
        ImGui::Text(programState->bloom ? "Bloom ON" : "Bloom OFF");
        if(ImGui::CollapsingHeader("HDR"))
        {
            ImGui::DragFloat("Exposure", (float *) &programState->exposure, 0.005f, 0.1f, 1.0f);
            ImGui::DragFloat("Gamma", (float *) &programState->gamma, 0.005f, 0.1f, 4.0f);
        }

        if(ImGui::CollapsingHeader("Bloom"))
        {
            ImGui::DragFloat("Exposure", (float *) &programState->exposure, 0.005f, 0.1f, 1.0f);
            ImGui::DragFloat("Gamma", (float *) &programState->gamma, 0.005f, 0.1f, 4.0f);
        }

        if(ImGui::CollapsingHeader("Backgorund"))
        {
            if(ImGui::TreeNode("Color"))
            {
                ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
                ImGui::TreePop();
            }
        }
        if(ImGui::CollapsingHeader("Models"))
        {
            if(ImGui::TreeNode("Island"))
            {
                if(ImGui::TreeNode("Position"))
                {
                    // Island controls
                    ImGui::DragFloat3("Island position", (float*)&programState->islandModelPosition);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Scale"))
                {
                    ImGui::DragFloat("Island scale", &programState->islandModelScale, 0.005, 0.1, 4.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Lighthouse"))
            {
                if(ImGui::TreeNode("Position"))
                {
                    ImGui::DragFloat3("Lighthouse position", (float*)&programState->lighthouseModelPosition,0.005f);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Scale"))
                {
                    ImGui::DragFloat("Lighthouse scale", &programState->lighthouseModelScale, 0.005, 0.1, 4.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Shed"))
            {
                if(ImGui::TreeNode("Position"))
                {
                    ImGui::DragFloat3("Shed position", (float*)&programState->shedModelPosition,0.005f);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Scale"))
                {
                    ImGui::DragFloat("Shed scale", &programState->shedModelScale, 0.005, 0.1, 4.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Picnic Table"))
            {
                if(ImGui::TreeNode("Position"))
                {
                    ImGui::DragFloat3("Picnic table position", (float*)&programState->picnicTableModelPosition,0.005f);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Scale"))
                {
                    ImGui::DragFloat("Picnic table scale", &programState->picnicTableModelScale, 0.005, 0.1, 4.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Tree"))
            {
                if(ImGui::TreeNode("Position"))
                {
                    ImGui::DragFloat3("Tree position", (float*)&programState->treeModelPosition,0.005f);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Scale"))
                {
                    ImGui::DragFloat("Tree scale", &programState->treeModelScale, 0.005, 0.1, 4.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Round Table"))
            {
                if(ImGui::TreeNode("Position"))
                {
                    ImGui::DragFloat3("Round Table position", (float*)&programState->roundTableModelPosition,0.005f);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Scale"))
                {
                    ImGui::DragFloat("Round Table scale", &programState->roundTableModelScale, 0.005, 0.1, 4.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Candle"))
            {
                if(ImGui::TreeNode("Position"))
                {
                    ImGui::DragFloat3("Candle position", (float*)&programState->candleModelPosition,0.005f);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Scale"))
                {
                    ImGui::DragFloat("Candle scale", &programState->candleModelScale, 0.005, 0.1, 4.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Firewood"))
            {
                if(ImGui::TreeNode("Position"))
                {
                    ImGui::DragFloat3("firewood position", (float*)&programState->firewoodModelPosition,0.005f);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Scale"))
                {
                    ImGui::DragFloat("firewood scale", &programState->firewoodModelScale, 0.005, 0.1, 4.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Eyeballs"))
            {
                if(ImGui::TreeNode("Scale"))
                {
                    ImGui::DragFloat("Planet scale", &programState->eyeModelScale, 0.05, 0.1, 4.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }

        if(ImGui::CollapsingHeader("Lights"))
        {
            ImGui::BulletText(programState->blinn ? "Blinn" : "Phong");
            if(ImGui::TreeNode("Eyeball 1 Point Light"))
            {
                if(ImGui::TreeNode("ADS"))
                {
                    ImGui::DragFloat3("Ambient", (float*)&programState->eyePointLight1.ambient, 0.005, 0.0, 1.0);
                    ImGui::DragFloat3("Diffuse", (float*)&programState->eyePointLight1.diffuse, 0.005, 0.0, 1.0);
                    ImGui::DragFloat3("Specular", (float*)&programState->eyePointLight1.specular, 0.005, 0.0, 1.0);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Attenuation"))
                {
                    ImGui::DragFloat("Constant", (float*)&programState->eyePointLight1.constant, 0.005, 0.0, 1.0);
                    ImGui::DragFloat("Linear", (float*)&programState->eyePointLight1.linear, 0.005, 0.0, 1.0);
                    ImGui::DragFloat("Quadratic", (float*)&programState->eyePointLight1.quadratic, 0.005, 0.0, 1.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Eyeball 2 Point Light"))
            {
                if(ImGui::TreeNode("ADS"))
                {
                    ImGui::DragFloat3("Ambient", (float*)&programState->eyePointLight2.ambient, 0.005, 0.0, 1.0);
                    ImGui::DragFloat3("Diffuse", (float*)&programState->eyePointLight2.diffuse, 0.005, 0.0, 1.0);
                    ImGui::DragFloat3("Specular", (float*)&programState->eyePointLight2.specular, 0.005, 0.0, 1.0);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Attenuation"))
                {
                    ImGui::DragFloat("Constant", (float*)&programState->eyePointLight2.constant, 0.005, 0.0, 1.0);
                    ImGui::DragFloat("Linear", (float*)&programState->eyePointLight2.linear, 0.005, 0.0, 1.0);
                    ImGui::DragFloat("Quadratic", (float*)&programState->eyePointLight2.quadratic, 0.005, 0.0, 1.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Candle Point Light"))
            {
                if(ImGui::TreeNode("Position"))
                {
                    ImGui::DragFloat3("Candle point light position", (float*)&programState->candlePointLight.position, 0.005);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("ADS"))
                {
                    ImGui::DragFloat3("Ambient", (float*)&programState->candlePointLight.ambient, 0.005, 0.0, 1.0);
                    ImGui::DragFloat3("Diffuse", (float*)&programState->candlePointLight.diffuse, 0.005, 0.0, 1.0);
                    ImGui::DragFloat3("Specular", (float*)&programState->candlePointLight.specular, 0.005, 0.0, 1.0);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Attenuation"))
                {
                    ImGui::DragFloat("Constant", (float*)&programState->candlePointLight.constant, 0.005, 0.0, 1.0);
                    ImGui::DragFloat("Linear", (float*)&programState->candlePointLight.linear, 0.005, 0.0, 1.0);
                    ImGui::DragFloat("Quadratic", (float*)&programState->candlePointLight.quadratic, 0.005, 0.0, 1.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNode("World Directional Light"))
            {

                if(ImGui::TreeNode("ADS"))
                {
                    ImGui::DragFloat3("Ambient", (float*)&programState->dirLight.ambient, 0.005, 0.0, 1.0);
                    ImGui::DragFloat3("Diffuse", (float*)&programState->dirLight.diffuse, 0.005, 0.0, 1.0);
                    ImGui::DragFloat3("Specular", (float*)&programState->dirLight.specular, 0.005, 0.0, 1.0);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }

        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        programState->CameraMouseMovementUpdateEnabled = !programState->ImGuiEnabled;
    }

    if(key == GLFW_KEY_B && action == GLFW_PRESS)
        programState->blinn = !programState->blinn;
    if(key == GLFW_KEY_F && action == GLFW_PRESS)
        programState->isCamSpotLightEnabled = !programState->isCamSpotLightEnabled;
    if(key == GLFW_KEY_H && action == GLFW_PRESS)
        programState->hdr = !programState->hdr;
    if(key == GLFW_KEY_J && action == GLFW_PRESS)
        programState->bloom = !programState->bloom;
}
