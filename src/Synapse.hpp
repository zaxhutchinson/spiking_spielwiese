/*
Synapse.cpp
Zachary Hutchinson

Provides a series of Synapse classes to model different types of 
connections.

Synapse class itself is an interface following the NVI pattern. Its
client is the Neuron class, or, said another way, the interface's 
functionality provides the necessary information to the Neuron class.
*/

#ifndef SYNAPSE_HPP
#define SYNAPSE_HPP

#include<cmath>

#include"spspdef.hpp"

namespace spsp {
    ////////////////////////////////////////////////////////////////////
    // ENUM
    ////////////////////////////////////////////////////////////////////

    enum class SynapseType {
        SIMPLE,
        STDP,
        COUNTING,
        PC,
    };

    ////////////////////////////////////////////////////////////////////
    // INTERFACE
    ////////////////////////////////////////////////////////////////////
    class Synapse {
    public:
        virtual ~Synapse() = default;
        Synapse(Synapse const &) = delete;
        Synapse & operator=(Synapse const &) = delete;

        SynapseType GetType() const;
        bool GetActive() const;
        void SetActive(bool active);
        double GetWeight() const;
        void SetWeight(double weight);
        double GetSignal(uint64_t delay=0) const;
        void SetSignal(double signal);
        const uint64_t * GetPreSpikeTime() const;
        void RegisterNewPreSpike(uint64_t time);
        const uint64_t * GetPostSpikeTime() const;
        void RegisterNewPostSpike(uint64_t time);

    protected:
        Synapse();

    private:
        virtual SynapseType do_GetType() const = 0;
        virtual bool do_GetActive() const = 0;
        virtual void do_SetActive(bool active) = 0;
        virtual double do_GetWeight() const = 0;
        virtual void do_SetWeight(double weight) = 0;
        virtual double do_GetSignal(uint64_t delay=0) const = 0;
        virtual void do_SetSignal(double signal) = 0;
        virtual const uint64_t * do_GetPreSpikeTime() const = 0;
        virtual void do_RegisterNewPreSpike(uint64_t time) = 0;
        virtual const uint64_t * do_GetPostSpikeTime() const = 0;
        virtual void do_RegisterNewPostSpike(uint64_t time) = 0;

    };

    ////////////////////////////////////////////////////////////////////
    // CLASSES
    ////////////////////////////////////////////////////////////////////

    class SimpleSynapse : public Synapse {
    public:
        SimpleSynapse();
        SimpleSynapse(double weight, uint64_t signal_history_size=1);
        virtual ~SimpleSynapse();

    protected:
        SynapseType do_GetType() const final;
        bool do_GetActive() const final;
        void do_SetActive(bool active) final;
        double do_GetWeight() const final;
        void do_SetWeight(double weight) final;
        double do_GetSignal(uint64_t delay=0) const final;
        void do_SetSignal(double signal) override;
        const uint64_t * do_GetPreSpikeTime() const final;
        void do_RegisterNewPreSpike(uint64_t time) override;
        const uint64_t * do_GetPostSpikeTime() const final;
        void do_RegisterNewPostSpike(uint64_t time) override;

    protected:
        SynapseType type;
        bool active;
        double weight;
        uint64_t time;
        uint64_t signal_history_size;
        vec<double> signal;
        uptr<uint64_t> pre_spike_time;
        uptr<uint64_t> post_spike_time;
    };



    class STDPSynapse : public SimpleSynapse {
    public:
        STDPSynapse();
        STDPSynapse(double weight, uint64_t signal_history_size=1);
        virtual ~STDPSynapse();

        double GetStrength() const;
        void SetStrength(double strength);
        uint64_t GetPostLearnWindow() const;
        void SetPostLearnWindow(uint64_t window);
        uint64_t GetPreLearnWindow() const;
        void SetPreLearnWindow(uint64_t window);
        double GetPostLearnRate() const;
        void SetPostLearnRate(double rate);
        double GetPreLearnRate() const;
        void SetPreLearnRate(double rate);


    private:
        void do_SetSignal(double signal) final;
        void do_RegisterNewPreSpike(uint64_t time) final;
        void do_RegisterNewPostSpike(uint64_t time) final;

    private:
        SynapseType type;
        bool active;
        double weight;
        uint64_t time;
        uint64_t signal_history_size;
        vec<double> signal;
        uptr<uint64_t> pre_spike_time;
        uptr<uint64_t> post_spike_time;
        double strength;
        uint64_t post_learn_window;
        uint64_t pre_learn_window;
        double post_learn_rate;
        double pre_learn_rate;

    };

    class CountingSynapse : public Synapse {
    public:
        CountingSynapse();
        CountingSynapse(double weight, sptr<double> count);
        virtual ~CountingSynapse();

        virtual double GetCount() const;
        virtual void SetCount(double count);
        virtual sptr<double> GetCountPtr() const;
        virtual void SetCountPtr(sptr<double> count);
        virtual bool GetHasSignal() const;
        virtual void SetHasSignal(bool has_signal);

    protected:
        SynapseType do_GetType() const final;
        bool do_GetActive() const final;
        void do_SetActive(bool active) final;
        double do_GetWeight() const final;
        void do_SetWeight(double weight) final;
        double do_GetSignal(uint64_t delay=0) const final;
        void do_SetSignal(double signal) final;
        const uint64_t * do_GetPreSpikeTime() const final;
        void do_RegisterNewPreSpike(uint64_t time) override;
        const uint64_t * do_GetPostSpikeTime() const final;
        void do_RegisterNewPostSpike(uint64_t time) override;
    
    protected:
        SynapseType type;
        bool active;
        bool has_signal;
        double weight;
        sptr<double> count;
    };

    class PCSynapse : public CountingSynapse {
    public:
        PCSynapse();
        PCSynapse(double weight, sptr<double> count);
        virtual ~PCSynapse();
    private:
        void do_RegisterNewPreSpike(uint64_t time) final;
        void do_RegisterNewPostSpike(uint64_t time) final;
    private:
        SynapseType type;
        bool active;
        bool has_signal;
        double weight;
        sptr<double> count;
    };

}

#endif