#ifndef __SCYTHE_BULLET_INCLUDE_H__
#define __SCYTHE_BULLET_INCLUDE_H__

#include "math/vector.h"
#include "math/quaternion.h"

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#define BV(v) (btVector3((v).x, (v).y, (v).z))
#define BQ(q) (btQuaternion((q).x, (q).y, (q).z, (q).w))

#endif