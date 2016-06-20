#ifndef INC_BEACON_H_
#define INC_BEACON_H_

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

class beacon_controller : public aicontroller, public TCompBase {
  static std::map<int, std::string> out;

  //main attributes
  //--------------------------------------
  float range = 2.0f;		// range sonar
  float width = 1.0f; ;		// width sonar
  float rot_speed_sonar = 3.0f;
  int id_beacon = 0;
  bool rotatingR = true;

  std::string full_name = "";
  std::string beacon_light = "";
  CObjectManager<beacon_controller> * om = nullptr;

  CHandle myHandle;
  CHandle	myParent;
  CEntity	*myEntity = nullptr;

  float max_idle_waiting = 5.0f;
  float idle_wait;

  bool active = true;

protected:
  // the states, as maps to functions
  static map<string, statehandler>statemap;

public:

  map<string, statehandler>* getStatemap() override {
    return &statemap;
  }

  beacon_controller() {}		//needed to create obj at load
  void Init() override;
  void init() { Init(); }
  bool load(MKeyValue& atts);
  bool playerInRange();
  float getRange() const { return range; }
  //Overload functions from TCompBase, needed to loop AI Component
  //--------------------------------------
  void update(float elapsed) {
    if (!isInRoom(myParent))return;
    Recalc();
  }  //Called from object_manager

  void SetHandleMeInit();
  void SetMyEntity();
  //--------------------------------------

  //Functions AI Nodes:
  //--------------------------------------
  void Idle();
  void Rotating();	// beacon rotating, looking for player
  void AimPlayer();   // beacon reached player and aims him

  //Messages:
  //--------------------------------------
  void onPlayerAction(TMsgBeaconBusy& msg); // We actiate or deactivate Beacon

  void renderInMenu();
};

#endif