#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;

	// for other things like
	// - set data
	// - lock - unlock mechanism
	// - stream data to vertex buffer
};