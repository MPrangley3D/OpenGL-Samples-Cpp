#include "Mesh.h"

Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	IndexCount = 0;
}

/*  NOTES ON VERTEX SPECIFICATON
the critical OpenGL call in the code sample you give is actually glVertexAttribPointer:
When you call glVertexAttribPointer, what it does is interpret the parameters are relative to the currently bound buffer,
and save off the state to the currently bound VAO. From this you can easily infer that it doesn’t actually matter what
order the buffer and VAO are bound in; what matters is that they are bound when glVertexAttribPointer is called.

There is of course an exception: binding the GL_ELEMENT_ARRAY_BUFFER is actually different, and the current GL_ELEMENT_ARRAY_BUFFER
is saved to the current VAO at the time it is bound. In that case, the VAO must be bound first.

For this reason, that VAO should be unbound BEFORE the IBO is unbound, to ensure no null references.
*/
void Mesh::CreateMesh(GLfloat* Verticies, unsigned int* Indicies, unsigned int NumOfVerticies, unsigned int NumOfIndicies)
{
	IndexCount = NumOfIndicies;

    // "VERTEX SPECIFICATION"
    // 1. Generate Vertex Array Object ID
    glGenVertexArrays(1, &VAO);
    // 2. Bind VAO to the ID
    glBindVertexArray(VAO);
    // Create Index Buffer ID for verticies
    glGenBuffers(1, &IBO);
    // Bind index buffer io ID
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    // Attach vertex data to the bound IBO
    // Due to the VAO, this IBO and the VBO below are automatically related
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indicies[0]) * NumOfIndicies, Indicies, GL_STATIC_DRAW);
    // 3. Generate VBO ID
    glGenBuffers(1, &VBO);
    // 4. Bind VBO to ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 5. Attach vertex data to the bound VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verticies) * NumOfVerticies, Verticies, GL_STATIC_DRAW);
    // 6. Define Attribute Pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // 7. Enable the Attribute Pointer
    glEnableVertexAttribArray(0);
    // 8. Unbind the VAO, VBO, and IBO
    // IMPORTANT:  VAO should be unbound FIRST, then IBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Unbind IBO *AFTER* VBO 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::RenderMesh()
{
    // Bind the VAO
    glBindVertexArray(VAO);

    // Bind the Element Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Draw the Elements
    glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);

    // Unbind the Element Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::ClearMesh()
{
    if (IBO != 0)
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    IndexCount = 0;
}

Mesh::~Mesh()
{
    ClearMesh();
}