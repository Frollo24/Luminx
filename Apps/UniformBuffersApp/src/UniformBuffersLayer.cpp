#include "UniformBuffersLayer.h"
#include "Luminx/Renderer/SkyboxVertices.h"  // TODO: remove from here
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
using namespace Luminx;

static Ref<VertexArray> s_SkyboxVertexArray = nullptr;

static Ref<Camera> s_Camera = nullptr;
static Ref<Texture> s_CubemapTexture = nullptr;

static Ref<Model> s_LightModel = nullptr;
static Ref<Model> s_PlaneModel = nullptr;

static Ref<Material> s_LightMaterial = nullptr;
static Ref<Material> s_PlaneMaterial = nullptr;

static Ref<Shader> s_SkyboxShader = nullptr;
static Ref<Shader> s_LightShader = nullptr;
static Ref<Shader> s_ModelShader = nullptr;

static Ref<Pipeline> s_SkyboxPipeline = nullptr;
static Ref<Pipeline> s_LightPipeline = nullptr;
static Ref<Pipeline> s_ModelPipeline = nullptr;

static Ref<Buffer> s_LightsUniformBuffer = nullptr;
static Ref<Buffer> s_FogUniformBuffer = nullptr;

static std::array<glm::vec3, 16> s_LightPositions;
static std::array<glm::mat4, 16> s_ModelMats;
static std::array<glm::mat4, 16> s_NormalMats;

static glm::mat4 s_ModelMatPlane = glm::scale(
	glm::translate(
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
		, glm::vec3(0.0f, 0.0f, -4.0f))
	, glm::vec3(10.0f));
static glm::mat4 s_NormalMatPlane = glm::transpose(glm::inverse(s_ModelMatPlane));

static void SelectLights()
{
	if (Input::IsKeyDown(KeyCode::D1))  // Only directional lights
	{
		s_ModelShader->SetBool("u_EnableDirLights", true);
		s_ModelShader->SetBool("u_EnablePointLights", false);
		s_ModelShader->SetBool("u_EnableSpotLights", false);
	}

	if (Input::IsKeyDown(KeyCode::D2))  // Only point lights
	{
		s_ModelShader->SetBool("u_EnableDirLights", false);
		s_ModelShader->SetBool("u_EnablePointLights", true);
		s_ModelShader->SetBool("u_EnableSpotLights", false);
	}

	if (Input::IsKeyDown(KeyCode::D3))  // Only spot lights
	{
		s_ModelShader->SetBool("u_EnableDirLights", false);
		s_ModelShader->SetBool("u_EnablePointLights", false);
		s_ModelShader->SetBool("u_EnableSpotLights", true);
	}

	if (Input::IsKeyDown(KeyCode::D4))  // All lights
	{
		s_ModelShader->SetBool("u_EnableDirLights", true);
		s_ModelShader->SetBool("u_EnablePointLights", true);
		s_ModelShader->SetBool("u_EnableSpotLights", true);
	}
}

void UniformBuffersLayer::OnAttach()
{
	// Load Shaders
	s_SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.glsl");
	s_ModelShader = CreateRef<Shader>("assets/shaders/BlinnPhongFog.glsl");
	s_LightShader = CreateRef<Shader>("assets/shaders/Lights.glsl");

	// Create Pipelines from device
	auto& pipelineState = PipelineState{};
	s_LightPipeline = RenderDevice::CreatePipeline(pipelineState, s_LightShader);

	auto& polygonState = PipelinePolygonState{};
	polygonState.CullEnable = false;
	pipelineState.PolygonState = polygonState;
	s_ModelPipeline = RenderDevice::CreatePipeline(pipelineState, s_ModelShader);

	polygonState = PipelinePolygonState{};
	pipelineState.PolygonState = polygonState;
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
	s_LightModel = CreateRef<Model>("assets/models/UVSphere.obj");
	s_PlaneModel = CreateRef<Model>("assets/models/plane.obj");

	// Setup image loading
	i32 imageWidth;
	i32 imageHeight;
	i32 imageChannels;

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
	std::array<glm::vec4, 3> planeColors = {
		glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // Diffuse
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Specular
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Emissive
	};
	s_PlaneMaterial = CreateRef<PhongMaterial>(s_ModelShader, planeColors);

	for (int i = 0; i < s_ModelMats.size(); i++)
	{
		glm::vec3 translation = glm::sphericalRand(0.5f);
		translation *= (i + 1);
		translation.y *= 0.5f;
		translation.y -= 1.5f;
		float rotation = glm::linearRand(0.0f, glm::two_pi<float>());
		glm::vec3 rotAxis = glm::sphericalRand(1.0f);
		s_ModelMats[i] = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), translation), rotation, rotAxis), glm::vec3(0.25f));
		s_NormalMats[i] = glm::transpose(glm::inverse(s_ModelMats[i]));
		s_LightPositions[i] = translation;
	}

	// Create camera
	s_Camera = Camera::LookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f));

	// Create uniform buffers
	size_t lightBufferSize =
		sizeof(glm::vec4) * 2 * 4     // DirLight size on GLSL * Number of directional lights
		+ sizeof(glm::vec4) * 2 * 16  // PointLight size on GLSL * Number of point lights
		+ sizeof(glm::vec4) * 3 * 8;  // SpotLight size on GLSL * Number of spot lights

	BufferDescription lightsBufferDesc{};
	lightsBufferDesc.Type = BufferType::Uniform;
	lightsBufferDesc.Size = lightBufferSize;

	s_LightsUniformBuffer = RenderDevice::CreateBuffer(lightsBufferDesc);

	// Create all lights and upload them to the lights uniform buffer

	std::array<std::array<glm::vec4, 2>, 4> dirLightData{};
	for (int i = 0; i < 4; i++)
	{
		Ref<Light> dirLight = nullptr;
		if (i == 0)
		{
			dirLight = CreateRef<Light>(LightType::Directional, glm::vec3(1.0f, 0.95686f, 0.83922f), 1.0f);
			dirLight->SetDirection(glm::vec3(2.0f, -2.0f, 5.0f));
		}
		else
		{
			dirLight = CreateRef<Light>(LightType::Directional, glm::vec3(0.0f), 0.0f);
		}

		dirLightData[i] = {
			dirLight->GetLightingData(), glm::vec4(dirLight->GetDirection(), 0.0f)
		};
	}
	s_LightsUniformBuffer->SetData(0, sizeof(glm::vec4) * 2 * 4, dirLightData.data());

	std::array<std::array<glm::vec4, 2>, 16> pointLightData{};
	for (int i = 0; i < 16; i++)
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

		pointLightData[i] = {
			pointLight->GetLightingData(), glm::vec4(pointLight->GetPosition(), 0.0f)
		};
	}
	u32 pointLightOffset = sizeof(glm::vec4) * 2 * 4;
	s_LightsUniformBuffer->SetData(pointLightOffset, sizeof(glm::vec4) * 2 * 16, pointLightData.data());

	std::array<std::array<glm::vec4, 3>, 8> spotLightData{};
	for (int i = 0; i < 8; i++)
	{
		float r = 0.0f, g = 0.0f, b = 0.0f;
		if (i <= 2 || i == 6) r = 1.0f;
		if (i > 1 && i <= 4) g = 1.0f;
		if (i > 3 && i <= 6) b = 1.0f;
		if (i == 1) g = 0.5f;
		if (i == 7) { r = 1.0f; g = 1.0f; b = 1.0f; }

		float x = -glm::cos(glm::quarter_pi<float>() * i + 3.0f * glm::half_pi<float>());
		float z = -glm::cos(glm::quarter_pi<float>() * i);

		Ref<Light> spotLight = CreateRef<Light>(LightType::Spot, glm::vec3(r, g, b), 16.0f);
		spotLight->SetPosition(glm::vec3(7.0f * x, -3.0f, 7.0f * z));
		spotLight->SetDirection(glm::vec3(-x, -1.0f, -z));
		spotLightData[i] = {
			spotLight->GetLightingData(),
			glm::vec4(spotLight->GetDirection(), glm::cos(glm::radians(spotLight->GetPhi()))),
			glm::vec4(spotLight->GetPosition(), glm::cos(glm::radians(spotLight->GetGamma())))
		};
	}
	u32 spotLightOffset = sizeof(glm::vec4) * 2 * 4  // DirLights offset
		+ sizeof(glm::vec4) * 2 * 16;  // PointLights offset
	s_LightsUniformBuffer->SetData(spotLightOffset, sizeof(glm::vec4) * 3 * 8, spotLightData.data());

	glm::vec4 fogData = glm::vec4(0.55f, 0.7f, 0.75f, 0.05f);

	BufferDescription fogBufferDesc{};
	fogBufferDesc.Type = BufferType::Uniform;
	fogBufferDesc.Size = sizeof(glm::vec4);
	fogBufferDesc.Data = &fogData;

	s_FogUniformBuffer = RenderDevice::CreateBuffer(fogBufferDesc);

	// Setting shader uniforms
	s_ModelShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection());
	s_ModelShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());

	s_ModelShader->SetBool("u_EnableDirLights", true);
	s_ModelShader->SetBool("u_EnablePointLights", true);
	s_ModelShader->SetBool("u_EnableSpotLights", true);

	// Setting texture bindings
	s_ModelShader->SetInt("u_DiffuseTexture", 0);
	s_ModelShader->SetInt("u_SpecularTexture", 1);
	s_ModelShader->SetInt("u_EmissiveTexture", 2);

	// Bind uniform buffers
	RenderCommand::BindUniformBuffer(s_LightsUniformBuffer, 0);
	RenderCommand::BindUniformBuffer(s_FogUniformBuffer, 1);
}

void UniformBuffersLayer::OnDetach()
{
	s_ModelShader = nullptr;
	s_SkyboxShader = nullptr;
	s_Camera = nullptr;

	RenderDevice::DestroyPipeline(s_ModelPipeline);
}

void UniformBuffersLayer::OnUpdate()
{
	SelectLights();
	s_Camera->Update();
	s_LightPipeline->Bind();
	s_LightShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());

	glm::mat4 inverseView = glm::inverse(s_Camera->GetView());

	for (int i = 0; i < s_ModelMats.size(); i++)
	{
		s_LightShader->SetMat4("u_ModelView", s_Camera->GetView() * s_ModelMats[i]);
		s_LightShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMats[i]);
		s_LightShader->SetInt("u_LightIndex", i);
		s_LightModel->Render();
	}

	s_ModelPipeline->Bind();
	s_ModelShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_PlaneMaterial->Select();
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

void UniformBuffersLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(UniformBuffersLayer::OnWindowResize));
}

bool UniformBuffersLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	float aspect_ratio = (float)e.GetWidth() / (float)e.GetHeight();
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), aspect_ratio, 0.3f, 50.0f));

	return false;
}
