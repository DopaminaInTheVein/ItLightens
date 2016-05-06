#ifndef INC_OBJECT_MANAGER_H_
#define INC_OBJECT_MANAGER_H_

#include "handle_manager.h"

// This is a Handle manager which stores objects of
// type TObj
template< class TObj >
class CObjectManager : public CHandleManager
{
	std::vector< unsigned char > allocated_memory;

	TObj* objs;

	void createObj(uint32_t internal_idx) override {
		TObj* addr_to_use = objs + internal_idx;
		new (addr_to_use) TObj;
	}

	void destroyObj(uint32_t internal_idx) override {
		TObj* addr_to_use = objs + internal_idx;
		// Llamar al dtor de la clase directamente, no llamar
		// al delete pq la memoria no es del sistema
		addr_to_use->~TObj();
	}

	void moveObj(uint32_t src_internal_idx, uint32_t dst_internal_idx) override {
		TObj* src = objs + src_internal_idx;
		TObj* dst = objs + dst_internal_idx;
		*dst = std::move(*src);
	}

	bool load(CHandle h, MKeyValue& atts) {
		TObj* o = getAddrFromHandle(h);
		assert(o);
		return o->load(atts);
	}

public:

	CObjectManager(const char* new_name) : objs(nullptr) {
		name = new_name;
	}

	void init(uint32_t max_objects) override {
		CHandleManager::init(max_objects);
		// Uso un std::vector como facility para pedir
		// memoria lineal de N bytes y que se libere
		// automaticamente en el dtor del manager
		allocated_memory.resize(max_objects * sizeof(TObj));
		auto first_byte_allocated = &allocated_memory[0];
		// Paso por void para 'borrar' el tipo unsigned char
		objs = static_cast<TObj *>((void*)first_byte_allocated);
	}

	// --------------------------------------------
	CHandle getHandleFromObjAddr(const TObj* obj_addr) {
		auto internal_index = obj_addr - objs;
		if (internal_index >= num_objs_used || internal_index < 0)
			return CHandle();
		auto external_index = internal_to_external[internal_index];
		auto ed = external_to_internal + external_index;
		return CHandle(type, external_index, ed->current_age);
	}

	// --------------------------------------------
	TObj* getAddrFromHandle(CHandle h) {

		if (!h.getType())
			return nullptr;

		// Si no es mi tipo, no eres valido
		if (h.getType() != getType()) {
			fatal("You have request to convert a handle of type %s to "
				"a class of type %s"
				, CHandleManager::getByType(h.getType())->getName()
				, getName()
				);
			return nullptr;
		}

		assert(h.getType() == getType());
		auto ed = external_to_internal + h.getExternalIndex();

		// El handle es 'viejo', en esa entrada hay otro objeto
		// ahora
		if (ed->current_age != h.getAge())
			return nullptr;

		return objs + ed->internal_index;
	}

	// -------------------------
	void updateAll(float dt) override {
		PROFILE_FUNCTION(getName());
		auto o = objs;
		for (size_t i = 0; i < num_objs_used; ++i, ++o) {
			PROFILE_FUNCTION("object");
			o->update(dt);
		}
	}

	// -------------------------
	void initAll() override {
		auto o = objs;
		for (size_t i = 0; i < num_objs_used; ++i, ++o) {
			o->init();
		}
	}

 // -------------------------
  void updateAllInParallel(float dt) {
    PROFILE_FUNCTION(getName());
    #pragma omp parallel for
    for (int i = 0; i<(int)num_objs_used; ++i) {
      PROFILE_FUNCTION("object");
      objs[i].update(dt);
    }
  }

	// -------------------------
	void renderInMenu(CHandle h) override {
		auto obj = getAddrFromHandle(h);
		if (!obj)
			return;
		obj->renderInMenu();
	}
	// -------------------------
	void onAll(void (TObj::*member_fn)() const) const {
		auto o = objs;
		for (size_t i = 0; i < num_objs_used; ++i, ++o)
			(o->*member_fn)();
	}
	// -------------------------
	void onAll(void (TObj::*member_fn)()) {
		auto o = objs;
		for (size_t i = 0; i < num_objs_used; ++i, ++o)
			(o->*member_fn)();
	}

// -------------------------
  template< typename CB >
  void each( CB cb ) {
    auto o = objs;
    for (size_t i = 0; i<num_objs_used; ++i, ++o)
      cb( o );
  }
  // -------------------------
  // Use it with care
  TObj* getFirstObject() {
    return objs;
  }
};

#define DECL_OBJ_MANAGER( obj_name, obj_class_name ) \
  template<> \
  CObjectManager< obj_class_name >* getHandleManager<obj_class_name>() { \
    static CObjectManager< obj_class_name > om(obj_name); \
    return &om; \
  }

#endif
