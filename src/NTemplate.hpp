#ifndef NTEMPLATE_HPP
#define NTEMPLATE_HPP

#include<memory>
#include<string>
#include<fstream>
#include<iostream>
#include<algorithm>
#include<map>
#include<stdexcept>

#include"spspdef.hpp"

namespace spsp {

    enum class NoiseType {
        None,
        Uniform,
        Normal
    };

    struct NT {
        std::string name;
        double cap;
        double vr;
        double vt;
        double k;
        double vpeak;
        double a;
        double b;
        double c;
        double d;
        double baseline;
        double alphabase;
    };

    struct NeuronTemplates {
        std::map<std::string,sptr<NT>> neuron_templates;

        bool LoadDefaultTemplates();
        sptr<NT> GetNeuronTemplate(std::string name);
        bool AddNeuronTemplate(sptr<NT> nt);
        bool AddNeuronTemplate(std::string name,
                                double cap,
                                double vr,
                                double vt,
                                double k,
                                double vpeak,
                                double a,
                                double b,
                                double c,
                                double d,
                                double baseline,
                                double alphabase                        
        );
    };
}

#endif