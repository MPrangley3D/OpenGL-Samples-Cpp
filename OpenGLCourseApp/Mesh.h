#pragma once
#include <GL/glew.h>


class Mesh
{
public:
	Mesh();
	void CreateMesh(GLfloat *Verticies, unsigned int *Indicies, unsigned int NumOfVerticies, unsigned int NumOfIndicies);
	void RenderMesh();
	void ClearMesh();
	~Mesh();
private:
	GLuint VAO;
	GLuint VBO;
	GLuint IBO;
	GLsizei IndexCount;

};