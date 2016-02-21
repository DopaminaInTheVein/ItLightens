#ifndef INC_HANDLE_H_
#define INC_HANDLE_H_

#include <cinttypes>

// CHandleManager is a class
class CHandleManager;
class MKeyValue;

template< class TObj >
class CObjectManager;

// There is exists somewhere a function which returns
// the handle manager of type TObj
template<class TObj>
CObjectManager<TObj>* getHandleManager();

// ---------------------------------
class CHandle {
public:
	static const uint32_t num_bits_type = 7;
	static const uint32_t num_bits_index = 12;
	static const uint32_t num_bits_age = 32 - num_bits_type - num_bits_index;
	static const uint32_t max_types = 1 << num_bits_type;

	CHandle() : type(0), external_index(0), age(0) {}
	CHandle(uint32_t new_type
		, uint32_t new_external_index
		, uint32_t new_age) : type(new_type), external_index(new_external_index), age(new_age) {}

	// Contruir un handle a partir de una direccion de un obj
	// Solo va a devolver un handle valido si la direccion
	// pertenece al manager de ese tipo de objetos
	template< class TObj>
	CHandle(const TObj* obj_addr) {
		auto hm = getHandleManager<TObj>();
		*this = hm->getHandleFromObjAddr(obj_addr);
	}

	uint32_t getType()          const { return type; }
	uint32_t getExternalIndex() const { return external_index; }
	uint32_t getAge()           const { return age; }

	bool isValid() const;

	template<class TObj>
	operator TObj*() {
		auto hm = getHandleManager<TObj>();
		return hm->getAddrFromHandle(*this);
	}

	template<class TObj>
	void create() {
		auto hm = getHandleManager<TObj>();// CHandleManager<TObj>::get();
		*this = hm->createHandle();
	}
	void destroy();

	bool operator==(CHandle h) const {
		return type == h.type
			&& external_index == h.external_index
			&& age == h.age;
	}

	void setOwner(CHandle new_owner);
	CHandle getOwner();
	bool load(MKeyValue& atts);
	void renderInMenu();

private:
	// Guardar N bits para cada members, con la intencion de que objeto
	// CHandle ocupe 32 bits
	uint32_t type : num_bits_type;  // Que tiopo de objeto representa
	uint32_t external_index : num_bits_index; // Sirve para encontrar el objeto de verdad
	uint32_t age : num_bits_age;   // Para descartar versiones antiguas de los objetos
};

#endif
