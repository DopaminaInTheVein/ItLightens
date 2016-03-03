#ifndef INC_HANDLE_MANAGER_H_
#define INC_HANDLE_MANAGER_H_

#include "handle.h"
#include <new>

// -----------------------------------
class CHandleManager {

protected:

  static const uint32_t max_total_objects_allowed = 1 << CHandle::num_bits_index;
  static const uint32_t invalid_index = ~0;   // all ones

  // Se accede por external_id via []
  struct TExternalData {
    uint32_t internal_index;
    uint32_t current_age : CHandle::num_bits_age;
    uint32_t next_external_index;
    CHandle  current_owner;
    TExternalData()
      : internal_index(0)
      , current_age(0)
      , next_external_index(0)
    { }
  };

  // El tipo de objetos que este handle maneja
  uint32_t        type;

  // Accessed using the external_index, gives info in the internal_table
  TExternalData*  external_to_internal;
  uint32_t*       internal_to_external;

  // Internal array of objects
  uint32_t        num_objs_used;
  uint32_t        num_objs_capacity;

  uint32_t        next_free_handle_ext_index;
  uint32_t        last_free_handle_ext_index;

  std::vector< CHandle > objs_to_destroy;

  // Shared by all managers
  static uint32_t next_type_of_handle_manager;
  static CHandleManager* all_managers[CHandle::max_types];
  static std::map<std::string, CHandleManager*> all_manager_by_name;

  const char*     name;

public:

  // ---------------------------------------
  CHandleManager()
    : type(0)
    , external_to_internal(nullptr)
    , internal_to_external(nullptr)
    , num_objs_used(0)
    , num_objs_capacity(0)
    , next_free_handle_ext_index(0)
    , last_free_handle_ext_index(0)
    , name( nullptr )
  { }

  CHandleManager(const CHandleManager&) = delete;

  virtual ~CHandleManager() {
    if (external_to_internal)
      delete[] external_to_internal;
    if (internal_to_external)
      delete[] internal_to_external;
  }

  // ---------------------------------------
  virtual void init(uint32_t max_objects) {
    assert(max_objects < max_total_objects_allowed);
    assert(max_objects > 0);

    // Register this as the manager that will handle
    // the type 'type' and has name 'getName()'
    type = next_type_of_handle_manager;
    next_type_of_handle_manager++;
    all_managers[type] = this;
    // Registrarme por nombre
    all_manager_by_name[getName()] = this;

    num_objs_used = 0;
    num_objs_capacity = max_objects;

    external_to_internal = new TExternalData[num_objs_capacity];
    internal_to_external = new uint32_t[num_objs_capacity];

    for (uint32_t i = 0; i < num_objs_capacity; ++i) {
      TExternalData* ed = external_to_internal + i;
      ed->current_age = 1;
      ed->internal_index = invalid_index;
      if (i != num_objs_capacity - 1)
        ed->next_external_index = i + 1;
      else
        ed->next_external_index = invalid_index;

      internal_to_external[i] = invalid_index;
    }

    next_free_handle_ext_index = 0;
    last_free_handle_ext_index = num_objs_capacity - 1;
  }

  // ---------------------------------------
  bool isValid(CHandle h) const {
    assert(h.getType() == type);
    assert(h.getExternalIndex() < num_objs_capacity);
    auto ed = external_to_internal + h.getExternalIndex();
    return ed->current_age == h.getAge();
  }

  const char* getName() const { return name; }
  uint32_t getType() const { return type; }
  uint32_t size() const { return num_objs_used; }
  uint32_t capacity() const { return num_objs_capacity; }

  // ---------------------------------------------
  void    setOwner(CHandle who, CHandle new_owner);
  CHandle getOwner(CHandle who);

  // ---------------------------------------------
  virtual void createObj(uint32_t internal_idx) = 0;
  virtual void destroyObj(uint32_t internal_idx) = 0;
  virtual void moveObj(uint32_t src_internal_idx, uint32_t dst_internal_idx) = 0;
  void destroyPendingObjects();

  // ---------------------------------------
  virtual void renderAll() const { }
  virtual void updateAll( float dt ) { }
  virtual bool load(CHandle h, MKeyValue& atts) { return true; }
  virtual void renderInMenu(CHandle h) { }

  // ---------------------------------------
  CHandle createHandle();
  void    destroyHandle(CHandle h);

  void    dumpInternals() const;

  static CHandleManager* getByType(uint32_t type);
  static CHandleManager* getByName(const char* obj_name);
  static uint32_t getNumDefinedTypes();
  static void destroyAllPendingObjects();
};


#include "object_manager.h"

#endif
