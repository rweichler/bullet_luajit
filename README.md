# LuaJIT bindings for bullet 2.83.7

NOTE: this is *not* complete. since it is written in C++ instead of C, i need to manually wrap each method in C. it's a huge work in progress.

i stripped out the build system and most of bullet3, because i need a unified shared lib

## How to build

1. Clone [aite](https://github.com/rweichler/aite) and put it somewhere

```
git clone https://github.com/rweichler/aite.git
```

2. Clone this and put it somewhere

```
git clone https://github.com/rweichler/bullet_luajit.git
```

3. cd into this repo and run aite

```
cd bullet_luajit
/PATH/TO/AITE/main.lua
```

## How to expand this

* C wrapper is in bullet_wrapper.cpp
* luajit ffi bindings in bullet.lua

workflow for this library:

basically you just write whatever functions you want to wrap in wrapper.cpp, and then put a slightly modified decl in bullet.lua

## Examples

Honestly, if you want good examples, just look at bullet\_wrapper.cpp and bullet.lua. But if you need more explanation then here you go:

```cpp
bool btCollisionShape::isConvex();
void btDynamicsWorld::removeRigidBody(btRigidBody *body);
btMotionState * btRigidBody::getMotionState();
```

you would write it in C as:
```c
bool btCollisionShape_isConvex(btCollisionShape *self){ ... }
void btDynamicsWorld_removeRigidBody(btDynamicsWorld *self, btRigidBody *body){ ... }
btMotionState * btRigidBody_getMotionState(btRigidBody *self){ ... }
```

and you would put it in bullet.lua as:
```c
bool btCollisionShape_isConvex(void *self);
void btDynamicsWorld_removeRigidBody(void *self, btRigidBody *body);
btMotionState * btRigidBody_getMotionState(void *self);
```

^ notice the difference: make self's type `void *`.

and the resulting lua call would look like
```lua
local is_convex = shape:isConvex()
dynamics_world:removeRigidBody(body)
local motion_state = body:getMotionState()
```

MAKE SURE THE CLASSES YOU USE ARE DEFINED AT THE TOP OF `bullet.lua`! And set the right superclass if you want to inherit methods.

## Constructor example

Let's say you want to recreate this:
```cpp
btBoxShape *shape = new btBoxShape(halfExtents);
```

write this in bullet\_wrapper.cpp:
```c
btBoxShape * btBoxShape_create(float *halfExtents){ ... }
```

and this in bullet.lua:
```c
btBoxShape * btBoxShape_create(float *halfExtents);
```

it has to have _create at the end.

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
local shape = bullet.BoxShape(halfExtents)
```

also i use regular float *'s for my vec3s, mat4s, etc so any function that returns a Vector3 or Quaternion or something i use as a pass-by-argument with just a float *
