#include "RenderHelper.h"
#include "Mesh.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "MatrixStack.h"
#include "DepthFBO\DepthFBO.h"
#include "GraphicsManager.h"
#include "MeshBuilder.h"

#define MAX_TEXTURES 8

void RenderHelper::RenderMesh(Mesh* _mesh)
{
	// Get all our transform matrices & update shader
	Mtx44 MVP;

	if (DepthFBO::GetInstance()->m_renderPass == DepthFBO::RENDER_PASS_PRE)
	{
		Mtx44 lightDepthMVP = DepthFBO::GetInstance()->m_lightDepthProj * DepthFBO::GetInstance()->m_lightDepthView * GraphicsManager::GetInstance()->GetModelStack().Top();
		GraphicsManager::GetInstance()->SetActiveShader("gpass");

		ShaderProgram* gpassShader = GraphicsManager::GetInstance()->GetActiveShader();
		gpassShader->UpdateMatrix44("lightDepthMVP", &lightDepthMVP.a[0]);

		for (int i = 0; i < MAX_TEXTURES; ++i)
		{
			if (_mesh->textureID[i] > 0)
			{
				std::string colourTextureEnabled = "colorTextureEnabled[";
				colourTextureEnabled += std::to_string(i);
				colourTextureEnabled += "]";
				gpassShader->UpdateInt(colourTextureEnabled, 1);

				GraphicsManager::GetInstance()->UpdateTexture(0, _mesh->textureID[i]);

				std::string colourTexture = "colorTexture[";
				colourTexture += std::to_string(i);
				colourTexture += "]";
				gpassShader->UpdateInt(colourTexture, 0);

			}
			else
			{
				std::string colourTextureEnabled = "colorTextureEnabled[";
				colourTextureEnabled += std::to_string(i);
				colourTextureEnabled += "]";
				gpassShader->UpdateInt(colourTextureEnabled, 0);
			}
		}
		_mesh->Render();

		GraphicsManager::GetInstance()->SetActiveShader("default");

		return;
	}
	GraphicsManager::GetInstance()->SetActiveShader("default");
	MVP = GraphicsManager::GetInstance()->GetProjectionMatrix() * GraphicsManager::GetInstance()->GetViewMatrix() * GraphicsManager::GetInstance()->GetModelStack().Top();
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();
	currProg->UpdateMatrix44("MVP", &MVP.a[0]);

	// Disable lighting stuff
	currProg->UpdateInt("lightEnabled", 0);

	for (int i = 0; i < MAX_TEXTURES; ++i)
	{
		if (_mesh->textureID[i] > 0)
		{
			std::string colourTextureEnabled = "colorTextureEnabled[";
			colourTextureEnabled += std::to_string(i);
			colourTextureEnabled += "]";
			currProg->UpdateInt(colourTextureEnabled, 1);

			GraphicsManager::GetInstance()->UpdateTexture(0, _mesh->textureID[i]);

			std::string colourTexture = "colorTexture[";
			colourTexture += std::to_string(i);
			colourTexture += "]";
			currProg->UpdateInt(colourTexture, 0);

		}
		else
		{
			std::string colourTextureEnabled = "colorTextureEnabled[";
			colourTextureEnabled += std::to_string(i);
			colourTextureEnabled += "]";
			currProg->UpdateInt(colourTextureEnabled, 0);
		}
	}

	// Do actual rendering
	_mesh->Render();

	// Unbind texture for safety (in case next render call uses it by accident)
	if (_mesh->textureID > 0)
	{
		GraphicsManager::GetInstance()->UnbindTexture(0);
	}
}

void RenderHelper::RenderMeshWithLight(Mesh* _mesh)
{
	// Get all our transform matrices & update shader
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	if (DepthFBO::GetInstance()->m_renderPass == DepthFBO::RENDER_PASS_PRE)
	{
		Mtx44 lightDepthMVP = DepthFBO::GetInstance()->m_lightDepthProj * DepthFBO::GetInstance()->m_lightDepthView * GraphicsManager::GetInstance()->GetModelStack().Top();
		GraphicsManager::GetInstance()->SetActiveShader("gpass");
		ShaderProgram* gpassShader = GraphicsManager::GetInstance()->GetActiveShader();
		gpassShader->UpdateMatrix44("lightDepthMVP", &lightDepthMVP.a[0]);

		for (int i = 0; i < MAX_TEXTURES; ++i)
		{
			if (_mesh->textureID[i] > 0)
			{
				std::string colourTextureEnabled = "colorTextureEnabled[";
				colourTextureEnabled += std::to_string(i);
				colourTextureEnabled += "]";
				gpassShader->UpdateInt(colourTextureEnabled, 1);

				GraphicsManager::GetInstance()->UpdateTexture(i, _mesh->textureID[i]);

				std::string colourTexture = "colorTexture[";
				colourTexture += std::to_string(i);
				colourTexture += "]";
				gpassShader->UpdateInt(colourTexture, i);

			}
			else
			{
				std::string colourTextureEnabled = "colorTextureEnabled[";
				colourTextureEnabled += std::to_string(i);
				colourTextureEnabled += "]";
				gpassShader->UpdateInt(colourTextureEnabled, 0);
			}
		}
		_mesh->Render();

		GraphicsManager::GetInstance()->SetActiveShader("default");

		return;
	}

	MVP = GraphicsManager::GetInstance()->GetProjectionMatrix() * GraphicsManager::GetInstance()->GetViewMatrix() * GraphicsManager::GetInstance()->GetModelStack().Top();
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();
	currProg->UpdateMatrix44("MVP", &MVP.a[0]);

	// Update light stuff
	currProg->UpdateInt("lightEnabled", 1);
	modelView = GraphicsManager::GetInstance()->GetViewMatrix() * GraphicsManager::GetInstance()->GetModelStack().Top();
	currProg->UpdateMatrix44("MV", &modelView.a[0]);
	modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
	currProg->UpdateMatrix44("MV_inverse_transpose", &modelView.a[0]);

	//load material
	currProg->UpdateVector3("material.kAmbient", &_mesh->material.kAmbient.r);
	currProg->UpdateVector3("material.kDiffuse", &_mesh->material.kDiffuse.r);
	currProg->UpdateVector3("material.kSpecular", &_mesh->material.kSpecular.r);
	currProg->UpdateFloat("material.kShininess", _mesh->material.kShininess);
	
	// Update textures first if available
	for (int i = 0; i < MAX_TEXTURES; ++i)
	{
		if (_mesh->textureID[i] > 0)
		{
			std::string colourTextureEnabled = "colorTextureEnabled[";
			colourTextureEnabled += std::to_string(i);
			colourTextureEnabled += "]";

			currProg->UpdateInt(colourTextureEnabled, 1);

			GraphicsManager::GetInstance()->UpdateTexture(i, _mesh->textureID[i]);

			std::string colourTexture = "colorTexture[";
			colourTexture += std::to_string(i);
			colourTexture += "]";
			currProg->UpdateInt(colourTexture, i);
		}
		else
		{
			std::string colourTextureEnabled = "colorTextureEnabled[";
			colourTextureEnabled += std::to_string(i);
			colourTextureEnabled += "]";
			currProg->UpdateInt(colourTextureEnabled, 0);
		}
	}
	//if (_mesh->textureID > 0)
	//{
	//	currProg->UpdateInt("colorTextureEnabled", 1);
	//	GraphicsManager::GetInstance()->UpdateTexture(0, _mesh->textureID);
	//	currProg->UpdateInt("colorTexture", 0);
	//}
	//else
	//{
	//	currProg->UpdateInt("colorTextureEnabled", 0);
	//}

	// Do actual rendering
	_mesh->Render();

	// Unbind texture for safety (in case next render call uses it by accident)
	if (_mesh->textureID > 0)
	{
		GraphicsManager::GetInstance()->UnbindTexture(0);
	}
}

void RenderHelper::RenderText(Mesh* _mesh, const std::string& _text, Color _color)
{
	// Trivial Rejection : Unable to render without mesh or texture
	if (!_mesh || _mesh->textureID <= 0)
		return;

	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();

	currProg->UpdateInt("textEnabled", 1);
	currProg->UpdateVector3("textColor", &_color.r);
	currProg->UpdateInt("lightEnabled", 0);
	currProg->UpdateInt("colorTextureEnabled", 1);
	
	currProg->UpdateInt("colorTextureEnabled", 1);
	//GraphicsManager::GetInstance()->UpdateTexture(0, _mesh->textureID);
	GraphicsManager::GetInstance()->UpdateTexture(0, _mesh->textureID[0]);
	currProg->UpdateInt("colorTexture", 0);

	for (unsigned i = 0; i < _text.length(); ++i)
	{
		Mtx44 characterSpacing, MVP;
		//characterSpacing.SetToTranslation((i+0.5f) * 1.0f, 0, 0); // 1.0f is the spacing of each character, you may change this value
		characterSpacing.SetToTranslation((float)(1 + (int)i), 0.0f, 0.0f); // 1.0f is the spacing of each character, you may change this value
		MVP = GraphicsManager::GetInstance()->GetProjectionMatrix() * GraphicsManager::GetInstance()->GetViewMatrix() * GraphicsManager::GetInstance()->GetModelStack().Top() * characterSpacing;
		currProg->UpdateMatrix44("MVP", &MVP.a[0]);

		_mesh->Render((unsigned)_text[i] * 6, 6);
	}

	GraphicsManager::GetInstance()->UnbindTexture(0);
	currProg->UpdateInt("textEnabled", 0);
}

void RenderHelper::DrawLine(Vector3 start, Vector3 end, Color color)
{
	MS& ms = GraphicsManager::GetInstance()->GetModelStack();

	Vector3 dir = end - start;
	Vector3 rotate_about(0, 0, 1);
	float angle = 0;
	try {
		rotate_about = Vector3(1, 0, 0).Cross(dir.Normalized()).Normalize();
		angle = Vector3(1, 0, 0).Dot(dir.Normalized());

		angle = Math::RadianToDegree(acos(angle));
	}
	catch (DivideByZero) {
		angle = 0;
	}
	ms.PushMatrix();
	ms.Translate(dir * 0.5f + start);
	ms.Rotate(45, rotate_about);
	ms.Scale(dir.Length());
	RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("redline"));
	ms.PopMatrix();
	
}
