#ifndef INC_MSGS_H_
#define INC_MSGS_H_

#include <map>
#include <cinttypes>
#include "handle.h"
#include "object_manager.h"

struct IFunctorBase {
	virtual ~IFunctorBase() {}
	virtual void execute(ClHandle handle, const void* msg_data) = 0;
	virtual void execute(ClHandle handle, void* msg_data) = 0;
};

template< class TObj, class TMsgData >
struct TFunctor : public IFunctorBase {
	// Save a pointer to a member of the class TObj
	// which receives a const TMsg& and returns nothing
	typedef void (TObj::*TMemberFnConst)(const TMsgData&);
	typedef void (TObj::*TMemberFn)(TMsgData&);
	TMemberFn member_fn;
	TMemberFnConst member_fn_const;

	// Save the member_fn received as argument of the ctor
	TFunctor(TMemberFnConst new_member_fn) : member_fn_const(new_member_fn) { }
	TFunctor(TMemberFn new_member_fn) : member_fn(new_member_fn) { }

	// Implement the virtual interface
	void execute(ClHandle handle, const void* msg) {
		assert(msg);
		TObj* obj = handle;
		// Confirm the handle is still valid
		assert(obj);

		if (!obj)
			return;

		// Llamar al member 'member_fn' del objeto obj
		// y pasar como argumento el msg casted to the
		// template type
		(obj->*member_fn_const)(*(const TMsgData*)msg);
	}

	void execute(ClHandle handle, void* msg) {
		assert(msg);
		TObj* obj = handle;
		// Confirm the handle is still valid
		assert(obj);

		if (!obj)
			return;

		// Llamar al member 'member_fn' del objeto obj
		// y pasar como argumento el msg casted to the
		// template type
		(obj->*member_fn)(*(TMsgData*)msg);
	}
};

//
typedef unsigned TMsgID;
TMsgID generateUniqueMsgID();

// A macro to include in all msgs structs to associate
// a unique id to each struct
#define DECLARE_MSG_ID() \
  static TMsgID getMsgID() {    \
    static TMsgID unique_id = generateUniqueMsgID(); \
    return unique_id;           \
  }

struct TComponentMsgHandler {
	uint32_t      comp_type;
	IFunctorBase* method;
};

typedef std::multimap< TMsgID, TComponentMsgHandler > MMsgSubscriptions;
extern MMsgSubscriptions msg_subscriptions;

template< class TObj >
void subscribeToMsg(TMsgID msg_id, IFunctorBase* method) {
	std::pair<TMsgID, TComponentMsgHandler> e;
	e.first = msg_id;
	e.second.comp_type = getHandleManager<TObj>()->getType();
	e.second.method = method;
	msg_subscriptions.insert(e);
}

#define SUBSCRIBE(acomp,amsg_arg,amethod) \
  subscribeToMsg<acomp>( amsg_arg::getMsgID()  \
                       , new TFunctor<acomp, amsg_arg >( &acomp::amethod ) )

#endif