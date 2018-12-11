#include"Synapse.hpp"
namespace spsp {
    /////////////////////////////////////////////////////////////////////////
    // SYNAPSE INTERFACE DEFINITIONS
    /////////////////////////////////////////////////////////////////////////
    Synapse::Synapse() {}
    SynapseType Synapse::GetType() const { return do_GetType(); }
    void Synapse::SetType(SynapseType type) { do_SetType(type); }
    bool Synapse::GetActive() const { return do_GetActive(); }
    void Synapse::SetActive(bool active) { do_SetActive(active); }
    double Synapse::GetSignal(uint64_t delay) const { return do_GetSignal(delay); }
    void Synapse::SetSignal(double signal) { do_SetSignal(signal); }
    void Synapse::RegisterNewPreSpike(uint64_t time) { do_RegisterNewPreSpike(time); }
    void Synapse::RegisterNewPostSpike(uint64_t time) { do_RegisterNewPostSpike(time); }

    /////////////////////////////////////////////////////////////////////////
    // SIMPLE SYNAPSE INTERFACE DEFINITIONS
    /////////////////////////////////////////////////////////////////////////
    SimpleSynapse::SimpleSynapse() 
        : type(SynapseType::SIMPLE), active(true), weight(0.0), time(0),
            signal_history_size(1),
            pre_spike_time(nullptr), post_spike_time(nullptr) {

        signal.push_back(0.0);
    }

    SimpleSynapse::SimpleSynapse(double weight, uint64_t signal_history_size)
        : type(SynapseType::SIMPLE), active(true), weight(weight), time(0),
            signal_history_size(signal_history_size),
            pre_spike_time(nullptr), post_spike_time(nullptr) {

        for(uint64_t i = 0; i < signal_history_size; i++) {
            signal.push_back(0.0);
        }
    }

    SimpleSynapse::~SimpleSynapse() {
    }


    double SimpleSynapse::GetWeight() const {
        return weight;
    }

    void SimpleSynapse::SetWeight(double weight) {
        this->weight = weight;
    }

    uint64_t SimpleSynapse::GetTime() const {
        return time;
    }

    void SimpleSynapse::SetTime(uint64_t time) {
        this->time = time;
    }

    uint64_t SimpleSynapse::GetSignalHistorySize() const {
        return signal_history_size;
    }

    void SimpleSynapse::SetSignalHistorySize(uint64_t size) {
        signal_history_size = size;
        for(int i = 0; i < signal_history_size; i++) {
            signal.push_back(0.0);
        }
    }

    const uint64_t * SimpleSynapse::GetPreSpikeTime() const {
        return pre_spike_time.get();
    }

    void SimpleSynapse::SetPreSpikeTime(uint64_t time) {
        *pre_spike_time = time;
    }

    void SimpleSynapse::ResetPreSpikeTime() {
        pre_spike_time = nullptr;
    }

    const uint64_t * SimpleSynapse::GetPostSpikeTime() const {
        return post_spike_time.get();
    }

    void SimpleSynapse::SetPostSpikeTime(uint64_t time) {
        *post_spike_time = time;
    }

    void SimpleSynapse::ResetPostSpikeTime() {
        post_spike_time = nullptr;
    }

    SynapseType SimpleSynapse::do_GetType() const {
        return type;
    }

    void SimpleSynapse::do_SetType(SynapseType type) {
        this->type = type;
    }

    bool SimpleSynapse::do_GetActive() const { 
        return active; 
    }

    void SimpleSynapse::do_SetActive(bool active) {
        this->active = active;
    }

    double SimpleSynapse::do_GetSignal(uint64_t delay) const {
        // Positive modulo
        // (delay+1) is to get time-1 when delay is 0, default.
        int index = ( (time-(delay+1))%signal_history_size+signal_history_size )
                    % signal_history_size;
        return signal[index];
    }

    void SimpleSynapse::do_SetSignal(double signal) {
        time = (time+1)%signal_history_size;
        this->signal[time]=signal*weight;
    }

    void SimpleSynapse::do_RegisterNewPreSpike(uint64_t time) {
        if(pre_spike_time) *pre_spike_time = time;
        else pre_spike_time = std::make_unique<uint64_t>(time);
    }

    void SimpleSynapse::do_RegisterNewPostSpike(uint64_t time) {
        if(post_spike_time) *post_spike_time = time;
        else post_spike_time = std::make_unique<uint64_t>(time);
    }

    /////////////////////////////////////////////////////////////////////////
    // STDP SYNAPSE DEFINITIONS
    /////////////////////////////////////////////////////////////////////////
    STDPSynapse::STDPSynapse() 
        : SimpleSynapse() {
        
        type=SynapseType::STDP;
    }   

    STDPSynapse::STDPSynapse(double weight, uint64_t signal_history_size) 
        : SimpleSynapse(weight,signal_history_size) {
        
        type=SynapseType::STDP;

    }

    STDPSynapse::~STDPSynapse() {

    }

    void STDPSynapse::do_SetSignal(double signal) {
        time = (time+1)%signal_history_size;
        this->signal[time] = signal *
                (weight*strength) /
                (std::abs(weight) + std::abs(strength));
    }

    void STDPSynapse::do_RegisterNewPreSpike(uint64_t time) {
        if(pre_spike_time) *pre_spike_time = time;
        else pre_spike_time = std::make_unique<uint64_t>(time);

        if(post_spike_time) {
            double diff_t = static_cast<double>(time-*post_spike_time);
            if(diff_t<pre_learn_window) {
                strength += (pre_learn_rate * std::exp(-diff_t / pre_learn_window));
            }
        }
    }

    void STDPSynapse::do_RegisterNewPostSpike(uint64_t time) {
        if(post_spike_time) *post_spike_time = time;
        else post_spike_time = std::make_unique<uint64_t>(time);

        if(pre_spike_time) {
            double diff_t = static_cast<double>(time-*pre_spike_time);
            if(diff_t<post_learn_window) {
                strength += (post_learn_rate * std::exp(-diff_t / post_learn_rate));
            }
        }
    }



    double STDPSynapse::GetStrength() const {
        return strength;
    }

    void STDPSynapse::SetStrength(double strength) {
        this->strength = strength;
    }

    uint64_t STDPSynapse::GetPostLearnWindow() const {
        return post_learn_window;
    }

    uint64_t STDPSynapse::GetPreLearnWindow() const {
        return pre_learn_window;
    }

    void STDPSynapse::SetPostLearnWindow(uint64_t window) {
        post_learn_window = window;
    }

    void STDPSynapse::SetPreLearnWindow(uint64_t window) {
        pre_learn_window = window;
    }

    double STDPSynapse::GetPostLearnRate() const {
        return post_learn_rate;
    }

    double STDPSynapse::GetPreLearnRate() const {
        return pre_learn_rate;
    }

    void STDPSynapse::SetPostLearnRate(double rate) {
        post_learn_rate = rate;
    }

    void STDPSynapse::SetPreLearnRate(double rate) {
        pre_learn_rate = rate;
    }

    //////////////////////////////////////////////////////////////////////////

    CountingSynapse::CountingSynapse() 
        : type(SynapseType::COUNTING), active(true), weight(1.0), 
            count(std::make_shared<double>(0.0)), has_signal(false) {
    }
    CountingSynapse::CountingSynapse(double weight, sptr<double> count)
        : type(SynapseType::COUNTING), active(true), weight(weight), 
            count(count), has_signal(false) {
    
    }
    CountingSynapse::~CountingSynapse() {

    }
    double CountingSynapse::GetCount() const {
        return *count;
    }
    void CountingSynapse::SetCount(double count) {
        *this->count = count;
    }
    sptr<double> CountingSynapse::GetCountPtr() const {
        return count;
    }
    void CountingSynapse::SetCountPtr(sptr<double> count) {
        this->count = count;
    }
    bool CountingSynapse::GetHasSignal() const {
        return has_signal;
    }
    void CountingSynapse::SetHasSignal(bool has_signal) {
        this->has_signal = has_signal;
    }
    SynapseType CountingSynapse::do_GetType() const {
        return type;
    }
    bool CountingSynapse::do_GetActive() const {
        return active;
    }
    void CountingSynapse::do_SetActive(bool active) {
        this->active = active;
    }
    double CountingSynapse::do_GetSignal(uint64_t delay) const {
        if(has_signal) return *count;
        else return 0.0;
    }
    void CountingSynapse::do_SetSignal(double signal) {
        // Do nothing
    }
    void CountingSynapse::do_RegisterNewPreSpike(uint64_t time) {
        *count += weight;
    }
    void CountingSynapse::do_RegisterNewPostSpike(uint64_t time) {
        // Do nothing
    }

    /////////////////////////////////////////////////////////////////////////

    PCSynapse::PCSynapse() 
        : CountingSynapse() {

        type = SynapseType::PC;
    }
    PCSynapse::PCSynapse(double weight, sptr<double> count)
        : CountingSynapse(weight,count) {

        type = SynapseType::PC;
    }
    PCSynapse::~PCSynapse() {

    }
    void PCSynapse::do_RegisterNewPreSpike(uint64_t time) {
        *count += weight;
    }
    void PCSynapse::do_RegisterNewPostSpike(uint64_t time) {
        *count -= weight;
    }

    /////////////////////////////////////////////////////////////////////////

}