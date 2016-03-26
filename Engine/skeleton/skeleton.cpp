#include "mcv_platform.h"
#include "skeleton.h"
#include "cal3d/cal3d.h"
#include "resources/resources_manager.h"

template<> IResource::eType getTypeOfResource<CSkeleton>() { return IResource::SKELETON; }

template<>
IResource* createObjFromName<CSkeleton>(const std::string& name) {
  CSkeleton* obj = new CSkeleton;
  auto full_name = IResource::getDataPath() + name;
  if( !obj->xmlParseFile(full_name))
    return nullptr;
  obj->setName(name.c_str());
  return obj;
}

void CSkeleton::onStartElement(const std::string &elem, MKeyValue &atts) {
  auto src = IResource::getDataPath() + atts.getString("src", "");
  if (elem == "skeleton") {
    bool is_ok = core_model->loadCoreSkeleton(src);
    assert(is_ok);
  }
  else if (elem == "animation") {
    int anim_id = core_model->loadCoreAnimation(src);
    core_model->getCoreAnimation(anim_id)->setName(src);
    assert(anim_id != -1);
  }
}

CSkeleton::CSkeleton() 
  : core_model(nullptr)
{
  CalLoader::setLoadingMode(LOADER_ROTATE_X_AXIS| LOADER_INVERT_V_COORD);
  core_model = new CalCoreModel("unknown");
}

void CSkeleton::destroy()
{
  delete core_model;
}