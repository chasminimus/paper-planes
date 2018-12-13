# Development

## Setup

I set out by trying to understand how to set up a 3D system in OpenFrameworks. It turns out, one of the examples they included does just that. The example was a swarm of particles bound by the spring force with different camera views. I mimicked the class structure of this example in my initial design of the project: there is a `Flock` class that subclasses the `ofNode` class so it has basic drawing capabilities as well as other nice features that nodes can have, such as the ability to be focused on by an `ofEasyCam`. The `Flock` class has subclasses that hold vectors for position, velocity, and acceleration. 

## 3D Things in 3D Space

From here I implemented basic motion and drawing for the particles in the system. The idea behind them is some simple calculus. The acceleration affects the velocity which affects the position. At this stage, all I had implemented was that they were created with random positions and velocities kept bouncing within a sphere. It wasn't computationally interesting, but it was something to show. At this stage, I had tried using cones to be able to point them in the direction of the particles' velocities, but the cone model was always off in its rotation when using `ofNode::lookAt()` because of how OpenFrameworks draws them, I guess. I never figured out how to correct this problem.

## Using the Force

Now things are getting interesting. I implemented the algorithms for computing the separation, alignment, and cohesion forces for the system. There's also a force for encouraging the particles to stay in a spherical range. It's written in a way that lets you control how much one force is weighted, so you can adjust it and get different results. With lots of particles in the system, it gets laggy since each one needs to go through the entire list to perform calculations. I have an idea on how to make this better I'll implement later.

## Taking Control

Next I added the GUI elements. I wanted to let users manipulate the force weights and other aspects of the system and see the effects in real time. The ofxDatGui library made this simple to do. I added sliders for the weights and a few system variables as well as a toggle controlling whether the particles were confined to a sphere or able to wrap around the edges of the region. The sliders can be bound to the actual variables with a convenient built in method, although the toggle switch had to done more manually.
I also discovered that the depth testing needs to be disabled for it to render correctly.

## Trying (& Failing) to be Efficient

I tried to implement bin-lattice spacial subdivision to increase efficiency. I think all the array operations and loops just kind of counteract that though. It gets slightly better using the bin system at least. Too many makes it even laggier. It works by splitting the region into cells and having the particles only compute using the particles in neighboring cells. I'm also beginning to realize that it might be better to hold a lsit of pointers to particles than just a list of particles for accessing them and passing them to functions.

## Internal Organization and Predators

I refactored pretty much everything to use pointers to particles instead of their indices in the list of particles. Now a lot of the force methods are consistent which is really nice. Then, I implemented predators. This required a new repel force method. I made predators a subclass of the normal particle class with higher max speed and max force. It was a quick lesson in virtual methods and polymorphism that caught me off guard. I added the new repelling force to the GUI to be controlled too, so if you wanted to not account for predators you can set its weight to 0.

## The End

With all the functionality in place, I finally used the paper plane model I made at the very beginning. The ofxAssimpModelLoader library seemed a little archaic, but I was able to avoid its strange rotation system by taking converting the model to an `ofMesh` using a method the library thankfully had.  I subclassed `ofNode` much like I did in the beginning and stored the mesh there so it could be transformed like a node. The model even looked correct when using `ofNode::lookAt()`! I changed up the background colors, and added documentation and polish.