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

    // Input, intermediate, and output synapses
    std::shared_ptr<spsp::Synapse> syn_input = 
        std::make_shared<spsp::SimpleSynapse>(10.0);
    
    std::shared_ptr<spsp::Synapse> syn_inter =
        std::make_shared<spsp::SimpleSynapse>(1000.0);

    std::shared_ptr<spsp::Synapse> syn_output = 
        std::make_shared<spsp::SimpleSynapse>(10.0);

    // Connect Synapses to Neuron
    neuronA.AddInputSynapse(syn_input);
    neuronA.AddOutputSynapse(syn_inter);
    neuronB.AddInputSynapse(syn_inter);
    neuronB.AddOutputSynapse(syn_output);

    // Constant input of 30.0
    syn_input->SetSignal(30.0);

    // Time in milliseconds
    uint64_t time = 0;
    
    for(int i = 0; i < 1000; i++) {
    
        // Update the neuron
        neuronA.Update(time);
        neuronB.Update(time);
        
        // Output
        std::cout << "Time: " 
            << std::setw(4) << time << " --- A: " 
            << std::setw(12) << neuronA.GetCurrentOutput() << " "
            << std::setw(12) << syn_inter->GetSignal() << " --- B: "
            << std::setw(12) << neuronB.GetCurrentOutput() << " "
            << std::setw(12) << syn_output->GetSignal()
            << std::endl;
    
        // Time forward.
        time++;
    }
    
    return 0;
}