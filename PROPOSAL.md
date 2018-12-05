# Project Proposal

## The Gist

My final project will be a paper plane simulation. More specifically, it will be a flocking algorithm simulation using lots of individual little agents (paper planes in this case) that move autonomously, steering themselves following a set of three main rules:

1. Separation - steer to avoid colliding with neighbors
2. Alignment - steer to match the direction of your neighbors
3. Cohesion - steer towards the center of your neighbors

These rules operate as vector-valued functions that affect a paper plane's acceleration (and in turn velocity and position). Each of the paper planes will follow these rules, and together in a group a flocking system emerges. [Here's](https://p5js.org/examples/simulate-flocking.html) an example of this. The rules can be added with different weights, allowing control over how the system acts.

My paper plane simulation will be a 3D version of this, using 3D graphics. I'll implement sliders to let users control the weights of the different rules and see how it affects the system, as well as any other control factors I can think of. The nice thing about the idea is there's a lot of interesting ways it can be expanded on!

## Resources

### Prior Experience

I've done flocking before in just 2D using JavaScript and p5.js, so the ideas at work here aren't entirely foreign to me and I can focus more on implementation and extra features than grasping the concepts behind it.

### Libraries

- ofx3DModelLoader? (use a paper plane model instead of drawing it with primitves)
- ~~ofxGui for control sliders/options~~
- ofxDatGui for _nice_ controls/sliders

### Links

- [Autonomous Agents](https://natureofcode.com/book/chapter-6-autonomous-agents/)

## Extra Ideas

- Improve efficiency by implementing bin-lattice spatial subdivision (split the simulation space into many cells and have the paper planes do their computations locally to whatever cell they're in)
- Include a flowfield (as well as visualization) as an additional factor affecting paper plane movement
- Implement more complicated dynamics
  - 'Predator' planes (uh, pretend it makes sense) that will hunt normal planes which in turn try to steer away from predators
  - Food objects for planes to seek for
    - Eating food increases a plane's mass which makes their acceleration lower and the plane more susceptible to predators
  - Different colors/groups of planes that try to stay with/avoid those of the same group
  - Extra extra ideas
    - Planes can 'remember' things that affects their steering
    - Some kind of nesting? A flock of planes could become one single 'organism' and those organisms could also flock