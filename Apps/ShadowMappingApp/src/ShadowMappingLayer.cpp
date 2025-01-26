#include "ShadowMappingLayer.h"
#include "Luminx/Renderer/SkyboxVertices.h"  // Needed if we want to load the skybox manually
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
using namespace Luminx;

static Ref<VertexArray> s_SkyboxVertexArray = nullptr;

static Ref<Camera> s_Camera = nullptr;
static Ref<Texture> s_CubemapTexture = nullptr;
static Ref<Texture> s_ShadowMapTexture = nullptr;
static Ref<Framebuffer> s_ShadowFramebuffer = nullptr;

static Ref<Model> s_CubeModel = nullptr;
static Ref<Model> s_PlaneModel = nullptr;

static Ref<Material> s_CubeMaterial = nullptr;
static Ref<Material> s_PlaneMaterial = nullptr;

static Ref<Shader> s_SkyboxShader = nullptr;
static Ref<Shader> s_ShadowShader = nullptr;
static Ref<Shader> s_ModelShader = nullptr;

static Ref<Pipeline> s_SkyboxPipeline = nullptr;
static Ref<Pipeline> s_ShadowPipeline = nullptr;
static Ref<Pipeline> s_ModelPipeline = nullptr;

static Ref<Texture> s_DiffuseTexture = nullptr;
static Ref<Texture> s_SpecularTexture = nullptr;
static Ref<Texture> s_EmissiveTexture = nullptr;

static Ref<Light> s_DirLight = nullptr;
static Ref<Light> s_PointLight = nullptr;
static Ref<Light> s_SpotLight = nullptr;

static std::array<glm::mat4, 11> s_ModelMats;
static std::array<glm::mat4, 11> s_NormalMats;

static glm::mat4 s_ModelMatPlane = glm::scale(
	glm::translate(
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
		, glm::vec3(0.0f, 0.0f, -4.0f))
	, glm::vec3(10.0f));
static glm::mat4 s_NormalMatPlane = glm::transpose(glm::inverse(s_ModelMatPlane));

static glm::vec3 s_LightDirection = glm::vec3(2.0f, -2.0f, 5.0f);
static glm::mat4 s_LightSpaceMatrix = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.75f, 35.0f)
	* glm::lookAt(-2.0f * s_LightDirection, glm::vec3(0.0f, 0.0f, 0.0f),
		(s_LightDirection.x == 0.0f && s_LightDirection.z == 0.0f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f));

void ShadowMappingLayer::OnAttach()
{
	// Load Shaders
	s_SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.glsl");
	s_ShadowShader = CreateRef<Shader>("assets/shaders/ShadowMap.glsl");
	s_ModelShader = CreateRef<Shader>("assets/shaders/BlinnPhongShadow.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_ModelPipeline = RenderDevice::CreatePipeline(pipelineState, s_ModelShader);
	s_ShadowPipeline = RenderDevice::CreatePipeline(pipelineState, s_ShadowShader);
	auto& depthState = PipelineDepthState{};
	depthState.DepthFunc = DepthComparison::LessOrEqual;
	depthState.DepthWrite = false;
	pipelineState.DepthState = depthState;
	s_SkyboxPipeline = RenderDevice::CreatePipeline(pipelineState, s_SkyboxShader);

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

	// Load models
	s_CubeModel = CreateRef<Model>("assets/models/CubeWall.obj");
	s_PlaneModel = CreateRef<Model>("assets/models/plane.obj");

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

	// Create shadow map texture and bind it to its texture unit
	TextureDescription shadowMapDesc{};
	shadowMapDesc.ImageType = ImageType::Image2D;
	shadowMapDesc.ImageExtent = { 2048, 2048, 1 };
	shadowMapDesc.ImageFormat = ImageFormat::DEPTH32;
	shadowMapDesc.WrapMode = TextureWrapMode::ClampBorder;
	shadowMapDesc.BorderColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	shadowMapDesc.GenerateMipmaps = false;

	s_ShadowMapTexture = RenderDevice::CreateTexture(shadowMapDesc);
	s_ShadowMapTexture->BindTextureUnit(3);

	// Create materials and assign model matrices
	std::array<Ref<Texture>, 3> textures = {
		s_DiffuseTexture,
		s_SpecularTexture,
		s_EmissiveTexture
	};
	s_CubeMaterial = CreateRef<PhongMaterial>(s_ModelShader, textures);

	std::array<glm::vec4, 3> planeColors = {
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Diffuse
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Specular
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Emissive
	};
	s_PlaneMaterial = CreateRef<PhongMaterial>(s_ModelShader, planeColors);

	for (int i = 0; i < s_ModelMats.size(); i++)
	{
		glm::vec3 translation = glm::sphericalRand(1.5f);
		translation *= (i + 1);
		float rotation = glm::linearRand(0.0f, glm::two_pi<float>());
		glm::vec3 rotAxis = glm::sphericalRand(1.0f);
		s_ModelMats[i] = glm::rotate(glm::translate(glm::mat4(1.0f), translation), rotation, rotAxis);
		s_NormalMats[i] = glm::transpose(glm::inverse(s_ModelMats[i]));
	}

	// Create camera
	s_Camera = Camera::LookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f));

	// Create directional light
	s_DirLight = CreateRef<Light>(LightType::Directional, glm::vec3(1.0f, 0.95686f, 0.83922f), 1.0f);
	s_DirLight->SetDirection(s_LightDirection);

	// Setting shader uniforms
	s_ModelShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection());
	s_ModelShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());

	s_ModelShader->SetFloat4("u_DirLight.color", s_DirLight->GetLightingData());
	s_ModelShader->SetFloat3("u_DirLight.direction", s_DirLight->GetDirection());

	s_ModelShader->SetFloat3("u_FogColor", glm::vec3(0.55f, 0.7f, 0.75f));
	s_ModelShader->SetFloat("u_FogDensity", 0.05f);

	// Setting texture bindings
	s_ModelShader->SetInt("u_DiffuseTexture", 0);
	s_ModelShader->SetInt("u_SpecularTexture", 1);
	s_ModelShader->SetInt("u_EmissiveTexture", 2);
	s_ModelShader->SetInt("u_ShadowMap", 3);

	// Create shadow framebuffer and attach depth texture
	FramebufferDescription framebufferDesc{};
	framebufferDesc.Width = 2048;
	framebufferDesc.Height = 2048;
	framebufferDesc.Attachments[0] = AttachmentType::Depth;
	framebufferDesc.RenderTargets[0] = s_ShadowMapTexture;

	ClearValues shadowClearValues{};
	shadowClearValues.Depth = 1.0f;
	shadowClearValues.ClearFlags = ClearFlags::Depth;
	framebufferDesc.ClearValues[0] = shadowClearValues;

	s_ShadowFramebuffer = RenderDevice::CreateFramebuffer(framebufferDesc);
}

void ShadowMappingLayer::OnDetach()
{
	s_ModelShader = nullptr;
	s_SkyboxShader = nullptr;
	s_CubeModel = nullptr;
	s_Camera = nullptr;

	RenderDevice::DestroyTexture(s_DiffuseTexture);
	RenderDevice::DestroyTexture(s_SpecularTexture);
	RenderDevice::DestroyTexture(s_EmissiveTexture);
	RenderDevice::DestroyPipeline(s_ShadowPipeline);
	RenderDevice::DestroyPipeline(s_ModelPipeline);
	RenderDevice::DestroyFramebuffer(s_ShadowFramebuffer);
}

void ShadowMappingLayer::OnUpdate()
{
	s_Camera->Update();

	RenderCommand::BeginRenderPass(s_ShadowFramebuffer);
	RenderCommand::SetViewport(0, 0, s_ShadowFramebuffer->GetDescription().Width, s_ShadowFramebuffer->GetDescription().Height);
	s_ShadowPipeline->Bind();
	s_ShadowShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_ShadowShader->SetMat4("u_LightSpaceMatrix", s_LightSpaceMatrix);

	s_CubeMaterial->Select();
	for (int i = 0; i < s_ModelMats.size(); i++)
	{
		s_ShadowShader->SetMat4("u_Model", s_ModelMats[i]);
		s_CubeModel->Render();
	}

	s_PlaneMaterial->Select();
	s_ShadowShader->SetMat4("u_Model", s_ModelMatPlane);
	s_PlaneModel->Render();

	RenderCommand::EndRenderPass();
	RenderCommand::DefaultFramebuffer();
	RenderCommand::SetViewport(0, 0, Application::Instance().GetWindow().GetWidth(), Application::Instance().GetWindow().GetHeight());
	s_ModelPipeline->Bind();
	s_ShadowMapTexture->BindTextureUnit(3);
	s_ModelShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_ModelShader->SetMat4("u_LightSpaceMatrix", s_LightSpaceMatrix);

	s_CubeMaterial->Select();
	for (int i = 0; i < s_ModelMats.size(); i++)
	{
		s_ModelShader->SetMat4("u_Model", s_ModelMats[i]);
		s_ModelShader->SetMat4("u_ModelView", s_Camera->GetView() * s_ModelMats[i]);
		s_ModelShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMats[i]);
		s_ModelShader->SetMat4("u_Normal", s_NormalMats[i]);
		s_CubeModel->Render();
	}

	s_PlaneMaterial->Select();
	s_ShadowMapTexture->BindTextureUnit(3);
	s_ModelShader->SetMat4("u_Model", s_ModelMatPlane);
	s_ModelShader->SetMat4("u_ModelView", s_Camera->GetView() * s_ModelMatPlane);
	s_ModelShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMatPlane);
	s_ModelShader->SetMat4("u_Normal", s_NormalMatPlane);
	s_PlaneModel->Render();

	s_SkyboxPipeline->Bind();
	glm::mat4 skyboxViewProj = s_Camera->GetProjection() * glm::mat4(glm::mat3(s_Camera->GetView()));
	s_SkyboxShader->SetMat4("u_ViewProj", skyboxViewProj);
	RenderCommand::BindVertexArray(s_SkyboxVertexArray);
	RenderCommand::DrawVertices(36);
}

void ShadowMappingLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(ShadowMappingLayer::OnWindowResize));
}

bool ShadowMappingLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	float aspect_ratio = (float)e.GetWidth() / (float)e.GetHeight();
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), aspect_ratio, 0.3f, 50.0f));

	return false;
}
