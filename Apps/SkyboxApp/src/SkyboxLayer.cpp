#include "SkyboxLayer.h"
#include "Luminx/Renderer/SkyboxVertices.h"  // TODO: remove from here
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

static Ref<Shader> s_SkyboxShader = nullptr;
static Ref<Shader> s_ModelShader = nullptr;

static Ref<Pipeline> s_SkyboxPipeline = nullptr;
static Ref<Pipeline> s_ModelPipeline = nullptr;

static Ref<Texture> s_DiffuseTexture = nullptr;
static Ref<Texture> s_SpecularTexture = nullptr;
static Ref<Texture> s_EmissiveTexture = nullptr;

static Ref<Light> s_DirLight = nullptr;
static Ref<Light> s_PointLight = nullptr;
static Ref<Light> s_SpotLight = nullptr;

static std::array<glm::mat4, 11> s_ModelMats;
static std::array<glm::mat4, 11> s_NormalMats;

static glm::mat4 s_ModelMatLight = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 3.0f));
static glm::mat4 s_NormalMatLight = glm::transpose(glm::inverse(s_ModelMatLight));

static glm::mat4 s_ModelMatPlane = glm::scale(
	glm::translate(
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
		, glm::vec3(0.0f, 0.0f, -4.0f))
	, glm::vec3(10.0f));
static glm::mat4 s_NormalMatPlane = glm::transpose(glm::inverse(s_ModelMatPlane));

static bool s_EnableFog = true;

void SkyboxLayer::OnAttach()
{
	// Load Shaders
	s_SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.glsl");
	s_ModelShader = CreateRef<Shader>("assets/shaders/BlinnPhongFog.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_ModelPipeline = RenderDevice::CreatePipeline(pipelineState, s_ModelShader);
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
	s_LightModel = CreateRef<Model>("assets/models/UVSphere.obj");
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

	// Create materials and assign model matrices
	std::array<Ref<Texture>, 3> textures = {
		s_DiffuseTexture,
		s_SpecularTexture,
		s_EmissiveTexture
	};
	s_CubeMaterial = CreateRef<PhongMaterial>(s_ModelShader, textures);

	std::array<glm::vec4, 3> lightColors = {
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Diffuse
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Specular
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), // Emissive
	};
	s_LightMaterial = CreateRef<PhongMaterial>(s_ModelShader, lightColors);

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
	s_DirLight->SetDirection(glm::vec3(2.0f, -2.0f, 5.0f));

	s_PointLight = CreateRef<Light>(LightType::Point, glm::vec3(0.0f, 1.0f, 1.0f), 8.0f);
	s_PointLight->SetPosition(glm::vec3(1.0f, 1.0f, 3.0f));

	s_SpotLight = CreateRef<Light>(LightType::Spot, glm::vec3(1.0f, 0.95686f, 0.0f), 16.0f);
	s_SpotLight->SetDirection(glm::vec3(0.0f, 0.0f, -1.0f));

	// Setting shader uniforms
	s_ModelShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection());
	s_ModelShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());

	s_ModelShader->SetFloat4("u_DirLight.color", s_DirLight->GetLightingData());
	s_ModelShader->SetFloat3("u_DirLight.direction", s_DirLight->GetDirection());

	s_ModelShader->SetFloat4("u_PointLight.color", s_PointLight->GetLightingData());
	s_ModelShader->SetFloat3("u_PointLight.position", s_PointLight->GetPosition());

	s_ModelShader->SetFloat4("u_SpotLight.color", s_SpotLight->GetLightingData());
	s_ModelShader->SetFloat3("u_SpotLight.direction", s_SpotLight->GetDirection());
	s_ModelShader->SetFloat3("u_SpotLight.position", s_SpotLight->GetPosition());
	s_ModelShader->SetFloat("u_SpotLight.innerCutoff", glm::cos(glm::radians(s_SpotLight->GetPhi())));
	s_ModelShader->SetFloat("u_SpotLight.outerCutoff", glm::cos(glm::radians(s_SpotLight->GetGamma())));

	s_ModelShader->SetFloat3("u_FogColor", glm::vec3(0.55f, 0.7f, 0.75f));
	s_ModelShader->SetFloat("u_FogDensity", 0.05f);

	// Setting texture bindings
	s_ModelShader->SetInt("u_DiffuseTexture", 0);
	s_ModelShader->SetInt("u_SpecularTexture", 1);
	s_ModelShader->SetInt("u_EmissiveTexture", 2);
}

void SkyboxLayer::OnDetach()
{
	s_ModelShader = nullptr;
	s_SkyboxShader = nullptr;
	s_CubeModel = nullptr;
	s_Camera = nullptr;

	RenderDevice::DestroyTexture(s_DiffuseTexture);
	RenderDevice::DestroyTexture(s_SpecularTexture);
	RenderDevice::DestroyTexture(s_EmissiveTexture);
	RenderDevice::DestroyPipeline(s_ModelPipeline);
}

void SkyboxLayer::OnUpdate()
{
	s_Camera->Update();
	s_ModelPipeline->Bind();
	s_ModelShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());

	// We want the spot light following the camera
	glm::mat4 inverseView = glm::inverse(s_Camera->GetView());
	s_ModelShader->SetFloat3("u_SpotLight.direction", glm::vec3(inverseView * glm::vec4(s_SpotLight->GetDirection(), 0.0f)));
	s_ModelShader->SetFloat3("u_SpotLight.position", glm::vec3(inverseView * glm::vec4(s_SpotLight->GetPosition(), 1.0f)));

	// Enable and disable fog
	if (Input::IsKeyDown(KeyCode::F))
	{
		s_EnableFog = !s_EnableFog;
		float density = s_EnableFog ? 0.05f : 0.0f;
		s_ModelShader->SetFloat("u_FogDensity", density);
	}

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
	s_ModelShader->SetMat4("u_Model", s_ModelMatPlane);
	s_ModelShader->SetMat4("u_Normal", s_NormalMatPlane);
	s_ModelShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMatPlane);
	s_PlaneModel->Render();

	s_LightMaterial->Select();
	s_ModelShader->SetMat4("u_Model", s_ModelMatLight);
	s_ModelShader->SetMat4("u_Normal", s_NormalMatLight);
	s_ModelShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMatLight);
	s_LightModel->Render();

	s_SkyboxPipeline->Bind();
	glm::mat4 skyboxViewProj = s_Camera->GetProjection() * glm::mat4(glm::mat3(s_Camera->GetView()));
	s_SkyboxShader->SetMat4("u_ViewProj", skyboxViewProj);
	RenderCommand::BindVertexArray(s_SkyboxVertexArray);
	RenderCommand::DrawVertices(36);
}

void SkyboxLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(SkyboxLayer::OnWindowResize));
}

bool SkyboxLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	float aspect_ratio = (float)e.GetWidth() / (float)e.GetHeight();
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), aspect_ratio, 0.3f, 50.0f));

	return false;
}
