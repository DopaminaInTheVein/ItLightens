#include "mcv_platform.h"
#include "ParticlesEmitter.h"

#include "skeleton\skeleton.h"
#include "components\comp_tags.h"
#include "components\entity.h"
#include "skeleton\comp_skeleton.h"
#include "components\comp_transform.h"


VEC3 CParticlesEmitter::GetPosBone(int num) {
	CEntity *player = tags_manager.getFirstHavingTag(target_pos);
	assert(player);
	TCompSkeleton *skel_player = player->get<TCompSkeleton>();
	assert(skel_player);

	//auto bone_test = skel_player->model->getSkeleton()->getBone(num);

	//auto bone_test = skel_player->model->getSkeleton()->get

	//auto bone_test = skel_player->model->getCoreModel()->getCoreSkeleton()->getCoreBone(num);
	auto bone_test = skel_player->model->getSkeleton()->getBone(num);
	// auto bone_test = skel_player->model->getSkeleton()->getBone(num);
	if (!bone_test) {
		return VEC3(0, 0, 0);
	}
	//auto traslacion_cal = bone_test->getTranslation();
	auto traslacion_cal = bone_test->getTranslationAbsolute();
	//auto traslacion_cal = bone_test->getTranslationBoneSpace();
	auto rotation_cal = bone_test->getRotationAbsolute();
	//auto rotation_cal = bone_test->getRotationBoneSpace();
	//auto rotation_cal = bone_test->getRotation();
	TCompTransform* transform = player->get<TCompTransform>();
	VEC3 traslacion = VEC3(traslacion_cal.x, traslacion_cal.y, traslacion_cal.z);
	//CQuaternion rotation = CQuaternion(rotation_cal.x, rotation_cal.y, rotation_cal.z, -rotation_cal.w);

	//VEC3 tras = traslacion;
	//rotate_vector_by_quaternion(tras, rotation, traslacion);
	
	//seems ok
	//auto cardan = transform->getRotation();
	//tras = traslacion;
	//rotate_vector_by_quaternion(tras, cardan, traslacion);
	//VEC3 position = traslacion + transform->getPosition();

	//return pos;
	//return traslacionEngine;
	return traslacion;
}


VEC3 CParticlesEmitter::testBones(int num, std::string* name, VEC3* traslacion) {
 /* CEntity *player = tags_manager.getFirstHavingTag("player");
  assert(player);
  TCompSkeleton *skel_player = player->get<TCompSkeleton>();
  assert(skel_player);

  //auto bone_test = skel_player->model->getSkeleton()->getBone(num);

   //auto bone_test = skel_player->model->getSkeleton()->get

  auto core_vector = skel_player->model->getCoreModel()->getCoreSkeleton()->getVectorRootCoreBoneId();
  auto vector_cal = skel_player->model->getCoreModel()->getCoreSkeleton()->getVectorCoreBone();

  auto bone_test = skel_player->model->getCoreModel()->getCoreSkeleton()->getCoreBone(num);
  auto vector_bone = skel_player->model->getSkeleton()->getVectorBone();
 // auto bone_test = skel_player->model->getSkeleton()->getBone(num);
  *name = bone_test->getName();
  if (!bone_test) {
    return VEC3(0, 0, 0);
  }
  //auto traslacion_cal = bone_test->getTranslation();
  auto traslacion_cal = bone_test->getTranslationAbsolute();
  auto rotation_cal = bone_test->getRotationAbsolute();
  //bone_test-
  TCompTransform* transform = player->get<TCompTransform>();
  *traslacion = VEC3(traslacion_cal.x, traslacion_cal.y, traslacion_cal.z);
  CQuaternion rotation = CQuaternion(rotation_cal.x, rotation_cal.y, rotation_cal.z, -rotation_cal.w);
  VEC3 tras = *traslacion;
  rotate_vector_by_quaternion(tras,rotation,*traslacion);

  VEC3 position = *traslacion + transform->getPosition();
  VEC3 cardan = transform->getRotation();
  
  //return traslacionEngine;
  return position;*/
	return VEC3(0,0,0);
}

PxVec3 CParticlesEmitter::GetInitialPosByShape(const VEC3& front, const VEC3& up)
{
	PxVec3 init_pos = m_initialPosition;
	float random_angle;
	float random_radius;
	switch (m_shape_type) {
		case SHAPE_BOX:
			init_pos.x += random(m_shape_emitter.min.x, m_shape_emitter.max.x);
			init_pos.y += random(m_shape_emitter.min.y, m_shape_emitter.max.y);
			init_pos.z += random(m_shape_emitter.min.z, m_shape_emitter.max.z);
			return init_pos;
			break;
		case SHAPE_CONE:
			/*random_angle = random(-2 * PI, 2*PI);
			random_radius = random(-m_shape_emitter.radius, m_shape_emitter.radius);
			random_angle = deg2rad(random_angle);
			init_pos.x += random_radius*sinf(random_angle)*m_shape_emitter.direction.x;
			init_pos.y += random_radius*cosf(random_angle)*m_shape_emitter.direction.y;
			init_pos.z += random_radius*tanf(random_angle)*m_shape_emitter.direction.z;*/
			return init_pos;
			break;
		case SHAPE_EDGE:
			init_pos.x += random(-m_shape_emitter.direction.x, m_shape_emitter.direction.x);
			init_pos.y += random(-m_shape_emitter.direction.y, m_shape_emitter.direction.y);
			init_pos.z += random(-m_shape_emitter.direction.z, m_shape_emitter.direction.z);
			return init_pos;
			break;
		case SHAPE_SPHERE:
			return init_pos;
			break;
		case SHAPE_NOTHING:
			return init_pos;
			break;
		case SHAPE_CIRCLE:
		{
			PxVec3 pos = init_pos;
			PxVec3 limit = PhysxConversion::Vec3ToPxVec3(VEC3(1, 1, 1) - up);

			PxVec3 dir;

			dir.x = random(-1, 1)*limit.x;
			dir.y = random(-1, 1)*limit.y;
			dir.z = random(-1, 1)*limit.z;

			dir.normalize();

			pos += dir*m_shape_emitter.radius;

			return pos;

			break;
		}
		default:
			return init_pos;
			break;

	}

	return init_pos;
}

PxVec3 CParticlesEmitter::GetInitialVelByShape(const VEC3& front, const VEC3& up, PxVec3 curr_position)
{
	PxVec3 init_vel = m_initialVelocity;
	float random_angle = random(-m_shape_emitter.angle, m_shape_emitter.angle);
	if (init_vel.x == 0.0f && init_vel.y == 0.0f && init_vel.z == 0.0f)
		return m_initialVelocity;

	switch (m_shape_type) {
	case SHAPE_BOX:
		return init_vel;
		break;
	case SHAPE_CONE:
		random_angle = deg2rad(random_angle);
		init_vel = PhysxConversion::Vec3ToPxVec3( m_shape_emitter.direction);
		init_vel.x += random_angle*(1 - up.x);
		init_vel.y += random_angle*(1 - up.y);
		init_vel.z += random_angle*(1 - up.z);

		return init_vel;
		break;
	case SHAPE_EDGE:
		return init_vel;
		break;
	case SHAPE_SPHERE:
		init_vel.x += random(-1, 1);
		init_vel.y += random(-1, 1);
		init_vel.z += random(-1, 1);
		init_vel *= m_shape_emitter.radius;
		return init_vel;
		break;
	case SHAPE_NOTHING:
		return init_vel;
		break;
	case SHAPE_CIRCLE:
		init_vel += curr_position - m_initialPosition;
		return init_vel;
		break;
	default:
		return init_vel;
		break;

	}
}
