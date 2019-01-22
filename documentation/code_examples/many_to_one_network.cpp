#include<iostream>
#include<memory>
#include<cstdint>
#include<iomanip>

#include"NTemplate.hpp"
#include"Synapse.hpp"
#include"Neuron.hpp"

int main(int argc, char**argv) {

    // Holds all NTs.
    spsp::NeuronTemplates nt;

    // Create neurons
    spsp::Neuron neuronA(nt.GetNeuronTemplate("RegularSpiking"));
    spsp::Neuron neuronB(nt.GetNeuronTemplate("RegularSpiking"));
    spsp::Neuron neuronC(nt.GetNeuronTemplate("RegularSpiking"));

    // Input, intermediate, and output synapses
    std::shared_ptr<spsp::Synapse> syn_inputA = 
        std::make_shared<spsp::SimpleSynapse>(10.0);
    std::shared_ptr<spsp::Synapse> syn_inputB = 
        std::make_shared<spsp::SimpleSynapse>(10.0);
    
    std::shared_ptr<spsp::Synapse> syn_interA =
        std::make_shared<spsp::SimpleSynapse>(500.0);
    std::shared_ptr<spsp::Synapse> syn_interB =
        std::make_shared<spsp::SimpleSynapse>(500.0);

    std::shared_ptr<spsp::Synapse> syn_output = 
        std::make_shared<spsp::SimpleSynapse>(10.0);

    // Connect Synapses to Neuron
    neuronA.AddInputSynapse(syn_inputA);
    neuronA.AddOutputSynapse(syn_interA);
    neuronB.AddInputSynapse(syn_inputB);
    neuronB.AddOutputSynapse(syn_interB);
    neuronC.AddInputSynapse(syn_interA);
    neuronC.AddInputSynapse(syn_interB);
    neuronC.AddOutputSynapse(syn_output);

    // Constant input of 30.0 and 31.0
    syn_inputA->SetSignal(30.0);
    syn_inputB->SetSignal(31.0);

    // Time in milliseconds
    uint64_t time = 0;
    
    for(int i = 0; i < 1000; i++) {
    
        // Update the neuron
        neuronA.Update(time);
        neuronB.Update(time);
        neuronC.Update(time);
        
        // Output
        std::cout << "Time: " 
            << std::setw(4) << time << " --- A: " 
            << std::setw(12) << syn_interA->GetSignal() << " --- B: "
            << std::setw(12) << syn_interB->GetSignal() << " --- C: "
            << std::setw(12) << syn_output->GetSignal()
            << std::endl;
    
        // Time forward.
        time++;
    }
    
    return 0;
}