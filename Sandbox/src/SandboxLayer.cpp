#include "SandboxLayer.h"
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<VertexArray> s_VertexArray = nullptr;
static Ref<Buffer> s_VertexBuffer = nullptr;
static Ref<Buffer> s_IndexBuffer = nullptr;

void SandboxLayer::OnAttach()
{
	// Load Shader
	auto& shader = CreateRef<Shader>("assets/shaders/Triangle.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, shader);

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
}

void SandboxLayer::OnDetach()
{
	RenderDevice::DestroyPipeline(s_Pipeline);
	RenderDevice::DestroyBuffer(s_VertexBuffer);
}

void SandboxLayer::OnUpdate()
{
	s_Pipeline->Bind();
	Renderer::Submit(s_VertexArray);
}
