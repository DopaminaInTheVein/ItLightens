#include "mcv_platform.h"
#include "bt.h"
#include "sbb.h"
#include "recast/navmesh.h"
#include "recast/navmesh_query.h"
#include "recast/DebugUtils/Include/DebugDraw.h"
#include "components/entity_tags.h"
#include "components/components.h"

void bt::create(string s)
{
  name = s;
}

btnode *bt::createNode(string s)
{
  if (findNode(s) != NULL)
  {
    printf("Error: node %s already exists\n", s.c_str());
    return NULL;	// error: node already exists
  }
  btnode *btn = new btnode(s);
  (*getTree())[s] = btn;
  return btn;
}

btnode *bt::findNode(string s)
{
  if (getTree()->find(s) == getTree()->end()) return NULL;
  else return (*getTree())[s];
}

btnode *bt::createRoot(string s, int type, btcondition btc, btaction bta)
{
  btnode *r = createNode(s);
  r->setParent(NULL);
  *getRoot() = r;
  r->setType(type);
  if (btc != NULL) addCondition(s, btc);
  if (bta != NULL) addAction(s, bta);

  current = NULL;
  return r;
}

btnode *bt::addChild(string parent, string son, int type, btcondition btc, btaction bta)
{
  btnode *p = findNode(parent);
  btnode *s = createNode(son);
  p->addChild(s);
  s->setParent(p);
  s->setType(type);
  if (btc != NULL) addCondition(son, btc);
  if (bta != NULL) addAction(son, bta);
  return s;
}

btnode *bt::addChild(string parent, string son, int type, btcondition btc, CLogicManagerModule::EVENT evt, string params)
{
  btnode *p = findNode(parent);
  btnode *s = createNode(son);
  p->addChild(s);
  s->setParent(p);
  s->setType(type);
  if (btc != NULL) addCondition(son, btc);
  if (evt != NULL) {
    btevent event;
    event.evt = evt;
    event.params = params;
    addEvent(son, event);
  }
  return s;
}

void bt::Recalc()
{
  if (current == NULL) (*getRoot())->recalc(this);	// I'm not in a sequence, start from the root
  else current->recalc(this);				// I'm in a sequence. Continue where I left
}

void bt::setCurrent(btnode *nc)
{
  current = nc;
}

void bt::addAction(string s, btaction act)
{
  if (getActions()->find(s) != getActions()->end())
  {
    printf("Error: node %s already has an action\n", s.c_str());
    return;	// if action already exists don't insert again...
  }
  (*getActions())[s] = act;
}

int bt::execAction(string s)
{
  if (getActions()->find(s) == getActions()->end())
  {
    printf("ERROR: Missing node action for node %s\n", s.c_str());
    return OK; // error: action does not exist
  }
  return (this->*(*getActions())[s])();
}

void bt::addCondition(string s, btcondition cond)
{
  if (getConditions()->find(s) != getConditions()->end())
  {
    printf("Error: node %s already has a condition\n", s.c_str());
    return;	// if condition already exists don't insert again...
  }
  (*getConditions())[s] = cond;
}

bool bt::testCondition(string s)
{
  if (getConditions()->find(s) == getConditions()->end())
  {
    return true;	// error: no condition defined, we assume TRUE
  }
  return (this->*(*getConditions())[s])();
}

void bt::addEvent(string s, btevent event)
{
  if (getEvents()->find(s) != getEvents()->end())
  {
    printf("Error: node %s already has a event\n", s.c_str());
    return;	// if condition already exists don't insert again...
  }
  (*getEvents())[s] = event;
}

int bt::execEvent(string s)
{
  if (getEvents()->find(s) == getEvents()->end())
  {
    printf("ERROR: Missing node event for node %s\n", s.c_str());
  }
  else
  {
    btevent to_execute = (*getEvents())[s];
    logic_manager->throwEvent(to_execute.evt, to_execute.params);
  }

  return OK;
}

// To be implemented in the subclasses
map<string, btnode *>* bt::getTree() {
  return nullptr;
}
map<string, btaction>* bt::getActions() {
  return nullptr;
}
map<string, btcondition>* bt::getConditions() {
  return nullptr;
}
map<string, btevent>* bt::getEvents() {
  return nullptr;
}

btnode** bt::getRoot() {
  return nullptr;
}

void bt::getPath(VEC3 startPoint, VEC3 endPoint, string nombreSala) {
  CNavmesh nav = SBB::readNavmesh();
  CNavmeshQuery query(&nav);
  query.updatePosIni(startPoint);
  query.updatePosEnd(endPoint);
  query.findPath(query.p1, query.p2);
  const float * path = query.getVertexSmoothPath();
  pathWpts.clear();
  totalPathWpt = query.getNumVertexSmoothPath();
  if (totalPathWpt > 0) {
    for (int i = 0; i < totalPathWpt * 3; i = i + 3) {
      pathWpts.push_back(VEC3(path[i], path[i + 1], path[i + 2]));
    }
  }
  currPathWpt = 0;
  if (totalPathWpt < 1)
    return;

  for (int i = 0; i < (pathWpts.size() - 1); i++) {
    Debug->DrawLine(pathWpts[i], pathWpts[i + 1]);
  }

  currPathWpt = 0;
}

CEntity* bt::frontCollisionIA(const VEC3 & npcPos, CHandle ownHandle) {
  TTagID tagIDia = getID("AI");
  vector<CHandle> colCandidates = tags_manager.getHandlesByTag(tagIDia);
  for (CHandle candidateH : colCandidates) {
    if (candidateH != ownHandle) {
      CEntity * candidateE = candidateH;
      TCompTransform * candidateT = candidateE->get<TCompTransform>();
      if (realDist(npcPos, candidateT->getPosition()) < 1.5f) {
        return candidateE;
      }
    }
  }
  return nullptr;
}

CEntity* bt::frontCollisionBOX(const TCompTransform * transform, CEntity *  molePursuingBoxi) {
  TTagID tagIDbox = getID("box");
  VEC3 npcPos = transform->getPosition();
  vector<CHandle> colCandidates = tags_manager.getHandlesByTag(tagIDbox);//SBB::readHandlesVector("collisionables");
  for (CHandle candidateH : colCandidates) {
    if (!candidateH.isValid()) continue;
    CEntity * candidateE = candidateH;
    TCompTransform * candidateT = candidateE->get<TCompTransform>();
    VEC3 colPos = candidateT->getPosition();
    if ((molePursuingBoxi == nullptr || molePursuingBoxi != candidateE) && realDist(npcPos, colPos) < 1.5f) {
      return candidateE;
    }
    /*
    VEC3 dir = transform->getFront();
    dir.Normalize();
    PxRaycastBuffer hit;
    bool ret = g_PhysxManager->raycast(npcPos, dir, 1.5f, hit);
    if (ret) {
      return candidateE;
    }
    */
  }
  return nullptr;
}
bool bt::avoidBoxByLeft(CEntity * candidateE, const TCompTransform * transform, string nombreSala) {
  VEC3 npcPos = transform->getPosition();
  VEC3 dir = transform->getLeft();
  dir.Normalize();
  PxRaycastBuffer hit;
  bool ret = g_PhysxManager->raycast(npcPos, dir, 10.f, hit);
  if (ret) {
    return true;
  }
  return false;
}
bool bt::needsSteering(VEC3 npcPos, TCompTransform * transform, float rotation_speed, CHandle myHandle, string nombreSala, CEntity *  molePursuingBoxi) {
  float yaw, pitch, delta_yaw = 0.25f;
  transform->getAngles(&yaw, &pitch);
  CEntity * collisionBOX = frontCollisionBOX(transform, molePursuingBoxi);
  if (frontCollisionIA(npcPos, myHandle) != nullptr) {
    transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
    return true;
  }
  else if (collisionBOX != nullptr && avoidBoxByLeft(collisionBOX, transform, nombreSala)) {
    transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
    return true;
  }
  else if (collisionBOX != nullptr) {
    delta_yaw = -0.25f;
    transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
    return true;
  }
  return false;
}