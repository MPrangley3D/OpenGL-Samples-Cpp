#pragma once
#include "ShadowMap.h"

class OmniShadowMap : public ShadowMap
{
public:
	OmniShadowMap();

	bool Initialize(unsigned int Width, unsigned int Height);
	void Write();
	void Read(GLenum TextureUnit);

	~OmniShadowMap();

private:
};

