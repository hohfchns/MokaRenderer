#include "moka/renderer/wrappers/texture.h"
#include "moka/renderer/renderer.h"

namespace moka::renderer::wrappers
{
  void Texture2D::Gen()
  {
    glGenTextures(1, &_id);
  }

  void Texture2D::Bind() const
  {
    glBindTexture(GL_TEXTURE_2D, _id);
  }

  Texture2D::Texture2D()
  {
  }

  Texture2D::Texture2D(const std::string& path)
  {
    Load(path);
  }

  Texture2D::Texture2D(TextureParams& texParams)
  {
    _params = std::move(texParams);
  }

  Texture2D::Texture2D(const std::string& path, TextureParams& texParams)
  {
    _params = std::move(texParams);
    Load(path);
  }

  bool Texture2D::Load(const std::string& path)
  {
    bool success = _img.Load(path);
    if (!success)
    {
      return false;
    }

    if (_id == 0)
    {
      Gen();
    }
    
    Bind();

    if (_params.wrap_s == 0)
    {
      _params.wrap_s = GL_REPEAT;
    }
    if (_params.wrap_t == 0)
    {
      _params.wrap_t = GL_REPEAT;
    }
    if (_params.min_filter == 0)
    {
      _params.min_filter = GL_LINEAR;
    }
    if (_params.mag_filter == 0)
    {
      _params.mag_filter = GL_LINEAR;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _params.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _params.wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _params.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _params.mag_filter);

    if (Channels() == 4)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width(), Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Data());
    }
    else
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width(), Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, Data());
    }
    
    glGenerateMipmap(GL_TEXTURE_2D);

    Unload();

    return true;
  }

  const moka::assets::RawImage& Texture2D::Img() const
  {
    return _img;
  }

  const unsigned int Texture2D::Id() const
  {
    return _id;
  }

  int Texture2D::Width() const
  {
    return _img.Width();
  }

  int Texture2D::Height() const
  {
    return _img.Height();
  }

  int Texture2D::Channels() const
  {
    return _img.Channels();
  }

  const unsigned char* Texture2D::Data() const
  {
    return _img.Data();
  }
  
  void Texture2D::Unload()
  {
    _img.Unload();
  }

  Texture2D::~Texture2D()
  {
    // Unload();
  }
}

