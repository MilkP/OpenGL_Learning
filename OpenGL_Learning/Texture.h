#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>

class Texture
{
public:
	Texture(GLenum textureSource, const char* texturePath)
	{
		textureTarget = textureSource;
		//Load texture
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

		//Bind texrtures
		glGenTextures(1, &texture);
		glBindTexture(textureTarget, texture);	
		
		if (data)
		{
			glTexImage2D(textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(textureTarget);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		//Set texture
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
		glBindTexture(textureTarget, 0);
	}

	void TextureBind(GLenum TextureUnit)
	{
		glActiveTexture(TextureUnit);
		glBindTexture(textureTarget, texture);
	}

private:
	unsigned int texture;
	GLenum textureTarget;
};


#endif