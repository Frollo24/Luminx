#include "DeferredShadingLayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
using namespace Luminx;

static Ref<VertexArray> s_SkyboxVertexArray = nullptr;

static Ref<Camera> s_Camera = nullptr;
static Ref<Texture> s_CubemapTexture = nullptr;

static Ref<Model> s_CubeModel = nullptr;
static Ref<Model> s_LightModel = nullptr;
static Ref<Model> s_PlaneModel = nullptr;

static Ref<Material> s_CubeMaterial = nullptr;
static Ref<Material> s_LightMaterial = nullptr;
static Ref<Material> s_PlaneMaterial = nullptr;

static Ref<Texture> s_DiffuseTexture = nullptr;
static Ref<Texture> s_SpecularTexture = nullptr;
static Ref<Texture> s_EmissiveTexture = nullptr;
static Ref<Texture> s_NormalTexture = nullptr;

static Ref<Shader> s_SkyboxShader = nullptr;
static Ref<Shader> s_LightShader = nullptr;
static Ref<Shader> s_DeferredShader = nullptr;
static Ref<Shader> s_GBufferShader = nullptr;

static Ref<Pipeline> s_SkyboxPipeline = nullptr;
static Ref<Pipeline> s_LightPipeline = nullptr;
static Ref<Pipeline> s_ModelPipeline = nullptr;
static Ref<Pipeline> s_GBufferPipeline = nullptr;
static Ref<Pipeline> s_DeferredPipeline = nullptr;

static Ref<Texture> s_PositionBuffer = nullptr;
static Ref<Texture> s_NormalBuffer = nullptr;
static Ref<Texture> s_AlbedoBuffer = nullptr;
static Ref<Texture> s_SpecGlossBuffer = nullptr;
static Ref<Texture> s_EmissiveBuffer = nullptr;
static Ref<Texture> s_DepthBuffer = nullptr;
static Ref<Framebuffer> s_GBufferFramebuffer = nullptr;

static Ref<Buffer> s_LightsUniformBuffer = nullptr;
static Ref<Buffer> s_FogUniformBuffer = nullptr;

#define NUM_POINT_LIGHTS 256  // Should not exceed 1024 unless it is changed on the shaders as well
static std::array<glm::vec3, NUM_POINT_LIGHTS> s_LightPositions = { glm::vec3(0.0f) };
static std::array<glm::mat4, NUM_POINT_LIGHTS> s_LightModelMats;

static std::array<glm::mat4, 8> s_ModelMats;
static std::array<glm::mat4, 8> s_NormalMats;

static glm::mat4 s_ModelMatPlane = glm::scale(
	glm::translate(
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
		, glm::vec3(0.0f, 0.0f, -4.0f))
	, glm::vec3(10.0f));
static glm::mat4 s_NormalMatPlane = glm::transpose(glm::inverse(s_ModelMatPlane));

static void SelectBuffer()
{
	if (Input::IsKeyDown(KeyCode::D0))  // Deferred shading
		s_DeferredShader->SetInt("u_GBufferTexture", 0);
	if (Input::IsKeyDown(KeyCode::D1))  // PositionBuffer
		s_DeferredShader->SetInt("u_GBufferTexture", 1);
	if (Input::IsKeyDown(KeyCode::D2))  // NormalBuffer
		s_DeferredShader->SetInt("u_GBufferTexture", 2);
	if (Input::IsKeyDown(KeyCode::D3))  // AlbedoBuffer
		s_DeferredShader->SetInt("u_GBufferTexture", 3);
	if (Input::IsKeyDown(KeyCode::D4))  // SpecularBuffer
		s_DeferredShader->SetInt("u_GBufferTexture", 4);
	if (Input::IsKeyDown(KeyCode::D5))  // EmissiveBuffer
		s_DeferredShader->SetInt("u_GBufferTexture", 5);
}

void DeferredShadingLayer::OnAttach()
{
	// Load Shaders
	s_SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.glsl");
	s_DeferredShader = CreateRef<Shader>("assets/shaders/BlinnPhongDeferredFog.glsl");
	s_LightShader = CreateRef<Shader>("assets/shaders/Lights.glsl");
	s_GBufferShader = CreateRef<Shader>("assets/shaders/GBufferNormal.glsl");

	// Create Pipelines from device
	auto& pipelineState = PipelineState{};
	s_LightPipeline = RenderDevice::CreatePipeline(pipelineState, s_LightShader);
	s_GBufferPipeline = RenderDevice::CreatePipeline(pipelineState, s_GBufferShader);
	s_DeferredPipeline = RenderDevice::CreatePipeline(pipelineState, s_DeferredShader);

	auto& polygonState = PipelinePolygonState{};
	polygonState.CullEnable = false;
	pipelineState.PolygonState = polygonState;
	s_ModelPipeline = RenderDevice::CreatePipeline(pipelineState, s_GBufferShader);

	// Create the skybox using the renderer functionality
	Renderer::LoadSkyboxPipeline(s_SkyboxShader);

	// Load models
	s_CubeModel = CreateRef<Model>("assets/models/CubeWall.obj");
	s_LightModel = CreateRef<Model>("assets/models/UVSphere.obj");
	s_PlaneModel = CreateRef<Model>("assets/models/plane.obj");

	// Create multisampled textures with no data
	TextureDescription bufferDesc{};
	bufferDesc.ImageExtent = { (u32)Application::Instance().GetWindow().GetWidth(), (u32)Application::Instance().GetWindow().GetHeight(), 1};
	bufferDesc.GenerateMipmaps = false;
	
	bufferDesc.ImageFormat = ImageFormat::RGBA16F;
	s_PositionBuffer = RenderDevice::CreateTexture(bufferDesc);

	bufferDesc.ImageFormat = ImageFormat::RGBA16F;
	s_NormalBuffer = RenderDevice::CreateTexture(bufferDesc);

	bufferDesc.ImageFormat = ImageFormat::RGBA8;
	s_AlbedoBuffer = RenderDevice::CreateTexture(bufferDesc);

	bufferDesc.ImageFormat = ImageFormat::RGBA8;
	s_SpecGlossBuffer = RenderDevice::CreateTexture(bufferDesc);

	bufferDesc.ImageFormat = ImageFormat::RGBA8;
	s_EmissiveBuffer = RenderDevice::CreateTexture(bufferDesc);

	bufferDesc.ImageFormat = ImageFormat::DEPTH32;
	s_DepthBuffer = RenderDevice::CreateTexture(bufferDesc);

	// Create framebuffer and attach texture
	FramebufferDescription framebufferDesc{};
	framebufferDesc.Attachments[0] = AttachmentType::Color;
	framebufferDesc.RenderTargets[0] = s_PositionBuffer;
	framebufferDesc.Attachments[1] = AttachmentType::Color;
	framebufferDesc.RenderTargets[1] = s_NormalBuffer;
	framebufferDesc.Attachments[2] = AttachmentType::Color;
	framebufferDesc.RenderTargets[2] = s_AlbedoBuffer;
	framebufferDesc.Attachments[3] = AttachmentType::Color;
	framebufferDesc.RenderTargets[3] = s_SpecGlossBuffer;
	framebufferDesc.Attachments[4] = AttachmentType::Color;
	framebufferDesc.RenderTargets[4] = s_EmissiveBuffer;
	framebufferDesc.Attachments[5] = AttachmentType::Depth;
	framebufferDesc.RenderTargets[5] = s_DepthBuffer;

	ClearValues clearValues = {};
	clearValues.ClearFlags = ClearFlags::Color;
	clearValues.Color = { 0.0f, 0.0f, 0.0f, 0.0f };
	framebufferDesc.ClearValues[0] = clearValues;
	framebufferDesc.ClearValues[1] = clearValues;
	framebufferDesc.ClearValues[2] = clearValues;
	framebufferDesc.ClearValues[3] = clearValues;
	framebufferDesc.ClearValues[4] = clearValues;
	clearValues.ClearFlags = ClearFlags::Depth;
	clearValues.Depth = 1.0f;
	framebufferDesc.ClearValues[5] = clearValues;

	s_GBufferFramebuffer = RenderDevice::CreateFramebuffer(framebufferDesc);

	// Setup image loading
	i32 imageWidth;
	i32 imageHeight;
	i32 imageChannels;
	void* imageData;

	// Create texture and set texture data for every texture

	// Diffuse texture
	imageData = Utils::LoadImageFromDisk("assets/textures/color2.png", imageWidth, imageHeight, imageChannels);

	TextureDescription textureDesc{};
	textureDesc.ImageExtent = { (u32)imageWidth, (u32)imageHeight, 1 };
	textureDesc.ImageFormat = ImageFormat::RGBA8;

	s_DiffuseTexture = RenderDevice::CreateTexture(textureDesc);
	s_DiffuseTexture->SetData(imageData);
	Utils::FreeImageData(imageData);

	// Specular texture
	imageData = Utils::LoadImageFromDisk("assets/textures/specMap.png", imageWidth, imageHeight, imageChannels);
	s_SpecularTexture = RenderDevice::CreateTexture(textureDesc);
	s_SpecularTexture->SetData(imageData);
	Utils::FreeImageData(imageData);

	// Emissive texture
	imageData = Utils::LoadImageFromDisk("assets/textures/emissive.png", imageWidth, imageHeight, imageChannels);
	s_EmissiveTexture = RenderDevice::CreateTexture(textureDesc);
	s_EmissiveTexture->SetData(imageData);
	Utils::FreeImageData(imageData);

	// Normal texture
	imageData = Utils::LoadImageFromDisk("assets/textures/normal.png", imageWidth, imageHeight, imageChannels);
	s_NormalTexture = RenderDevice::CreateTexture(textureDesc);
	s_NormalTexture->SetData(imageData);
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
	s_CubemapTexture->BindTextureUnit(0);
	Utils::FreeCubemapData(cubemapData);

	// Create materials and assign model matrices
	std::array<Ref<Texture>, 3> textures = {
		s_DiffuseTexture,
		s_SpecularTexture,
		s_EmissiveTexture
	};
	s_CubeMaterial = CreateRef<PhongMaterial>(s_GBufferShader, textures);
	DynamicCast<PhongMaterial>(s_CubeMaterial)->SetNormalTexture(s_NormalTexture);

	std::array<glm::vec4, 3> planeColors = {
		glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // Diffuse
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Specular
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Emissive
	};
	s_PlaneMaterial = CreateRef<PhongMaterial>(s_GBufferShader, planeColors);

	for (int i = 0; i < s_ModelMats.size(); i++)
	{
		glm::vec3 translation = glm::sphericalRand(1.5f);
		translation *= (i + 1);
		float rotation = glm::linearRand(0.0f, glm::two_pi<float>());
		glm::vec3 rotAxis = glm::sphericalRand(1.0f);
		s_ModelMats[i] = glm::rotate(glm::translate(glm::mat4(1.0f), translation), rotation, rotAxis);
		s_NormalMats[i] = glm::transpose(glm::inverse(s_ModelMats[i]));
	}

	for (int i = 0; i < s_LightModelMats.size(); i++)
	{
		glm::vec3 translation = glm::sphericalRand(10.0f);
		translation *= glm::sqrt(glm::linearRand(0.0f, 1.0f));
		float rotation = glm::linearRand(0.0f, glm::two_pi<float>());
		glm::vec3 rotAxis = glm::sphericalRand(1.0f);
		s_LightModelMats[i] = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), translation), rotation, rotAxis), glm::vec3(0.1f));
		s_LightPositions[i] = translation;
	}

	// Create camera
	s_Camera = Camera::LookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f));

	// Create uniform buffers
	size_t lightBufferSize =
		+ sizeof(glm::vec4) * 2 * NUM_POINT_LIGHTS;  // PointLight size on GLSL * Number of point lights

	BufferDescription lightsBufferDesc{};
	lightsBufferDesc.Type = BufferType::Uniform;
	lightsBufferDesc.Size = lightBufferSize;

	s_LightsUniformBuffer = RenderDevice::CreateBuffer(lightsBufferDesc);

	// Create all lights and upload them to the lights uniform buffer
	std::array<std::array<glm::vec4, 2>, NUM_POINT_LIGHTS> pointLightData{};
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		Ref<Light> pointLight = nullptr;
		if (i == 0)
		{
			pointLight = CreateRef<Light>(LightType::Point, glm::vec3(0.0f, 1.0f, 1.0f), 4.0f);
		}
		else
		{
			glm::vec3 randColor = glm::vec3(glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f));
			pointLight = CreateRef<Light>(LightType::Point, randColor, 2.0f);
		}
		pointLight->SetPosition(s_LightPositions[i]);
		pointLight->SetRadius(4.0f);

		pointLightData[i] = {
			pointLight->GetLightingData(), glm::vec4(pointLight->GetPosition(), pointLight->GetRadius())
		};
	}
	u32 pointLightOffset = 0;
	s_LightsUniformBuffer->SetData(pointLightOffset, sizeof(glm::vec4) * 2 * NUM_POINT_LIGHTS, pointLightData.data());

	glm::vec4 fogData = glm::vec4(0.55f, 0.7f, 0.75f, 0.05f);

	BufferDescription fogBufferDesc{};
	fogBufferDesc.Type = BufferType::Uniform;
	fogBufferDesc.Size = sizeof(glm::vec4);
	fogBufferDesc.Data = &fogData;

	s_FogUniformBuffer = RenderDevice::CreateBuffer(fogBufferDesc);

	// Setting shader uniforms
	s_GBufferShader->SetBool("u_UsingNormalmap", true);
	s_DeferredShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection());
	s_DeferredShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_DeferredShader->SetInt("u_GBufferTexture", 0);

	// Bind uniform buffers
	RenderCommand::BindUniformBuffer(s_LightsUniformBuffer, 0);
	RenderCommand::BindUniformBuffer(s_FogUniformBuffer, 1);
}

void DeferredShadingLayer::OnDetach()
{
	s_SkyboxShader = nullptr;
	s_Camera = nullptr;

	RenderDevice::DestroyPipeline(s_ModelPipeline);
}

void DeferredShadingLayer::OnUpdate()
{
	SelectBuffer();
	s_Camera->Update();

	RenderCommand::BeginRenderPass(s_GBufferFramebuffer);
	s_GBufferPipeline->Bind();
	s_GBufferShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_CubeMaterial->Select();
	for (int i = 0; i < s_ModelMats.size(); i++)
	{
		s_GBufferShader->SetMat4("u_Model", s_ModelMats[i]);
		s_GBufferShader->SetMat4("u_ModelView", s_Camera->GetView() * s_ModelMats[i]);
		s_GBufferShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMats[i]);
		s_GBufferShader->SetMat4("u_Normal", s_NormalMats[i]);
		s_CubeModel->Render();
	}

	s_ModelPipeline->Bind();
	s_GBufferShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_PlaneMaterial->Select();
	s_GBufferShader->SetMat4("u_Model", s_ModelMatPlane);
	s_GBufferShader->SetMat4("u_ModelView", s_Camera->GetView() * s_ModelMatPlane);
	s_GBufferShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMatPlane);
	s_GBufferShader->SetMat4("u_Normal", s_NormalMatPlane);
	s_PlaneModel->Render();
	RenderCommand::EndRenderPass();
	RenderCommand::DefaultFramebuffer();

	s_DeferredPipeline->Bind();
	const auto& renderTargets = s_GBufferFramebuffer->GetDescription().RenderTargets;
	renderTargets[0]->BindTextureUnit(0);
	renderTargets[1]->BindTextureUnit(1);
	renderTargets[2]->BindTextureUnit(2);
	renderTargets[3]->BindTextureUnit(3);
	renderTargets[4]->BindTextureUnit(4);
	renderTargets[5]->BindTextureUnit(5);
	s_DeferredShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	RenderCommand::DrawVertices(6);

	s_LightPipeline->Bind();
	s_LightShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());

	for (int i = 0; i < s_LightModelMats.size(); i++)
	{
		s_LightShader->SetMat4("u_ModelView", s_Camera->GetView() * s_LightModelMats[i]);
		s_LightShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_LightModelMats[i]);
		s_LightShader->SetInt("u_LightIndex", i);
		s_LightModel->Render();
	}

	Renderer::DrawSkybox(s_Camera);
}

void DeferredShadingLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(DeferredShadingLayer::OnWindowResize));
}

bool DeferredShadingLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	float aspect_ratio = (float)e.GetWidth() / (float)e.GetHeight();
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), aspect_ratio, 0.3f, 50.0f));

	s_GBufferFramebuffer->Resize(e.GetWidth(), e.GetHeight());

	return false;
}
