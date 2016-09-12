local ffi = require 'ffi'

local lib = ffi.load('bullet.so')

-- grunt work

local bt = {}
bt.CollisionWorld = {}
bt.CollisionObject = {}
bt.CollisionShape = {}
bt.MotionState = {}
bt.DynamicsWorld = {
    super = 'CollisionWorld',
}
bt.DiscreteDynamicsWorld = {
    super = 'DynamicsWorld',
}
bt.RigidBody = {
    super = 'CollisionObject',
}
bt.DefaultMotionState = {
    super = 'MotionState',
}
bt.StaticPlaneShape = {
    super = 'CollisionShape',
}
bt.BoxShape = {
    super = 'CollisionShape',
}
bt.BvhTriangleMeshShape = {
    super = 'CollisionShape',
}
bt.TriangleIndexVertexArray = {}

local defs = {
'btDiscreteDynamicsWorld * btDiscreteDynamicsWorld_create();',
'void btDynamicsWorld_setGravity(void *btDynamicsWorld, float *gravity);',
'void btDynamicsWorld_addRigidBody(void *btDynamicsWorld, btRigidBody *body);',
'int btDynamicsWorld_stepSimulation(void *btDynamicsWorld, float timeStep, int maxSubSteps);',
'btStaticPlaneShape * btStaticPlaneShape_create(float *planeNormal, float planeConstant);',
'btBoxShape * btBoxShape_create(float *halfExtents);',
'void btCollisionShape_calculateLocalInertia(void *btCollisionShape, float mass, float *inertia);',
'btDefaultMotionState * btDefaultMotionState_create(float *ang, float *pos);',
'void btMotionState_getWorldTransform(void *btMotionState, float *ang, float *pos);',
'void btMotionState_setWorldTransform(void *btMotionState, float *ang, float *pos);',
'btRigidBody * btRigidBody_create(float mass, void *btMotionState, void *btCollisionShape, float *localInertia);',
'btMotionState * btRigidBody_getMotionState(void *btRigidBody);',
'void btBoxShape_getHalfExtentsWithoutMargin(void *btBoxShape, float *halfExtents);',
'btCollisionShape * btRigidBody_getCollisionShape(void *btRigidBody);',
'const char * btCollisionShape_getName(void *btCollisionShape);',
'btBvhTriangleMeshShape * btBvhTriangleMeshShape_create(void *btStridingMeshInterface, bool useAABB);',
'btTriangleIndexVertexArray * btTriangleIndexVertexArray_create(int numTriangles, int *triangles, int triangleStride, int numVertices, float *vertices, int vertexStride);',
'btCollisionObject * btCollisionWorld_rayTest(void *btCollisionWorld, float *from, float *to, float *out_closestHitFraction);',
'void btRigidBody_getCenterOfMassTransform(void *btRigidBody, float *ang, float *pos);',
'void btRigidBody_setCenterOfMassTransform(void *btRigidBody, float *ang, float *pos);',
'void btDynamicsWorld_removeRigidBody(void *btDynamicsWorld, btRigidBody *body);',
'void btRigidBody_applyCentralImpulse(void *btRigidBody, float *impulse);',
'void btRigidBody_applyCentralForce(void *btRigidBody, float *force);',
'void btCollisionObject_activate(void *btCollisionObject, bool forceActivation);',
'float btRigidBody_getInvMass(void *btRigidBody);',
}

-- metatable voodoo, efficient af

local result = setmetatable({}, {__index = lib})

for k,v in pairs(bt) do
    v.funcs = v.funcs or {}
end

for k,v in pairs(bt) do
    if v.super then
        setmetatable(v.funcs, {__index = bt[v.super].funcs})
    end
    ffi.cdef('typedef struct {} bt'..k..';')
    ffi.metatype('bt'..k, {__index = v.funcs})
    local create = 'bt'..k..'_create'
    result[k] = function(...)
        return lib[create](...)
    end
end

for i,v in ipairs(defs) do
    local _, _, class, func = string.find(v, '.* bt(%w+)_(%w+)%(.*%)%;')
    if not class or not func then
        error('function def syntax error: '..v)
    end
    local full_func = 'bt'..class..'_'..func
    bt[class].funcs[func] = function(...)
        return lib[full_func](...)
    end
end

ffi.cdef(table.concat(defs))

return result
