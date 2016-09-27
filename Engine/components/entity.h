#ifndef INC_HANDLE_ENTITY_H_
#define INC_HANDLE_ENTITY_H_

#include "handle/handle.h"
#include "comp_base.h"
#include "handle/msgs.h"

#include "utils/XMLParser.h"

// --------------------------------------------
class CEntity : public TCompBase {
	CHandle comps[CHandle::max_types];
	int id = -1;
	bool permanent;
	bool need_reload;
	bool temp;
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
	CHandle get() const {
		auto hm = getHandleManager<TObj>();
		return comps[hm->getType()];
	}

	void renderInMenu();

	CHandle getByCompIndex(uint32_t idx) const {
		return comps[idx];
	}

	template< typename TObj >
	void del() {
		get<TObj>().destroy();
		// No hay necesidad de comps[ h.getType() ] = CHandle()
		// pq el age se encarga de invalidar mi handle
	}

	template< class TMsg >
	void sendMsg(const TMsg& msg) {
		//if(profiler) PROFILE_FUNCTION("entity: send message");
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
		PROFILE_FUNCTION("entity: send message with reply");
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

	// ---------------------------------------------
	const char* getName() const;
	void setName(const char* new_name);
	void setId(int new_id) { id = new_id; }
	int getId() { return id; }
	bool hasName(const char* new_name) const;
	bool hasTag(std::string tag);
	bool isPermanent() { return permanent; }
	bool needReload() { return need_reload; }
	void setPermanent(bool new_permanent) { permanent = new_permanent; }
	void setReload(bool new_reload) { need_reload = new_reload; }
	void setTemp(bool new_temp) { temp = new_temp; }
	bool isTemp() { return temp; }
	bool hasToClear(bool reloading) { return !isPermanent() && (isTemp() || !reloading || needReload()); }
	bool save(std::ofstream& os, MKeyValue& atts);
	// --------------------------------------------
	CEntity() {
	}

	~CEntity() {
		for (uint32_t i = 0; i < CHandle::max_types; ++i) {
			if (comps[i].isValid())
				comps[i].destroy();
		}
		tags_manager.removeAllTags(CHandle(this));
	}
};

#endif
