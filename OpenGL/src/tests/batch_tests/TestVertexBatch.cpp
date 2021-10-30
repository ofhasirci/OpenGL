#include "TestVertexBatch.h"

#include "Renderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {

	TestVertexBatch::TestVertexBatch()
	{
        float positions[] = {
            -50.0f, -50.0f, 0.8f, 0.3f, 0.8f, 1.0f, // 0
             50.0f, -50.0f, 0.8f, 0.3f, 0.8f, 1.0f, // 1
             50.0f,  50.0f, 0.8f, 0.3f, 0.8f, 1.0f, // 2
            -50.0f,  50.0f, 0.8f, 0.3f, 0.8f, 1.0f, // 3

            100.0f, -50.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 0
            200.0f, -50.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 1
            200.0f,  50.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 2
            100.0f,  50.0f, 0.18f, 0.6f, 0.96f, 1.0f  // 3
        };

        unsigned int indicies[] = {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4
        };

        GLCall(glEnable(GL_BLEND)); // enable it
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 6 * 8 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(4);

        m_VAO->AddBuffer(*m_VertexBuffer, layout);

        m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 12);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        /*m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);*/
	}

	TestVertexBatch::~TestVertexBatch()
	{
	}

	void TestVertexBatch::OnUpdate(float deltaTime)
	{
	}

	void TestVertexBatch::OnRender()
	{
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;

        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 200.0f, 0.0f));
        glm::mat4 mvp = proj * view * model;
        m_Shader->Bind(); // binding for each shader uniform set and draw not optimised
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}

	void TestVertexBatch::OnImGuiRender()
	{
	}
}

