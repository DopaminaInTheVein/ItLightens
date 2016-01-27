#ifndef INC_RESOURCES_H_
#define INC_RESOURCES_H_



class IResource {

  uint32_t tag;

public:

  // ---------------------------------
  enum eType {
    UNDEFINED
    , MESH
    , SKIN_MESH
    , TEXTURE
    , SHADER
    , PATH
    , NUM_RESOURCES_TYPE
  };
  static const char* getTypeName(eType atype);

  virtual ~IResource() { }
  virtual const std::string& getName() const = 0;
  virtual bool isValid() const = 0;
  virtual void destroy() = 0;
  virtual bool reload() { return false; }
  virtual eType getType() const = 0;
  virtual void renderUIDebug() { }

  uint32_t getTag() const { return tag; }
  void setTag(uint32_t new_tag) { tag = new_tag; }

  template< typename TObj>
  const TObj* as() const {
    assert(getType() == getTypeOfResource<TObj>()
      || fatal( "Trying to convert instance of type %d to %d\n"
        , getType()
        , getTypeOfResource<TObj>())
      );
    return static_cast<const TObj*>(this);
  }

};

// ---------------------------------
template< class TObj >
IResource::eType getTypeOfResource();


#endif


