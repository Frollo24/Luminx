#include "SandboxLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<Pipeline> s_ModelPipeline = nullptr;
static Ref<VertexArray> s_VertexArray = nullptr;
static Ref<Buffer> s_VertexBuffer = nullptr;
static Ref<Buffer> s_IndexBuffer = nullptr;

static Ref<Model> s_Model = nullptr;

void SandboxLayer::OnAttach()
{
	// Load Shader
	auto& triangleShader = CreateRef<Shader>("assets/shaders/Triangle.glsl");
	auto& modelShader = CreateRef<Shader>("assets/shaders/TestModel.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, triangleShader);
	s_ModelPipeline = RenderDevice::CreatePipeline(pipelineState, modelShader);

	s_VertexArray = CreateRef<VertexArray>(); // TODO: handled by device

	float triangleVertices[] = {
		 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.Type = BufferType::Vertex;
	vertexBufferDesc.Size = sizeof(triangleVertices);
	vertexBufferDesc.Data = triangleVertices;
	s_VertexBuffer = RenderDevice::CreateBuffer(vertexBufferDesc);

	BufferLayout layout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float3, "a_Color" },
	};

	s_VertexArray->AddVertexBuffer(s_VertexBuffer, layout);

	s_Model = CreateRef<Model>("assets/models/bunny.obj");

	glm::mat4 modelViewProj = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -1.25f, 0.0f));
	modelShader->SetMat4("u_ModelViewProj", modelViewProj);
}

void SandboxLayer::OnDetach()
{
	RenderDevice::DestroyPipeline(s_Pipeline);
	RenderDevice::DestroyBuffer(s_VertexBuffer);
}

void SandboxLayer::OnUpdate()
{
#define SHOW_TRIANGLE 0
#if SHOW_TRIANGLE
	s_Pipeline->Bind();
	Renderer::Submit(s_VertexArray);
#endif
	s_ModelPipeline->Bind();
	s_Model->Render();
}
