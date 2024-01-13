#pragma once
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb/stb_image.h"
#endif
#include "moka/assets/resource_dir.h"
#include <string>

namespace moka::assets
{

class RawImage
{
public:
  RawImage();
  RawImage(const std::string& path, bool absolute=false);
  RawImage(std::string&& path, bool absolute=false);

  bool Load(const std::string& path, bool absolute=false);
  bool Load(std::string&& path, bool absolute=false);

  int Width() const;
  int Height() const;
  int Channels() const;
  const unsigned char* Data() const;

  void Unload();
  ~RawImage();

private:
  int _width, _height, _channels;
  unsigned char* _data = nullptr;
};

}
