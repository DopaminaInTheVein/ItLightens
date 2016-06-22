#ifndef INC_SKELETON_MANAGER_H_
#define INC_SKELETON_MANAGER_H_

#include "cal3d/cal3d.h"

// Mas otras cosas que cal3d no tiene
class CCoreModel : public CalCoreModel {

public:
  CCoreModel();

  struct TBoneCorrector {
    int         bone_id;
    CalVector   local_dir;
    float       local_amount;
    bool        render;
    TBoneCorrector() : bone_id(-1), local_dir(1, 0, 0) {}
    TBoneCorrector(int abone_id, CalVector alocal_dir)
      : bone_id(abone_id)
      , local_dir(alocal_dir)
    {}
	void apply(CalModel* model, CalVector target, float amount);
  };

  typedef std::vector <TBoneCorrector> VBoneCorrections;
  VBoneCorrections bone_corrections;
};

#endif
