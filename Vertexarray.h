#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
class Vertexarray
{
private:
	unsigned int m_RendererID;
public:
	Vertexarray();
	~Vertexarray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void UnBind() const;
};

