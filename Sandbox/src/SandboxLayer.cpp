#include "SandboxLayer.h"
#include "Luminx/Renderer/SkyboxVertices.h"  // Needed if we want to load the skybox manually
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<VertexArray> s_SkyboxVertexArray = nullptr;

static Ref<Pipeline> s_SkyboxPipeline = nullptr;
static Ref<Pipeline> s_TexturePipeline = nullptr;
static Ref<Pipeline> s_AlphaPipeline = nullptr;

static Ref<Model> s_Model = nullptr;
static Ref<Texture> s_Texture = nullptr;
static Ref<Texture> s_MultisampledTexture = nullptr;
static Ref<Texture> s_CubemapTexture = nullptr;
static Ref<Framebuffer> s_Framebuffer = nullptr;

static Ref<Buffer> s_UniformBuffer = nullptr;

void SandboxLayer::OnAttach()
{
	// Load Shaders
	auto& skyboxShader = CreateRef<Shader>("assets/shaders/TestSkybox.glsl");
	auto& textureShader = CreateRef<Shader>("assets/shaders/TestTexture.glsl");
	auto& alphaShader = CreateRef<Shader>("assets/shaders/TestAlpha.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_TexturePipeline = RenderDevice::CreatePipeline(pipelineState, textureShader);
	auto& blendState = PipelineBlendState{};
	blendState.BlendAttachments[0].BlendEnable = true;
	blendState.BlendAttachments[0].ColorEquation.SrcFactor = BlendFactor::SrcAlpha;
	blendState.BlendAttachments[0].ColorEquation.DstFactor = BlendFactor::OneMinusSrcAlpha;
	blendState.BlendAttachments[1].BlendEnable = true;
	blendState.BlendAttachments[1].ColorEquation.SrcFactor = BlendFactor::ConstantColor;
	blendState.BlendAttachments[1].ColorEquation.DstFactor = BlendFactor::OneMinusConstantColor;
	blendState.BlendAttachments[1].ColorWriteMask = ColorWriteMask::ColorWriteMaskR | ColorWriteMask::ColorWriteMaskB;
	blendState.ConstantColor = { 1.0f, 0.6f, 0.3f, 1.0f };
	pipelineState.BlendState = blendState;
	s_AlphaPipeline = RenderDevice::CreatePipeline(pipelineState, alphaShader);

	// We don't want skybox to render depth nor any blending
	pipelineState.DepthState.DepthTest = false;
	pipelineState.BlendState.BlendAttachments[0].BlendEnable = false;
	pipelineState.BlendState.BlendAttachments[1].BlendEnable = false;
	s_SkyboxPipeline = RenderDevice::CreatePipeline(pipelineState, skyboxShader);

	// Create vertex array and vertex buffer for the skybox
	s_SkyboxVertexArray = CreateRef<VertexArray>();

	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.Type = BufferType::Vertex;
	vertexBufferDesc.Size = sizeof(g_SkyboxVertices);
	vertexBufferDesc.Data = g_SkyboxVertices;
	Ref<Buffer> vertexBuffer = RenderDevice::CreateBuffer(vertexBufferDesc);

	BufferLayout skyboxBufferLayout = {
		{ ShaderDataType::Float3, "a_Position" },
	};
	s_SkyboxVertexArray->AddVertexBuffer(vertexBuffer, skyboxBufferLayout);

	// Load model
	s_Model = CreateRef<Model>("assets/models/UVSphere.obj");

	glm::mat4 modelViewProj = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f) *
		glm::lookAt(glm::vec3(2.0f, 2.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 skyboxViewProj = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f) *
		glm::mat4(glm::mat3(glm::lookAt(glm::vec3(2.0f, 2.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f))));

	skyboxShader->SetMat4("u_ViewProj", skyboxViewProj);
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
	textureDesc.ImageExtent = { (u32)imageWidth, (u32)imageHeight, 1 };
	textureDesc.ImageFormat = imageChannels == 4 ? ImageFormat::RGBA8 : ImageFormat::RGB16F;

	s_Texture = RenderDevice::CreateTexture(textureDesc);
	s_Texture->SetData(imageData);
	s_Texture->BindTextureUnit(0);
	Utils::FreeImageData(imageData);

	// Load cubemap data
	std::vector<std::string_view> facesNames = {
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};
	auto cubemapData = Utils::LoadCubemapFromDisk("assets/textures/skyboxFaces/", facesNames, imageWidth, imageHeight, imageChannels);

	// Create texture and set texture data
	TextureDescription cubemapDesc{};
	cubemapDesc.ImageType = ImageType::Cubemap;
	cubemapDesc.ImageExtent = { (u32)imageWidth, (u32)imageHeight, 1 };
	cubemapDesc.ImageFormat = ImageFormat::RGB8;
	cubemapDesc.GenerateMipmaps = true;

	s_CubemapTexture = RenderDevice::CreateTexture(cubemapDesc);
	s_CubemapTexture->SetData(cubemapData.data());
	s_CubemapTexture->BindTextureUnit(1);
	Utils::FreeCubemapData(cubemapData);

	// Create multisampled textures with no data
	textureDesc.SampleCount = SampleCount::Count8;
	s_MultisampledTexture = RenderDevice::CreateTexture(textureDesc);

	textureDesc.ImageFormat = ImageFormat::RGB16F;
	Ref<Texture> textureColorHDR = RenderDevice::CreateTexture(textureDesc);

	textureDesc.ImageFormat = ImageFormat::DEPTH32;
	Ref<Texture> textureDepth = RenderDevice::CreateTexture(textureDesc);	

	// Create framebuffer and attach texture
	FramebufferDescription framebufferDesc{};
	framebufferDesc.Attachments[0] = AttachmentType::Color;
	framebufferDesc.RenderTargets[0] = s_MultisampledTexture;
	framebufferDesc.Attachments[1] = AttachmentType::Color;
	framebufferDesc.RenderTargets[1] = textureColorHDR;
	framebufferDesc.Attachments[2] = AttachmentType::Depth;
	framebufferDesc.RenderTargets[2] = textureDepth;

	ClearValues clearValues = {};
	clearValues.ClearFlags = ClearFlags::Color;
	clearValues.Color = { 1.0f, 0.2f, 0.2f, 1.0f };
	framebufferDesc.ClearValues[0] = clearValues;
	clearValues.Color = { 0.6f, 1.0f, 0.3f, 1.0f };
	framebufferDesc.ClearValues[1] = clearValues;
	clearValues.ClearFlags = ClearFlags::Depth;
	clearValues.Depth = 1.0f;
	framebufferDesc.ClearValues[2] = clearValues;

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
	s_SkyboxPipeline->Bind();
	RenderCommand::BindVertexArray(s_SkyboxVertexArray);
	RenderCommand::DrawVertices(36);

	s_TexturePipeline->Bind();
	s_Model->Render();

	ClearValues commonClearValues{};
	commonClearValues.Color = { 0.6f, 1.0f, 0.3f, 1.0f };
	commonClearValues.ClearFlags = ClearFlags::All;
	// RenderCommand::BeginRenderPass(s_Framebuffer, commonClearValues);
	RenderCommand::BeginRenderPass(s_Framebuffer);
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
