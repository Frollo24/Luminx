#include "BasicLightingLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<Shader> s_Shader = nullptr;
static Ref<Model> s_Model = nullptr;
static Ref<Camera> s_Camera = nullptr;
static Ref<Light> s_Light = nullptr;
static Ref<Material> s_Material = nullptr;

static Ref<Texture> s_DiffuseTexture = nullptr;
static Ref<Texture> s_DiffuseNoEmissiveTexture = nullptr;
static Ref<Texture> s_SpecularTexture = nullptr;
static Ref<Texture> s_EmissiveTexture = nullptr;

static glm::mat4 s_ModelMat1 = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, 0.0f));
static glm::mat4 s_ModelMat2 = glm::translate(glm::mat4(1.0f), glm::vec3(-2.5f, 0.0f, 0.0f));

void BasicLightingLayer::OnAttach()
{
	// Load Shader
	s_Shader = CreateRef<Shader>("assets/shaders/BasicLighting.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, s_Shader);

	// Load model
	s_Model = CreateRef<Model>("assets/models/CubeWall.obj");

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

	// Create material and selecting it
	std::array<Ref<Texture>, 3> textures = {
		s_DiffuseTexture,
		s_SpecularTexture,
		s_EmissiveTexture
	};
	s_Material = CreateRef<PhongMaterial>(s_Shader, textures);
	s_Material->Select();

	// Create camera
	s_Camera = Camera::LookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f));

	// Create directional light
	s_Light = CreateRef<Light>(LightType::Directional, glm::vec3(1.0f, 0.95686f, 0.83922f), 1.0f);
	s_Light->SetDirection(glm::vec3(-1.0f, -2.0f, -3.0f));

	// Setting shader uniforms
	s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection());
	s_Shader->SetFloat4("u_DirLight.color", s_Light->GetLightingData());
	s_Shader->SetFloat3("u_DirLight.direction", s_Light->GetDirection());
	s_Shader->SetFloat3("u_CameraPosition", -glm::vec3(s_Camera->GetView()[3]));

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

void BasicLightingLayer::OnDetach()
{
	s_Shader = nullptr;
	s_Model = nullptr;
	s_Camera = nullptr;

	RenderDevice::DestroyTexture(s_DiffuseTexture);
	RenderDevice::DestroyPipeline(s_Pipeline);
}

void BasicLightingLayer::OnUpdate()
{
	s_Camera->Update();
	s_Shader->Use();
	s_Shader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	s_Shader->SetMat4("u_Model", s_ModelMat1);
	s_Shader->SetMat4("u_Normal", glm::transpose(glm::inverse(s_ModelMat1)));
	s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMat1);
	s_Model->Render();
	s_Shader->SetMat4("u_Model", s_ModelMat2);
	s_Shader->SetMat4("u_Normal", glm::transpose(glm::inverse(s_ModelMat2)));
	s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMat2);
	s_Model->Render();
}

void BasicLightingLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(BasicLightingLayer::OnWindowResize));
}

bool BasicLightingLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	float aspect_ratio = (float)e.GetWidth() / (float)e.GetHeight();
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), aspect_ratio, 0.3f, 50.0f));

	return false;
}
