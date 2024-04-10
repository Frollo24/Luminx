#include "TriangleLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<Shader> s_Shader = nullptr;
static Ref<VertexArray> s_VertexArray = nullptr;
static Ref<Buffer> s_VertexBuffer = nullptr;

void TriangleLayer::OnAttach()
{
	// Load Shader
	s_Shader = CreateRef<Shader>("assets/shaders/Triangle.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	pipelineState.BlendState.BlendEnable = false;
	pipelineState.PolygonState.FrontFace = FrontFaceMode::CounterClockwise;
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, s_Shader);

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

	RenderCommand::SetDefaultState(pipelineState);
}

void TriangleLayer::OnDetach()
{
	s_Shader = nullptr;
	s_VertexArray = nullptr;
	RenderDevice::DestroyBuffer(s_VertexBuffer);
}

void TriangleLayer::OnUpdate()
{
	s_Shader->Use();
	RenderCommand::BindVertexArray(s_VertexArray);
	RenderCommand::DrawVertices(3);
}
