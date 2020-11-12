#include "GLTexture.h"

GLTexture::GLTexture()
  : tex(0), textureMode(GL_MODULATE), filterModeMag(GL_LINEAR), filterModeMin(GL_LINEAR), wrapModeS(GL_CLAMP), wrapModeT(GL_CLAMP)
{
}

GLTexture::~GLTexture()
{
    this->Release();
}

void GLTexture::Params(int textureMode, int filterModeMag, int filterModeMin, int wrapModeS, int wrapModeT)
{
    this->textureMode = textureMode;
    this->filterModeMag = filterModeMag;
    this->filterModeMin = filterModeMin;
    this->wrapModeS = wrapModeS;
    this->wrapModeT = wrapModeT;
}

bool GLTexture::Set(const unsigned char* pixels, int width, int height, int size, GLenum format)
{
    if (!pixels || width <= 0 || height <= 0 || size <= 0)
    {
        return false;
    }

    if (!this->tex)
    {
        glGenTextures(1, &this->tex);

        if (!this->tex)
        {
            return false;
        }
    }

    glBindTexture(GL_TEXTURE_2D, this->tex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, this->textureMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapModeS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapModeT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterModeMag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterModeMin);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void GLTexture::Clear()
{
    if (this->tex)
    {
        glDeleteTextures(1, &this->tex);
        this->tex = 0;
    }
}

void GLTexture::Apply()
{
    if (this->tex)
    {
        glBindTexture(GL_TEXTURE_2D, this->tex);
        glEnable(GL_TEXTURE_2D);
        //glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void GLTexture::Revoke()
{
    if (this->tex)
    {
        //glBindTexture(GL_TEXTURE_2D, this->tex);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void GLTexture::Release()
{
    this->Clear();
}

GLTexture::operator bool() const
{
    return this->tex ? true : false;
}