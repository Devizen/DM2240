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
		//characterSpacing.SetToTranslation((float)(1 + (int)i), 0.0f, 0.0f); // 1.0f is the spacing of each character, you may change this value
		characterSpacing.SetToTranslation((float)(0.5f + (int)i), 0.0f, 0.0f); // 1.0f is the spacing of each character, you may change this value
		MVP = GraphicsManager::GetInstance()->GetProjectionMatrix() * GraphicsManager::GetInstance()->GetViewMatrix() * GraphicsManager::GetInstance()->GetModelStack().Top() * characterSpacing;
		currProg->UpdateMatrix44("MVP", &MVP.a[0]);

		_mesh->Render((unsigned)_text[i] * 6, 6);
	}

	GraphicsManager::GetInstance()->UnbindTexture(0);
	currProg->UpdateInt("textEnabled", 0);
}

//void RenderHelper::DrawLine(Vector3 start, Vector3 end, Color color)
//{
//	Vertex v;
//	std::vector<Vertex> vertex_buffer_data;
//	v.pos.Set(start.x, start.y, start.z);
//	v.color = color;
//	vertex_buffer_data.push_back(v);
//	v.pos.Set(end.x, end.y, end.z);
//	v.color = color;
//	vertex_buffer_data.push_back(v);
//
//	std::vector<GLuint> index_buffer_data;
//	index_buffer_data.push_back(0);
//	index_buffer_data.push_back(1);
//
//	Mesh *mesh = new Mesh("line");
//
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
//	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
//
//	mesh->indexSize = index_buffer_data.size();
//	mesh->mode = Mesh::DRAW_LINES;
//
//	RenderMesh(mesh);
//
//	delete mesh;
//	
//}

void RenderHelper::DrawLine(Vector3 start, Vector3 end, Color color)
{
	Vector3 dir = -start + end;
	float length = dir.Length();
	dir.Normalize();
	Vector3 rotateAxis;
	try {
		rotateAxis = (Vector3(1, 0, 0).Cross(dir).Normalize());
	}
	catch (DivideByZero)
	{
		rotateAxis.Set(0, 0, 1);
	}
	float angle = Math::RadianToDegree(acos(Vector3(1, 0, 0).Dot(dir)));

	Mesh* line = MeshBuilder::GetInstance()->GetMesh("redline");

	MS& ms = GraphicsManager::GetInstance()->GetModelStack();
	ms.PushMatrix();
	ms.Translate(start + (dir * length * 0.5f));
	ms.Rotate(angle, rotateAxis);
	ms.Scale(length);
	RenderMesh(line);
	ms.PopMatrix();

	//Vertex v;
	//std::vector<Vertex> vertex_buffer_data;
	//v.pos.Set(start.x, start.y, start.z);
	//v.color = color;
	//vertex_buffer_data.push_back(v);
	//v.pos.Set(end.x, end.y, end.z);
	//v.color = color;
	//vertex_buffer_data.push_back(v);

	//std::vector<GLuint> index_buffer_data;
	//index_buffer_data.push_back(0);
	//index_buffer_data.push_back(1);

	//Mesh *mesh = new Mesh("line");

	//glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	//glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	//mesh->indexSize = index_buffer_data.size();
	//mesh->mode = Mesh::DRAW_LINES;

	//RenderMesh(mesh);

	//delete mesh;

}

void RenderHelper::DrawLine(std::vector<std::pair<Vector3, Vector3>>& lines, Color color)
{
	if (lines.size() == 0)
		return;

	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	int index = 0;
	for (size_t i = 0; i < lines.size(); ++i)
	{
		v.pos.Set(lines[i].first.x, lines[i].first.y, lines[i].first.z);
		v.color = color;
		vertex_buffer_data.push_back(v);
		v.pos.Set(lines[i].second.x, lines[i].second.y, lines[i].second.z);
		v.color = color;
		vertex_buffer_data.push_back(v);

		index_buffer_data.push_back(index++);
		index_buffer_data.push_back(index++);
	}
	
	Mesh *mesh = new Mesh("line");

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_LINES;

	RenderMesh(mesh);

	delete mesh;
}
