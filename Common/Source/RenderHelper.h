#ifndef RENDER_HELPER_H
#define RENDER_HELPER_H

#include "Vertex.h"
#include <vector>
#include <utility>
class Mesh;

class RenderHelper
{
public:
	static void RenderMesh(Mesh* _mesh);
	static void RenderMeshWithLight(Mesh* _mesh);
	static void RenderText(Mesh* _mesh, const std::string& _text, Color _color);
	static void DrawLine(Vector3 start, Vector3 end, Color color);
	static void DrawLine(std::vector<std::pair<Vector3, Vector3>>& lines, Color color);
	static void DrawLine(std::vector<Vector3> vertices, Color color, int numPerSet = 2);
};

#endif // RENDER_HELPER_H