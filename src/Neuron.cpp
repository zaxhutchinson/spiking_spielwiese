#include"Neuron.hpp"
#include"Synapse.hpp"

namespace spsp {

    Neuron::Neuron() {
    }

    Neuron::Neuron(sptr<NT> nt) {
        cap = nt->cap;
        vr = nt->vr;
        vt = nt->vt;
        k = nt->k;
        vpeak = nt->vpeak;
        a = nt->a;
        b = nt->b;
        c = nt->c;
        d = nt->d;
        baseline = nt->baseline;
        v = nt->c;
        v_prev = nt->c;
        u = nt->d;
        alphabase = nt->alphabase;
        max_spike_age = alphabase*10;

        EnableNoise(NoiseType::None);
    }

    void Neuron::Reset() {
        spike_age_buffer.clear();
    }

    void Neuron::Update(uint64_t time) {
        
        // Euler Method
        v = v_prev + (k * (v_prev-vr) * (v_prev-vt) - u +
            baseline + Input() + noise()) / cap;
        u = u + a * (b *(v_prev-vr) - u);
        v_prev = v;

        if(v >= vpeak) {
            v_prev = c;
            u = u + d;
            UpdateSpikes(true);
            RegisterSpike(time);
        } 
        else {
            UpdateSpikes(false);
        }

        Output();
    }

    void Neuron::UpdateSpikes(bool new_spike) {
        current_output = 0.0;

        if(new_spike) spike_age_buffer.push_back(0);

        for(std::list<int>::iterator it = spike_age_buffer.begin();
                it != spike_age_buffer.end(); ) {
            
            if((*it) > max_spike_age) {
                it = spike_age_buffer.erase(it);
            } else {
                current_output += ( (*it) / alphabase) * 
                    std::exp( -( (*it) - alphabase) / alphabase);
                (*it)++;
                it++;
            }
        }
    }

    void Neuron::RegisterSpike(uint64_t time) {
        for(lsptr<Synapse>::iterator it = i_syn.begin();
                it != i_syn.end(); it++) {
            (*it)->RegisterNewPostSpike(time);
        }
        for(lsptr<Synapse>::iterator it = o_syn.begin();
                it != o_syn.end(); it++) {
            (*it)->RegisterNewPreSpike(time);
        }
    }

    void Neuron::Output() {
        for(lsptr<Synapse>::iterator it = o_syn.begin();
                it != o_syn.end(); ) {
            if(!(*it)->GetActive()) {
                it = o_syn.erase(it);
            } else {
                (*it)->SetSignal(current_output);
                it++;
            }
        }
    }

    void Neuron::EnableNoise(NoiseType type, double val_a, double val_b, long seed) {
        EnableNoise(type,val_a,val_b,std::make_shared<std::mt19937_64>(seed));
    }
    void Neuron::EnableNoise(NoiseType type, double val_a, double val_b, sptr<std::mt19937_64> rng){
        this->rng = rng;
        if(type==NoiseType::Normal) {
            normal_dist = std::normal_distribution<double>(val_a,val_b);
            noise = std::bind(&Neuron::normal_noise,this);
        } else if(type==NoiseType::Uniform) {
            uniform_dist = std::uniform_real_distribution<double>(val_a,val_b);
            noise = std::bind(&Neuron::uniform_noise,this);
        } else {
            noise = std::bind(&Neuron::no_noise,this);
        }
    }

    void Neuron::SetAlphaBase(double alphabase) {
        this->alphabase = alphabase;
        this->max_spike_age = alphabase*10.0;
    }

    void Neuron::AddInputSynapse(sptr<Synapse> synapse) {
        i_syn.push_back(synapse);
    }
    void Neuron::AddOutputSynapse(sptr<Synapse> synapse) {
        o_syn.push_back(synapse);
    }   

    double Neuron::no_noise() {
        return 0.0;
    }
    double Neuron::uniform_noise() {
        return uniform_dist(*rng);
    }
    double Neuron::normal_noise() {
        return normal_dist(*rng);
    }

    double Neuron::Input() {
        double input = 0.0;
        for(lsptr<Synapse>::iterator it = i_syn.begin(); it != i_syn.end(); ) {
            if(!(*it)->GetActive()) {
                it = i_syn.erase(it);
            } else {
                input += (*it)->GetSignal();
                it++;
            }
        }
        return input;
    }

    double Neuron::V() { return v; }
    double Neuron::U() { return u; }
    double Neuron::GetCurrentOutput() {
        return current_output;
    }

    double Neuron::C() { return c; }
}