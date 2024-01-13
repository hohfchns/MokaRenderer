#include "moka/assets/raw_image.h"
#include "moka/assets/resource_dir.h"
#include <iostream>
#include <string>
#include <string.h>

// TODO ADD PROPER PROJECT DIRECTORY PATH

namespace moka::assets
{

RawImage::RawImage()
{
}

RawImage::RawImage(std::string&& path, bool absolute)
{
  Load(std::move(path), absolute);
}

RawImage::RawImage(const std::string& path, bool absolute)
{
  Load(path, absolute);
}

bool RawImage::Load(std::string&& path, bool absolute)
{
  path = relativePath(path);

  _data = stbi_load(path.c_str(), &_width, &_height, &_channels, 0);

  if (!_data)
  {
    std::cout << "ERROR::ASSETS::STB::LOAD | Could not load image from path '" << path << "'!" << std::endl;
    _data = nullptr;
    return false;
  }

  return true;
}

bool RawImage::Load(const std::string& path, bool absolute)
{
  std::string newPath = relativePath(path);

  _data = stbi_load(newPath.c_str(), &_width, &_height, &_channels, 0);

  if (!_data)
  {
    std::cout << "ERROR::ASSETS::STB::LOAD | Could not load image from path '" << newPath << "'!" << std::endl;
    _data = nullptr;
    return false;
  }

  return true;
}

int RawImage::Width() const
{
  return _width;
}

int RawImage::Height() const
{
  return _height;
}

int RawImage::Channels() const
{
  return _channels;
}

const unsigned char* RawImage::Data() const
{
  return _data;
}

void RawImage::Unload()
{
  if (_data != nullptr)
  {
    stbi_image_free(_data);
  }

  _data = nullptr;
}

RawImage::~RawImage()
{
  // Unload();
}

}
