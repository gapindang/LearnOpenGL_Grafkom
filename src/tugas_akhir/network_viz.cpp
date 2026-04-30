#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <string>

// --- Settings ---
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// --- Camera ---
Camera camera(glm::vec3(0.0f, 3.0f, 8.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool autoOrbit = false;

// --- Timing ---
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --- Modes ---
enum NetworkMode { LAN, MAN, WAN };
NetworkMode currentMode = LAN;
bool modeChanged = true;

// --- Data Structures ---
enum NodeType { PC, SWITCH, ROUTER, BUILDING, EARTH };

struct Node {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 scale;
    NodeType type;
    std::string label;
    unsigned int labelTexture;
    float labelAspect;
};

struct Connection {
    int fromNode;
    int toNode;
    glm::vec3 color;
};

struct Packet {
    std::vector<glm::vec3> path;
    int currentSegment;
    float t;
    float speed;
    bool active;
    glm::vec3 color;
};

// --- Scene Data ---
std::vector<Node> nodes;
std::vector<Connection> connections;
std::vector<Packet> packets;
std::vector<glm::vec3> currentPath; // the path a packet will take when SPACE is pressed

// --- Global GL IDs ---
unsigned int cubeVAO, cubeVBO;
unsigned int sphereVAO, sphereVBO;
unsigned int lineVAO, lineVBO;
int sphereIndexCount;
unsigned int earthTexture;

// --- Callbacks & Input ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// --- Setup Functions ---
void setupGeometry();
void buildSphere(float radius, int sectorCount, int stackCount);
unsigned int loadTexture(const char *path);
unsigned int createTextTexture(const char* text, float* outAspect);

// --- Procedural Geometry Drawing ---
void drawPC(class Shader& shader, glm::mat4 model, glm::vec3 color);
void drawSwitch(class Shader& shader, glm::mat4 model, glm::vec3 color);
void drawRouter(class Shader& shader, glm::mat4 model, glm::vec3 color);

// --- Scene Management ---
void loadLANScene();
void loadMANScene();
void loadWANScene();

int main()
{
    // 1. GLFW Initialization
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Network Visualization - LAN (1), MAN (2), WAN (3) | SPACE to send packet", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 2. GLAD Initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 3. GL State
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 4. Load Shaders
    Shader shader("src/tugas_akhir/network_viz.vs", "src/tugas_akhir/network_viz.fs");

    // 5. Setup Geometry & Textures
    setupGeometry();
    earthTexture = loadTexture(FileSystem::getPath("resources/textures/earth.jpg").c_str());

    // 6. Initial Scene
    loadLANScene();

    // 7. Render Loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Update Packets
        for (auto& packet : packets) {
            if (packet.active) {
                packet.t += packet.speed * deltaTime;
                if (packet.t >= 1.0f) {
                    packet.t = 0.0f;
                    packet.currentSegment++;
                    if (packet.currentSegment >= packet.path.size() - 1) {
                        packet.active = false;
                    }
                }
            }
        }

        // Auto Orbit (WAN)
        if (autoOrbit && currentMode == WAN) {
            float radius = glm::length(camera.Position);
            float camX = sin(currentFrame * 0.2f) * radius; // Reduced from 0.5f
            float camZ = cos(currentFrame * 0.2f) * radius; // Reduced from 0.5f
            camera.Position = glm::vec3(camX, camera.Position.y, camZ);
            camera.Front = glm::normalize(-camera.Position); // look at origin
            camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
            camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
        }

        // Render Background
        if (currentMode == WAN) {
            glClearColor(0.01f, 0.01f, 0.03f, 1.0f); // Space
        } else {
            glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // Dark mode
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // Update Window Title based on Mode
        if (modeChanged) {
            std::string title = "Network Viz - ";
            if (currentMode == LAN) title += "[LAN] ";
            else if (currentMode == MAN) title += "[MAN] ";
            else title += "[WAN] ";
            title += " | 1=LAN, 2=MAN, 3=WAN | SPACE=Send Packet";
            glfwSetWindowTitle(window, title.c_str());
            modeChanged = false;
        }

        // Set global uniforms
        shader.setBool("isText", false);
        shader.setVec3("viewPos", camera.Position);
        glm::vec3 lightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
        shader.setVec3("lightDir", lightDir);
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // Global WAN Rotation (so buildings and earth rotate together)
        glm::mat4 globalWanRotation = glm::mat4(1.0f);
        if (currentMode == WAN) {
            globalWanRotation = glm::rotate(glm::mat4(1.0f), currentFrame * 0.02f, glm::vec3(0.0f, 1.0f, 0.0f)); // Slower speed
        }

        // Draw Connections (Lines)
        shader.setBool("useTexture", false);
        shader.setFloat("objectAlpha", 1.0f);
        shader.setFloat("emissiveStrength", 0.5f);
        
        glBindVertexArray(lineVAO);
        for (const auto& conn : connections) {
            glm::vec3 p1 = nodes[conn.fromNode].position;
            glm::vec3 p2 = nodes[conn.toNode].position;
            // Draw cables on the ground in MAN mode
            if (currentMode == MAN) {
                p1.y = 0.1f;
                p2.y = 0.1f;
            }
            float lineVertices[] = {
                p1.x, p1.y, p1.z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                p2.x, p2.y, p2.z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
            };
            glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lineVertices), lineVertices);
            
            glm::mat4 model = glm::mat4(1.0f);
            if (currentMode == WAN) model = globalWanRotation;
            
            shader.setMat4("model", model);
            shader.setVec3("objectColor", conn.color);
            glDrawArrays(GL_LINES, 0, 2);
        }

        // Draw Nodes
        shader.setFloat("emissiveStrength", 0.0f);
        for (const auto& node : nodes) {
            glm::mat4 model = glm::mat4(1.0f);
            if (currentMode == WAN) model = globalWanRotation;
            model = glm::translate(model, node.position);
            
            // Radially orient buildings in WAN mode
            if (currentMode == WAN && node.type == BUILDING) {
                glm::vec3 N = glm::normalize(node.position);
                glm::vec3 up(0.0f, 1.0f, 0.0f);
                glm::vec3 T = glm::cross(up, N);
                if (glm::length(T) < 0.001f) T = glm::vec3(1.0f, 0.0f, 0.0f);
                else T = glm::normalize(T);
                glm::vec3 B = glm::normalize(glm::cross(N, T));
                
                glm::mat4 rot(1.0f);
                rot[0] = glm::vec4(T, 0.0f);
                rot[1] = glm::vec4(N, 0.0f); // Y axis points outward from earth
                rot[2] = glm::vec4(B, 0.0f);
                
                model = model * rot;
            }

            model = glm::scale(model, node.scale);
            shader.setMat4("model", model);

            if (node.type == EARTH) {
                shader.setBool("useTexture", true);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, earthTexture);
                shader.setInt("texture1", 0);
            } else {
                shader.setBool("useTexture", false);
                shader.setVec3("objectColor", node.color);
            }

            if (node.type == EARTH) {
                glBindVertexArray(sphereVAO);
                glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
            } else if (node.type == BUILDING) {
                glBindVertexArray(cubeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            } else if (node.type == PC) {
                drawPC(shader, model, node.color);
            } else if (node.type == SWITCH) {
                drawSwitch(shader, model, node.color);
            } else if (node.type == ROUTER) {
                drawRouter(shader, model, node.color);
            }

            // Draw Label
            if (node.labelTexture != 0) {
                glm::mat4 textModel = glm::mat4(1.0f);
                if (currentMode == WAN) textModel = globalWanRotation;
                
                glm::vec3 labelPos;
                if (currentMode == WAN && node.type == BUILDING) {
                    glm::vec3 N = glm::normalize(node.position);
                    labelPos = node.position + N * (node.scale.y * 0.5f + 0.4f);
                } else {
                    float yOffset = (node.type == EARTH) ? node.scale.y * 1.1f : node.scale.y * 0.5f + 0.5f;
                    if (node.type == PC || node.type == SWITCH || node.type == ROUTER) yOffset = 1.0f;
                    labelPos = node.position + glm::vec3(0.0f, yOffset, 0.0f);
                }
                textModel = glm::translate(textModel, labelPos);
                
                // Billboard: Face camera using camera's vectors
                textModel[0] = glm::vec4(camera.Right, 0.0f);
                textModel[1] = glm::vec4(camera.Up, 0.0f);
                textModel[2] = glm::vec4(-camera.Front, 0.0f); // -Front because front looks from camera to target
                
                // Fix orientation: OpenGL expects bottom-up texture, but GDI top-down DIB provides top-down.
                // We scale Y by -1 to flip it right-side up. X should be positive to avoid horizontal mirroring.
                textModel = glm::scale(textModel, glm::vec3(0.5f * node.labelAspect, -0.5f, 0.01f)); 

                
                shader.setMat4("model", textModel);
                shader.setBool("isText", true);
                shader.setVec3("objectColor", glm::vec3(1.0f)); // White text
                shader.setFloat("emissiveStrength", 1.0f); // Make text bright
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, node.labelTexture);
                shader.setInt("texture1", 0);
                glBindVertexArray(cubeVAO);
                // Draw only the front face of the cube (vertices 6 to 11)
                glDrawArrays(GL_TRIANGLES, 6, 6);
                shader.setBool("isText", false);
            }
        }

        // Draw Packets
        shader.setBool("useTexture", false);
        shader.setFloat("emissiveStrength", 2.0f); // Make it glow
        glBindVertexArray(sphereVAO);
        for (const auto& packet : packets) {
            if (packet.active) {
                glm::vec3 pos = glm::mix(packet.path[packet.currentSegment], packet.path[packet.currentSegment+1], packet.t);
                glm::mat4 model = glm::mat4(1.0f);
                if (currentMode == WAN) model = globalWanRotation;
                
                // If WAN mode, packets should arc slightly above the surface instead of interpolating straight through the earth
                // This is a simplification: we just spherically interpolate (slerp) or project out if crossing long distances
                if (currentMode == WAN) {
                    glm::vec3 p1 = packet.path[packet.currentSegment];
                    glm::vec3 p2 = packet.path[packet.currentSegment+1];
                    // Simple curved path
                    pos = glm::normalize(glm::mix(p1, p2, packet.t)) * (glm::length(p1) + 0.5f * sin(packet.t * 3.14159f));
                }

                model = glm::translate(model, pos);
                
                // Pulsing size
                float pulse = 1.0f + 0.2f * sin(currentFrame * 15.0f);
                model = glm::scale(model, glm::vec3(0.2f * pulse));
                
                shader.setMat4("model", model);
                shader.setVec3("objectColor", packet.color);
                
                // Inner core
                shader.setFloat("objectAlpha", 1.0f);
                glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
                
                // Outer glow
                model = glm::scale(model, glm::vec3(1.5f));
                shader.setMat4("model", model);
                shader.setFloat("objectAlpha", 0.4f);
                glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &lineVBO);

    glfwTerminate();
    return 0;
}

// --- Setup Geometry ---
void setupGeometry() {
    // 1. Cube Geometry
    float cubeVertices[] = {
        // positions          // normals           // texture coords
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

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 2. Sphere Geometry
    buildSphere(1.0f, 36, 18);

    // 3. Line Geometry (Dynamic)
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void buildSphere(float radius, int sectorCount, int stackCount)
{
    std::vector<float> vertices;
    std::vector<int> indices;

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorStep = 2 * 3.14159265359f / sectorCount;
    float stackStep = 3.14159265359f / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = 3.14159265359f / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            // vertex position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // vertex normal
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            // vertex tex coord
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    sphereIndexCount = indices.size();

    unsigned int VBO, EBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

// --- Texture Loading ---
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

// --- Text Texture Generation using Windows GDI ---
unsigned int createTextTexture(const char* text, float* outAspect) {
    if (text == nullptr || text[0] == '\0') {
        if (outAspect) *outAspect = 1.0f;
        return 0;
    }

    HDC hdc = CreateCompatibleDC(NULL);
    HFONT hFont = CreateFontA(64, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
                             DEFAULT_PITCH | FF_SWISS, "Arial");
    SelectObject(hdc, hFont);
    
    RECT rect = {0, 0, 0, 0};
    DrawTextA(hdc, text, -1, &rect, DT_CALCRECT);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (outAspect) *outAspect = (float)width / (float)height;
    
    width = (width + 3) & ~3; // 4-byte align
    
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    // Negative height means top-down DIB. GDI draws normally.
    // OpenGL will load it upside down, which we fix with the negative Y scale in rendering.
    bmi.bmiHeader.biHeight = -height; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    void* bits;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    SelectObject(hdc, hBitmap);
    
    SetBkColor(hdc, RGB(0, 0, 0));
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, OPAQUE);
    
    RECT drawRect = {0, 0, width, height};
    FillRect(hdc, &drawRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    DrawTextA(hdc, text, -1, &drawRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    DeleteObject(hBitmap);
    DeleteObject(hFont);
    DeleteDC(hdc);
    
    return textureID;
}

// --- Scenes ---
void clearScene() {
    for(auto& node : nodes) {
        if(node.labelTexture != 0) glDeleteTextures(1, &node.labelTexture);
    }
    nodes.clear();
    connections.clear();
    packets.clear();
    currentPath.clear();
}

void loadLANScene() {
    clearScene();
    currentMode = LAN;
    modeChanged = true;
    autoOrbit = false;
    
    // Camera
    camera.Position = glm::vec3(0.0f, 3.0f, 8.0f);
    camera.Yaw = -90.0f;
    camera.Pitch = -15.0f;
    camera.ProcessMouseMovement(0, 0);

    glm::vec3 pcColor(0.2f, 0.8f, 1.0f);
    glm::vec3 swColor(0.2f, 1.0f, 0.2f);
    glm::vec3 rtColor(0.2f, 0.3f, 1.0f);
    glm::vec3 connColor(0.5f, 0.5f, 0.6f);

    // Nodes
    float asp;
    nodes.push_back({glm::vec3(-4.0f, 0.0f,  2.0f), pcColor, glm::vec3(1.0f), PC, "PC A", createTextTexture("PC A", &asp), asp});   // 0
    nodes.push_back({glm::vec3(-4.0f, 0.0f, -2.0f), pcColor, glm::vec3(1.0f), PC, "PC B", createTextTexture("PC B", &asp), asp});   // 1
    nodes.push_back({glm::vec3(-2.0f, 0.0f,  0.0f), swColor, glm::vec3(1.0f), SWITCH, "Switch 1", createTextTexture("Switch 1", &asp), asp}); // 2
    
    nodes.push_back({glm::vec3( 0.0f, 0.0f,  0.0f), rtColor, glm::vec3(1.0f), ROUTER, "Router", createTextTexture("Router", &asp), asp}); // 3
    
    nodes.push_back({glm::vec3( 2.0f, 0.0f,  0.0f), swColor, glm::vec3(1.0f), SWITCH, "Switch 2", createTextTexture("Switch 2", &asp), asp}); // 4
    nodes.push_back({glm::vec3( 4.0f, 0.0f,  2.0f), pcColor, glm::vec3(1.0f), PC, "PC C", createTextTexture("PC C", &asp), asp});   // 5
    nodes.push_back({glm::vec3( 4.0f, 0.0f, -2.0f), pcColor, glm::vec3(1.0f), PC, "PC D", createTextTexture("PC D", &asp), asp});   // 6

    // Connections
    connections.push_back({0, 2, connColor});
    connections.push_back({1, 2, connColor});
    connections.push_back({2, 3, connColor});
    connections.push_back({3, 4, connColor});
    connections.push_back({4, 5, connColor});
    connections.push_back({4, 6, connColor});

    // Default Path: PC A -> Switch 1 -> Router -> Switch 2 -> PC C
    currentPath = {
        nodes[0].position,
        nodes[2].position,
        nodes[3].position,
        nodes[4].position,
        nodes[5].position
    };
}

void loadMANScene() {
    clearScene();
    currentMode = MAN;
    modeChanged = true;
    autoOrbit = false;

    // Camera
    camera.Position = glm::vec3(0.0f, 12.0f, 18.0f);
    camera.Yaw = -90.0f;
    camera.Pitch = -35.0f;
    camera.ProcessMouseMovement(0, 0);

    glm::vec3 connColor(1.0f, 0.8f, 0.2f); // Golden connections

    // Nodes (Buildings)
    float asp;
    nodes.push_back({glm::vec3(-4.0f, 1.5f,  2.0f), glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 3.0f, 1.0f), BUILDING, "Gedung A", createTextTexture("Gedung A", &asp), asp}); // 0
    nodes.push_back({glm::vec3(-1.0f, 2.5f, -2.0f), glm::vec3(0.5f, 0.7f, 0.8f), glm::vec3(1.5f, 5.0f, 1.5f), BUILDING, "Gedung B", createTextTexture("Gedung B", &asp), asp}); // 1
    nodes.push_back({glm::vec3( 2.0f, 2.0f,  1.0f), glm::vec3(0.7f, 0.6f, 0.5f), glm::vec3(1.2f, 4.0f, 1.2f), BUILDING, "Gedung C", createTextTexture("Gedung C", &asp), asp}); // 2
    nodes.push_back({glm::vec3( 5.0f, 1.0f, -1.0f), glm::vec3(0.5f, 0.8f, 0.6f), glm::vec3(1.0f, 2.0f, 1.0f), BUILDING, "Gedung D", createTextTexture("Gedung D", &asp), asp}); // 3
    nodes.push_back({glm::vec3(-1.0f, 1.0f,  4.0f), glm::vec3(0.8f, 0.5f, 0.5f), glm::vec3(1.0f, 2.0f, 1.0f), BUILDING, "Gedung E", createTextTexture("Gedung E", &asp), asp}); // 4
    
    // Ground plane
    nodes.push_back({glm::vec3(0.0f, -0.1f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(15.0f, 0.2f, 10.0f), BUILDING, "Ground", 0, 1.0f}); // 5

    // Connections (at base of buildings)
    connections.push_back({0, 1, connColor});
    connections.push_back({1, 2, connColor});
    connections.push_back({2, 3, connColor});
    connections.push_back({0, 4, connColor});
    connections.push_back({1, 4, connColor});

    // Path: A -> B -> C -> D
    currentPath = {
        glm::vec3(-4.0f, 0.1f,  2.0f),
        glm::vec3(-1.0f, 0.1f, -2.0f),
        glm::vec3( 2.0f, 0.1f,  1.0f),
        glm::vec3( 5.0f, 0.1f, -1.0f)
    };
}

void loadWANScene() {
    clearScene();
    currentMode = WAN;
    modeChanged = true;
    autoOrbit = true;

    // Camera
    camera.Position = glm::vec3(0.0f, 0.0f, 8.0f);
    camera.Yaw = -90.0f;
    camera.Pitch = 0.0f;
    camera.ProcessMouseMovement(0, 0);

    glm::vec3 connColor(0.2f, 0.5f, 1.0f);

    // Earth Sphere
    nodes.push_back({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(3.0f), EARTH, "Earth", 0, 1.0f}); // 0

    // Mini Buildings on Earth
    float r = 3.0f;
    float asp;
    nodes.push_back({glm::vec3(-r*0.8f, r*0.5f, r*0.33f), glm::vec3(1.0f, 0.2f, 0.2f), glm::vec3(0.5f, 1.0f, 0.5f), BUILDING, "Gedung America", createTextTexture("Gedung America", &asp), asp}); // 1
    nodes.push_back({glm::vec3( r*0.7f, r*0.6f,-r*0.38f), glm::vec3(0.2f, 1.0f, 0.2f), glm::vec3(0.5f, 1.0f, 0.5f), BUILDING, "Gedung Europe", createTextTexture("Gedung Europe", &asp), asp}); // 2
    nodes.push_back({glm::vec3( r*0.9f,-r*0.2f, r*0.38f), glm::vec3(0.2f, 0.2f, 1.0f), glm::vec3(0.5f, 1.0f, 0.5f), BUILDING, "Gedung Asia", createTextTexture("Gedung Asia", &asp), asp});   // 3

    connections.push_back({1, 2, connColor});
    connections.push_back({2, 3, connColor});

    // Path
    currentPath = {
        nodes[1].position,
        nodes[2].position,
        nodes[3].position
    };
}


// --- Input Callbacks ---
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_1 && action == GLFW_PRESS && currentMode != LAN)
        loadLANScene();
    if (key == GLFW_KEY_2 && action == GLFW_PRESS && currentMode != MAN)
        loadMANScene();
    if (key == GLFW_KEY_3 && action == GLFW_PRESS && currentMode != WAN)
        loadWANScene();

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        // Send Packet
        if (!currentPath.empty()) {
            Packet p;
            p.path = currentPath;
            p.currentSegment = 0;
            p.t = 0.0f;
            p.speed = 2.0f; // units per second
            p.active = true;
            
            // Color based on mode
            if (currentMode == LAN) p.color = glm::vec3(0.0f, 1.0f, 1.0f);
            else if (currentMode == MAN) p.color = glm::vec3(1.0f, 1.0f, 0.0f);
            else p.color = glm::vec3(1.0f, 0.2f, 1.0f);

            packets.push_back(p);
        }
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // Only rotate camera if mouse right button is pressed (or left)
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; 

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
        autoOrbit = false; // user took control
    } else {
        firstMouse = true;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// --- Procedural Implementations ---
void drawPC(Shader& shader, glm::mat4 model, glm::vec3 color) {
    glBindVertexArray(cubeVAO);
    // Monitor screen
    glm::mat4 m = glm::translate(model, glm::vec3(-0.2f, 0.25f, -0.1f));
    m = glm::scale(m, glm::vec3(0.8f, 0.5f, 0.05f));
    shader.setMat4("model", m);
    shader.setVec3("objectColor", color);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // Monitor screen glowing area
    m = glm::translate(model, glm::vec3(-0.2f, 0.25f, -0.07f));
    m = glm::scale(m, glm::vec3(0.7f, 0.4f, 0.05f));
    shader.setMat4("model", m);
    shader.setVec3("objectColor", glm::vec3(0.8f, 0.9f, 1.0f));
    shader.setFloat("emissiveStrength", 0.5f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    shader.setFloat("emissiveStrength", 0.0f);
    // Monitor stand
    m = glm::translate(model, glm::vec3(-0.2f, 0.0f, -0.1f));
    m = glm::scale(m, glm::vec3(0.1f, 0.3f, 0.1f));
    shader.setMat4("model", m);
    shader.setVec3("objectColor", color * 0.8f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // CPU Case
    m = glm::translate(model, glm::vec3(0.4f, 0.15f, -0.1f));
    m = glm::scale(m, glm::vec3(0.25f, 0.8f, 0.5f));
    shader.setMat4("model", m);
    shader.setVec3("objectColor", color * 0.9f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // Keyboard
    m = glm::translate(model, glm::vec3(-0.2f, -0.2f, 0.3f));
    m = glm::rotate(m, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::scale(m, glm::vec3(0.6f, 0.05f, 0.25f));
    shader.setMat4("model", m);
    shader.setVec3("objectColor", color * 0.7f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawSwitch(Shader& shader, glm::mat4 model, glm::vec3 color) {
    glBindVertexArray(cubeVAO);
    // Main Body
    glm::mat4 m = glm::scale(model, glm::vec3(1.2f, 0.3f, 0.6f));
    shader.setMat4("model", m);
    shader.setVec3("objectColor", color);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    // Ports on the front
    for(int i=0; i<6; i++) {
        m = glm::translate(model, glm::vec3(-0.45f + i*0.18f, 0.0f, 0.31f));
        m = glm::scale(m, glm::vec3(0.12f, 0.1f, 0.02f));
        shader.setMat4("model", m);
        shader.setVec3("objectColor", glm::vec3(0.1f)); // Dark holes
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void drawRouter(Shader& shader, glm::mat4 model, glm::vec3 color) {
    glBindVertexArray(cubeVAO);
    // Main Body
    glm::mat4 m = glm::scale(model, glm::vec3(0.8f, 0.2f, 0.6f));
    shader.setMat4("model", m);
    shader.setVec3("objectColor", color);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    // Antennas (back left, back right)
    for(int i=0; i<2; i++) {
        m = glm::translate(model, glm::vec3(-0.3f + i*0.6f, 0.3f, -0.25f));
        m = glm::rotate(m, glm::radians(15.0f * (i == 0 ? -1.0f : 1.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::rotate(m, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        m = glm::scale(m, glm::vec3(0.05f, 0.6f, 0.05f));
        shader.setMat4("model", m);
        shader.setVec3("objectColor", glm::vec3(0.2f)); // Dark gray antennas
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
    // Router status lights on front
    m = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.31f));
    m = glm::scale(m, glm::vec3(0.4f, 0.05f, 0.02f));
    shader.setMat4("model", m);
    shader.setVec3("objectColor", glm::vec3(0.0f, 1.0f, 0.0f)); // Green lights
    shader.setFloat("emissiveStrength", 0.5f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    shader.setFloat("emissiveStrength", 0.0f);
}
