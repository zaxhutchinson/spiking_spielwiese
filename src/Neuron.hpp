/*
Neuron.hpp
by zax

*/
#ifndef NEURON_HPP
#define NEURON_HPP

#include<iostream>
#include<string>
#include<memory>
#include<functional>
#include<random>
#include<limits>
#include<list>

#include"NTemplate.hpp"
#include"Synapse.hpp"
#include"spspdef.hpp"

namespace spsp {
    class Neuron {

    public:
        Neuron();
        Neuron(sptr<NT> nt);

        void Reset();

        void Update(uint64_t time);
        void UpdateSpikes(bool new_spike);
        void RegisterSpike(uint64_t time);

        void EnableNoise(NoiseType type, double val_a=0.0, double val_b=1.0, long seed=0);
        void EnableNoise(NoiseType type, double val_a, double val_b, sptr<std::mt19937_64> rng);

        void SetAlphaBase(double alphabase);

        void AddInputSynapse(sptr<Synapse> synapse);
        void AddOutputSynapse(sptr<Synapse> synapse);

        double Input();
        void Output();

        double V();
        double U();
        double GetCurrentOutput();

        double C();

    private:
        double v;
        double u;
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

        std::list<int> spike_age_buffer;
        int max_spike_age;
        double current_output;

        lsptr<Synapse> i_syn;
        lsptr<Synapse> o_syn;

        std::function<double()> noise;
        std::uniform_real_distribution<double> uniform_dist;
        std::normal_distribution<double> normal_dist;
        sptr<std::mt19937_64> rng;

        double no_noise();
        double uniform_noise();
        double normal_noise();
    };
}
#endif