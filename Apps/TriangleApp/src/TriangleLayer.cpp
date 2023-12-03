#include "TriangleLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<Pipeline> s_ModelPipeline = nullptr;
static Ref<Pipeline> s_TexturePipeline = nullptr;
static Ref<VertexArray> s_VertexArray = nullptr;
static Ref<Buffer> s_VertexBuffer = nullptr;
static Ref<Buffer> s_IndexBuffer = nullptr;

static Ref<Model> s_Model = nullptr;
static Ref<Texture> s_Texture = nullptr;

void TriangleLayer::OnAttach()
{
	// Load Shader
	auto& triangleShader = CreateRef<Shader>("assets/shaders/Triangle.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, triangleShader);

	s_VertexArray = CreateRef<VertexArray>(); // TODO: handled by device

	// Create buffer
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
}

void TriangleLayer::OnDetach()
{
	s_VertexArray = nullptr;
	RenderDevice::DestroyBuffer(s_VertexBuffer);
}

void TriangleLayer::OnUpdate()
{
	s_Pipeline->Bind();
	Renderer::Submit(s_VertexArray);
}
