#include "TextureLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<Shader> s_Shader = nullptr;
static Ref<VertexArray> s_VertexArray = nullptr;
static Ref<Buffer> s_VertexBuffer = nullptr;
static Ref<Buffer> s_IndexBuffer = nullptr;
static Ref<Texture> s_Texture = nullptr;

static bool s_ShowTexCoords = false;
static bool s_EnableColoring = true;

void TextureLayer::OnAttach()
{
	// Load Shader
	s_Shader = CreateRef<Shader>("assets/shaders/Texture.glsl");

	// Load image data
	i32 imageWidth;
	i32 imageHeight;
	i32 imageChannels;

	void* imageData = Utils::LoadImageFromDisk("assets/textures/texture-brick.png", imageWidth, imageHeight, imageChannels);

	// Create texture and set texture data
	TextureDescription textureDesc{};
	textureDesc.ImageExtent = { (u32)imageWidth, (u32)imageHeight, 1 };
	textureDesc.ImageFormat = imageChannels == 4 ? ImageFormat::RGBA8 : ImageFormat::RGB8;

	s_Texture = RenderDevice::CreateTexture(textureDesc);
	s_Texture->SetData(imageData);
	s_Texture->BindTextureUnit(0);
	Utils::FreeImageData(imageData);

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	pipelineState.BlendState.BlendEnable = false;
	pipelineState.PolygonState.FrontFace = FrontFaceMode::CounterClockwise;
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, s_Shader);

	s_VertexArray = CreateRef<VertexArray>(); // TODO: handled by device

	// Create vertex buffer
	float quadVertices[] = {
		-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	};

	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.Type = BufferType::Vertex;
	vertexBufferDesc.Size = sizeof(quadVertices);
	vertexBufferDesc.Data = quadVertices;
	s_VertexBuffer = RenderDevice::CreateBuffer(vertexBufferDesc);

	// Create index buffer
	u16 indices[] = { 0, 3, 1, 1, 3, 2 };
	BufferDescription indexBufferDesc{};
	indexBufferDesc.Type = BufferType::Index;
	indexBufferDesc.Size = sizeof(indices);
	indexBufferDesc.Data = indices;
	s_IndexBuffer = RenderDevice::CreateBuffer(indexBufferDesc);

	BufferLayout layout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float3, "a_Color" },
		{ ShaderDataType::Float2, "a_TexCoord" },
	};

	s_VertexArray->AddVertexBuffer(s_VertexBuffer, layout);
	s_VertexArray->SetIndexBuffer(s_IndexBuffer, IndexType::U16);

	glm::mat4 modelViewProj = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Shader->SetMat4("u_ModelViewProj", modelViewProj);
	s_Shader->SetBool("u_EnableColoring", s_EnableColoring);
	s_Shader->SetBool("u_ShowTexCoords", s_ShowTexCoords);

	RenderCommand::SetDefaultState(pipelineState);
}

void TextureLayer::OnDetach()
{
	s_Shader = nullptr;
	s_VertexArray = nullptr;
	RenderDevice::DestroyTexture(s_Texture);
	RenderDevice::DestroyBuffer(s_VertexBuffer);
	RenderDevice::DestroyBuffer(s_IndexBuffer);
}

void TextureLayer::OnUpdate()
{
	s_Shader->Use();
	RenderCommand::BindVertexArray(s_VertexArray);
	RenderCommand::DrawIndices(6, IndexType::U16);
}

void TextureLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>(LUM_BIND_EVENT_FN(TextureLayer::OnKeyPressedEvent));
}

bool TextureLayer::OnKeyPressedEvent(KeyPressedEvent& e)
{
	if (e.GetKeyCode() == KeyCode::C)
	{
		s_EnableColoring = !s_EnableColoring;
		s_Shader->SetBool("u_EnableColoring", s_EnableColoring);
		return false;
	}

	if (e.GetKeyCode() == KeyCode::T)
	{
		s_ShowTexCoords = !s_ShowTexCoords;
		s_Shader->SetBool("u_ShowTexCoords", s_ShowTexCoords);
		return false;
	}

	return false;
}


