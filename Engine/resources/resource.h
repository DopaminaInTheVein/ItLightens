#ifndef INC_RESOURCES_H_
#define INC_RESOURCES_H_



class IResource {
  std::string  name;
  uint32_t     tag;

public:

  // ---------------------------------
  enum eType {
    UNDEFINED
    , MESH
    , PIXEL_SHADER
    , VERTEX_SHADER
    , TECHNIQUE
    , TEXTURE
    , MATERIAL
    , STATIC_MESH
    , SKELETON
    , NUM_RESOURCES_TYPE
  };
  static const char* getTypeName(eType atype);

  virtual ~IResource() { }

  virtual bool isValid() const = 0;
  virtual void destroy() = 0;
  virtual bool reload() { return false; }
  virtual eType getType() const = 0;
  virtual void renderUIDebug() { }

  uint32_t getTag() const { return tag; }
  void setTag(uint32_t new_tag) { tag = new_tag; }

  void setName(const std::string& new_name) { name = new_name; }
  void setName(const char* new_name) { name = new_name; }
  const std::string& getName() const {
    return name;
  }

  template< typename TObj>
  const TObj* as() const {
    assert(getType() == getTypeOfResource<TObj>()
      || fatal( "Trying to convert instance of type %d to %d\n"
        , getType()
        , getTypeOfResource<TObj>())
      );
    return (const TObj*)(this);
  }

  static const char* getDataPath() {
    return "data/";
  }

};

// ---------------------------------
template< class TObj >
IResource::eType getTypeOfResource();


#endif


