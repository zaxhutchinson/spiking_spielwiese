#include<iostream>
#include<string>
#include<chrono>
#include<thread>
#include<cmath>
#include<random>

#include<SFML/Graphics.hpp>

#include"spspdef.hpp"
#include"Neuron.hpp"
#include"Synapse.hpp"
#include"NTemplate.hpp"

using namespace spsp;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
const float LEADER_SPEED_Y = 500.0f;
const float LEADER_SPEED_X = 500.0f;
const float FOLLOWER_SPEED_Y = 30.0f;
const float FOLLOWER_SPEED_X = 30.0f;
const int num_followers = 10;
const float ACCELERATION = 100.0;
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
class Leader : public sf::CircleShape {
public:
    float x;
    float y;
    float vx;
    float vy;
    sptr<Synapse> x_pos;
    sptr<Synapse> x_neg;
    sptr<Synapse> y_pos;
    sptr<Synapse> y_neg;
    Leader(float x, float y) 
        : sf::CircleShape(10.0) {
            this->x=x; this->y=y;
            this->vx = 0.0f; this->vy = 0.0f;
            this->setPosition(x,y);
            this->setFillColor(sf::Color::Blue);
            x_pos = std::make_shared<SimpleSynapse>(5.0);
            x_neg = std::make_shared<SimpleSynapse>(-5.0);
            y_pos = std::make_shared<SimpleSynapse>(5.0);
            y_neg = std::make_shared<SimpleSynapse>(-5.0);
    }
    virtual void Update(float dt, uint64_t time) {
        x += vx*dt;
        y += vy*dt;

        x_pos->SetSignal(x);
        x_neg->SetSignal(x);
        y_pos->SetSignal(y);
        y_neg->SetSignal(y);

        this->setPosition(x,y);
    }
    void MoveE(float dt) { vx += ACCELERATION; }//x += LEADER_SPEED_X*dt;}
    void MoveW(float dt) { vx -= ACCELERATION; }//x -= LEADER_SPEED_X*dt;}
    void MoveN(float dt) { vy -= ACCELERATION; }//y -= LEADER_SPEED_Y*dt;}
    void MoveS(float dt) { vy += ACCELERATION; }//y += LEADER_SPEED_Y*dt;}
};
class Follower : public Leader {
public:
    uptr<Neuron> E;
    uptr<Neuron> W;
    uptr<Neuron> S;
    uptr<Neuron> N;
    Follower(float x, float y, sptr<NeuronTemplates> templates, sptr<Leader> leader)
        : Leader(x,y) {
            this->setFillColor(sf::Color::Green);
            E = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
            W = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
            N = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
            S = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
            E->SetAlphaBase(50.0);
            W->SetAlphaBase(50.0);
            N->SetAlphaBase(50.0);
            S->SetAlphaBase(50.0);

            E->AddInputSynapse(leader->x_pos);
            E->AddInputSynapse(this->x_neg);
            W->AddInputSynapse(leader->x_neg);
            W->AddInputSynapse(this->x_pos);
            N->AddInputSynapse(this->y_pos);
            N->AddInputSynapse(leader->y_neg);
            S->AddInputSynapse(this->y_neg);
            S->AddInputSynapse(leader->y_pos);
    } 
    virtual void Update(float dt, uint64_t time) override {
        x_pos->SetSignal(x);
        x_neg->SetSignal(x);
        y_pos->SetSignal(y);
        y_neg->SetSignal(y);

        E->Update(time); W->Update(time); S->Update(time); N->Update(time);
        float dx = (E->GetCurrentOutput()-W->GetCurrentOutput())*dt*FOLLOWER_SPEED_X;
        float dy = (S->GetCurrentOutput()-N->GetCurrentOutput())*dt*FOLLOWER_SPEED_Y;
        x += dx;
        y += dy;

        this->setPosition(x,y);
    }
};
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color);
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
int main(int argc, char**argv) {
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sf::Text text;
    sf::Font font;
    sf::RenderWindow window;
    sf::Event event;
    bool run=true;

    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_real_distribution<float> dist(200.0f,600.0f);

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    if(!font.loadFromFile("resources/saxmono.ttf")) {
        std::cout << "DISPLAY: Unable to load font\n";
    } else {
        text.setFont(font);
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(15);
    }

    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    window.create(sf::VideoMode(1100,1000),
            "TEST SPSP",sf::Style::Titlebar,settings);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    run = true;
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sptr<NeuronTemplates> templates = 
            std::make_shared<NeuronTemplates>();
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sptr<Leader> leader = std::make_shared<Leader>(500.0f,500.0f);
    //sptr<Follower> follower1 = std::make_shared<Follower>(200.0f,200.0f,templates,leader);
    vsptr<Follower> followers;
    for(int i = 0; i < num_followers; i++) {
        followers.push_back(std::make_shared<Follower>(
            dist(rng),dist(rng),templates,leader
        ));
    }
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sf::Clock clock;
    sf::Time dt;
    float time_elapsed;
    uint64_t time = 0;
    while(run) {

        dt = clock.restart();
        time_elapsed = dt.asSeconds();
        
        //--------------------------------------------------
        // INPUT
        while(window.pollEvent(event)) {
            if(event.type==sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Q: run=false; break;
                    case sf::Keyboard::W: leader->MoveN(time_elapsed); break;
                    case sf::Keyboard::A: leader->MoveW(time_elapsed); break;
                    case sf::Keyboard::S: leader->MoveS(time_elapsed); break;
                    case sf::Keyboard::D: leader->MoveE(time_elapsed); break;
                        
                }
            }
        }
        //--------------------------------------------------
        // UPDATE
        leader->Update(time_elapsed, time);
        //follower1->Update();
        for(int i = 0; i < num_followers; i++) {
            followers[i]->Update(time_elapsed, time);
        }
        //--------------------------------------------------
        // DRAW
        window.clear(sf::Color::Black);
        PrintMsg(window,text,"UPDT: "+std::to_string(time_elapsed),5,5);
        //window.draw(*follower1);
        for(int i = 0; i < num_followers; i++) {
            window.draw(*(followers[i]));
        }
        window.draw(*leader);

        window.display();

        time++;

        //while(std::chrono::duration_cast<ms>(std::chrono::steady_clock::now() - prev_time).count() < dt);
        //prev_time = std::chrono::steady_clock::now();
    }

    return 0;
}

void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y) {
    PrintMsg(win,text,msg,x,y,sf::Color::White);
}
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color) {
    text.setString(msg);
    text.setPosition(x,y);
    text.setFillColor(color);
    win.draw(text);
}