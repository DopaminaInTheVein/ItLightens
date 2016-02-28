
#include "mcv_platform.h"
#include "static_bomb.h"
#include "components\comp_msgs.h"
#include "components\entity.h"
#include "components\comp_transform.h"
#include "components\entity_tags.h"

void CStaticBomb::update(float elapsed)
{
	t_waiting += getDeltaTime();
	if (t_waiting >= t_explode) {
		Explode();
	}
}

void CStaticBomb::Explode()
{
	dbg("STATIC BOMB -> I am going to explode\n");
	SendMsg();
	//TODO: animation
	dbg("STATIC BOMB -> exploded\n");
	destroy();
}

void CStaticBomb::SendMsg()
{

	auto om = getHandleManager<CStaticBomb>();
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	TMsgStaticBomb msg;
	float rad = 10.0f;
	CHandle player_h = tags_manager.getFirstHavingTag(getID("target"));
	CEntity *p_e = player_h;
	TCompTransform *mtx = p_e->get<TCompTransform>();
	VEC3 org = mtx->getPosition();

	msg.x_max = org.x + rad;
	msg.x_min = org.x - rad;
	msg.z_max = org.z + rad;
	msg.z_min = org.z - rad;

	//TODO: for each NPC, tag NPC
	VEntities ets = tags_manager.getHandlesPointerByTag(getID("AI_cientifico"));
	for (CEntity *e : ets) {
		e->sendMsg(msg);
	}
}