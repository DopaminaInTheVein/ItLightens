#ifndef INC_HANDLE_H_
#define INC_HANDLE_H_

#include <cinttypes>
#include <fstream>

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
	CHandle(TObj* obj_addr) {
		auto hm = getHandleManager<std::remove_const<TObj>::type>();
		*this = hm->getHandleFromObjAddr(obj_addr);
	}

	template< class TObj>
	CHandle(TObj* obj_addr, uint32_t type) {
		auto hm = (CHandleManager::getByType(type));
		//auto hm2 = getHandleManager<std::remove_const<TObj>::type>();
		*this = hm->getHandleFromObjAddr(obj_addr);
	}

	uint32_t getType()          const { return type; }
	uint32_t getExternalIndex() const { return external_index; }
	uint32_t getAge()           const { return age; }

	bool isValid() const;

	template<class TObj>
	operator TObj*() const {
		// std::remove_const<T>::type returns the TObj without const
		// Used when TObj is const*. We want the manager of <TLife> objs
		// not the manager of <const TLife>, so we use the remove_const
		auto hm = getHandleManager< std::remove_const<TObj>::type >();
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

	bool operator!=(CHandle h) const {
		return !(*this == h);
	}

	void setOwner(CHandle new_owner);
	CHandle getOwner() const;
	bool hasTag(std::string tag);
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	void renderInMenu();

	// --------------------------------------
	template< class TMsg >
	void sendMsg(const TMsg& msg);

	template< class TMsg >
	void sendMsgWithReply(TMsg& msg);

	uint32_t asUnsigned() const {
		return *(unsigned*)this;
	}
	void fromUnsigned(uint32_t id) const {
		*((unsigned*)this) = id;
	}

private:
	// Guardar N bits para cada members, con la intencion de que objeto
	// CHandle ocupe 32 bits
	uint32_t type : num_bits_type;  // Que tiopo de objeto representa
	uint32_t external_index : num_bits_index; // Sirve para encontrar el objeto de verdad
	uint32_t age : num_bits_age;   // Para descartar versiones antiguas de los objetos
};
//

#include "handle/msgs.h"

// --------------------------------------
// Assuming the TMsg goes to valid CEntities
template< class TMsg >
void CHandle::sendMsg(const TMsg& msg) {
	CEntity* e = getHandleManager< std::remove_const<CEntity>::type >()->getAddrFromHandle(*this);
	if (e)
		e->sendMsg(msg);
}

template< class TMsg >
void CHandle::sendMsgWithReply(TMsg& msg) {
	CEntity* e = getHandleManager< std::remove_const<CEntity>::type >()->getAddrFromHandle(*this);
	if (e)
		e->sendMsgWithReply(msg);
}

#define GETH_COMP(handle, type) ((CEntity*)handle)->get<type>()
#define GET_COMP(var, handle, type) type * var = ((CEntity*)handle)->get<type>()
#define GETH_MY(type) ((CEntity*)(CHandle(this).getOwner()))->get<type>()
#define GET_MY(var, type) type * var = ((CEntity*)(CHandle(this).getOwner()))->get<type>()
#define GET_ECOMP(var, entity_ptr, type) type * var = entity_ptr ? entity_ptr->get<type>() : CHandle()
#define GET_NAME(handle) ((CEntity*)handle)->getName()
#define MY_NAME ((CEntity*)(CHandle(this).getOwner()))->getName()
#define MY_OWNER CHandle(this).getOwner()

#endif
