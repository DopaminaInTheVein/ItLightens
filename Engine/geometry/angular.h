#ifndef INC_GEOMETRY_ANGULAR_H_
#define INC_GEOMETRY_ANGULAR_H_

void getYawPitchFromVector(const VEC3& front, float* yaw, float *pitch);
VEC3 getVectorFromYawPitch(float yaw, float pitch);

#endif
