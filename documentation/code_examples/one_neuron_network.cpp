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

    // Create neuron
    spsp::Neuron neuron(nt.GetNeuronTemplate("RegularSpiking"));

    // Input and output synapses
    std::shared_ptr<spsp::Synapse> syn_input = 
        std::make_shared<spsp::SimpleSynapse>(10.0);
    
    std::shared_ptr<spsp::Synapse> syn_output = 
        std::make_shared<spsp::SimpleSynapse>(10.0);

    // Connect Synapses to Neuron
    neuron.AddInputSynapse(syn_input);
    neuron.AddOutputSynapse(syn_output);

    // Constant input of 10.0
    syn_input->SetSignal(10.0);

    // Time in milliseconds
    uint64_t time = 0;
    
    for(int i = 0; i < 1000; i++) {
    
        // Update the neuron
        neuron.Update(time);
        
        // Output
        std::cout << "Time: " 
            << std::setw(4) << time << "  " 
            << std::setw(12) << neuron.GetCurrentOutput() << "  "
            << std::setw(12) << syn_output->GetSignal()
            << std::endl;
    
        // Time forward.
        time++;
    }
    
    return 0;
}