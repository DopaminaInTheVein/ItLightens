#ifndef	INC_TRIGGER_STANDAR_H_
#define INC_TRIGGER_STANDAR_H_

#include "comp_trigger.h"
#include "app_modules\logic_manager\logic_manager.h"

struct TCompTriggerStandar : public TTrigger {
	std::vector<std::string> m_tagsAffect;
	std::string m_id_trigger;


	bool load(MKeyValue& atts) {
		m_id_trigger	= atts.getString("id", "");
		return true;
	}

	void mUpdate(float dt) {
		//nothing
	}
	void onTriggerInside(const TMsgTriggerIn& msg) {
		//nothing 
	}
	void onTriggerEnter(const TMsgTriggerIn& msg) {
		if (msg.other.isValid() && m_id_trigger != "") {
			//TODO: need params? other handle?
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnEnter, m_id_trigger, msg.other.asUnsigned());
		}
	}
	void onTriggerExit(const TMsgTriggerOut& msg) {
		if (msg.other.isValid() && m_id_trigger != "") {
			//TODO: need params? other handle?
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnLeave, m_id_trigger);
		}
	}
};


#endif