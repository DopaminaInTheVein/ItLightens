#ifndef	INC_PARTICLES_EMITTER_H_
#define INC_PARTICLES_EMITTER_H_

class CParticlesEmitter {
public:
  enum EShapeEmitter {
    SHAPE_NOTHING = 0,
    SHAPE_CIRCLE,
    SHAPE_SPHERE,
    SHAPE_CONE,
    SHAPE_BOX,
    SHAPE_EDGE,
  };

  //Definition of types of emitter shapes
  //-----------------------------------------------------------------
  struct TShapeEmitter {
    VEC3 min;
    VEC3 max;
    VEC3 direction;
    float radius;
    float angle;
    TShapeEmitter()
    {
      min = VEC3(0, 0, 0);
      max = VEC3(0, 0, 0);
      radius = 1.0f;
      angle = 45.0f;
    }
    ~TShapeEmitter() {}

    void CreateBoxShape(VEC3 new_min, VEC3 new_max) {
      min = new_min;
      max = new_max;
    }

    void CreateConeShape(float new_angle, float new_rad = 1.0f) {
      radius = new_rad;
      angle = new_angle;
    }

    void CreateEdgeShape(VEC3 line_direction) {
      direction = line_direction;
    }

    void CreateCircleShape(float new_radius, VEC3 up_vector = VEC3(0, 1, 0)) {
      radius = new_radius;
      direction = up_vector;
    }

    void CreateSphereShape(float new_radius) {
      radius = new_radius;
    }
  };

  TShapeEmitter	m_shape_emitter;

private:
  //Emitter attributes
  //-----------------------------------------------------------------

  //shape
  int				m_shape_type;

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
  PxReal			m_randomLifeTimeMax;
  PxReal			m_randomLifeTimeMin;

  PxReal			m_delay_start;
  PxReal			m_delay_start_particles;

  //color
  PxVec4			m_color;
  PxReal			m_modifier_color;	//based on lifetime always

  PxVec3			m_front_transform;

  bool				m_collisions;
  bool				m_gravity;
  bool				m_usePhysx;

public:
  CParticlesEmitter() {
    m_initialPosition = PxVec3(0, 0, 0);
    m_RandomMaxPos = PxVec3(0, 0, 0);
    m_RandomMinPos = PxVec3(0, 0, 0);
    m_RandomMaxVel = PxVec3(0, 0, 0);
    m_RandomMinVel = PxVec3(0, 0, 0);

    m_shape_type = SHAPE_NOTHING;
    m_shape_emitter = TShapeEmitter();

    m_initialVelocity = PxVec3(0, 0, 0);
    m_velocityOverLifeTime = PxVec3(0, 0, 0);

    m_acceleration = PxVec3(0, 0, 0);
    m_accel_modifier = PxVec3(0, 0, 0);

    m_size = 1.0f;
    m_modifier_lifetime = -1.0f;

    m_lifeTimeMax = 1.0f;
    m_randomLifeTimeMax = 0.0f;
    m_randomLifeTimeMin = 0.0f;
    m_delay_start = 0.0f;			//absolute delay
    m_delay_start_particles = 5.0f;	//this delay is generated randomly internal by each particle to give continuity

    m_color = PxVec4(1, 1, 1, 1);
    m_modifier_color = 1.0f;

    m_gravity = false;
    m_collisions = false;
  }
  CParticlesEmitter(PxVec3 position, bool gravity = false, bool collisions = true) {
    m_initialPosition = position;
    m_shape_type = SHAPE_NOTHING;
    m_shape_emitter = TShapeEmitter();

    m_front_transform = PxVec3(0, 0, 0);

    m_RandomMaxPos = PxVec3(0, 0, 0);
    m_RandomMinPos = PxVec3(0, 0, 0);
    m_RandomMaxVel = PxVec3(0, 0, 0);
    m_RandomMinVel = PxVec3(0, 0, 0);

    m_initialVelocity = PxVec3(0, 0, 0);
    m_velocityOverLifeTime = PxVec3(0, 0, 0);

    m_acceleration = PxVec3(0, 0, 0);
    m_accel_modifier = PxVec3(0, 0, 0);

    m_size = 1.0f;
    m_modifier_lifetime = 1.0f;

    m_lifeTimeMax = 1.0f;
    m_randomLifeTimeMax = 0.0f;
    m_randomLifeTimeMin = 0.0f;
    m_delay_start = 0.0f;			//absolute delay
    m_delay_start_particles = 5.0f;	//this delay is generated randomly internal by each particle to give continuity

    m_color = PxVec4(1, 1, 1, 1);
    m_modifier_color = 1.0f;

    m_gravity = gravity;
    m_collisions = collisions;
  }
  ~CParticlesEmitter() {}

  //particles attached to bones
  bool      m_useSkeleton;

  //use physx for particle simulation
  bool* GetUsePhysx() { return &m_usePhysx; }
  void SetUsePhysx(bool new_physx) {
    m_usePhysx = new_physx;
  }

  //Position
  //-----------------------------------------------------------------
  PxVec3* GetPosition() { return &m_initialPosition; }
  void SetPosition(const PxVec3& new_position) {
    m_initialPosition = new_position;
  }

  PxVec3* GetPositionRandomMax() { return &m_RandomMaxPos; }
  void SetPositionRandomMax(const PxVec3& new_position) {
    m_RandomMaxPos = new_position;
  }

  PxVec3* GetPositionRandomMin() { return &m_RandomMinPos; }
  void SetPositionRandomMin(const PxVec3& new_position) {
    m_RandomMinPos = new_position;
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

  PxVec3* GetVelocityRandomMax() { return &m_RandomMaxVel; }
  void SetVelocityRandomMax(const PxVec3& new_vel) {
    m_RandomMaxVel = new_vel;
  }

  PxVec3* GetVelocityRandomMin() { return &m_RandomMinVel; }
  void SetVelocityRandomMin(const PxVec3& new_vel) {
    m_RandomMinVel = new_vel;
  }
  //-----------------------------------------------------------------

  //Acceleration
  //-----------------------------------------------------------------
  PxVec3* GetAcceleration() { return &m_acceleration; }
  void SetAcceleration(const PxVec3& new_accel) {
    m_acceleration = new_accel;
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

  float* GetLifeTimeRandomMax() { return &m_randomLifeTimeMax; }
  void SetLifeTimeRandmoMax(float new_max) {
    m_randomLifeTimeMax = new_max;
  }

  float* GetLifeTimeRandomMin() { return &m_randomLifeTimeMin; }
  void SetLifeTimeRandomMin(float new_min) {
    m_randomLifeTimeMin = new_min;
  }

  //-----------------------------------------------------------------

  //Delay at new start
  //-----------------------------------------------------------------
  float* GetDelayStart() { return &m_delay_start; }
  void SetDelayStart(float new_delay) {
    m_delay_start = new_delay;
  }
  //-----------------------------------------------------------------

  //size particle
  //-----------------------------------------------------------------
  float* GetSize() { return &m_size; }
  void SetSize(float new_size) {
    m_size = new_size;
  }

  float* GetModifierSize() { return &m_modifier_lifetime; }
  void SetModifierSize(float new_modif) {
    m_modifier_lifetime = new_modif;
  }
  //-----------------------------------------------------------------

  //color
  //-----------------------------------------------------------------

  PxVec4* GetColor() { return &m_color; }
  void SetColor(PxVec4 new_color) {
    m_color = new_color;
  }

  float* GetModifierColor() { return &m_modifier_color; }
  void SetModifierColor(float new_modifier) {
    m_modifier_color = new_modifier;
  }

  //Shape
  //-----------------------------------------------------------------
  void SetShape(int type_shape) {
    m_shape_type = type_shape;
  }

  int* GetShape() { return &m_shape_type; }

  PxVec3 GetInitialPosByShape(const VEC3& front, const VEC3& up);
  PxVec3 GetInitialVelByShape(const VEC3& front, const VEC3& up);

  //delays
  //-----------------------------------------------------------------
  float* GetAbsoluteDelay() { return &m_delay_start; }
  void SetAbsoluteDelay(float new_modifier) {
    m_delay_start = new_modifier;
  }

  float* GetRandomDelay() { return &m_delay_start_particles; }
  void SetRandomDelay(float new_modifier) {
    m_delay_start_particles = new_modifier;
  }

  //TESTING BONES
  //-----------------------------------------------------------------
  VEC3 testBones(int num, std::string* name, VEC3* traslacion);
};

#endif