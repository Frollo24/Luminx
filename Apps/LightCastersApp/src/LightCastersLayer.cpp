#include "LightCastersLayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<Shader> s_Shader = nullptr;
static Ref<Camera> s_Camera = nullptr;

static Ref<Light> s_DirLight = nullptr;
static Ref<Light> s_PointLight = nullptr;
static Ref<Light> s_SpotLight = nullptr;

static Ref<Material> s_CubeMaterial = nullptr;
static Ref<Material> s_LightMaterial = nullptr;

static Ref<Model> s_CubeModel = nullptr;
static Ref<Model> s_LightModel = nullptr;

static Ref<Material> s_Material = nullptr;

static Ref<Texture> s_DiffuseTexture = nullptr;
static Ref<Texture> s_DiffuseNoEmissiveTexture = nullptr;
static Ref<Texture> s_SpecularTexture = nullptr;
static Ref<Texture> s_EmissiveTexture = nullptr;

static glm::mat4 s_ModelMatLight = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 3.0f));
static glm::mat4 s_NormalMatLight = glm::transpose(glm::inverse(s_ModelMatLight));;

static std::array<glm::mat4, 11> s_ModelMats;
static std::array<glm::mat4, 11> s_NormalMats;

void LightCastersLayer::OnAttach()
{
	// Load Shader
	s_Shader = CreateRef<Shader>("assets/shaders/MultipleLights.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, s_Shader);

	// Load models
	s_CubeModel = CreateRef<Model>("assets/models/CubeWall.obj");
	s_LightModel = CreateRef<Model>("assets/models/UVSphere.obj");

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

	// Creating materials
	std::array<glm::vec4, 3> colors = {
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Diffuse
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Specular
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), // Emissive
	};
	std::array<glm::vec4, 3> tcolors = {
		glm::vec4(0.7f, 0.0f, 0.0f, 1.0f), // Diffuse
		glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), // Specular
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Emissive
	};
	std::array<Ref<Texture>, 3> textures = {
		s_DiffuseTexture,
		s_SpecularTexture,
		s_EmissiveTexture
	};

	s_CubeMaterial = CreateRef<PhongMaterial>(s_Shader, textures);
	s_LightMaterial = CreateRef<PhongMaterial>(s_Shader, colors);

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
	s_DirLight = CreateRef<Light>(LightType::Directional, glm::vec3(1.0f, 0.95686f, 0.83922f), 0.25f);
	s_DirLight->SetDirection(glm::vec3(-1.0f, -2.0f, -3.0f));

	s_PointLight = CreateRef<Light>(LightType::Point, glm::vec3(0.0f, 1.0f, 1.0f), 10.0f);
	s_PointLight->SetPosition(glm::vec3(5.0f, 0.0f, 3.0f));

	s_SpotLight = CreateRef<Light>(LightType::Spot, glm::vec3(1.0f, 0.95686f, 0.0f), 4.0f);
	s_SpotLight->SetDirection(glm::vec3(0.0f, 0.0f, -1.0f));

	// Setting shader uniforms
	s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection());
	s_Shader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());

	s_Shader->SetFloat4("u_DirLight.color", s_DirLight->GetLightingData());
	s_Shader->SetFloat3("u_DirLight.direction", s_DirLight->GetDirection());

	s_Shader->SetFloat4("u_PointLight.color", s_PointLight->GetLightingData());
	s_Shader->SetFloat3("u_PointLight.position", s_PointLight->GetPosition());

	s_Shader->SetFloat4("u_SpotLight.color", s_SpotLight->GetLightingData());
	s_Shader->SetFloat3("u_SpotLight.direction", s_SpotLight->GetDirection());
	s_Shader->SetFloat3("u_SpotLight.position", s_SpotLight->GetPosition());
	s_Shader->SetFloat("u_SpotLight.innerCutoff", glm::cos(glm::radians(s_SpotLight->GetPhi())));
	s_Shader->SetFloat("u_SpotLight.outerCutoff", glm::cos(glm::radians(s_SpotLight->GetGamma())));

	// Setting texture bindings
	s_Shader->SetInt("u_DiffuseTexture", 0);
	s_Shader->SetInt("u_SpecularTexture", 1);
	s_Shader->SetInt("u_EmissiveTexture", 2);

	// Setting default clear value
	ClearValues clearValues{};
	clearValues.Color = { 0.01f, 0.01f, 0.01f, 1.0f };

	RenderCommand::SetDefaultState(pipelineState);
	RenderCommand::DefaultClearValues(clearValues);
}

void LightCastersLayer::OnDetach()
{
	s_Shader = nullptr;
	s_CubeModel = nullptr;
	s_Camera = nullptr;

	RenderDevice::DestroyTexture(s_DiffuseTexture);
	RenderDevice::DestroyTexture(s_SpecularTexture);
	RenderDevice::DestroyTexture(s_EmissiveTexture);
	RenderDevice::DestroyPipeline(s_Pipeline);
}

void LightCastersLayer::OnUpdate()
{
	s_Camera->Update();
	s_Shader->Use();
	s_CubeMaterial->Select();
	s_Shader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());

	// We want the spot light following the camera
	s_Shader->SetFloat3("u_SpotLight.direction", glm::vec3(glm::inverse(s_Camera->GetView()) * glm::vec4(s_SpotLight->GetDirection(), 0.0f)));
	s_Shader->SetFloat3("u_SpotLight.position", glm::vec3(glm::inverse(s_Camera->GetView()) * glm::vec4(s_SpotLight->GetPosition(), 1.0f)));

	for (int i = 0; i < s_ModelMats.size(); i++)
	{
		s_Shader->SetMat4("u_Model", s_ModelMats[i]);
		s_Shader->SetMat4("u_Normal", s_NormalMats[i]);
		s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMats[i]);
		s_CubeModel->Render();
	}
	s_LightMaterial->Select();
	s_Shader->SetMat4("u_Model", s_ModelMatLight);
	s_Shader->SetMat4("u_Normal", s_NormalMatLight);
	s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMatLight);
	s_LightModel->Render();
}

void LightCastersLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(LightCastersLayer::OnWindowResize));
}

bool LightCastersLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	float aspect_ratio = (float)e.GetWidth() / (float)e.GetHeight();
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), aspect_ratio, 0.3f, 50.0f));

	return false;
}
