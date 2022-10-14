#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader.h"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include <freetype.h>
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void kickInput(GLFWwindow* window, float& dt, int& timer);
void checkForX(float(&offsets)[3], bool(&flags)[3], float(&heights)[6], float& yoffset, int& score, bool& game_state);
void calculateHeight(float(&offsets)[3], float(&heights)[6], bool(&flags)[3]);
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

GLuint VBO[4], VAO[4], EBO[4];

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

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // compile and setup the shader
    // ----------------------------
    Shader shader("Game/shaders/text.vs", "Game/shaders/text.fs");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


  


    // configure VAO/VBO for texture quads
    // -----------------------------------
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    std::cout << glGetError() << std::endl;




    Shader ourShader("Game/shaders/3.3.shader.vs", "Game/shaders/3.3.shader.fs"); // you can name your shader files however you like
    Shader pipe1("Game/shaders/pipe1.vs", "Game/shaders/pipe1.fs"); // you can name your shader files however you like
    Shader pipe2("Game/shaders/pipe2.vs", "Game/shaders/pipe2.fs"); // you can name your shader files however you like



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
    std::cout << glGetError() << std::endl;


    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pipes), pipes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pipesindices), pipesindices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    std::cout << glGetError() << std::endl;

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pipes2), pipes2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pipesindices), pipesindices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    std::cout << glGetError() << std::endl;


    float offsets[3];
    float yoffset = -0.0f;
    float boostFall = 0.0025f;
    float velocityUp = 0.5f;
    float dt = 0;
    int timer = 0;
    float whitespace = .5f;
    float heights[6];
    bool flags[3];
    int score = 0;
    flags[0] = false;

    offsets[0] = 1.0f;
    bool game_state = true;






    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        calculateHeight(offsets, heights, flags);
        checkForX(offsets, flags, heights, yoffset, score, game_state);
        kickInput(window, dt, timer);
        if (yoffset <= -10 || game_state == false) {
            //break;
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        dt++;
        timer++;
        yoffset -= boostFall * dt;

        //offsets[0] -= 0.001f;
        offsets[0] -= 0.005f;





        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(0.1, 0.1, 0.1));
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        ourShader.setFloat("yOffset", yoffset);
        ourShader.use();
        //std::cout << glGetError() << std::endl;




        glm::mat4 trans2 = glm::mat4(1.0f);
        trans2 = glm::scale(trans2, glm::vec3(1, 1, 1));
        trans2 = glm::translate(trans2, glm::vec3(.0f, heights[0], .0f));
        unsigned int transformLoc2 = glGetUniformLocation(pipe1.ID, "transform");
        glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(trans2));
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        pipe1.setFloat("xOffset", offsets[0]);
        pipe1.use();
        //std::cout << glGetError() << std::endl;


        glm::mat4 trans3 = glm::mat4(1.0f);
        trans3 = glm::scale(trans3, glm::vec3(1, 1, 1));
        trans3 = glm::translate(trans3, glm::vec3(.0f, heights[1], .0f));
        unsigned int transformLoc3 = glGetUniformLocation(pipe2.ID, "transform");
        glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(trans3));
        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        pipe2.setFloat("xOffset", offsets[0]);
        pipe2.use();
        //std::cout << glGetError() << std::endl;

        std::string text = "Score: " + std::to_string(score);

        //RenderText(shader, text, 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        //RenderText(shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        //std::cout << glGetError() << std::endl;




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(4, VAO);
    glDeleteBuffers(4, VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void calculateHeight(float(&offsets)[3], float(&heights)[6], bool(&flags)[3])
{
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
    if (offsets[0] <= -2.0f) {
        offsets[0] = 1.0f;
        flags[0] = false;
    }
    if (offsets[1] <= -1.0f) {
        offsets[1] = -0.5f;
    }
    if (offsets[2] <= -1.0f) {
        offsets[2] = -0.5f;
    }
    //std::cout << offsets[0] << std::endl;
    if (offsets[0] <= -0.4f && offsets[0] >= -1.05f) {
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
    //if (offsets[1] <= -0.4f && offsets[0] >= -1.05f) {
    //    if (yoffset / 10 > heights[2] + 0.25 || yoffset / 10 < heights[3] - 0.25) {
    //        std::cout << "hitted";
    //    }
    //}
    //if (offsets[2] <= -0.4f && offsets[0] >= -1.05f) {
    //    if (yoffset / 10 > heights[4] + 0.25 || yoffset / 10 < heights[5] - 0.25) {
    //        std::cout << "hitted";
    //    }
    //}

}

void kickInput(GLFWwindow* window, float& dt, int& timer)
{
    if (timer >= 30) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            dt = -30;
            timer = 0;
        }
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

void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO[3]);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
