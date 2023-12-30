#include <iostream>

// OpenGL及其附属库头文件
#include <glad/glad.h>
#include <GLFW//glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Assimp 库头文件
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//工具 头文件
#include "Shader.h"
#include "Texture.h"
#include "Camera.hpp"
#include "Mesh.h"
#include "Model.h"

static const int SCR_WIDTH = 1920;
static const int SCR_HEIGHT = 1080;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

float deltaTime = 0.0f;
bool firstMouse = true;

bool flashLight = false;
bool lampLight = false;
bool pillowFront = false;
bool FocalorsSee = false;
bool tvOff = true;
int roomCenterLight = 1;

float FUHUA_rotateSpeed = 500.0f;

static Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

/**
 * 帧缓冲回调函数
 * @param window  窗口
 * @param width  窗口宽度
 * @param height  窗口高度
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

/**
 * 鼠标操作回调函数
 * @param window
 * @param xposIn
 * @param yposIn
 */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

/**
 * 处理IO输入，即键盘操作,使用回调方式处理
 * @param window
 * @param key
 * @param scancode
 * @param action
 * @param mods
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_F && action == GLFW_PRESS){
        flashLight = ! flashLight;
    }
    if(key == GLFW_KEY_L && action == GLFW_PRESS){
        lampLight = ! lampLight;
    }
    if(key == GLFW_KEY_UP && action == GLFW_PRESS){
        roomCenterLight++ ;
        roomCenterLight = roomCenterLight > 3 ? 3: roomCenterLight;
    }
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
        roomCenterLight-- ;
        roomCenterLight = roomCenterLight < 0 ? 0: roomCenterLight;
    }
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
        FUHUA_rotateSpeed -= 500.0f;
        FUHUA_rotateSpeed = FUHUA_rotateSpeed <= -5000.0f ?
                -5000.0f : FUHUA_rotateSpeed;
    }
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
        FUHUA_rotateSpeed += 500.0f;
        FUHUA_rotateSpeed = FUHUA_rotateSpeed >= 5000.0f ?
                5000.0f : FUHUA_rotateSpeed;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        pillowFront = !pillowFront;
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        FocalorsSee = !FocalorsSee;
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        tvOff = !tvOff;
    }
}

/**
 * 处理IO输入，即键盘操作,使用侦听方式处理
 * @param window 窗口
 */
void ProcessInput(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS){
        std::cout << "Camera Location:"
                  << camera.Position.x << " "
                  << camera.Position.y << " "
                  << camera.Position.z << "\n";
    }

}

/**
 * 对cube格式的顶点进行绑定VAO操作
 * @param VAO 待绑定的顶点数组对象
 * @param VBO 待绑定的顶点缓冲对象
 * @param vertices 待绑定的顶点数组，元素格式为 位置（3float） 法向量（3float） 纹理坐标（2float）
 * @param size 顶点数组数据大小
 */
void BindVAO_VBO(unsigned int & VAO, unsigned int & VBO, const float* vertices, int size){
    // 生成VAO与VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    // 绑定Vertex位置信息
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    // 绑定Vertex纹理信息
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 绑定Normal向量信息
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //解绑VAO与VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

int main() {
    // 初始化glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw 创建窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 设置帧缓冲回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 设置键盘回调函数
    glfwSetKeyCallback(window, key_callback);
    // 设置鼠标输入回调函数
    glfwSetCursorPosCallback(window, mouse_callback);
    // 设置鼠标输入
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 加载glad函数指针
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float cube[] = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // 加载模型
    // ______________________________________________________________

    auto* model_FuHua = new Model("model/FuHua/FuHua.pmx", false);

    auto* model_Focalors = new Model("model/Focalors/Focalors.pmx", false);
    
    // 加载着色器信息
    // ______________________________________________________________
    // 基础着色器
    auto* basicShader = new Shader("shader/vertex.glsl" ,
                                   "shader/fragment.glsl");
    // 光源着色器
    auto* lightShader = new Shader("Shader/vertex.glsl",
                                   "Shader/lightFragment.glsl");

    // 发光物体着色器
    auto* lightingShader = new Shader("shader/vertex.glsl",
                                   "shader/mutiLightFragment.glsl");

    // 模型绘制基础着色器
    auto* modelShader = new Shader("shader/modelVertex.glsl",
                                   "shader/modelFragment.glsl");

    // 光照模型着色器
    auto* modelLightingShader = new Shader("shader/modelLightingVertex.glsl",
                                   "shader/modelLightingFragment.glsl");
    // 加载纹理材质信息
    // ______________________________________________________________
    // 测试木箱纹理
    auto* testBoxTexture = new TextureInf("texture/testBox.png");
    auto* testBoxSpecularTexture = new TextureInf("texture/testBoxspecular.png");

    // 草地纹理
    auto* grassTexture = new TextureInf("texture/grass.png");

    // 地板纹理
    auto* floorTexture = new TextureInf("texture/floor.png");

    // 墙面纹理
    auto* wallTexture = new TextureInf("texture/wall.png");

    // 天花板纹理
    auto* ceilingTexture = new TextureInf("texture/ceiling.png");

    // 桌面纹理
    auto* tableTexture = new TextureInf("texture/table.png");

    // 椅子纹理
    auto* chairTexture = new TextureInf("texture/chair.png");

    // 台灯纹理
    auto* lampTexture = new TextureInf("texture/lamp.png");

    // 床面纹理
    auto* bedTexture = new TextureInf("texture/bed.jpg");

    // 枕头纹理
    auto* pillowTexture = new TextureInf("texture/pillow.jpg");

    // 电视纹理
    auto* tvTexture = new TextureInf("texture/TV.jpg");

    // 绑定VAO与VBO
    // ______________________________________________________________
    unsigned int cubeVAO, cubeVBO;
    BindVAO_VBO(cubeVAO, cubeVBO, cube, sizeof cube);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {

        // 计算两次绘制的时间差
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 初始化视角变换矩阵
        auto view = glm::mat4 (1.0f);
        view  = camera.GetViewMatrix();
        // 初始化投影变换矩阵
        auto projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        auto model = glm::mat4 (1.0f);

        // 处理IO输入
        ProcessInput(window);

        // 绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, testBoxTexture->getID());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, testBoxSpecularTexture->getID());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, grassTexture->getID());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, floorTexture->getID());
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, wallTexture->getID());
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ceilingTexture->getID());
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, tableTexture->getID());
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, chairTexture->getID());
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, lampTexture->getID());
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, bedTexture->getID());
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, pillowTexture->getID());
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, tvTexture->getID());

        // 清除缓存
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        modelShader->use();
        modelShader->setMat4f("view", view);
        modelShader->setMat4f("projection", projection);


        // 绘制芙卡洛斯
        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-2, -0.3, -3.0));
        model = glm::scale(model, glm::vec3(0.05,0.05,0.05));

        // 计算芙卡洛斯看向摄像机的旋转角
        if (FocalorsSee) {
            auto v1 = glm::normalize(glm::vec3(camera.Position.x, 0, camera.Position.z)
                - glm::vec3(-2.0, 0, -3.0));
            auto v2 = glm::vec3(0, 0, -1);
            auto rotate_angle = glm::radians(180.0) - glm::acos(glm::dot(v1, v2));
            rotate_angle = camera.Position.x < -2 ? glm::radians(360.0) - rotate_angle : rotate_angle;
            model = glm::rotate(model, (float)rotate_angle, glm::vec3(0.0, 1.0, 0.0));
        }
        else {
            model = glm::rotate(model, glm::radians((float) 45), glm::vec3(0.0, 1.0, 0.0));
        }
        modelShader->setMat4f("model", model);
        model_Focalors->Draw(*modelShader);


        modelLightingShader->use();
        modelLightingShader->setVec3f("viewPos", camera.Position);
        modelLightingShader->setMat4f("view", view);
        modelLightingShader->setMat4f("projection", projection);
        modelLightingShader->setFloat("shininess", 64.0f);

        // 平行光初始化
        modelLightingShader->setVec3f("dirLight.direction",glm::vec3(0.0f, 0.0f, 1.0f));
        modelLightingShader->setVec3f("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        modelLightingShader->setVec3f("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        modelLightingShader->setVec3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        // 点光源初始化
        modelLightingShader->setVec3f("pointLights[0].position", glm::vec3(-1.66041, 1.0, -2.0829));
        modelLightingShader->setVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        modelLightingShader->setVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        modelLightingShader->setVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        modelLightingShader->setFloat("pointLights[0].constant", 1.0f);
        modelLightingShader->setFloat("pointLights[0].linear", 0.7f);
        modelLightingShader->setFloat("pointLights[0].quadratic", 1.8f);

        modelLightingShader->setBool("LightStatus[0]", roomCenterLight >= 1);
        switch (roomCenterLight) {
            case 1:
                modelLightingShader->setVec3f("pointLights[0].position", glm::vec3(-1.66041, 1.0, -2.0829));
                modelLightingShader->setVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
                modelLightingShader->setVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
                modelLightingShader->setVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
                modelLightingShader->setFloat("pointLights[0].constant", 1.0f);
                modelLightingShader->setFloat("pointLights[0].linear", 0.7f);
                modelLightingShader->setFloat("pointLights[0].quadratic", 1.8f);
                break;
            case 2:
                modelLightingShader->setVec3f("pointLights[0].position", glm::vec3(-1.66041, 1.0, -2.0829));
                modelLightingShader->setVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
                modelLightingShader->setVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
                modelLightingShader->setVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
                modelLightingShader->setFloat("pointLights[0].constant", 1.0f);
                modelLightingShader->setFloat("pointLights[0].linear", 0.14f);
                modelLightingShader->setFloat("pointLights[0].quadratic", 0.07f);
                break;
            case 3:
                modelLightingShader->setVec3f("pointLights[0].position", glm::vec3(-1.66041, 1.0, -2.0829));
                modelLightingShader->setVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
                modelLightingShader->setVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
                modelLightingShader->setVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
                modelLightingShader->setFloat("pointLights[0].constant", 1.0f);
                modelLightingShader->setFloat("pointLights[0].linear", 0.09f);
                modelLightingShader->setFloat("pointLights[0].quadratic", 0.032f);
                break;
            default:
                break;
        }

        modelLightingShader->setBool("LightStatus[1]", lampLight);
        modelLightingShader->setVec3f("pointLights[1].position", glm::vec3(0.15, 0.3, -4.0));
        modelLightingShader->setVec3f("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        modelLightingShader->setVec3f("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        modelLightingShader->setVec3f("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        modelLightingShader->setFloat("pointLights[1].constant", 1.0f);
        modelLightingShader->setFloat("pointLights[1].linear", 0.35f);
        modelLightingShader->setFloat("pointLights[1].quadratic",0.44f);

        modelLightingShader->setBool("LightStatus[2]", !tvOff);
        modelLightingShader->setVec3f("pointLights[2].position", glm::vec3(0.875, 0.5, -1.0));
        modelLightingShader->setVec3f("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        modelLightingShader->setVec3f("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        modelLightingShader->setVec3f("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        modelLightingShader->setFloat("pointLights[2].constant", 1.0f);
        modelLightingShader->setFloat("pointLights[2].linear", 0.35f);
        modelLightingShader->setFloat("pointLights[2].quadratic", 0.44f);

        // 手电筒聚光初始化
        modelLightingShader->setVec3f("spotLight.position", camera.Position);
        modelLightingShader->setVec3f("spotLight.direction", camera.Front);

        modelLightingShader->setFloat("spotLight.constant", 1.0f);
        modelLightingShader->setFloat("spotLight.linear", 0.09f);
        modelLightingShader->setFloat("spotLight.quadratic", 0.032f);
        if(flashLight){
            modelLightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
            modelLightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
            modelLightingShader->setVec3f("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            modelLightingShader->setVec3f("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
            modelLightingShader->setVec3f("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        } else {
            modelLightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(0.0f)));
            modelLightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(0.0f)));
            modelLightingShader->setVec3f("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            modelLightingShader->setVec3f("spotLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
            modelLightingShader->setVec3f("spotLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
        }

        // 绘制符华
        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-0.3, 0.0, -4.0));
        model = glm::scale(model, glm::vec3(0.025,0.025,0.025));
        model = glm::rotate(model, glm::radians((float)glfwGetTime() * FUHUA_rotateSpeed),
                            glm::vec3(0.0,1.0,0.0));
        modelLightingShader->setMat4f("model", model);
        model_FuHua->Draw(*modelLightingShader);


        lightingShader->use();
        lightingShader->setVec3f("viewPos", camera.Position);
        lightingShader->setFloat("material.shininess", 64.0f);

        // 平行光初始化
        lightingShader->setVec3f("dirLight.direction",glm::vec3(0.0f, 0.0f, 1.0f));
        lightingShader->setVec3f("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader->setVec3f("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        lightingShader->setVec3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        // 点光源初始化
        lightingShader->setBool("LightStatus[0]", roomCenterLight >= 1);
        switch (roomCenterLight) {
            case 1:
                lightingShader->setVec3f("pointLights[0].position", glm::vec3(-1.66041, 1.0, -2.0829));
                lightingShader->setVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
                lightingShader->setVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
                lightingShader->setVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
                lightingShader->setFloat("pointLights[0].constant", 1.0f);
                lightingShader->setFloat("pointLights[0].linear", 0.7f);
                lightingShader->setFloat("pointLights[0].quadratic", 1.8f);
                break;
            case 2:
                lightingShader->setVec3f("pointLights[0].position", glm::vec3(-1.66041, 1.0, -2.0829));
                lightingShader->setVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
                lightingShader->setVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
                lightingShader->setVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
                lightingShader->setFloat("pointLights[0].constant", 1.0f);
                lightingShader->setFloat("pointLights[0].linear", 0.14f);
                lightingShader->setFloat("pointLights[0].quadratic", 0.07f);
                break;
            case 3:
                lightingShader->setVec3f("pointLights[0].position", glm::vec3(-1.66041, 1.0, -2.0829));
                lightingShader->setVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
                lightingShader->setVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
                lightingShader->setVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
                lightingShader->setFloat("pointLights[0].constant", 1.0f);
                lightingShader->setFloat("pointLights[0].linear", 0.09f);
                lightingShader->setFloat("pointLights[0].quadratic", 0.032f);
                break;
            default:
                break;
        }

        lightingShader->setBool("LightStatus[1]", lampLight);
        lightingShader->setVec3f("pointLights[1].position", glm::vec3(0.15, 0.3, -4.0));
        lightingShader->setVec3f("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader->setVec3f("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightingShader->setVec3f("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader->setFloat("pointLights[1].constant", 1.0f);
        lightingShader->setFloat("pointLights[1].linear", 0.35f);
        lightingShader->setFloat("pointLights[1].quadratic",0.44f);

        lightingShader->setBool("LightStatus[2]", !tvOff);
        lightingShader->setVec3f("pointLights[2].position", glm::vec3(0.875, 0.5, -1.0));
        lightingShader->setVec3f("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader->setVec3f("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightingShader->setVec3f("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader->setFloat("pointLights[2].constant", 1.0f);
        lightingShader->setFloat("pointLights[2].linear", 0.35f);
        lightingShader->setFloat("pointLights[2].quadratic", 0.44f);

        // 手电筒聚光初始化
        lightingShader->setVec3f("spotLight.position", camera.Position);
        lightingShader->setVec3f("spotLight.direction", camera.Front);

        lightingShader->setFloat("spotLight.constant", 1.0f);
        lightingShader->setFloat("spotLight.linear", 0.09f);
        lightingShader->setFloat("spotLight.quadratic", 0.032f);
        if(flashLight){
            lightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
            lightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
            lightingShader->setVec3f("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            lightingShader->setVec3f("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
            lightingShader->setVec3f("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        } else {
            lightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(0.0f)));
            lightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(0.0f)));
            lightingShader->setVec3f("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            lightingShader->setVec3f("spotLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
            lightingShader->setVec3f("spotLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
        }


        lightingShader->setMat4f("view", view);
        lightingShader->setMat4f("projection", projection);

        // 绘制草地
        glBindVertexArray(cubeVAO);
        for(float locx = -10.0; locx <= 10.0; locx += 1.0){
            for(float locz = -10.0; locz <= 10.0; locz += 1.0){
                model = glm::mat4 (1.0f);
                model = glm::translate(model, glm::vec3 (locx, -0.8, locz));
                model = glm::scale(model, glm::vec3(1,1,1));
                if(locz >= -4.5 && locz <= 1.0 && locx >=-4.5 && locx <= 1.2){
                    lightingShader->setInt("material.diffuse", 3);
                    lightingShader->setInt("material.specular", 3);
                }
                else{
                    lightingShader->setInt("material.diffuse", 2);
                    lightingShader->setInt("material.specular", 2);
                }
                lightingShader->setMat4f("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // 绘制墙面
        // __________________________________________________________
        lightingShader->setInt("material.diffuse", 4);
        lightingShader->setInt("material.specular", 4);
        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0.0, 0.5, 1.0));
        model = glm::scale(model, glm::vec3(3,2,0.5));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-3.0, 0.5, 1.0));
        model = glm::scale(model, glm::vec3(3,2,0.5));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-4.5, 0.5, -0.2));
        model = glm::scale(model, glm::vec3(0.5,2,3));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-4.5, 0.5, -3.2));
        model = glm::scale(model, glm::vec3(0.5,2,3));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-3.0, 0.5, -4.5));
        model = glm::scale(model, glm::vec3(3,2,0.5));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0.0, 0.5, -4.5));
        model = glm::scale(model, glm::vec3(3,2,0.5));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (1.2, 0.5, -3.2));
        model = glm::scale(model, glm::vec3(0.5,2,3));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (1.2, 0.5, -0.2));
        model = glm::scale(model, glm::vec3(0.5,2,3));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制天花板
        // __________________________________________________________
        lightingShader->setInt("material.diffuse", 5);
        lightingShader->setInt("material.specular", 5);
        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-1.44, 1.47, -1.47));
        model = glm::scale(model, glm::vec3(5.5,0.5,5.5));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制桌面与桌腿
        // __________________________________________________________
        lightingShader->setInt("material.diffuse", 6);
        lightingShader->setInt("material.specular", 6);
        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0, 0.0, -4));
        model = glm::scale(model, glm::vec3(1,0.03, 0.5));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-0.3, -0.25, -4));
        model = glm::scale(model, glm::vec3(0.05,0.5, 0.05));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0.3, -0.25, -4));
        model = glm::scale(model, glm::vec3(0.05,0.5, 0.05));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制椅子
        // __________________________________________________________
        lightingShader->setInt("material.diffuse", 7);
        lightingShader->setInt("material.specular", 7);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0, -0.1, -3.5));
        model = glm::scale(model, glm::vec3(0.3,0.03, 0.3));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-0.1, -0.2, -3.6));
        model = glm::scale(model, glm::vec3(0.05,0.2, 0.05));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0.1, -0.2, -3.6));
        model = glm::scale(model, glm::vec3(0.05,0.2, 0.05));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-0.1, -0.2, -3.4));
        model = glm::scale(model, glm::vec3(0.05,0.2, 0.05));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0.1, -0.2, -3.4));
        model = glm::scale(model, glm::vec3(0.05,0.2, 0.05));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0.0, 0.01, -3.35));
        model = glm::scale(model, glm::vec3(0.27,0.27, 0.01));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制台灯架子
        // __________________________________________________________
        lightingShader->setInt("material.diffuse", 8);
        lightingShader->setInt("material.specular", 8);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0.15, 0.15, -4.0));
        model = glm::scale(model, glm::vec3(0.05,0.3, 0.05));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (0.15, 0.02, -4.0));
        model = glm::scale(model, glm::vec3(0.1,0.01, 0.1));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindTexture(GL_TEXTURE_2D, 0);

        // 绘制床面与床板
        // __________________________________________________________
        
        // 绘制床板
        lightingShader->setInt("material.diffuse", 6);
        lightingShader->setInt("material.specular", 6);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-3.5, -0.4, -1.0));
        model = glm::scale(model, glm::vec3(1.4,1, 1));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (-4.2, -0.2, -1.0));
        model = glm::scale(model, glm::vec3(0.02,1.3, 1));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制床垫
        lightingShader->setInt("material.diffuse", 9);
        lightingShader->setInt("material.specular", 9);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5, 0.1, -1.0));
        model = glm::scale(model, glm::vec3(1.3, 0.02, 1));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制枕头
        lightingShader->setInt("material.diffuse", 10);
        lightingShader->setInt("material.specular", 10);
        model = glm::mat4(1.0f);
        if(pillowFront)
            model = glm::translate(model, glm::vec3(-4.0, 0.1, -1.0));
        else
            model = glm::translate(model, glm::vec3(-3.0, 0.1, -1.0));
        model = glm::scale(model, glm::vec3(0.5, 0.1, 0.8));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制柜子
        // __________________________________________________________
        lightingShader->setInt("material.diffuse", 6);
        lightingShader->setInt("material.specular", 6);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-4.0, -0.2, 0.0));
        model = glm::scale(model, glm::vec3(0.8, 1, 0.8));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-4.0, 0.3, 0.0));
        model = glm::scale(model, glm::vec3(1, 0.01, 1));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightingShader->setInt("material.diffuse", 8);
        lightingShader->setInt("material.specular", 8);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.6, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制电视
        // __________________________________________________________
        if (tvOff) {
            lightingShader->setInt("material.diffuse", 8);
            lightingShader->setInt("material.specular", 8);
        } 
        else {
            lightingShader->setInt("material.diffuse", 11);
            lightingShader->setInt("material.specular", 11);
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.875, 0.5, -1.0));
        model = glm::scale(model, glm::vec3(0.03, 0.7, 1));
        lightingShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制灯泡
        // __________________________________________________________
        lightShader->use();
        lightShader->setMat4f("view", view);
        lightShader->setMat4f("projection", projection);
        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (glm::vec3(-1.66041, 1.0, -2.0829)));
        model = glm::scale(model, glm::vec3(0.3,0.3,0.3));
        lightShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // 绘制小灯泡
        // __________________________________________________________
        lightShader->use();
        lightShader->setMat4f("view", view);
        lightShader->setMat4f("projection", projection);
        model = glm::mat4 (1.0f);
        model = glm::translate(model, glm::vec3 (glm::vec3(0.15, 0.3, -4.0)));
        model = glm::scale(model, glm::vec3(0.1,0.1,0.1));
        lightShader->setMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);




        // 更换缓存
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 中断glfw窗口
    glfwTerminate();

    delete(basicShader);
    return 0;
}
