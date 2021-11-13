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

    static Vertex* CreateQuad(Vertex* target, float x, float y, float textureID)
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

        float size = 100.0f;

        target->Position = { x, y };
        target->Color = { 0.8f, 0.3f, 0.8f, 1.0f, };
        target->TexCoord = { 0.0f, 0.0f };
        target->TexID = textureID;
        target++;

        target->Position = { x + size, y };
        target->Color = { 0.8f, 0.3f, 0.8f, 1.0f, };
        target->TexCoord = { 1.0f, 0.0f };
        target->TexID = textureID;
        target++;

        target->Position = { x + size, y + size };
        target->Color = { 0.8f, 0.3f, 0.8f, 1.0f, };
        target->TexCoord = { 1.0f, 1.0f };
        target->TexID = textureID;
        target++;

        target->Position = { x, y + size };
        target->Color = { 0.8f, 0.3f, 0.8f, 1.0f, };
        target->TexCoord = { 0.0f, 1.0f };
        target->TexID = textureID;
        target++;

        return target;
    }

    TestVertexBatch::TestVertexBatch()
    {
        const size_t MaxQuadCount = 1000;
        const size_t MaxVertexCount = MaxQuadCount * 4;
        const size_t MaxIndexCount = MaxQuadCount * 6;

        /*unsigned int indicies[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4
        };*/

        uint32_t indicies[MaxIndexCount];
        uint32_t offset = 0;
        for (size_t i = 0; i < MaxIndexCount; i += 6)
        {
            indicies[i + 0] = 0 + offset;
            indicies[i + 1] = 1 + offset;
            indicies[i + 2] = 2 + offset;

            indicies[i + 3] = 2 + offset;
            indicies[i + 4] = 3 + offset;
            indicies[i + 5] = 0 + offset;

            offset += 4;
        }

        GLCall(glEnable(GL_BLEND)); // enable it
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, MaxVertexCount * sizeof(Vertex));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(4);
        layout.Push<float>(2);
        layout.Push<float>(1);

        m_VAO->AddBuffer(*m_VertexBuffer, layout);

        m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, MaxIndexCount);

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

        std::array<Vertex, 1000> verticies;
        Vertex* buffer = verticies.data();
        for (int y = 0; y < 5; y++)
        {
            for (int x = 0; x < 5; x++)
            {
                buffer = CreateQuad(buffer, x*100, y*100, (x + y) % 2);
            }
        }

        buffer = CreateQuad(buffer, m_QuadPosition[0], m_QuadPosition[1], 0.0f);

        m_VertexBuffer->Bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, verticies.size() * sizeof(Vertex), verticies.data()));


        Renderer renderer;

        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 0.0f));
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

