# LuaJIT bindings for bullet 2.83.7

NOTE: this is *not* complete. since it is written in C++ instead of C, i need to manually wrap each method in C. it's a huge work in progress.

i stripped out the build system and most of bullet3, because i need a unified shared lib

## How to build

1: Clone [aite](https://github.com/rweichler/aite) and put it somewhere

```
git clone https://github.com/rweichler/aite.git
```

2: Clone this and put it somewhere

```
git clone https://github.com/rweichler/bullet_luajit.git
```

3: cd into this repo and run aite

```
cd bullet_luajit
/PATH/TO/AITE/main.lua
```

This will create a file bullet.so that you can use in conjunction with bullet.lua in your project.

## How to expand this

* C wrapper is in bullet\_wrapper.cpp
* luajit ffi bindings in bullet.lua

workflow for this library:

basically you just write whatever functions you want to wrap in bullet\_wrapper.cpp, and then put a slightly modified decl in bullet.lua

## Examples

Honestly, if you want good examples, just look at bullet\_wrapper.cpp and bullet.lua. But if you need more explanation then here you go:

These are the three functions

```cpp
bool btCollisionShape::isConvex();
void btDynamicsWorld::removeRigidBody(btRigidBody *body);
btMotionState * btRigidBody::getMotionState();
btTransform btRigidBody::getCenterOfMassTransform();
```

you would write it in C as:
```c
bool btCollisionShape_isConvex(btCollisionShape *self) {
    return self->isConvex();
}
void btDynamicsWorld_removeRigidBody(btDynamicsWorld *self, btRigidBody *body){
    self->removeRigidBody(body);
}
btMotionState * btRigidBody_getMotionState(btRigidBody *self){
    return self->getMotionState();
}
void btRigidBody_getCenterOfMassTransform(btRigidBody *self, float *ang, float *pos)
{
    btTransform transform = self->getCenterOfMassTransform();
    GET_TRANSFORM(transform, ang, pos);
}
```

Note how, in the third one, I didn't return the btTransform. Implementing C *and* LuaJIT bindings for Bullet's math library would be a pain, so I just use simple `float *`'s.

and you would put it in bullet.lua as:
```c
bool btCollisionShape_isConvex(void *self);
void btDynamicsWorld_removeRigidBody(void *self, btRigidBody *body);
btMotionState * btRigidBody_getMotionState(void *self);
void btRigidBody_getMotionState(void *self, float *ang, float *pos);
```

^ notice the difference: self's type is now `void *`.

and the resulting lua call would look like
```lua
-- btCollisionShape::isConvex
local is_convex = shape:isConvex()
-- btDynamicsWorld::removeRigidBody
dynamics_world:removeRigidBody(body)
-- btRigidBody::getMotionState
local motion_state = body:getMotionState()
-- btRigidBody::getCenterOfMassTransform
local ang = ffi.new('float[3]')
local pos = ffi.new('float[3]')
body:getCenterOfMassTransform(ang, pos)
```

## Constructor example

Let's say you want to recreate this:
```cpp
btBoxShape *shape = new btBoxShape(halfExtents);
```

write this in bullet\_wrapper.cpp:
```c
btBoxShape * btBoxShape_create(float *halfExtents) {
    btVector3 tmp = VEC3(halfExtents);
    return new btBoxShape(tmp);
}
```

and this in bullet.lua:
```c
btBoxShape * btBoxShape_create(float *halfExtents);
```

note: it has to have _create at the end.

Then, you'll have to define the class at the top:
```lua
bt.BoxShape = {}
-- or, if it has a superclass
bt.BoxShape = {
    super = 'CollisionShape'
}
```

And you'd actually call that in Lua like:
```lua
local bullet = require 'bullet'
local halfExtents = ffi.new('float[3]', 2, 2, 2)
local shape = bullet.BoxShape(halfExtents)
```
