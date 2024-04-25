#include "MaterialsPhongLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<Camera> s_Camera = nullptr;
static Ref<Light> s_Light = nullptr;
static Ref<Model> s_SphereModel = nullptr;

static Ref<Shader> s_Shader = nullptr;
static Ref<Shader> s_PhongShader = nullptr;
static Ref<Shader> s_BlinnPhongShader = nullptr;

static Ref<Material> s_CubeMaterial = nullptr;
static Ref<Material> s_PlaneMaterial = nullptr;

static std::array<Ref<PhongMaterial>, 11> s_Materials;
static std::array<glm::mat4, 11> s_ModelMats;
static std::array<glm::mat4, 11> s_NormalMats;

static bool s_EnergyConserving = false;

static void SelectShading()
{
	if (Input::IsKeyDown(KeyCode::P))
	{
		s_Shader = s_PhongShader;
		for (const auto& material : s_Materials)
			material->SetShader(s_Shader);
	}

	if (Input::IsKeyDown(KeyCode::B))
	{
		s_Shader = s_BlinnPhongShader;
		for (const auto& material : s_Materials)
			material->SetShader(s_Shader);
	}

	if (Input::IsKeyDown(KeyCode::C))
	{
		s_EnergyConserving = !s_EnergyConserving;
		s_PhongShader->SetBool("u_EnergyConserving", s_EnergyConserving);
		s_BlinnPhongShader->SetBool("u_EnergyConserving", s_EnergyConserving);
	}
}

void MaterialsPhongLayer::OnAttach()
{
	// Load Shaders
	s_PhongShader = CreateRef<Shader>("assets/shaders/BasicLighting.glsl");
	s_BlinnPhongShader = CreateRef<Shader>("assets/shaders/BlinnPhongLighting.glsl");
	s_Shader = s_BlinnPhongShader; // We default to BlinnPhong shading

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, s_Shader);

	// Load model
	s_SphereModel = CreateRef<Model>("assets/models/UVSphere.obj");

	// Creating materials
	std::array<glm::vec4, 3> colors = {
		glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), // Diffuse
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // Specular
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Emissive
	};

	for (int i = 0; i < s_Materials.size(); i++)
	{
		s_Materials[i] = CreateRef<PhongMaterial>(s_Shader, colors);
		s_Materials[i]->SetGlossiness((float)i / 10.0f);
	}

	for (int i = 0; i < s_ModelMats.size(); i++)
	{
		s_ModelMats[i] = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f + 2.0f * (float)i, 0.0f, 0.0f));
		s_NormalMats[i] = glm::transpose(glm::inverse(s_ModelMats[i]));
	}

	// Create camera
	s_Camera = Camera::LookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f));

	// Create directional light
	s_Light = CreateRef<Light>(LightType::Directional, glm::vec3(1.0f, 0.95686f, 0.83922f), 1.0f);
	s_Light->SetDirection(glm::vec3(-1.0f, -2.0f, -3.0f));

	// Setting shader uniforms
	s_PhongShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection());
	s_PhongShader->SetFloat4("u_DirLight.color", s_Light->GetLightingData());
	s_PhongShader->SetFloat3("u_DirLight.direction", s_Light->GetDirection());
	s_PhongShader->SetFloat3("u_CameraPosition", -glm::vec3(s_Camera->GetView()[3]));
	s_PhongShader->SetBool("u_EnergyConserving", s_EnergyConserving);

	s_BlinnPhongShader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection());
	s_BlinnPhongShader->SetFloat4("u_DirLight.color", s_Light->GetLightingData());
	s_BlinnPhongShader->SetFloat3("u_DirLight.direction", s_Light->GetDirection());
	s_BlinnPhongShader->SetFloat3("u_CameraPosition", -glm::vec3(s_Camera->GetView()[3]));
	s_BlinnPhongShader->SetBool("u_EnergyConserving", s_EnergyConserving);

	// Setting texture bindings
	s_PhongShader->SetInt("u_DiffuseTexture", 0);
	s_PhongShader->SetInt("u_SpecularTexture", 1);
	s_PhongShader->SetInt("u_EmissiveTexture", 2);

	s_BlinnPhongShader->SetInt("u_DiffuseTexture", 0);
	s_BlinnPhongShader->SetInt("u_SpecularTexture", 1);
	s_BlinnPhongShader->SetInt("u_EmissiveTexture", 2);

	// Setting default clear value
	ClearValues clearValues{};
	clearValues.Color = { 0.01f, 0.01f, 0.01f, 1.0f };

	RenderCommand::SetDefaultState(pipelineState);
	RenderCommand::DefaultClearValues(clearValues);
}

void MaterialsPhongLayer::OnDetach()
{
	s_Shader = nullptr;
	s_SphereModel = nullptr;
	s_Camera = nullptr;

	RenderDevice::DestroyPipeline(s_Pipeline);
}

void MaterialsPhongLayer::OnUpdate()
{
	SelectShading();

	s_Camera->Update();
	s_Shader->Use();
	s_Shader->SetFloat3("u_CameraPosition", s_Camera->GetPosition());
	for (int i = 0; i < s_Materials.size(); i++)
	{
		s_Materials[i]->Select();
		s_Shader->SetMat4("u_Model", s_ModelMats[i]);
		s_Shader->SetMat4("u_Normal", s_NormalMats[i]);
		s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMats[i]);
		s_SphereModel->Render();
	}
}

void MaterialsPhongLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(MaterialsPhongLayer::OnWindowResize));
}

bool MaterialsPhongLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	float aspect_ratio = (float)e.GetWidth() / (float)e.GetHeight();
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), aspect_ratio, 0.3f, 50.0f));

	return false;
}
