#ifndef	INC_BOX_SPAWNER_H_
#define INC_BOX_SPAWNER_H_

#include "comp_base.h"
#include "components\entity_parser.h"
#include "comp_physics.h"
#include "entity.h"

struct TCompBoxSpawner : public TCompBase {
	VEC3	m_pointSpawn;
	int		m_maxBoxAlive;
	int		m_currBoxAlive;

	float	m_timerSpawn;
	float	m_timerMaxSpawn;

	bool load(MKeyValue& atts) {
		m_pointSpawn = VEC3(0, 0, 0);
		m_pointSpawn = atts.getPoint("spawn");

		m_maxBoxAlive = atts.getInt("max_box", 3);
		m_currBoxAlive = 0;

		m_timerMaxSpawn = atts.getFloat("step", 5.0f);
		m_timerSpawn = 0.0f;

		return true;
	}

	void update(float dt) {
		m_timerSpawn += dt;
		if (m_timerSpawn >= m_timerMaxSpawn) {
			CHandle box = spawnPrefab("box");
			if (box.isValid()) {
				VEC3 posBox = m_pointSpawn;
				GET_COMP(t, box, TCompPhysics);
				if (t) t->setPosition(posBox, CQuaternion(0, 0, 0, 1));
			}
			m_timerSpawn = 0.0f;
		}
	}

	void onDestroyedBox(const TMsgBoxDestroyed& msg) {
		m_maxBoxAlive++;
	}
};

#endif