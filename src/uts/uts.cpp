#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_s.h>

#include <iostream>
#include <cmath>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, float deltaTime);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.6f;
float translateX = 0.0f;
float translateY = 0.0f;
float rotationAngle = 0.0f;
float scaleValue = 1.0f;
bool autoRotate = true;
float moveSpeed = 1.0f;
float rotateSpeed = 2.0f;
float scaleSpeed = 1.0f;

bool isDragging = false;
double lastMouseX = 0.0, lastMouseY = 0.0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "UTS Grafkom - NIM 240533600866 | Interaktif", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "================================================" << std::endl;
    std::cout << "   UTS Grafkom - Interaksi Keyboard & Mouse" << std::endl;
    std::cout << "   NIM: 240533600866" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << std::endl;
    std::cout << " KONTROL KEYBOARD:" << std::endl;
    std::cout << "   Arrow Keys   : Geser objek (translate)" << std::endl;
    std::cout << "   Q / E        : Rotasi kiri / kanan" << std::endl;
    std::cout << "   Z / X        : Perkecil / Perbesar (scale)" << std::endl;
    std::cout << "   1 / 2        : Kurangi / Tambah mix value" << std::endl;
    std::cout << "   SPACE        : Toggle auto-rotate" << std::endl;
    std::cout << "   R            : Reset semua transformasi" << std::endl;
    std::cout << "   ESC          : Keluar" << std::endl;
    std::cout << std::endl;
    std::cout << " KONTROL MOUSE:" << std::endl;
    std::cout << "   Scroll       : Ubah mix value" << std::endl;
    std::cout << "   Klik Kiri + Drag : Geser objek" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << std::endl;

    Shader ourShader("./uts.vs", "./uts.fs");

    float vertices[] = {
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f 
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture1, texture2;
    
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("../../resources/textures/fotoprofil.jfif", &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Texture 1 (Foto Profil) loaded!" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture 1 (Foto Profil)" << std::endl;
    }
    stbi_image_free(data);

   
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("../../resources/textures/nim.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Texture 2 (NIM) loaded!" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture 2 (NIM)" << std::endl;
    }
    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // Hitung delta time
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glm::mat4 transform = glm::mat4(1.0f);

        transform = glm::translate(transform, glm::vec3(translateX, translateY, 0.0f));

        float totalRotation = rotationAngle;
        if (autoRotate)
            totalRotation += currentFrame;
        transform = glm::rotate(transform, totalRotation, glm::vec3(0.0f, 0.0f, 1.0f));

        transform = glm::scale(transform, glm::vec3(scaleValue, scaleValue, scaleValue));

        ourShader.use();
        ourShader.setFloat("mixValue", mixValue);
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

bool spacePressed = false;
bool rPressed = false;

void processInput(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        translateY += moveSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        translateY -= moveSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        translateX -= moveSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        translateX += moveSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        rotationAngle += rotateSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        rotationAngle -= rotateSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        scaleValue = std::max(0.1f, scaleValue - scaleSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        scaleValue = std::min(3.0f, scaleValue + scaleSpeed * deltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        mixValue = std::max(0.0f, mixValue - 0.5f * deltaTime);
        std::cout << "\rMix Value: " << mixValue << "   " << std::flush;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        mixValue = std::min(1.0f, mixValue + 0.5f * deltaTime);
        std::cout << "\rMix Value: " << mixValue << "   " << std::flush;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed)
    {
        autoRotate = !autoRotate;
        spacePressed = true;
        std::cout << "\rAuto-Rotate: " << (autoRotate ? "ON" : "OFF") << "      " << std::flush;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        spacePressed = false;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rPressed)
    {
        mixValue = 0.6f;
        translateX = 0.0f;
        translateY = 0.0f;
        rotationAngle = 0.0f;
        scaleValue = 1.0f;
        autoRotate = true;
        rPressed = true;
        std::cout << "\r[RESET] Semua transformasi direset!      " << std::flush;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
        rPressed = false;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mixValue += (float)yoffset * 0.05f;
    if (mixValue < 0.0f) mixValue = 0.0f;
    if (mixValue > 1.0f) mixValue = 1.0f;
    std::cout << "\rMix Value (scroll): " << mixValue << "   " << std::flush;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            isDragging = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        else if (action == GLFW_RELEASE)
        {
            isDragging = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (isDragging)
    {
        double dx = xpos - lastMouseX;
        double dy = ypos - lastMouseY;

        translateX += (float)(dx / SCR_WIDTH) * 2.0f;
        translateY -= (float)(dy / SCR_HEIGHT) * 2.0f;

        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
