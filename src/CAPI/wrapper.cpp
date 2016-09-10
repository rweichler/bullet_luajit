#include "btBulletDynamicsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

// convenience functions

#define VEC3(ptr) (ptr == NULL ? btVector3(0, 0, 0) : btVector3(ptr[0], ptr[1], ptr[2]))
#define QUAT(ptr) (ptr == NULL ? btQuaternion(0, 0, 0, 1) : btQuaternion(ptr[0], ptr[1], ptr[2], ptr[3]))
#define TRANSFORM(ang, pos) \
    btTransform(    \
        QUAT(ang),  \
        VEC3(pos)   \
    )

static inline void get_transform(btTransform trans, float *ang, float *pos)
{
    btQuaternion q = trans.getRotation();
    btVector3 v = trans.getOrigin();

    ang[0] = q.x();
    ang[1] = q.y();
    ang[2] = q.z();
    ang[3] = q.w();

    pos[0] = v.x();
    pos[1] = v.y();
    pos[2] = v.z();
}

// world

btDiscreteDynamicsWorld * btDiscreteDynamicsWorld_create()
{
    // TODO memory leaks
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();

    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

    return new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
}

void btDiscreteDynamicsWorld_setGravity(btDiscreteDynamicsWorld *self, float *gravity)
{
    self->setGravity(VEC3(gravity));
}

void btDiscreteDynamicsWorld_addRigidBody(btDiscreteDynamicsWorld *self, btRigidBody *body)
{
    self->addRigidBody(body);
}

int btDiscreteDynamicsWorld_stepSimulation(btDiscreteDynamicsWorld *self, float timeStep, int maxSubSteps)
{
    return self->stepSimulation(timeStep, maxSubSteps);
}

// collision shapes

btCollisionShape * btStaticPlaneShape_create(float *planeNormal, float planeConstant)
{
    return new btStaticPlaneShape(VEC3(planeNormal), planeConstant);
}

btCollisionShape * btBoxShape_create(float *halfExtents)
{
    return new btBoxShape(VEC3(halfExtents));
}

btBvhTriangleMeshShape * btBvhTriangleMeshShape_create(btStridingMeshInterface *meshInterface, bool useAABB)
{
    return new btBvhTriangleMeshShape(meshInterface, useAABB);
}

const char * btCollisionShape_getName(btCollisionShape *self)
{
    return self->getName();
}

void btCollisionShape_calculateLocalInertia(btCollisionShape *self, float mass, float *inertia)
{
    assert(inertia != NULL);
    btVector3 tmp(inertia[0], inertia[1], inertia[2]);
    self->calculateLocalInertia(mass, tmp);
    inertia[0] = tmp.x();
    inertia[1] = tmp.y();
    inertia[2] = tmp.z();
}

void btBoxShape_getHalfExtentsWithoutMargin(btBoxShape *self, float *halfExtents)
{
    btVector3 vec = self->getHalfExtentsWithoutMargin();
    halfExtents[0] = vec.x();
    halfExtents[1] = vec.y();
    halfExtents[2] = vec.z();
}

// motion state ???

btMotionState * btDefaultMotionState_create(float *ang, float *pos)
{
    return new btDefaultMotionState(TRANSFORM(ang, pos));
}

void btMotionState_getWorldTransform(btMotionState *self, float *ang, float *pos)
{
    btTransform trans;
    self->getWorldTransform(trans);
    get_transform(trans, ang, pos);
}

// rigid body

btRigidBody * btRigidBody_create(float mass, btMotionState *motionState, btCollisionShape *collisionShape, float *localInertia)
{
    btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, collisionShape, VEC3(localInertia));
    return new btRigidBody(info);
}

btMotionState * btRigidBody_getMotionState(btRigidBody *self)
{
    return self->getMotionState();
}

btCollisionShape * btRigidBody_getCollisionShape(btRigidBody *self)
{
    return self->getCollisionShape();
}

// btTriangleIndexVertexArray

btTriangleIndexVertexArray * btTriangleIndexVertexArray_create( int numTriangles, int *triangles, int triangleStride,
                                                                int numVertices, float *vertices, int vertexStride)
{
    btScalar *v = (btScalar *)malloc(sizeof(btScalar) * numVertices * vertexStride);
    for(int i = 0; i < numVertices * vertexStride; i++) {
        v[i] = vertices[i];
    }
    return new btTriangleIndexVertexArray(numTriangles, triangles, triangleStride, numVertices, v, vertexStride);
}

#ifdef __cplusplus
} // extern "C"
#endif
