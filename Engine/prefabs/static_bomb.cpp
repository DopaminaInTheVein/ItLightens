
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
	myHandle = CHandle(this);
	myParent = myHandle.getOwner();
	dbg("STATIC BOMB -> I am going to explode\n");
	if (myHandle.isValid() && myParent.isValid()) {
		SendMsg();
		//TODO: animation
		dbg("STATIC BOMB -> exploded\n");
		destroy();
	}
}

void CStaticBomb::toExplode()
{
	Explode();
}

void CStaticBomb::SendMsg()
{
	TMsgStaticBomb msg;
	CEntity *p_e = myParent;
	TCompTransform *mtx = p_e->get<TCompTransform>();
	VEC3 org = mtx->getPosition();

	msg.pos = org;
	msg.r = rad;

	VHandles ets = tags_manager.getHandlesByTag(getID("AI"));
	for (CEntity *e : ets) {
		e->sendMsg(msg);
	}
}