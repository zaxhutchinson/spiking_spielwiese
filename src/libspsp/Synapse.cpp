#include"Synapse.hpp"
namespace spsp {
    /////////////////////////////////////////////////////////////////////////
    // SYNAPSE INTERFACE DEFINITIONS
    /////////////////////////////////////////////////////////////////////////
    Synapse::Synapse() {}
    bool Synapse::GetActive() const { return do_GetActive(); }
    void Synapse::SetActive(bool active) { do_SetActive(active); }
    double Synapse::GetSignal() const { return do_GetSignal(); }
    void Synapse::SetSignal(double signal) { do_SetSignal(signal); }
    void Synapse::RegisterNewPreSpike(uint64_t time) { do_RegisterNewPreSpike(time); }
    void Synapse::RegisterNewPostSpike(uint64_t time) { do_RegisterNewPostSpike(time); }

    /////////////////////////////////////////////////////////////////////////
    // SIMPLE SYNAPSE INTERFACE DEFINITIONS
    /////////////////////////////////////////////////////////////////////////
    SimpleSynapse::SimpleSynapse() 
        : active(true), weight(0.0), delay(0), time(0),
            signal_history_size(1),
            pre_spike_time(nullptr), post_spike_time(nullptr) {

        signal.push_back(0.0);
        
    }

    SimpleSynapse::SimpleSynapse(double weight, uint64_t signal_history_size)
        :active(true), weight(weight), delay(0), time(0),
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

    uint64_t SimpleSynapse::GetDelay() {
        return delay;
    }

    void SimpleSynapse::SetDelay(uint64_t delay) {
        this->delay = delay;
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

    bool SimpleSynapse::do_GetActive() const { 
        return active; 
    }

    void SimpleSynapse::do_SetActive(bool active) {
        this->active = active;
    }

    double SimpleSynapse::do_GetSignal() const {
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
    }   

    STDPSynapse::STDPSynapse(double strength, double weight, uint64_t signal_history_size) 
        : SimpleSynapse(weight,signal_history_size), strength(strength) {

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
                strength += (post_learn_rate * std::exp(-diff_t / post_learn_window));
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
        : active(true), weight(1.0), 
            count(std::make_shared<double>(0.0)) {
    }
    CountingSynapse::CountingSynapse(double weight, sptr<double> count)
        : active(true), weight(weight), 
            count(count) {
    
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
    double CountingSynapse::GetWeight() const {
        return weight;
    }
    void CountingSynapse::SetWeight(double weight) {
        this->weight = weight;
    }

    bool CountingSynapse::do_GetActive() const {
        return active;
    }
    void CountingSynapse::do_SetActive(bool active) {
        this->active = active;
    }
    double CountingSynapse::do_GetSignal() const {
        return 0.0;
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
    }
    PCSynapse::PCSynapse(double weight, sptr<double> count)
        : CountingSynapse(weight,count) {
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