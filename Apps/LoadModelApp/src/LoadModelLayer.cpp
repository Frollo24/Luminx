#include "LoadModelLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<Shader> s_Shader = nullptr;
static Ref<Model> s_Model = nullptr;

void LoadModelLayer::OnAttach()
{
	// Load Shader
	s_Shader = CreateRef<Shader>("assets/shaders/Model.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, s_Shader);

	// Load model
	s_Model = CreateRef<Model>("assets/models/bunny.obj");

	glm::mat4 modelViewProj = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Shader->SetMat4("u_ModelViewProj", modelViewProj);

	RenderCommand::SetDefaultState(pipelineState);
}

void LoadModelLayer::OnDetach()
{
	s_Shader = nullptr;
	s_Model = nullptr;
}

void LoadModelLayer::OnUpdate()
{
	s_Shader->Use();
	s_Model->Render();
}
