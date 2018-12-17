# Spiking Spielwiese

Spiking Spielwiese [SPSP] is a modern C++ library created to simplify the addition of Izhikevich spiking neuron models to larger projects. My hope is that this will facilitate their use in areas outside Computational Neuroscience and Artificial Intelligence research, such as video games and simulations.

As I work on the library I am adding examples both to test and to demonstrate my research into extending their usefulness. Presently, both library and examples are lumped together. The library files consist of (cpp/hpp):

* NTemplate
* Neuron
* Synapse
* spspdef

## Examples

All files with "Example" in the name are exactly that.

A makefile is included. You can build the current examples using:

`make trace`:  TraceExample is a spike detector and current multimeter GUI for the various neuron models. LEFT/RIGHT changes neuron models. UP/DOWN changes the amount of input into the model.

`make follow`: FollowExample shows how spiking neurons can be used to move an agent about in 2d space. Use WASD to move your agent. The others will follow.

`make target`: TargetExample is a variant of the follow example. The agents will follow the player around the screen and fire small shells in the direction of the player. Currently not shown, the "turret" object of each agent rotates before firing at the player. The turret, just like the non-player agents, are controlled with spiking neuron output. Same controls as FollowExample. There is no end. Being hit by the projectile does nothing and there is no way to win.

`make gridwar`: GridWarExample shows how "energy" from a single spiking neuron can spread throughout a lattice network and how sufficient energy is sellf-sustaining. The visual is comprised of N grids for N starting locations. Each grid (shown in a different color) inhibits activity in the other grids. Each growth will fight the others for space, hence the name. External input is provided to an initial neuron in each grid for 1000 time steps after which it is removed and persistent activity is maintained through recurrent excitation.

## Dependencies

The examples use SFML as their graphics library. I recommend you install the latest version of SFML. If during compilation you receive warnings about deprecated SFML functions dealing with color, you have a slightly out of date version. I believe the version in the Ubuntu repo is slightly behind.

## Neuron Class
The neuron class is an implementation of the Izhikevich neuron model. Instances of the neuron model are created by providing a template. Templates (see NTemplate class) provide the constants necessary to replicate various neuron types described in [Dynamical Systems in Neuroscience:
The Geometry of Excitability and Bursting by Eugene M. Izhikevich 2007](https://www.izhikevich.org/publications/dsn/index.htm). Many of the neuron types are represented in this project.

Apart from just replicating the voltage and reset equations, other aspects of incorporating neurons into a network must be attended to and that is the main benefit of the library.

Action potential output is modeled through an alpha function. The neuron model keeps a list of recent spike times and uses their age to determine the output, overlaying each alpha additively. The length of the alpha function corresponds to the constant `alphabase`. A smaller base value creates a shorter action potential. The age of each spike is maintained until `max_spike_age` after which it is discarded. Currently, `max_spike_age` is `10*alphabase` and is set in the Neuron's constructor. [COMMENT: I've a mind to move `max_spike_age` to the templates; however, `10*alphabase` covers the smearing of an action potential just fine. Presently, the only situation I envision where this would be insufficient is one where the program does not want smearing but a single blip.]

Each Neuron object stores lists of input and output synapses. Each call to update sums the values of all its input synapses, updates itself and pushes its current output into each of the output synapses.

Each Neuron object also has the capability to introduce random noise (uniform or normal) into its update method.

## NT Struct
Variables needed to initialize a new neuron object to particular prototype. As mentioned above the set of neuron types provided by SPSP come from Eugene Izhikevich's book. They are designed to mimic real-world neuron types. However, since the aim of SPSP is to open spiking neuron models to other uses, there is no reason new, unreal types cannot be created.

### Baseline
`baseline` provides a way to give a neuron object an always on current. The following is a list of baseline values for several of the neuron types that are just below the value which would produce a spike. This is not to say, these values will never produce a spike. Over longer periods of exposure, they might. The number in parenthesis will produce at least one spike per second.

* Regular Spiking:     51.0  (~52)
* Intrinsic Bursting:  346.0 (~347 or 348)
* Chattering:          160.0 (~161)
* Medium Spiny:        305.0 (306)
* Relay:               40.0  (~41)
* Fast Spiking:        120.0 (~125) Slightly oscillatory near the bifurfaction. Once it begins to spike, a value lower than 125 will sustain the behavior.

These values can be used to set a neuron just below the bifurcation point so that very small values will cause it to spike. These values were gathering through testing with the provided `trace_example`. A more thorough treatment of phase spaces, bifurcations and behavior can be found in the reference above.

## Synapse Classes
The Synapse class is an interface following the NVI pattern. Its client is the Neuron class and provides all the functionality required by that class.

Some synapse models were designed to connect neurons together, but most (if not all) can be used to connect spiking neurons to non-SNN objects. Each synapse's signal value can be set and retrieved by non-neuronal objects.

NOTE: Each time either the pre- or post-synaptic neuron fires, the current time is stored in the synapse. Time is modeled using a 64bit unsigned int. Only the most recent spike is stored in the neuron. In order to model the case when a synapse has not experienced a spike, the last spike time is stored in a pointer. A null value means a spike at an end has not registered a spike. This is necessary for classes that contain some form of synaptic plasticity.

### SimpleSynapse Class
A simple derivative of the interface. It connects two neurons in the simplest manner. The presynaptic input is scaled by a preset weight and stored for retrieval by the postsynaptic neuron.

### STDPSynapse Class
Derived from SimpleSynapse, it overrides enough functionality to provide unsupervised Hebbian learning.

Variables:

* learn_rate (pre and post): determines how fast a synapse is enhanced or depressed. It scales the typical Hebbian piecewise function. SPSP makes no assumptions whether a synapse changes due to Hebbian or anti-Hebbian activity. pre_learn_rate scales synaptic changes due to presynaptic activity. And post_learn_rate scales changes due to post-synaptic activity.

* learn_window (pre and post): The typical Hebbian function relies on a window of effect. If two connected neurons spike outside this window, there is no change. Again, the library makes no assumption that pre and post windows are symmetrical, providing a variable for both. The window value is also used as the denominator in the exponent.

### Counting Synapse

A counting synapse is an accumulator based on the number of pre-synaptic spikes it has seen. In other words, it is a spike detector coupled with incrementation. Counting synapses must be connected to neuron output. They will not count the activity of a post-synaptic neuron. Each time the connected neuron spikes, count increases by the value stored in weight.

NOTE: Count is stored as a shared pointer to a double. This was done to allow for the count to exist outside the synapse and remove a separate query. The synapse can be provided with a shared pointer upon creation.

### PC Synapse
PC stands for Producer-Consumer. It is a type of Counting Synapse that adds to the count when the presynaptic neuron spikes and subtracts from count when the post-synaptic neuron spikes. 