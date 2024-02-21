#include "SandboxLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_ModelPipeline = nullptr;
static Ref<Pipeline> s_TexturePipeline = nullptr;
static Ref<Pipeline> s_AlphaPipeline = nullptr;

static Ref<Model> s_Model = nullptr;
static Ref<Texture> s_Texture = nullptr;
static Ref<Texture> s_MultisampledTexture = nullptr;

static Ref<Buffer> s_UniformBuffer = nullptr;

void SandboxLayer::OnAttach()
{
	// Load Shaders
	auto& modelShader = CreateRef<Shader>("assets/shaders/TestModel.glsl");
	auto& textureShader = CreateRef<Shader>("assets/shaders/TestTexture.glsl");
	auto& alphaShader = CreateRef<Shader>("assets/shaders/TestAlpha.glsl");

	// Load image data
	i32 imageWidth;
	i32 imageHeight;
	i32 imageChannels;

#define PNG 1
#if PNG
	void* imageData = Utils::LoadImageFromDisk("assets/textures/texture-brick.png", imageWidth, imageHeight, imageChannels);
#else
	// void* imageData = Utils::LoadImageFromDisk("assets/textures/texture-wood.jpg", imageWidth, imageHeight, imageChannels);
	void* imageData = Utils::LoadHDRImageFromDisk("assets/textures/clarens_night_02_4k.hdr", imageWidth, imageHeight, imageChannels);
#endif

	// Create Pipeline from device
	auto& blendState = PipelineBlendState{};
	blendState.SrcColorFactor = BlendFactor::SrcAlpha;
	blendState.DstColorFactor = BlendFactor::OneMinusSrcAlpha;
	auto& polygonState = PipelinePolygonState{};
	polygonState.PolygonMode = PolygonRasterMode::Fill;
	auto& pipelineState = PipelineState{};
	pipelineState.BlendState = blendState;
	pipelineState.PolygonState = polygonState;
	s_ModelPipeline = RenderDevice::CreatePipeline(pipelineState, modelShader);
	s_TexturePipeline = RenderDevice::CreatePipeline(pipelineState, textureShader);
	s_AlphaPipeline = RenderDevice::CreatePipeline(pipelineState, alphaShader);

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
	alphaShader->SetMat4("u_ModelViewProj", modelViewProj);
	alphaShader->SetFloat("u_AlphaValue", 0.6);

	// Create texture and set texture data
	TextureDescription textureDesc{};
	textureDesc.ImageExtent = {(u32)imageWidth, (u32)imageHeight, 1};
	textureDesc.ImageFormat = imageChannels == 4 ? ImageFormat::RGBA8 : ImageFormat::RGB16F;

	s_Texture = RenderDevice::CreateTexture(textureDesc);
	s_Texture->SetData(imageData);
	s_Texture->BindTextureUnit(0);
	Utils::FreeImageData(imageData);

	// Create multisampled texture with no data
	textureDesc.SampleCount = SampleCount::Count8;
	s_MultisampledTexture = RenderDevice::CreateTexture(textureDesc);

	// Create uniform buffer
	glm::vec4 testData = { 0.8f, 0.3f, 0.2f, 1.0f };

	BufferDescription uniformBufferDesc{};
	uniformBufferDesc.Type = BufferType::Uniform;
	uniformBufferDesc.Size = sizeof(glm::vec4);
	uniformBufferDesc.Data = &testData;
	s_UniformBuffer = RenderDevice::CreateBuffer(uniformBufferDesc);
	RenderCommand::BindUniformBuffer(s_UniformBuffer, 0);
}

void SandboxLayer::OnDetach()
{
	RenderDevice::DestroyBuffer(s_UniformBuffer);
	RenderDevice::DestroyTexture(s_Texture);
	RenderDevice::DestroyPipeline(s_ModelPipeline);
	RenderDevice::DestroyPipeline(s_TexturePipeline);
}

void SandboxLayer::OnUpdate()
{
#define USE_TEXTURE 0
#if USE_TEXTURE
	s_TexturePipeline->Bind();
#else
	// s_ModelPipeline->Bind();
	s_AlphaPipeline->Bind();
#endif
	s_Model->Render();
}
