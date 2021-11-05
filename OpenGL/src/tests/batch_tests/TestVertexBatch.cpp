#include "TestVertexBatch.h"

#include "Renderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"

#include <array>

namespace test {

    struct Vec2
    {
        float x;
        float y;
    };

    struct Vec4
    {
        float x;
        float y;
        float z;
        float t;
    };

    struct Vertex
    {
        Vec2 Position;
        Vec4 Color;
        Vec2 TexCoord;
        float TexID;
    };

    static std::array<Vertex, 4> CreateQuad(float x, float y, float textureID)
    {
        //float verticies[] = {
        //    // |Vertex Coords|--------Color----------|-TexCoords-|Index|
        //       -50.0f, -50.0f, 0.8f, 0.3f, 0.8f, 1.0f, 0.0f, 0.0f, 0.0f, // 0
        //        50.0f, -50.0f, 0.8f, 0.3f, 0.8f, 1.0f, 1.0f, 0.0f, 0.0f, // 1
        //        50.0f,  50.0f, 0.8f, 0.3f, 0.8f, 1.0f, 1.0f, 1.0f, 0.0f, // 2
        //       -50.0f,  50.0f, 0.8f, 0.3f, 0.8f, 1.0f, 0.0f, 1.0f, 0.0f, // 3

        //       100.0f, -50.0f, 0.18f, 0.6f, 0.96f, 1.0f, 0.0f, 0.0f, 1.0f, // 0
        //       200.0f, -50.0f, 0.18f, 0.6f, 0.96f, 1.0f, 1.0f, 0.0f, 1.0f, // 1
        //       200.0f,  50.0f, 0.18f, 0.6f, 0.96f, 1.0f, 1.0f, 1.0f, 1.0f, // 2
        //       100.0f,  50.0f, 0.18f, 0.6f, 0.96f, 1.0f, 0.0f, 1.0f, 1.0f  // 3
        //};

        float size = 100;

        Vertex v0;
        v0.Position = { x, y };
        v0.Color = { 0.8f, 0.3f, 0.8f, 1.0f, };
        v0.TexCoord = { 0.0f, 0.0f };
        v0.TexID = textureID;

        Vertex v1;
        v1.Position = { x + size, y };
        v1.Color = { 0.8f, 0.3f, 0.8f, 1.0f, };
        v1.TexCoord = { 1.0f, 0.0f };
        v1.TexID = textureID;

        Vertex v2;
        v2.Position = { x + size, y + size };
        v2.Color = { 0.8f, 0.3f, 0.8f, 1.0f, };
        v2.TexCoord = { 1.0f, 1.0f };
        v2.TexID = textureID;

        Vertex v3;
        v3.Position = { x, y + size };
        v3.Color = { 0.8f, 0.3f, 0.8f, 1.0f, };
        v3.TexCoord = { 0.0f, 1.0f };
        v3.TexID = textureID;

        return { v0, v1, v2, v3 };
    }

    TestVertexBatch::TestVertexBatch()
    {
        unsigned int indicies[] = {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4
        };

        GLCall(glEnable(GL_BLEND)); // enable it
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, sizeof(Vertex) * 1000);
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(4);
        layout.Push<float>(2);
        layout.Push<float>(1);

        m_VAO->AddBuffer(*m_VertexBuffer, layout);

        m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 12);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        /*m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);*/

        m_Shader->Bind();

        m_texture1 = std::make_unique<Texture>("res/textures/MyLogo.png");
        m_texture2 = std::make_unique<Texture>("res/textures/morty.png");

        int sampler[2] = { 0, 1 };
        m_Shader->SetUniform1iv("u_Textures", 2, sampler);

        m_texture1->Bind(0);
        m_texture2->Bind(1);
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

        auto quad0 = CreateQuad(m_QuadPosition[0], m_QuadPosition[1], 0.0f);
        auto quad1 = CreateQuad(100.0f, -50.0f, 1.0f);

        Vertex verticies[8];
        memcpy(verticies, quad0.data(), quad0.size() * sizeof(Vertex));
        memcpy(verticies + quad0.size(), quad1.data(), quad1.size() * sizeof(Vertex));

        m_VertexBuffer->Bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticies), verticies));


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
        ImGui::Begin("Controls");
        ImGui::DragFloat2("Quad Position", m_QuadPosition, 0.1f);
        ImGui::End();
	}
}

