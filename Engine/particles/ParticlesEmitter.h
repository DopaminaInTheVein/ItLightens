#ifndef	INC_PARTICLES_EMITTER_H_
#define INC_PARTICLES_EMITTER_H_

class CParticlesEmitter {

public:
	struct Shape
	{
		enum Enum
		{
			eBOX = 0,
		};
	};

private:
	PxTransform			m_LocalPose;
	PxRigidDynamic*		m_FrameBody;

	PxReal				m_ExtentX;
	PxReal				m_ExtentY;
	Shape::Enum			m_Shape;

	//state derived quantities
	PxReal				m_EllipseRadius2;
	PxReal				m_EllipseConstX0;
	PxReal				m_EllipseConstX1;
	PxReal				m_EllipseConstY0;
	PxReal				m_EllipseConstY1;

	//only needed during step computations.
	PxVec3				m_BodyAngVel;
	PxVec3				m_BodyLinVel;
	PxVec3				m_BodyCenter;
	PxTransform			m_GlobalPose;
	PxVec3				m_LinMomentum;
	PxVec3				m_AngMomentum;

protected:

	PxVec3				m_RandomPos;
	PxReal				m_RandomAngle;
	PxReal				m_Velocity;
	PxReal				m_ParticleMass;
						 
	//derived quantities 
	PxU32				m_NumSites;
	PxU32				m_NumX;
	PxU32				m_NumY;
	PxReal				m_SpacingX;
	PxReal				m_SpacingY;
	PxReal				m_SpacingZ;
						 
	//only needed during step computations.
	PxVec3				m_AxisX;
	PxVec3				m_AxisY;
	PxVec3				m_AxisZ;
	PxVec3				m_BasePos;

public:
	CParticlesEmitter(Shape::Enum shape, PxReal extentX, PxReal extentY, PxReal spacing);
	~CParticlesEmitter();

	void				setLocalPose(const PxTransform& pose) { m_LocalPose = pose; }
	PxTransform			getLocalPose()						const { return m_LocalPose; }

	void				setFrameRigidBody(PxRigidDynamic* rigidBody) { m_FrameBody = rigidBody; }
	PxRigidDynamic*		getFrameRigidBody()					const { return m_FrameBody; }

	void 				setRandomPos(PxVec3 t) { m_RandomPos = t; }
	PxVec3 				getRandomPos()						const { return m_RandomPos; }

	void 				setRandomAngle(PxReal t) { m_RandomAngle = t; }
	PxReal 				getRandomAngle()					const { return m_RandomAngle; }

	void 				setVelocity(PxReal t) { m_Velocity = t; }
	PxReal 				getVelocity()						const { return m_Velocity; }

	// Used for two way interaction, zero meaning, there is none
	void				setParticleMass(PxReal m) { m_ParticleMass = m; }
	PxReal 				getParticleMass()						const { return m_ParticleMass; }

	void				step(PxParticleSystem& particles, PxReal dt, const PxVec3& externalAcceleration, PxReal maxParticleVelocity);
};

#endif