#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Game/Shaders.h"
#include "Game/stb_image.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include <map>




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void checkForX(float(&offsets)[3], bool(&flags)[3], float(&heights)[6], float& yoffset, int& score, bool& game_state);
void calculateHeight(float(&offsets)[3], float(&heights)[6], bool(&flags)[3]);
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);
double extractDouble(std::string text);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;





GLuint VBO[6], VAO[6], EBO[6];
unsigned int texture[2];

int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glGenVertexArrays(4, VAO);
    glGenBuffers(4, VBO);
    glGenBuffers(4, EBO);






    Shader ourShader("Game/shaders/3.3.shader.vs", "Game/shaders/3.3.shader.fs"); // you can name your shader files however you like
    Shader pipe1("Game/shaders/pipe1.vs", "Game/shaders/pipe1.fs"); // you can name your shader files however you like
    Shader pipe2("Game/shaders/pipe2.vs", "Game/shaders/pipe2.fs"); // you can name your shader files however you like
    Shader pipe3("Game/shaders/pipe1.vs", "Game/shaders/pipe1.fs"); // you can name your shader files however you like
    Shader pipe4("Game/shaders/pipe2.vs", "Game/shaders/pipe2.fs"); // you can name your shader files however you like






    float pipes[] = {
        // positions          // colors           // texture coords
         1.0f,  2.5f, 0.0f,   0.0f, 1.0f, 0.0f,    // top right
         1.0f,  0.25f, 0.0f,   0.0f, 1.0f, 0.0f,   // bottom right
         0.5f, 0.25f, 0.0f,    0.0f, 1.0f, 0.0f,   // bottom left
         0.5f,  2.5f, 0.0f,   0.0f, 1.0f, 0.0f,   // top left 
    };
    float pipes2[] = {
        // positions          // colors           // texture coords
         1.0f,  -2.5f, 0.0f,   0.0f, 1.0f, 0.0f,    // top right
         1.0f,  -0.25f, 0.0f,   0.0f, 1.0f, 0.0f,   // bottom right
         0.5f, -0.25f, 0.0f,    0.0f, 1.0f, 0.0f,   // bottom left
         0.5f,  -2.5f, 0.0f,   0.0f, 1.0f, 0.0f,   // top left 
    };
    unsigned int pipesindices[] = {
        0, 1, 3,
        1, 2, 3
    };

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Game/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);



    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pipes), pipes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pipesindices), pipesindices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pipes2), pipes2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pipesindices), pipesindices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pipes), pipes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pipesindices), pipesindices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pipes2), pipes2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pipesindices), pipesindices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    float offsets[3];
    float yoffset = .0f;
    float whitespace = .5f;
    float heights[6];
    bool flags[3];
    int score = 0;
    flags[0] = false;
    flags[1] = false;
    offsets[0] = 1.0f;
    offsets[1] = 2.5f;
    bool game_state = true;
    float speed = 0.2;





    float pipemovement = 0.6;
    float fallmovement = 0.2;
    float yspeed = 0.0;

    
    double a = 0.0;

    double deltaTime = 0.0;
    double lastFrame = 0.0;


    while (!glfwWindowShouldClose(window))
    {
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        calculateHeight(offsets, heights, flags);
        checkForX(offsets, flags, heights, yoffset, score, game_state);


        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            yspeed = -0.06;
        }


        if (yoffset <= -10 || game_state == false) {
            break;
        }

        // render
        // ------
        glClearColor(0.0f, 0.6f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        a += deltaTime * speed;


        offsets[0] -= pipemovement * deltaTime;
        offsets[1] -= pipemovement * deltaTime;


        yspeed += fallmovement * deltaTime;

        yoffset -= yspeed;




        std::string text = "Score: " + std::to_string(score);
        const char* str = text.c_str();
        glfwSetWindowTitle(window, str);


        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(0.1, 0.1, 0.1));
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        ourShader.setFloat("yOffset", yoffset);
        ourShader.use();



        glm::mat4 trans2 = glm::mat4(1.0f);
        trans2 = glm::scale(trans2, glm::vec3(1, 1, 1));
        trans2 = glm::translate(trans2, glm::vec3(.0f, heights[0], .0f));
        unsigned int transformLoc2 = glGetUniformLocation(pipe1.ID, "transform");
        glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(trans2));
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        pipe1.setFloat("xOffset", offsets[0]);
        pipe1.use();


        glm::mat4 trans3 = glm::mat4(1.0f);
        trans3 = glm::scale(trans3, glm::vec3(1, 1, 1));
        trans3 = glm::translate(trans3, glm::vec3(.0f, heights[1], .0f));
        unsigned int transformLoc3 = glGetUniformLocation(pipe2.ID, "transform");
        glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(trans3));
        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        pipe2.setFloat("xOffset", offsets[0]);
        pipe2.use();


        glm::mat4 trans5 = glm::mat4(1.0f);
        trans5 = glm::scale(trans5, glm::vec3(1, 1, 1));
        trans5 = glm::translate(trans5, glm::vec3(.0f, heights[2], .0f));
        unsigned int transformLoc5 = glGetUniformLocation(pipe3.ID, "transform");
        glUniformMatrix4fv(transformLoc5, 1, GL_FALSE, glm::value_ptr(trans5));
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        pipe3.setFloat("xOffset", offsets[1]);
        pipe3.use();


        glm::mat4 trans4 = glm::mat4(1.0f);
        trans4 = glm::scale(trans4, glm::vec3(1, 1, 1));
        trans4 = glm::translate(trans4, glm::vec3(.0f, heights[3], .0f));
        unsigned int transformLoc4 = glGetUniformLocation(pipe4.ID, "transform");
        glUniformMatrix4fv(transformLoc4, 1, GL_FALSE, glm::value_ptr(trans4));
        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        pipe4.setFloat("xOffset", offsets[1]);
        pipe4.use();




        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(4, VAO);
    glDeleteBuffers(4, VBO);


    glfwTerminate();
    return 0;
}



void calculateHeight(float(&offsets)[3], float(&heights)[6], bool(&flags)[3])
{
    srand((int)time(0));
    float rndm1 = (rand() % 700 + 1); // 0.6
    int rndm2 = rand() % 2 + 1;

    if (rndm2 == 1) {
        rndm1 = (rndm1 / 1000);
    }
    else {  
        rndm1 = -(rndm1 / 1000);
    }

    if (offsets[0] > 0.9f) { // first
        heights[0] = rndm1;
        heights[1] = rndm1;
    }
    if (offsets[1] > 0.9f) { // second
        heights[2] = rndm1;
        heights[3] = rndm1;
    }
    if (offsets[2] > 0.9f) { // third
        heights[4] = rndm1;
        heights[5] = rndm1;
    }
}

void checkForX(float(&offsets)[3], bool(&flags)[3], float(&heights)[6], float& yoffset, int& score, bool& game_state)
{
    // delete windows32
    if (offsets[0] <= -2.0f) {
        offsets[0] = 1.0f;
        flags[0] = false;
    }
    if (offsets[1] <= -2.0f) {
        offsets[1] = 1.0f;
        flags[1] = false;
    }
    if (offsets[2] <= -1.0f) {
        offsets[2] = -0.5f;
    }
    //std::cout << offsets[0] << std::endl;
    if (offsets[0] <= -0.45f && offsets[0] >= -1.05f) {
        //std::cout << heights[0]+0.25 << "  " << yoffset / 10 << "  " << heights[1] - 0.25 << std::endl;
        if (yoffset / 10 > heights[0] + 0.21 || yoffset / 10 < heights[1] - 0.21) {
            game_state = false;
        }

    }
    if (offsets[0] <= -1.1f && flags[0] == false) {
        score++;
        std::cout << score << std::endl;
        flags[0] = true;
    }
    if (offsets[1] <= -0.45f && offsets[1] >= -1.05f) {
        //std::cout << heights[0]+0.25 << "  " << yoffset / 10 << "  " << heights[1] - 0.25 << std::endl;
        if (yoffset / 10 > heights[2] + 0.21 || yoffset / 10 < heights[3] - 0.21) {
            game_state = false;
        }

    }
    if (offsets[1] <= -1.1f && flags[1] == false) {
        score++;
        std::cout << score << std::endl;
        flags[1] = true;
    }
    //if (offsets[2] <= -0.4f && offsets[0] >= -1.05f) {
    //    if (yoffset / 10 > heights[4] + 0.25 || yoffset / 10 < heights[5] - 0.25) {
    //        std::cout << "hitted";
    //    }
    //}

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

double extractDouble(std::string text)
{
    const std::string digits = "0123456789";
    double x = 0.0;

    unsigned ipos = text.find_first_of(digits);
    if (ipos != std::string::npos) std::stringstream(text.substr(ipos)) >> x;
    else                        std::cout << "No number found\n";

    return x;
}
