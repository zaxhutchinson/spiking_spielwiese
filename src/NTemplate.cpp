#include"NTemplate.hpp"

namespace spsp {

    bool NeuronTemplates::LoadDefaultTemplates() {
        // Regular Spiking
        sptr<NT> nt = std::make_shared<NT>();
        nt->name="RegularSpiking";
        nt->cap=100.0;
        nt->vr=-60.0;
        nt->vt=-40.0;
        nt->vpeak=35.0;
        nt->k=0.7;
        nt->a=0.03;
        nt->b=-2.0;
        nt->c=-50.0;
        nt->d=100.0;
        nt->baseline=0.0;
        neuron_templates[nt->name] = nt;

        // Chattering
        nt = std::make_shared<NT>();
        nt->name="Chattering";
        nt->cap=50;
        nt->vr=-60.0;
        nt->vt=-40.0;
        nt->k=1.5;
        nt->vpeak=25.0;
        nt->a=0.03;
        nt->b=1.0;
        nt->c=-40.0;
        nt->d=150.0;
        nt->baseline=0.0;
        neuron_templates[nt->name]=nt;

        // Medium Spiny Neuron
        nt = std::make_shared<NT>();
        nt->name="MediumSpiny";
        nt->cap=50.0;
        nt->vr=-80.0;
        nt->vt=-25.0;
        nt->k=1.0;
        nt->vpeak=40.0;
        nt->a=0.01;
        nt->b=-20.0;
        nt->c=-55.0;
        nt->d=150.0;
        nt->baseline=0.0;
        neuron_templates[nt->name] = nt;

        // Relay
        nt = std::make_shared<NT>();
        nt->name="Relay";
        nt->cap=200.0;
        nt->vr=-60.0;
        nt->vt=-50.0;
        nt->k=1.6;
        nt->vpeak=35.0;
        nt->a=0.01;
        nt->b=0.0;
        nt->c=-50.0;
        nt->d=100.0;
        nt->baseline=0.0;
        neuron_templates[nt->name] = nt;

        // Fast Spiking
        nt = std::make_shared<NT>();
        nt->name="FastSpiking";
        nt->cap=20.0;
        nt->vr=-55.0;
        nt->vt=-40.0;
        nt->k=1.0;
        nt->vpeak=25.0;
        nt->a=0.15;
        nt->b=8.0;
        nt->c=-55.0;
        nt->d=200.0;
        nt->baseline=0.0;
        neuron_templates[nt->name] = nt;

        // Reticular Thalamic
        nt = std::make_shared<NT>();
        nt->name="ReticularThalamic";
        nt->cap=40.0;
        nt->vr=-65.0;
        nt->vt=-45.0;
        nt->k=0.25;
        nt->vpeak=0.0;
        nt->a=0.015;
        nt->b=0.2;
        nt->c=-55.0;
        nt->d=50.0;
        nt->baseline=0.0;
        neuron_templates[nt->name] = nt;

        // Intrinsic Bursting
        nt = std::make_shared<NT>();
        nt->name="IntrinsicBursting";
        nt->cap=150.0;
        nt->vr=-75.0;
        nt->vt=-45.0;
        nt->k=1.2;
        nt->vpeak=50.0;
        nt->a=0.01;
        nt->b=5.0;
        nt->c=-56.0;
        nt->d=130.0;
        nt->baseline=0.0;
        neuron_templates[nt->name] = nt;
    }

    sptr<NT> NeuronTemplates::GetNeuronTemplate(std::string name) {
        try {
            return neuron_templates.at(name);
        } catch (std::out_of_range e) {
            return nullptr;
        }
    }

    bool NeuronTemplates::AddNeuronTemplate(sptr<NT> nt) {
        return neuron_templates.emplace(nt->name, nt).second;
    }

    bool NeuronTemplates::AddNeuronTemplate(std::string name,
                            double cap,
                            double vr,
                            double vt,
                            double k,
                            double vpeak,
                            double a,
                            double b,
                            double c,
                            double d,
                            double baseline                        
    ) {
        sptr<NT> nt = std::make_shared<NT>();
        nt->cap = cap;
        nt->vr = vr;
        nt->vt = vt;
        nt->k = k;
        nt->vpeak = vpeak;
        nt->a = a;
        nt->b = b;
        nt->c = c;
        nt->d = d;
        nt->baseline = baseline;

        return neuron_templates.emplace(nt->name, nt).second;
    }
}