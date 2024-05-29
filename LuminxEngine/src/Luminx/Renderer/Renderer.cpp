#include "lumpch.h"
#include "Renderer.h"

#include "RenderDevice.h"
#include "RenderCommand.h"
#include "SkyboxVertices.h"

namespace Luminx
{
	struct RendererData
	{
		Ref<Pipeline> SkyboxPipeline = nullptr;
		Ref<Shader> SkyboxShader = nullptr;
		Ref<VertexArray> SkyboxVertexArray = nullptr;
	};

	static Scope<RendererData> s_Data = nullptr;

	namespace RendererUtils
	{
		static void InitRendererData()
		{
			s_Data = CreateScope<RendererData>();

			// Create vertex array and vertex buffer for the skybox
			s_Data->SkyboxVertexArray = CreateRef<VertexArray>();

			BufferDescription vertexBufferDesc{};
			vertexBufferDesc.Type = BufferType::Vertex;
			vertexBufferDesc.Size = sizeof(g_SkyboxVertices);
			vertexBufferDesc.Data = g_SkyboxVertices;
			Ref<Buffer> vertexBuffer = RenderDevice::CreateBuffer(vertexBufferDesc);

			BufferLayout skyboxBufferLayout = {
				{ ShaderDataType::Float3, "a_Position" },
			};
			s_Data->SkyboxVertexArray->AddVertexBuffer(vertexBuffer, skyboxBufferLayout);
		}

		static void DeleteRendererData()
		{
			s_Data->SkyboxVertexArray = nullptr;
			s_Data->SkyboxShader = nullptr;
			s_Data->SkyboxPipeline = nullptr;
			s_Data = nullptr;
		}
	}

	void Renderer::Init()
	{
		RenderCommand::Init();
		RendererUtils::InitRendererData();
	}

	void Renderer::Shutdown()
	{
		RendererUtils::DeleteRendererData();
		RenderCommand::Shutdown();
	}

	void Renderer::LoadSkyboxPipeline(const Ref<Shader>& skyboxShader)
	{
		// Select skybox shader
		s_Data->SkyboxShader = skyboxShader;

		// Update pipeline shader if pipeline is already created
		if (s_Data->SkyboxPipeline)
		{
			PipelineState skyboxPipelineState = s_Data->SkyboxPipeline->GetPipelineState();
			s_Data->SkyboxPipeline = RenderDevice::CreatePipeline(skyboxPipelineState, s_Data->SkyboxShader);
			return;
		}

		// Create skybox pipeline based on skybox shader if we haven't created it yet
		auto& pipelineState = PipelineState{};
		auto& depthState = PipelineDepthState{};
		depthState.DepthFunc = DepthComparison::LessOrEqual;
		depthState.DepthWrite = false;
		pipelineState.DepthState = depthState;
		s_Data->SkyboxPipeline = RenderDevice::CreatePipeline(pipelineState, s_Data->SkyboxShader);
	}

	void Renderer::BeginFrame()
	{
		RenderCommand::BeginFrame();
	}

	void Renderer::EndFrame()
	{
		RenderCommand::EndFrame();
	}

	void Renderer::Submit(const Ref<RenderEntity>& renderEntity)
	{
		RenderCommand::BindVertexArray(renderEntity->GetVertexArray());
		if (renderEntity->GetIndexCount())
			RenderCommand::DrawIndices(renderEntity->GetIndexCount(), renderEntity->GetVertexArray()->GetIndexType());
		else
			RenderCommand::DrawVertices(renderEntity->GetVertexCount());
	}

	void Renderer::DrawSkybox(const Ref<Camera>& camera)
	{
		s_Data->SkyboxPipeline->Bind();
		glm::mat4 skyboxViewProj = camera->GetProjection() * glm::mat4(glm::mat3(camera->GetView()));
		s_Data->SkyboxShader->SetMat4("u_ViewProj", skyboxViewProj);
		RenderCommand::BindVertexArray(s_Data->SkyboxVertexArray);
		RenderCommand::DrawVertices(36);
	}
}