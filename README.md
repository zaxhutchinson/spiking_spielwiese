# Spiking Spielwiese

Spiking Spielwiese [SPSP] is a modern C++ library created to simplify the addition of Izhikevich spiking neuron models to larger projects. My hope is that this will facilitate their use in areas outside Computational Neuroscience and Artificial Intelligence research, such as video games and simulations.

As I work on the library I am adding examples both to test and to demonstrate my research into extending their usefulness. Presently, both library and examples are lumped together. The library files consist of (cpp/hpp):

* NTemplate
* Neuron
* Synapse
* spspdef

All files with "Example" in the name are exactly that.

A makefile is included. You can build the current examples using:

<i>make trace</i>
TraceExample is a spike detector and current multimeter GUI for the various neuron models. LEFT/RIGHT changes neuron models. UP/DOWN changes the amount of input into the model.

<i>make follow</i>
FollowExample

<i>make target</i>
TargetExample

<i>make gridwar</i>
GridWarExample


