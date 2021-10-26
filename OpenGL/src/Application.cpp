#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

int main(void)
{
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

    /* set the number of screen updates to wait from the time */
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Glew not init..." << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    
    {
        float positions[] = {
            -50.0f, -50.0f, 0.0f, 0.0f, // 0
             50.0f, -50.0f, 1.0f, 0.0f, // 1
             50.0f,  50.0f, 1.0f, 1.0f, // 2
            -50.0f,  50.0f, 0.0f, 1.0f  // 3
        };

        unsigned int indicies[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Blending
        // Blend 2 overlaping pixels' color
        GLCall(glEnable(GL_BLEND)); // enable it
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        // (SourceValue, DestinationValue) -> default values (1, 0)
        // In our case GL_SRC_ALPHA -> 0, GL_ONE_MINUS_SRC_ALPHA -> (1 - 0)
        // glBlendEquation(mode) -> how to combine src and dest colors
        // Default mode value is GL_FUNC_ADD
        // So the result in our case:
        // R = (rsrc * 0) + (rdest (1 - 0)) 
        // G = (gsrc * 0) + (gdest (1 - 0)) 
        // B = (gsrc * 0) + (bdest (1 - 0)) 
        // A = (asrc * 0) + (adest (1 - 0)) 

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indicies, 6);

        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        Texture texture("res/textures/MyLogo.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        // unbind everything
        vb.UnBind();
        ib.UnBind();
        va.Unbind();
        shader.Unbind();

        Renderer renderer;

        // setup ImGui
        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(400, 200, 0);

        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplGlfwGL3_NewFrame();

            // Using Uniforms
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = proj * view * model;
                shader.Bind(); // binding for each shader uniform set and draw not optimised
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }
            
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = proj * view * model;
                shader.Bind(); // binding for each shader uniform set and draw not optimised
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }

            /*  1) Using uniforms(more specifically our previously defined u_MVP) :
                Using this approach, we call our draw function several times.
                Advantage : We don't have to create a new vertex buffer object/ add data to our vertex buffer object.
                Disadvantage : This can be slow, as uniforms transfer data from the CPU to the GPU, and so we don't want to use this solution for a lot of objects.
                With this approach, you use setuniform4f(or whatever you called your function) to pass a u_MVP matrix(which has been pre - calculated on our side, i.e.the CPU side).For each object, you translate the model matrix a bit differently to move the model into a different area.

                2) Batching.
                Only need to call draw function once.
                Advantage: If all the data is sent at once, there'll only be one transfer and so it won't slow down as it goes through the objects.
                Disadvantage : Requires us sending and storing more data to the GPU, taking up more VRAM.
                With this approach, when making the data that will be copied into your VBO, make sure you plot all vertex positions, and draw as you would normally.
                Way faster for tile, text, 2d rendering */


            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            // 1. Show a simple window.
            // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
            {
                ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);
                ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    
    return 0;
}