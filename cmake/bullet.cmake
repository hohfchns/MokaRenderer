find_package(Bullet REQUIRED)

include_directories(${BULLET_INCLUDE_DIR})

set(PHYSICS_LIBS BulletSoftBody BulletDynamics BulletCollision LinearMath)

