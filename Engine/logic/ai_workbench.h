#ifndef INC_AI_WORKBENCH_H
#define	INC_AI_WORKBENCH_H

#include "aicontroller.h"
#include "sbb.h"

#include "components\comp_base.h"
#include "handle\handle.h"
#include "components\comp_msgs.h"

//forward declarations
//--------------------------------------
class CEntity;

template< class TObj >
class CObjectManager;
//--------------------------------------

class workbench_controller : public aicontroller, public TCompBase {
  static std::map<int, std::string> out;

  //main attributes
  //--------------------------------------
  float range; 				//range sonar
  float rot_speed_sonar;
  float rot_speed_disable;
  //--------------------------------------

  //general info
  //--------------------------------------

  int id_wb = 0;

  std::string full_name = "";

  CObjectManager<workbench_controller>		*om = nullptr;
  CHandle								 myHandle;
  CHandle								 myParent;
  CEntity								*myEntity = nullptr;
  //--------------------------------------

  //messages types
  TMsgWBEmpty				msg_empty;
  TMsgWBTakenByPlayer		msg_taken;

protected:
  // the states, as maps to functions
  static map<string, statehandler>statemap;

public:

  static int id_curr_max_wb;	//count ids, id++ each beacon

                  //states wb
  enum {
    INACTIVE = 0,
    INACTIVE_TAKEN,
    BUSY
  };

  map<string, statehandler>* getStatemap() override {
    return &statemap;
  }

  workbench_controller() {}		//needed to create obj at load
  void Init() override;
  void init() { Init(); }
  void readIniFileAttr();

  //Overload functions from TCompBase, needed to loop AI Component
  //--------------------------------------
  void update(float elapsed) { if (!isInRoom(myParent))return; Recalc(); }  //Called from object_manager
  //--------------------------------------

  //Init handlers
  //--------------------------------------
  void SetHandleMeInit();
  void SetMyEntity();
  void SendMessageEmpty();
  void SendMessageTaken();
  //--------------------------------------

  //Functions AI Nodes:
  //--------------------------------------
  void Idle();
  void Inactive();		//beacon empty
  void Busy();
  //--------------------------------------

  void renderInMenu();
};

#endif