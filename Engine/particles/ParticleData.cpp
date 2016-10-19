#include "mcv_platform.h"
#include "ParticleData.h"
#include "ParticlesEmitter.h"

#include "render\draw_utils.h"

void TParticleData::ForcesCircle(std::vector<PxVec3>* forces, CParticlesEmitter* emitter)
{
	float r = emitter->m_shape_emitter.radius;
	auto forces_vec = *forces;
	for (int idx = 0; idx < forces_vec.size(); idx++) {
		PxVec3 dir = positionBuffer[idx] - positionInitBuffer[idx];
		float w = 2 * PI / sinf(shader_ctes_globals.world_time);
		float a = w*w*r;
		dir.normalize();
		forces_vec[idx] = dir * a;
	}

	*forces = forces_vec;
}

void TParticleData::UpdateForces(std::vector<PxVec3>* forces, CParticlesEmitter* emitter)
{
	switch (*emitter->GetShape()) {
	case CParticlesEmitter::SHAPE_BOX:
		//nothing
		break;
	case CParticlesEmitter::SHAPE_CONE:
		//nothing
		break;
	case CParticlesEmitter::SHAPE_EDGE:
		//nothing
		break;
	case CParticlesEmitter::SHAPE_SPHERE:
		//nothing
		break;
	case CParticlesEmitter::SHAPE_NOTHING:
		//nothing
		break;
	case CParticlesEmitter::SHAPE_CIRCLE:
		ForcesCircle(forces, emitter);
		break;
	default:
		//nothing
		break;

	}
}

