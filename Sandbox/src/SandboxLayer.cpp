#include "SandboxLayer.h"
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

void SandboxLayer::OnAttach()
{
	// Load Shaders
	auto& triangleShader = CreateRef<Shader>("assets/shaders/Triangle.glsl");
	auto& modelShader = CreateRef<Shader>("assets/shaders/TestModel.glsl");
	auto& textureShader = CreateRef<Shader>("assets/shaders/TestTexture.glsl");

	// Load image data
	i32 imageWidth;
	i32 imageHeight;
	i32 imageChannels;

#define PNG 0
#if PNG
	void* imageData = Utils::LoadImageFromDisk("assets/textures/texture-brick.png", imageWidth, imageHeight, imageChannels);
#else
	// void* imageData = Utils::LoadImageFromDisk("assets/textures/texture-wood.jpg", imageWidth, imageHeight, imageChannels);
	void* imageData = Utils::LoadHDRImageFromDisk("assets/textures/clarens_night_02_4k.hdr", imageWidth, imageHeight, imageChannels);
#endif

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, triangleShader);
	s_ModelPipeline = RenderDevice::CreatePipeline(pipelineState, modelShader);
	s_TexturePipeline = RenderDevice::CreatePipeline(pipelineState, textureShader);

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

	// Load model
	s_Model = CreateRef<Model>("assets/models/UVSphere.obj");

#define TRANSLATE 0
#if TRANSLATE
	glm::mat4 modelViewProj = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -1.25f, 0.0f));
#else
	glm::mat4 modelViewProj = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
#endif
	modelShader->SetMat4("u_ModelViewProj", modelViewProj);
	textureShader->SetMat4("u_ModelViewProj", modelViewProj);

	// Create texture and set texture data
	TextureDescription textureDesc{};
	textureDesc.ImageExtent = {(u32)imageWidth, (u32)imageHeight, 1};
	textureDesc.ImageFormat = imageChannels == 4 ? ImageFormat::RGBA8 : ImageFormat::RGB16F;

	s_Texture = RenderDevice::CreateTexture(textureDesc);
	s_Texture->SetData(imageData);
	s_Texture->BindTextureUnit(0);
	Utils::FreeImageData(imageData);
}

void SandboxLayer::OnDetach()
{
	RenderDevice::DestroyTexture(s_Texture);
	RenderDevice::DestroyPipeline(s_Pipeline);
	RenderDevice::DestroyBuffer(s_IndexBuffer);
	RenderDevice::DestroyBuffer(s_VertexBuffer);
}

void SandboxLayer::OnUpdate()
{
#define SHOW_TRIANGLE 0
#if SHOW_TRIANGLE
	s_Pipeline->Bind();
	Renderer::Submit(s_VertexArray);
#endif
#define USE_TEXTURE 1
#if USE_TEXTURE
	s_TexturePipeline->Bind();
#else
	s_ModelPipeline->Bind();
#endif
	s_Model->Render();
}
