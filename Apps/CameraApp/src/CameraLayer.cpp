#include "CameraLayer.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace Luminx;

static Ref<Pipeline> s_Pipeline = nullptr;
static Ref<Shader> s_Shader = nullptr;
static Ref<Model> s_Model = nullptr;
static Ref<Texture> s_Texture = nullptr;
static Ref<Camera> s_Camera = nullptr;

static glm::mat4 s_ModelMat1 = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, 0.0f));
static glm::mat4 s_ModelMat2 = glm::translate(glm::mat4(1.0f), glm::vec3(-2.5f, 0.0f, 0.0f));

void CameraLayer::OnAttach()
{
	// Load Shader
	s_Shader = CreateRef<Shader>("assets/shaders/TextureModel.glsl");

	// Create Pipeline from device
	auto& pipelineState = PipelineState{};
	pipelineState.BlendState.BlendEnable = false;
	pipelineState.PolygonState.FrontFace = FrontFaceMode::CounterClockwise;
	s_Pipeline = RenderDevice::CreatePipeline(pipelineState, s_Shader);

	// Load model
	s_Model = CreateRef<Model>("assets/models/cube.obj");

	// Load image data
	i32 imageWidth;
	i32 imageHeight;
	i32 imageChannels;

	void* imageData = Utils::LoadImageFromDisk("assets/textures/texture-brick.png", imageWidth, imageHeight, imageChannels);

	// Create texture and set texture data
	TextureDescription textureDesc{};
	textureDesc.ImageExtent = { (u32)imageWidth, (u32)imageHeight, 1 };
	textureDesc.ImageFormat = ImageFormat::RGBA8;

	s_Texture = RenderDevice::CreateTexture(textureDesc);
	s_Texture->SetData(imageData);
	s_Texture->BindTextureUnit(0);
	Utils::FreeImageData(imageData);

	// Create camera
	s_Camera = Camera::LookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.3f, 50.0f));

	s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection());

	RenderCommand::SetDefaultState(pipelineState);
}

void CameraLayer::OnDetach()
{
	s_Shader = nullptr;
	s_Model = nullptr;
	s_Camera = nullptr;

	RenderDevice::DestroyTexture(s_Texture);
	RenderDevice::DestroyPipeline(s_Pipeline);
}

void CameraLayer::OnUpdate()
{
	s_Camera->Update();
	s_Shader->Use();
	s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMat1);
	s_Model->Render();
	s_Shader->SetMat4("u_ModelViewProj", s_Camera->GetViewProjection() * s_ModelMat2);
	s_Model->Render();
}

void CameraLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(CameraLayer::OnWindowResize));
}

bool CameraLayer::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		return false;

	float aspect_ratio = (float)e.GetWidth() / (float)e.GetHeight();
	s_Camera->SetProjection(glm::perspective(glm::radians(60.0f), aspect_ratio, 0.3f, 50.0f));

	return false;
}
