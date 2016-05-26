#ifndef	INC_PARTICLES_EMITTER_H_
#define INC_PARTICLES_EMITTER_H_

struct TParticlesEmitter {
	
	//Position
	PxVec3			m_RandomMaxPos;		//randoms are modifiers from initalPosition
	PxVec3			m_RandomMinPos;

	PxVec3			m_initialPosition;

	//velocity
	PxVec3			m_RandomMaxVel;	//randoms are modifiers from initialVelocity
	PxVec3			m_RandomMinVel;

	PxVec3			m_initialVelocity;	
	PxVec3			m_velocityOverLifeTime;

	//velocity changes (acceleration) on particle lifetime
	PxVec3			m_acceleration;
	PxVec3			m_accel_modifier;

	//size
	PxReal			m_size;
	PxReal			m_modifier_lifetime;

	//lifeTime
	PxReal			m_lifeTimeMax;
	PxReal			m_delay_start;

	//color
	PxReal			m_modifier_color;	//based on lifetime always

	bool			m_collisions;
	bool			m_gravity;
	

public:
	TParticlesEmitter() {
		m_initialPosition = PxVec3(0, 0, 0);
		m_RandomMaxPos = PxVec3(0, 0, 0);
		m_RandomMinPos = PxVec3(0, 0, 0);

		m_gravity = false;
		m_collisions = false;
	}
	TParticlesEmitter(PxVec3 position, bool gravity = false, bool collisions = true) {
		m_RandomMaxPos = PxVec3(0,0,0);
		m_RandomMinPos = PxVec3(0, 0, 0);

		m_initialPosition = position;
		m_gravity = gravity;
		m_collisions = collisions;
	}
	~TParticlesEmitter() {}

	//Position
	//-----------------------------------------------------------------
	PxVec3* GetPosition() { return &m_initialPosition; }
	void SetPosition(const PxVec3& new_position) {
		m_initialPosition = new_position;
	}
	//-----------------------------------------------------------------

	//Velocity
	//-----------------------------------------------------------------
	PxVec3* GetVelocity() { return &m_initialVelocity; }
	void SetVelocity(const PxVec3& new_velocity) {
		m_initialVelocity = new_velocity;
	}

	PxVec3* GetVelocityModifier() { return &m_velocityOverLifeTime; }
	void SetVelocityModifier(const PxVec3& new_modif) {
		m_velocityOverLifeTime = new_modif;
	}
	//-----------------------------------------------------------------

	//Acceleration
	//-----------------------------------------------------------------
	PxVec3* GetAcceleration() { return &m_acceleration; }
	void SetAcceleration(const PxVec3& new_accel) {
		m_initialVelocity = new_accel;
	}

	PxVec3* GetAccelModifier() { return &m_accel_modifier; }
	void SetAccelModifier(const PxVec3& new_modif) {
		m_accel_modifier = new_modif;
	}
	//-----------------------------------------------------------------

	//Gravity
	//-----------------------------------------------------------------
	bool* GetGravity() { return &m_gravity; }
	void SetGravity(bool new_gravity) {
		m_gravity = new_gravity;
	}
	//-----------------------------------------------------------------

	//Collisions
	//-----------------------------------------------------------------
	bool* GetCollisions() { return &m_collisions; }
	void SetCollisions(bool new_collisions) {
		m_collisions = new_collisions;
	}
	//-----------------------------------------------------------------

	//LifeTime
	//-----------------------------------------------------------------
	float* GetLifeTime() { return &m_lifeTimeMax; }
	void SetLifeTime(float new_lifeTime) {
		m_lifeTimeMax = new_lifeTime;
	}
	//-----------------------------------------------------------------

	//Delay at new start
	//-----------------------------------------------------------------
	float* GetDelayStart() { return &m_delay_start; }
	void SetDelayStart(float new_delay) {
		m_delay_start = new_delay;
	}
	//-----------------------------------------------------------------

	//Modifier texture color, base on lifetime always
	//-----------------------------------------------------------------
	float* GetModifierColor() { return &m_modifier_color; }
	void SetModifierColor(float new_modifier) {
		m_modifier_color = new_modifier;
	}
	//-----------------------------------------------------------------

};

#endif