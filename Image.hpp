#pragma once

struct RawROM;

class Image
{
public:
  virtual ~Image();

  virtual void addROM( std::shared_ptr<RawROM> rom ) = 0;
  virtual bool isComplete() const = 0;
  virtual void build( std::filesystem::path const& out ) const = 0;

  static std::shared_ptr<Image> fromTemplate( std::string const& tpl );
  static std::shared_ptr<Image> custom();
};
