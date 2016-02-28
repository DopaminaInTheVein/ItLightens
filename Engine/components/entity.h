#ifndef INC_HANDLE_ENTITY_H_
#define INC_HANDLE_ENTITY_H_

#include "handle/handle.h"
#include "comp_base.h"
#include "handle/msgs.h"

// --------------------------------------------
class CEntity : public TCompBase {
	CHandle comps[CHandle::max_types];
public:

	void add(CHandle h) {
		// Lo que viene tiene que ser valido
		assert(h.isValid());
		// Lo que yo tenia no tenia que ser valido¿?¿?¿
		assert(!comps[h.getType()].isValid());
		// Me lo quedo
		comps[h.getType()] = h;
		// Que el componente sepa que soy su owner
		h.setOwner(CHandle(this));
	}

	template< typename TObj >
	CHandle get() {
		auto hm = getHandleManager<TObj>();
		return comps[hm->getType()];
	}

	template< typename TObj >
	void del() {
		get<TObj>().destroy();
		// No hay necesidad de comps[ h.getType() ] = CHandle()
		// pq el age se encarga de invalidar mi handle
	}

	template< class TMsg >
	void sendMsg(const TMsg& msg) {
		// Get all entries matching the msg_id of the TMsg
		auto range = msg_subscriptions.equal_range(TMsg::getMsgID());
		while (range.first != range.second) {
			const TComponentMsgHandler& msg_handler = range.first->second;

			// If this entity HAS that component, and it's valid
			CHandle my_comp = comps[msg_handler.comp_type];
			if (my_comp.isValid()) {
				// use the method object to call to the method
				// which was registered to that msg in the subscribe macro
				msg_handler.method->execute(my_comp, &msg);
			}

			range.first++;
		}
	}

	template< class TMsg >
	void sendMsgWithReply(TMsg& msg) {
		// Get all entries matching the msg_id of the TMsg
		auto range = msg_subscriptions.equal_range(TMsg::getMsgID());
		while (range.first != range.second) {
			const TComponentMsgHandler& msg_handler = range.first->second;

			// If this entity HAS that component, and it's valid
			CHandle my_comp = comps[msg_handler.comp_type];
			if (my_comp.isValid()) {
				// use the method object to call to the method
				// which was registered to that msg in the subscribe macro
				msg_handler.method->execute(my_comp, &msg);
			}

			range.first++;
		}
	}


	// --------------------------------------------
	void renderInMenu();

	// --------------------------------------------
	CEntity() {
	}

	~CEntity() {
		for (uint32_t i = 0; i < CHandle::max_types; ++i) {
			if (comps[i].isValid())
				comps[i].destroy();
		}
	}
};

#endif
