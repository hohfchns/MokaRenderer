#pragma once
#include "moka/assets/raw_image.h"

namespace moka::renderer::wrappers
{
  enum class TextureType
  {
    DIFFUSE = 0,
    SPECULAR
  };

  static const char* TextureTypeStr[]
  {
    // TextureType.DIFFUSE
    "texture_diffuse",
    // TextureType.SPECULAR
    "texture_specular"
  };

  struct TextureParams
  {
    // Initialized in definition to avoid including opengl here
    int wrap_s = 0;
    int wrap_t = 0;
    int min_filter = 0;
    int mag_filter = 0;
  };

  class Texture2D
  {
    public:
      Texture2D();
      Texture2D(TextureParams& texParams);
      Texture2D(const std::string& path);
      Texture2D(const std::string& path, TextureParams& texParams);
      bool Load(const std::string& path);

      void Gen();
      void Bind() const;

      const moka::assets::RawImage& Img() const;
      const unsigned int Id() const;

      int Width() const;
      int Height() const;
      int Channels() const;
      const unsigned char* Data() const;

      void Unload();
      ~Texture2D();

      TextureType type;
    private:
      unsigned int _id = 0;
      moka::assets::RawImage _img; 
      TextureParams _params;
  };
}
