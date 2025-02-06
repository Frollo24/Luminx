#include "GeometryExplodingLayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
using namespace Luminx;

static Ref<VertexArray> s_SkyboxVertexArray = nullptr;

static Ref<Camera> s_Camera = nullptr;
static Ref<Texture> s_CubemapTexture = nullptr;
static Ref<Light> s_DirLight = nullptr;

static Ref<Model> s_GeometryModel = nullptr;
static Ref<Model> s_PlaneModel = nullptr;

static Ref<Material> s_GeometryMaterial = nullptr;
static Ref<Material> s_PlaneMaterial = nullptr;

static Ref<Texture> s_DiffuseTexture = nullptr;
static Ref<Texture> s_SpecularTexture = nullptr;
static Ref<Texture> s_EmissiveTexture = nullptr;
static Ref<Texture> s_NormalTexture = nullptr;

static Ref<Shader> s_SkyboxShader = nullptr;
static Ref<Shader> s_GeometryModelShader = nullptr;
static Ref<Shader> s_PlaneModelShader = nullptr;

static Ref<Pipeline> s_SkyboxPipeline = nullptr;
static Ref<Pipeline> s_GeometryModelPipeline = nullptr;
static Ref<Pipeline> s_PlaneModelPipeline = nullptr;

static glm::mat4 s_ModelMatGeom = glm::mat4(1.0f);
static glm::mat4 s_NormalMatGeom = glm::transpose(glm::inverse(s_ModelMatGeom));

static glm::mat4 s_ModelMatPlane = glm::scale(
	glm::translate(
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
		, glm::vec3(0.0f, 0.0f, -4.0f))
	, glm::vec3(10.0f));
static glm::mat4 s_NormalMatPlane = glm::transpose(glm::inverse(s_ModelMatPlane));

static float s_Time = 0.0f;
static bool s_Exploded = false;

static void ExplodeGeometry()
{
	if (Input::IsKeyDown(KeyCode::Space))
	{
		s_Exploded = true;
	}
	
	if (Input::IsKeyDown(KeyCode::R))
	{
		s_Exploded = false;
		s_Time = 0.0f;
	}

	if (s_Exploded)
		s_Time += Time::DeltaTime;
		
	s_GeometryModelShader->SetFloat("u_Time", s_Time);
}

void GeometryExplodingLayer::OnAttach()
{
	// Load Shaders
	s_SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.glsl");
	s_GeometryModelShader = CreateRef<Shader>("assets/shaders/BlinnPhongLightingGeometry.glsl");
	s_PlaneModelShader = CreateRef<Shader>("assets/shaders/BlinnPhongLighting.glsl");

	// Create Pipelines from device
	auto& pipelineState = PipelineState{};
	auto& polygonState = PipelinePolygonState{};
	polygonState.CullEnable = false;
	pipelineState.PolygonState = polygonState;
	s_GeometryModelPipeline = RenderDevice::CreatePipeline(pipelineState, s_GeometryModelShader);
	s_PlaneModelPipeline = RenderDevice::CreatePipeline(pipelineState, s_PlaneModelShader);

	// Create the skybox using the renderer functionality
	Renderer::LoadSkyboxPipeline(s_SkyboxShader);

	// Load models
	s_GeometryModel = CreateRef<Model>("assets/models/UVSphere.obj");
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
	s_GeometryMaterial = CreateRef<PhongMaterial>(s_GeometryModelShader, textures);
	DynamicCast<PhongMaterial>(s_GeometryMaterial)->SetNormalTexture(s_NormalTexture);

	std::array<glm::vec4, 3> planeColors = {
		glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // Diffuse
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Specular
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Emissive
	};
	s_PlaneMaterial = CreateRef<PhongMaterial>(s_PlaneModelShader, planeColors);

	// Create camera
	s_Camera = Camera::LookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f));

	// Create directional light
	s_DirLight = CreateRef<Light>(LightType::Directional, glm::vec3(1.0f, 0.95686f, 0.83922f), 1.0f);
	s_DirLight->SetDirection(glm::vec3(3.0f, -2.0f, 5.0f));

	// Setting shader uniforms
	s_GeometryModelShader->SetMat4("u_ViewProj", s_Camera->GetViewProjection());
	s_GeometryModelShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_GeometryModelShader->SetFloat("u_Force", 1.0f);

	s_GeometryModelShader->SetFloat4("u_DirLight.color", s_DirLight->GetLightingData());
	s_GeometryModelShader->SetFloat3("u_DirLight.direction", s_DirLight->GetDirection());
	s_PlaneModelShader->SetFloat4("u_DirLight.color", s_DirLight->GetLightingData());
	s_PlaneModelShader->SetFloat3("u_DirLight.direction", s_DirLight->GetDirection());
}

void GeometryExplodingLayer::OnDetach()
{
	s_SkyboxShader = nullptr;
	s_Camera = nullptr;

	RenderDevice::DestroyPipeline(s_GeometryModelPipeline);
	RenderDevice::DestroyPipeline(s_PlaneModelPipeline);
}

void GeometryExplodingLayer::OnUpdate()
{
	ExplodeGeometry();
	s_Camera->Update();

	s_GeometryModelPipeline->Bind();
	s_GeometryMaterial->Select();
	s_GeometryModelShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_GeometryModelShader->SetMat4("u_Model", s_ModelMatGeom);
	s_GeometryModelShader->SetMat4("u_ViewProj", s_Camera->GetViewProjection());
	s_GeometryModelShader->SetMat4("u_Normal", s_NormalMatGeom);
	s_GeometryModel->Render();

	s_PlaneModelPipeline->Bind();
	s_PlaneMaterial->Select();
	s_PlaneModelShader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_PlaneModelShader->SetMat4("u_Model", s_ModelMatPlane);
	s_PlaneModelShader->SetMat4("u_ModelView", s_Camera->GetView() * s_ModelMatPlane);
	s_PlaneModelShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMatPlane);
	s_PlaneModelShader->SetMat4("u_Normal", s_NormalMatPlane);
	s_PlaneModel->Render();

	Renderer::DrawSkybox(s_Camera);
}

void GeometryExplodingLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(GeometryExplodingLayer::OnWindowResize));
}

bool GeometryExplodingLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	float aspect_ratio = (float)e.GetWidth() / (float)e.GetHeight();
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), aspect_ratio, 0.3f, 50.0f));

	return false;
}
