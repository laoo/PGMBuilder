#pragma once

struct RawROM;

class Image
{
public:
  virtual ~Image();

  virtual void addROM( std::shared_ptr<RawROM> rom ) = 0;

  static std::shared_ptr<Image> fromTemplate( std::string const& tpl );
  static std::shared_ptr<Image> custom();
};
