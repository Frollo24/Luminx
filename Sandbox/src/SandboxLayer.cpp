#include "SandboxLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_TexturePipeline = nullptr;
static Ref<Pipeline> s_AlphaPipeline = nullptr;

static Ref<Model> s_Model = nullptr;
static Ref<Texture> s_Texture = nullptr;
static Ref<Texture> s_MultisampledTexture = nullptr;
static Ref<Framebuffer> s_Framebuffer = nullptr;

static Ref<Buffer> s_UniformBuffer = nullptr;

void SandboxLayer::OnAttach()
{
	// Load Shaders
	auto& textureShader = CreateRef<Shader>("assets/shaders/TestTexture.glsl");
	auto& alphaShader = CreateRef<Shader>("assets/shaders/TestAlpha.glsl");

	// Create Pipeline from device
	auto& blendState = PipelineBlendState{};
	blendState.SrcColorFactor = BlendFactor::SrcAlpha;
	blendState.DstColorFactor = BlendFactor::OneMinusSrcAlpha;
	auto& polygonState = PipelinePolygonState{};
	polygonState.PolygonMode = PolygonRasterMode::Fill;
	auto& pipelineState = PipelineState{};
	pipelineState.BlendState = blendState;
	pipelineState.PolygonState = polygonState;
	s_TexturePipeline = RenderDevice::CreatePipeline(pipelineState, textureShader);
	s_AlphaPipeline = RenderDevice::CreatePipeline(pipelineState, alphaShader);

	// Load model
	s_Model = CreateRef<Model>("assets/models/UVSphere.obj");


	glm::mat4 modelViewProj = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	textureShader->SetMat4("u_ModelViewProj", modelViewProj);
	alphaShader->SetMat4("u_ModelViewProj", modelViewProj);
	alphaShader->SetFloat("u_AlphaValue", 0.6);

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

	// Create framebuffer and attach texture
	FramebufferDescription framebufferDesc{};
	framebufferDesc.Attachments.push_back(AttachmentType::Color);
	framebufferDesc.RenderTargets.push_back(s_MultisampledTexture);
	s_Framebuffer = RenderDevice::CreateFramebuffer(framebufferDesc);

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
	RenderDevice::DestroyPipeline(s_TexturePipeline);
	RenderDevice::DestroyFramebuffer(s_Framebuffer);
	RenderDevice::DestroyTexture(s_MultisampledTexture);
}

void SandboxLayer::OnUpdate()
{
	s_TexturePipeline->Bind();
	s_Model->Render();

	ClearValues clearValues{};
	clearValues.Color = { 1.0f, 0.6f, 0.3f, 1.0f };
	clearValues.ClearFlags = ClearFlags::Color;
	RenderCommand::BeginRenderPass(s_Framebuffer, clearValues);
	s_AlphaPipeline->Bind();
	s_Model->Render();
	RenderCommand::EndRenderPass();
	RenderCommand::DefaultFramebuffer();
}

void SandboxLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
}

bool SandboxLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	s_Framebuffer->Resize(e.GetWidth(), e.GetHeight());
	return false;
}
