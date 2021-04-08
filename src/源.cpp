#define GLEW_STATIC
#include <gl/glew.h>
#include <GL/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <time.h>
#include"Renderer.h"

#include"VertexBuffer.h"
#include"VertexBufferLayout.h"
#include"IndexBuffer.h"
#include"VertexArray.h"
#include"Shader.h"
#include"Texture.h"

#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"

#include"Camera.h"
#include"Position.h"

using namespace std;

bool Equalf(float x, float y);
glm::mat4 RotateArbitraryLine(glm::mat4 PerMat, glm::vec3 v1, glm::vec3 v2, float theta);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void kick_callback(GLFWwindow* window, int x, int y, int z);
void LeftRotate();
void RightRotate();
int FindNum(int value);
void Initial();
void RandomDisorder();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(2.0f, 2.0f, 2.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int ifPressed = 0;
bool ifActionQ = false;
bool ifActionE = false;
bool ifActionRandom = false;
int RandomTime = 0;
float RandomOnceTime = 0;

int main(void)
{
    srand((unsigned)time(NULL));
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, kick_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        cout << "Error!" << endl;
    cout << glGetString(GL_VERSION) << endl;

    unsigned int indices[36];
    for (int i = 0; i < 36; i++)    indices[i] = i;

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    VertexArray va;
    VertexBuffer vb(positions, 36 * 10 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    layout.Push<float>(4);
    layout.Push<float>(1);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 36);

    glEnable(GL_DEPTH_TEST);

    Shader shader("res/shaders/Basic.shader");
    shader.Bind();

    Texture texture[6] =
    {
        Texture("res/textures/green.bmp"),
        Texture("res/textures/blue.bmp"),
        Texture("res/textures/orange.bmp"),
        Texture("res/textures/red.bmp"),
        Texture("res/textures/white.bmp"),
        Texture("res/textures/yellow.bmp")
    };
    
    for (int i = 0; i < 6; ++i)
    {
        stringstream ss;
        string index;
        ss << i;
        index = ss.str();
        texture[i].Bind(i);
        shader.SetUniform1i(("u_Texture["+index+"]").c_str(), i);
    }
    
    Initial();

    Renderer renderer;
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----       
        processInput(window);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        renderer.Clear();
        glClear( GL_DEPTH_BUFFER_BIT);

        {
            glm::mat4 view;
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            if (ifActionQ)
                LeftRotate();
            if (ifActionE)
                RightRotate();
            if (ifActionRandom)
            {
                RandomOnceTime += deltaTime;
                if (RandomOnceTime >= 0.4f)
                {
                    RandomDisorder();
                    RandomOnceTime = 0;
                    RandomTime--;
                }
                if (RandomTime <= 0)
                {
                    RandomTime = 0;
                    RandomOnceTime = 0;
                    ifActionRandom = false;
                }
            }
            for (int i = 0; i < 27; i++)
            {
                stringstream ss;
                string index;
                ss << i;
                index = ss.str();
                if (ifPressed == cubeLayer[cubeNumber[i]][0] || ifPressed == cubeLayer[cubeNumber[i]][1] || ifPressed == cubeLayer[cubeNumber[i]][2])
                {
                    shader.SetUniform1f(("HighLight[" + index + "]").c_str(), 1.0f);
                }
                else
                {
                    shader.SetUniform1f(("HighLight[" + index + "]").c_str(), 0.0f);
                }
            }

            shader.Bind();
            for (int i = 0; i < 27; i++)
            {
                stringstream ss;
                string index;
                ss << i;
                index = ss.str();
                glm::mat4 mvp = proj * view * model[i] * cubeRotations[i];
                
                shader.SetUniformMat4f(("u_MVP["+index+"]").c_str(), mvp);
            }
            
            renderer.Draw(va, ib, shader);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
void processInput(GLFWwindow* window)
{
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -=  cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        Initial();
    if (!ifActionE && !ifActionQ && RandomTime == 0)
    {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            ifPressed = 1;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            ifPressed = 2;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            ifPressed = 3;
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            ifPressed = 4;
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
            ifPressed = 5;
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            ifPressed = 6;
        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            ifPressed = 7;
        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
            ifPressed = 8;
        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
            ifPressed = 9;
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
            ifPressed = 0;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && ifPressed != 0)
            ifActionQ = true;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && ifPressed != 0)
            ifActionE = true;
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
            ifActionRandom = true, RandomTime = 25;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
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

        float sensitivity = 0.2f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float cameraSpeed = 40 * deltaTime;
    if (yoffset >= 0)
        cameraPos += cameraSpeed * cameraFront;
    else
        cameraPos -= cameraSpeed * cameraFront;

}

void kick_callback(GLFWwindow* window, int x, int y, int z)
{
    if (x == GLFW_MOUSE_BUTTON_LEFT && y == GLFW_PRESS)
        cout << "wuhu" << endl;
    if (x == GLFW_MOUSE_BUTTON_RIGHT && y == GLFW_PRESS)
    {
        glfwSetCursorPosCallback(window, mouse_callback);
    }
    if (x == GLFW_MOUSE_BUTTON_RIGHT && y == GLFW_RELEASE)
    {
        glfwSetCursorPosCallback(window, NULL);
        firstMouse = true;
    }
}
//绕任意轴的旋转
glm::mat4 RotateArbitraryLine(glm::mat4 PerMat, glm::vec3 v1, glm::vec3 v2, float theta)
{

    glm::mat4 rmatrix;
    float a = v1.x;
    float b = v1.y;
    float c = v1.z;

    glm::vec3 p1 = v2 - v1;
    glm::vec3 p = glm::normalize(p1);

    float u = p.x;
    float v = p.y;
    float w = p.z;

    float uu = u * u;
    float uv = u * v;
    float uw = u * w;
    float vv = v * v;
    float vw = v * w;
    float ww = w * w;
    float au = a * u;
    float av = a * v;
    float aw = a * w;
    float bu = b * u;
    float bv = b * v;
    float bw = b * w;
    float cu = c * u;
    float cv = c * v;
    float cw = c * w;

    float costheta = glm::cos(theta);
    float sintheta = glm::sin(theta);

    rmatrix[0][0] = uu + (vv + ww) * costheta;
    rmatrix[0][1] = uv * (1 - costheta) + w * sintheta;
    rmatrix[0][2] = uw * (1 - costheta) - v * sintheta;
    rmatrix[0][3] = 0;

    rmatrix[1][0] = uv * (1 - costheta) - w * sintheta;
    rmatrix[1][1] = vv + (uu + ww) * costheta;
    rmatrix[1][2] = vw * (1 - costheta) + u * sintheta;
    rmatrix[1][3] = 0;

    rmatrix[2][0] = uw * (1 - costheta) + v * sintheta;
    rmatrix[2][1] = vw * (1 - costheta) - u * sintheta;
    rmatrix[2][2] = ww + (uu + vv) * costheta;
    rmatrix[2][3] = 0;

    rmatrix[3][0] = (a * (vv + ww) - u * (bv + cw)) * (1 - costheta) + (bw - cv) * sintheta;
    rmatrix[3][1] = (b * (uu + ww) - v * (au + cw)) * (1 - costheta) + (cu - aw) * sintheta;
    rmatrix[3][2] = (c * (uu + vv) - w * (au + bv)) * (1 - costheta) + (av - bu) * sintheta;
    rmatrix[3][3] = 1;

    return rmatrix * PerMat;
}

bool Equalf(float x, float y)
{
    return fabs(x - y) < 0.001f;
}

void LeftRotate()
{
    if (Radians <= glm::radians(90.0f))
    {
        for (int i = 0; i < 27; i++)
        {
            if (ifPressed == cubeLayer[cubeNumber[i]][0] || ifPressed == cubeLayer[cubeNumber[i]][1] || ifPressed == cubeLayer[cubeNumber[i]][2])
            {
                if (ifPressed == 1 || ifPressed == 2 || ifPressed == 3)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(1.0f, VectorDif[i].y, VectorDif[i].z), glm::vec3(-1.0f, VectorDif[i].y, VectorDif[i].z), deltaTime*5);
                else if(ifPressed == 4 || ifPressed == 5 || ifPressed == 6)                                                                                                              
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(VectorDif[i].x, 1.0f, VectorDif[i].z), glm::vec3(VectorDif[i].x, -1.0f, VectorDif[i].z), deltaTime*5);
                else if (ifPressed == 7 || ifPressed == 8 || ifPressed == 9)                                                                                                             
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(VectorDif[i].x, VectorDif[i].y, 1.0f), glm::vec3(VectorDif[i].x, VectorDif[i].y, -1.0f), deltaTime*5);
            }
                
        }
        Radians += deltaTime * 5;
    }
    else
    {
        for (int i = 0; i < 27; i++)
        {
            if (ifPressed == cubeLayer[cubeNumber[i]][0] || ifPressed == cubeLayer[cubeNumber[i]][1] || ifPressed == cubeLayer[cubeNumber[i]][2])
            {
                if (ifPressed == 1 || ifPressed == 2 || ifPressed == 3)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(1.0f, VectorDif[i].y, VectorDif[i].z), glm::vec3(-1.0f, VectorDif[i].y, VectorDif[i].z), glm::radians(90.0f) - Radians);
                else if (ifPressed == 4 || ifPressed == 5 || ifPressed == 6)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(VectorDif[i].x, 1.0f, VectorDif[i].z), glm::vec3(VectorDif[i].x, -1.0f, VectorDif[i].z), glm::radians(90.0f) - Radians);
                else if (ifPressed == 7 || ifPressed == 8 || ifPressed == 9)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(VectorDif[i].x, VectorDif[i].y, 1.0f), glm::vec3(VectorDif[i].x, VectorDif[i].y, -1.0f), glm::radians(90.0f) - Radians);
            }
        }
        Radians = 0;
        ifActionQ = false;
        int temp[8];
        int Index[8];
        if (ifPressed == 1 || ifPressed == 2 || ifPressed == 3)
        {   
            Index[0] = FindNum(0 + (ifPressed - 1));
            Index[1] = FindNum(3 + (ifPressed - 1));
            Index[2] = FindNum(6 + (ifPressed - 1));
            Index[3] = FindNum(15 + (ifPressed - 1));
            Index[4] = FindNum(24 + (ifPressed - 1));
            Index[5] = FindNum(21 + (ifPressed - 1));
            Index[6] = FindNum(18 + (ifPressed - 1));
            Index[7] = FindNum(9 + (ifPressed - 1));

            cubeNumber[Index[0]] = 6 + (ifPressed - 1);
            cubeNumber[Index[1]] = 15 + (ifPressed - 1);
            cubeNumber[Index[2]] = 24 + (ifPressed - 1);
            cubeNumber[Index[3]] = 21 + (ifPressed - 1);
            cubeNumber[Index[4]] = 18 + (ifPressed - 1);
            cubeNumber[Index[5]] = 9 + (ifPressed - 1);
            cubeNumber[Index[6]] = 0 + (ifPressed - 1);
            cubeNumber[Index[7]] = 3 + (ifPressed - 1);
        }
       
        if (ifPressed == 4 || ifPressed == 5 || ifPressed == 6)
        {
            Index[0] = FindNum(0 + 3 * (ifPressed - 4));
            Index[1] = FindNum(9 + 3 * (ifPressed - 4));
            Index[2] = FindNum(18 + 3 * (ifPressed - 4));
            Index[3] = FindNum(19 + 3 * (ifPressed - 4));
            Index[4] = FindNum(20 + 3 * (ifPressed - 4));
            Index[5] = FindNum(11 + 3 * (ifPressed - 4));
            Index[6] = FindNum(2 + 3 * (ifPressed - 4));
            Index[7] = FindNum(1 + 3 * (ifPressed - 4));
            cubeNumber[Index[0]] = 18 + 3 * (ifPressed - 4);
            cubeNumber[Index[1]] = 19 + 3 * (ifPressed - 4);
            cubeNumber[Index[2]] = 20 + 3 * (ifPressed - 4);
            cubeNumber[Index[3]] = 11 + 3 * (ifPressed - 4);
            cubeNumber[Index[4]] = 2 + 3 * (ifPressed - 4);
            cubeNumber[Index[5]] = 1 + 3 * (ifPressed - 4);
            cubeNumber[Index[6]] = 0 + 3 * (ifPressed - 4);
            cubeNumber[Index[7]] = 9 + 3 * (ifPressed - 4);
        }
        if (ifPressed == 7 || ifPressed == 8 || ifPressed == 9)
        {
            Index[0] = FindNum(0 + 9*(ifPressed - 7));
            Index[1] = FindNum(3 + 9*(ifPressed - 7));
            Index[2] = FindNum(6 + 9*(ifPressed - 7));
            Index[3] = FindNum(7 + 9*(ifPressed - 7));
            Index[4] = FindNum(8 + 9*(ifPressed - 7));
            Index[5] = FindNum(5 + 9*(ifPressed - 7));
            Index[6] = FindNum(2 + 9*(ifPressed - 7));
            Index[7] = FindNum(1 + 9*(ifPressed - 7));
            cubeNumber[Index[0]] = 6 + 9 * (ifPressed - 7);
            cubeNumber[Index[1]] = 7 + 9 * (ifPressed - 7);
            cubeNumber[Index[2]] = 8 + 9 * (ifPressed - 7);
            cubeNumber[Index[3]] = 5 + 9 * (ifPressed - 7);
            cubeNumber[Index[4]] = 2 + 9 * (ifPressed - 7);
            cubeNumber[Index[5]] = 1 + 9 * (ifPressed - 7);
            cubeNumber[Index[6]] = 0 + 9 * (ifPressed - 7);
            cubeNumber[Index[7]] = 3 + 9 * (ifPressed - 7);
        }
    }
}
void RightRotate()
{
    if (Radians <= glm::radians(90.0f))
    {
        for (int i = 0; i < 27; i++)
        {
            if (ifPressed == cubeLayer[cubeNumber[i]][0] || ifPressed == cubeLayer[cubeNumber[i]][1] || ifPressed == cubeLayer[cubeNumber[i]][2])
            {
                if (ifPressed == 1 || ifPressed == 2 || ifPressed == 3)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(1.0f, VectorDif[i].y, VectorDif[i].z), glm::vec3(-1.0f, VectorDif[i].y, VectorDif[i].z), -deltaTime * 5);
                else if (ifPressed == 4 || ifPressed == 5 || ifPressed == 6)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(VectorDif[i].x, 1.0f, VectorDif[i].z), glm::vec3(VectorDif[i].x, -1.0f, VectorDif[i].z), -deltaTime * 5);
                else if (ifPressed == 7 || ifPressed == 8 || ifPressed == 9)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(VectorDif[i].x, VectorDif[i].y, 1.0f), glm::vec3(VectorDif[i].x, VectorDif[i].y, -1.0f), -deltaTime * 5);
            }

        }
        Radians += deltaTime * 5;
    }
    else
    {
        for (int i = 0; i < 27; i++)
        {
            if (ifPressed == cubeLayer[cubeNumber[i]][0] || ifPressed == cubeLayer[cubeNumber[i]][1] || ifPressed == cubeLayer[cubeNumber[i]][2])
            {
                if (ifPressed == 1 || ifPressed == 2 || ifPressed == 3)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(1.0f, VectorDif[i].y, VectorDif[i].z), glm::vec3(-1.0f, VectorDif[i].y, VectorDif[i].z), -glm::radians(90.0f) + Radians);
                else if (ifPressed == 4 || ifPressed == 5 || ifPressed == 6)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(VectorDif[i].x, 1.0f, VectorDif[i].z), glm::vec3(VectorDif[i].x, -1.0f, VectorDif[i].z), -glm::radians(90.0f) + Radians);
                else if (ifPressed == 7 || ifPressed == 8 || ifPressed == 9)
                    cubeRotations[i] = RotateArbitraryLine(cubeRotations[i], glm::vec3(VectorDif[i].x, VectorDif[i].y, 1.0f), glm::vec3(VectorDif[i].x, VectorDif[i].y, -1.0f), -glm::radians(90.0f) + Radians);
            }
        }
        Radians = 0;
        ifActionE = false;
        int temp[8];
        int Index[8];
        if (ifPressed == 1 || ifPressed == 2 || ifPressed == 3)
        {
            Index[0] = FindNum(0 + (ifPressed - 1));
            Index[1] = FindNum(9 + (ifPressed - 1));
            Index[2] = FindNum(18 + (ifPressed - 1));
            Index[3] = FindNum(21 + (ifPressed - 1));
            Index[4] = FindNum(24 + (ifPressed - 1));
            Index[5] = FindNum(15 + (ifPressed - 1));
            Index[6] = FindNum(6 + (ifPressed - 1));
            Index[7] = FindNum(3 + (ifPressed - 1));

            cubeNumber[Index[0]] = 18 + (ifPressed - 1);
            cubeNumber[Index[1]] = 21 + (ifPressed - 1);
            cubeNumber[Index[2]] = 24 + (ifPressed - 1);
            cubeNumber[Index[3]] = 15 + (ifPressed - 1);
            cubeNumber[Index[4]] = 6 + (ifPressed - 1);
            cubeNumber[Index[5]] = 3 + (ifPressed - 1);
            cubeNumber[Index[6]] = 0 + (ifPressed - 1);
            cubeNumber[Index[7]] = 9 + (ifPressed - 1);
        }

        if (ifPressed == 4 || ifPressed == 5 || ifPressed == 6)
        {
            Index[0] = FindNum(0 + 3 * (ifPressed - 4));
            Index[1] = FindNum(1 + 3 * (ifPressed - 4));
            Index[2] = FindNum(2 + 3 * (ifPressed - 4));
            Index[3] = FindNum(11 + 3 * (ifPressed - 4));
            Index[4] = FindNum(20 + 3 * (ifPressed - 4));
            Index[5] = FindNum(19 + 3 * (ifPressed - 4));
            Index[6] = FindNum(18 + 3 * (ifPressed - 4));
            Index[7] = FindNum(9 + 3 * (ifPressed - 4));
            cubeNumber[Index[0]] = 2 + 3 * (ifPressed - 4);
            cubeNumber[Index[1]] = 11 + 3 * (ifPressed - 4);
            cubeNumber[Index[2]] = 20 + 3 * (ifPressed - 4);
            cubeNumber[Index[3]] = 19 + 3 * (ifPressed - 4);
            cubeNumber[Index[4]] = 18 + 3 * (ifPressed - 4);
            cubeNumber[Index[5]] = 9 + 3 * (ifPressed - 4);
            cubeNumber[Index[6]] = 0 + 3 * (ifPressed - 4);
            cubeNumber[Index[7]] = 1 + 3 * (ifPressed - 4);

        }
        if (ifPressed == 7 || ifPressed == 8 || ifPressed == 9)
        {
            Index[0] = FindNum(0 + 9*(ifPressed - 7));
            Index[1] = FindNum(1 + 9*(ifPressed - 7));
            Index[2] = FindNum(2 + 9*(ifPressed - 7));
            Index[3] = FindNum(3 + 9*(ifPressed - 7));
            Index[4] = FindNum(5 + 9*(ifPressed - 7));
            Index[5] = FindNum(6 + 9*(ifPressed - 7));
            Index[6] = FindNum(7 + 9*(ifPressed - 7));
            Index[7] = FindNum(8 + 9*(ifPressed - 7));
            cubeNumber[Index[0]] = 2 + 9 * (ifPressed - 7);
            cubeNumber[Index[1]] = 5 + 9 * (ifPressed - 7);
            cubeNumber[Index[2]] = 8 + 9 * (ifPressed - 7);
            cubeNumber[Index[3]] = 1 + 9 * (ifPressed - 7);
            cubeNumber[Index[4]] = 7 + 9 * (ifPressed - 7);
            cubeNumber[Index[5]] = 0 + 9 * (ifPressed - 7);
            cubeNumber[Index[6]] = 3 + 9 * (ifPressed - 7);
            cubeNumber[Index[7]] = 6 + 9 * (ifPressed - 7);

        }
    }
}
int FindNum(int value)
{
    for (int i = 0; i < 27; i++)
    {
        if (cubeNumber[i] == value)
            return i;
    }
}
void Initial()
{
    ifPressed = 0;
    ifActionE = false;
    ifActionQ = false;
    ifActionRandom = false;
    RandomTime = 0;
    RandomOnceTime = 0;
    Radians = 0;
    for (int i = 0; i < 27; ++i)
    {
        model[i] = glm::mat4(1.0f);
    }
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
            {
                cubePositions[i + j * 3 + k * 9] = glm::vec3(i-1, j-1, -k-7);
                cubeLayer[i + j * 3 + k * 9][0] = i + 1;
                cubeLayer[i + j * 3 + k * 9][1] = j + 4;
                cubeLayer[i + j * 3 + k * 9][2] = k + 7;
                cubeNumber[i + j * 3 + k * 9] = i + j * 3 + k * 9;
            }
    for (int i = 0; i < 27; i++)
    {
        model[i] = glm::translate(model[i], cubePositions[i]);
    }
    for (int i = 0; i < 27; i++)
    {
        cubeRotations[i] = glm::mat4(1.0f);
    }
    for (int i = 0; i < 27; i++)
    {
        cubePreRotations[i] = glm::mat4(1.0f);
    } 
    for (int i = 0; i < 27; i++)
    {
        VectorDif[i] = cubePositions[13] - cubePositions[i];
    }
}

void RandomDisorder()
{
    int random = (rand() % 9) + 1;
    int direction = (rand() % 2);
    ifPressed = random;
    if (direction == 0)
        ifActionQ = true;
    else
        ifActionE = true;
}
