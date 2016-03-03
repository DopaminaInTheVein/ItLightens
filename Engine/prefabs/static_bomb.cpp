
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
	
	CHandle bomb_h = CHandle(this);
	CEntity *p_e = bomb_h.getOwner();
	TCompTransform *mtx = p_e->get<TCompTransform>();
	VEC3 org = mtx->getPosition();

	msg.pos = org;
	msg.r = rad;

	VEntities ets = tags_manager.getHandlesPointerByTag(getID("AI"));
	for (CEntity *e : ets) {
		e->sendMsg(msg);
	}
}