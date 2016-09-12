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

static inline void GET_VEC3(btVector3 v, float *out)
{
    if(out != NULL) {
        out[0] = v.x();
        out[1] = v.y();
        out[2] = v.z();
    }
}

static inline void GET_QUAT(btQuaternion q, float *out)
{
    if(out != NULL) {
        out[0] = q.x();
        out[1] = q.y();
        out[2] = q.z();
        out[3] = q.w();
    }
}

static inline void GET_TRANSFORM(btTransform trans, float *ang, float *pos)
{
    GET_QUAT(trans.getRotation(), ang);
    GET_VEC3(trans.getOrigin(), pos);
}

// world

btDiscreteDynamicsWorld * btDiscreteDynamicsWorld_create()
{
    // TODO memory leak
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();

    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

    return new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
}

void btDynamicsWorld_setGravity(btDynamicsWorld *self, float *gravity)
{
    self->setGravity(VEC3(gravity));
}

void btDynamicsWorld_addRigidBody(btDynamicsWorld *self, btRigidBody *body)
{
    self->addRigidBody(body);
}

void btDynamicsWorld_removeRigidBody(btDynamicsWorld *self, btRigidBody *body)
{
    self->removeRigidBody(body);
}

int btDynamicsWorld_stepSimulation(btDynamicsWorld *self, float timeStep, int maxSubSteps)
{
    return self->stepSimulation(timeStep, maxSubSteps);
}

const btCollisionObject * btCollisionWorld_rayTest(btCollisionWorld *self, float *from, float *to, float *out_closestHitFraction)
{
    btVector3 fromV = VEC3(from);
    btVector3 toV = VEC3(to);
    btCollisionWorld::ClosestRayResultCallback ray(fromV, toV);

    self->rayTest(fromV, toV, ray);
    if(ray.hasHit()) {
        if(out_closestHitFraction != NULL) {
            out_closestHitFraction[0] = ray.m_closestHitFraction;
        }
        return ray.m_collisionObject;
    } else {
        return NULL;
    }
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
    btVector3 tmp = VEC3(inertia);
    self->calculateLocalInertia(mass, tmp);
    GET_VEC3(tmp, inertia);
}

void btBoxShape_getHalfExtentsWithoutMargin(btBoxShape *self, float *halfExtents)
{
    GET_VEC3(self->getHalfExtentsWithoutMargin(), halfExtents);
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
    GET_TRANSFORM(trans, ang, pos);
}

void btMotionState_setWorldTransform(btMotionState *self, float *ang, float *pos)
{
    self->setWorldTransform(TRANSFORM(ang, pos));
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

void btRigidBody_getCenterOfMassTransform(btRigidBody *self, float *ang, float *pos)
{
    btTransform transform = self->getCenterOfMassTransform();
    GET_TRANSFORM(transform, ang, pos);
}

void btRigidBody_setCenterOfMassTransform(btRigidBody *self, float *ang, float *pos)
{
    self->setCenterOfMassTransform(TRANSFORM(ang, pos));
}

void btRigidBody_applyCentralImpulse(btRigidBody *self, float *impulse)
{
    self->applyCentralImpulse(VEC3(impulse));
}

void btRigidBody_applyCentralForce(btRigidBody *self, float *force)
{
    self->applyCentralForce(VEC3(force));
}

float btRigidBody_getInvMass(btRigidBody *self)
{
    return self->getInvMass();
}

void btCollisionObject_activate(btCollisionObject *self, bool forceActivation)
{
    self->activate(forceActivation);
}

// btTriangleIndexVertexArray

btTriangleIndexVertexArray * btTriangleIndexVertexArray_create( int numTriangles, int *triangles, int triangleStride,
                                                                int numVertices, float *vertices, int vertexStride)
{
    // TODO memory leak
    btScalar *v = (btScalar *)malloc(sizeof(btScalar) * numVertices * vertexStride);
    for(int i = 0; i < numVertices * vertexStride; i++) {
        v[i] = vertices[i];
    }
    return new btTriangleIndexVertexArray(numTriangles, triangles, triangleStride, numVertices, v, vertexStride);
}

#ifdef __cplusplus
} // extern "C"
#endif
