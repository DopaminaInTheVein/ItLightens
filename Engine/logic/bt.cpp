#include "mcv_platform.h"
#include "bt.h"
#include "sbb.h"
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

bool bt::load_bt(MKeyValue& atts)
{
	state_ini = atts.getString("state", "");
	return true;
}

bool bt::save_bt(std::ofstream& os, MKeyValue& atts)
{
	if (current) atts.put("state", current->getName());
	return true;
}