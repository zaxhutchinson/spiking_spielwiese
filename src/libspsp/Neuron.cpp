#include"Neuron.hpp"
#include"Synapse.hpp"

namespace spsp {

    Neuron::Neuron() {
    }

    Neuron::Neuron(NT * nt) {
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
        current_output=0.0;

        EnableNoise(NoiseType::None);
    }

    Neuron::Neuron(Neuron && other) {
        *this = std::move(other);
    }
    Neuron & Neuron::operator=(Neuron && other) {
        if(this != &other) {
            v=other.v;
            v_prev=other.v_prev;
            u=other.u;
            cap=other.cap;
            vr=other.vr;
            vt=other.vt;
            k=other.k;
            vpeak=other.vpeak;
            a=other.a;
            b=other.b;
            c=other.c;
            d=other.d;
            baseline=other.baseline;
            alphabase=other.alphabase;
            spike_age_buffer=other.spike_age_buffer;
            max_spike_age=other.max_spike_age;
            current_output=other.current_output;
            i_syn= std::move(other.i_syn); other.i_syn.clear();
            o_syn= std::move(other.o_syn); other.o_syn.clear();
            noise=other.noise; 
            uniform_dist=other.uniform_dist;
            normal_dist=other.normal_dist;
            rng=std::move(other.rng); other.rng=nullptr;
        }
        return *this;
    }

    void Neuron::Reset() {
        spike_age_buffer.clear();
        v=v_prev=c;
        u=d;
    }

    void Neuron::Update(uint64_t time) {
        
        // Euler Method
        v = v_prev + (k * (v_prev-vr) * (v_prev-vt) - u +
            baseline + Input(time) + noise()) / cap;
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

        Output(time);
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

    void Neuron::Output(uint64_t time) {
        double c_output = GetCurrentOutputNormalized();
        for(lsptr<Synapse>::iterator it = o_syn.begin();
                it != o_syn.end(); ) {
            if(!(*it)->GetActive()) {
                //it = o_syn.erase(it);
            } else {
                (*it)->SetSignal(time, c_output);
                it++;
            }
        }
    }

    void Neuron::EnableNoise(NoiseType type, long seed, double val_a, double val_b) {
        EnableNoise(type,std::make_shared<std::mt19937_64>(seed),val_a,val_b);
    }
    void Neuron::EnableNoise(NoiseType type, sptr<std::mt19937_64> rng, double val_a, double val_b){
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

    double Neuron::Input(uint64_t time) {
        double input = 0.0;
        for(lsptr<Synapse>::iterator it = i_syn.begin(); it != i_syn.end(); ) {
            if(!(*it)->GetActive()) {
                //it = i_syn.erase(it);
            } else {
                input += (*it)->GetSignal(time);
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
    double Neuron::GetCurrentOutputNormalized() {
        return current_output / (1.0+std::abs(current_output));
    }

    double Neuron::C() { return c; }
}