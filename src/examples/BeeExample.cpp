/*
Bee Dance Example, by Zachary Hutchinson
In this example, each Bee is given a specific number of leader bees
which it follows. The number of leaders is determined by NUM_LEADERS.

*/
#include<cstring>
#include<iostream>
#include<string>
#include<cmath>
#include<random>
#include<algorithm>

#include<SFML/Graphics.hpp>

#include"spspdef.hpp"
#include"Neuron.hpp"
#include"Synapse.hpp"
#include"NTemplate.hpp"


using namespace spsp;
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
const float WIDTH=1000.0f;
const float HEIGHT=1000.0f;
const int NUM_BEES=100;
const int NUM_LEADERS=NUM_BEES;
const double BEE_SYN_STRENGTH=5.0;
const float SPEED=10.0f;
const float SHIFT_AMT = 100.0f;
const float BEE_CIRCLE_SIZE = 32.0f;
const double ALPHABASE = 60.0;

inline bool RANDOM_LEADERS = false;
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
struct Bee : public sf::CircleShape  
{
    float radius;
    float x;
    float y;
    float vx;
    float vy;
    sptr<Synapse> x_pos;
    sptr<Synapse> x_neg;
    sptr<Synapse> y_pos;
    sptr<Synapse> y_neg;
    uptr<Neuron> E;
    uptr<Neuron> W;
    uptr<Neuron> S;
    uptr<Neuron> N;
    vsptr<Bee> leaders;

    Bee(float x, float y, float radius, sptr<NeuronTemplates> templates)
        : sf::CircleShape(radius)
    {
        this->radius = radius;
        this->setFillColor(sf::Color::Yellow);
        this->x=x; this->y=y;
        this->vx=0.0f;; this->vy=0.0f;
        this->setPosition({x,y});
        x_pos = std::make_shared<SimpleSynapse>(BEE_SYN_STRENGTH);
        x_neg = std::make_shared<SimpleSynapse>(-BEE_SYN_STRENGTH);
        y_pos = std::make_shared<SimpleSynapse>(BEE_SYN_STRENGTH);
        y_neg = std::make_shared<SimpleSynapse>(-BEE_SYN_STRENGTH);

        E = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
        W = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
        N = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
        S = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
        
        E->SetAlphaBase(ALPHABASE);
        W->SetAlphaBase(ALPHABASE);
        N->SetAlphaBase(ALPHABASE);
        S->SetAlphaBase(ALPHABASE);
    }
    void AddOwnSynapses() {
        E->AddInputSynapse(this->x_neg);
        W->AddInputSynapse(this->x_pos);
        N->AddInputSynapse(this->y_pos);
        S->AddInputSynapse(this->y_neg);
    }
    void SetLeader(sptr<Bee> leader) {
        AddOwnSynapses();
        leaders.push_back(leader);
        E->AddInputSynapse(leader->x_pos);
        W->AddInputSynapse(leader->x_neg);
        N->AddInputSynapse(leader->y_neg);
        S->AddInputSynapse(leader->y_pos);
    }
    void SetLeaders(vsptr<Bee> leaders) {
        for(unsigned i = 0; i < leaders.size(); i++) {
            this->leaders.push_back(leaders[i]);
            AddOwnSynapses();
            E->AddInputSynapse(leaders[i]->x_pos);
            W->AddInputSynapse(leaders[i]->x_neg);
            N->AddInputSynapse(leaders[i]->y_neg);
            S->AddInputSynapse(leaders[i]->y_pos);
        }
    }
    void Update(float dt, uint64_t time) {
        x_pos->SetSignal(time,x);
        x_neg->SetSignal(time,x);
        y_pos->SetSignal(time,y);
        y_neg->SetSignal(time,y);

        E->Update(time); W->Update(time); S->Update(time); N->Update(time);
        float dx = (E->GetCurrentOutput()-W->GetCurrentOutput())*dt*SPEED;
        float dy = (S->GetCurrentOutput()-N->GetCurrentOutput())*dt*SPEED;
        x += dx;
        y += dy;

        this->setPosition({x,y});
    }
    void ShiftPosition(float x, float y) {
        this->x += x;
        this->y += y;
        setPosition({this->x, this->y});
    }
    void Draw(sf::RenderWindow & window, float scale) {
        setRadius(radius/scale);
        setPosition({x/scale, y/scale});
        window.draw(*this);
    }
};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color);
void RandomizeBeeLeadersOne(vsptr<Bee> bees, std::mt19937_64 & rng);
void RandomizeBeeLeadersTwo(vsptr<Bee> bees, std::mt19937_64 & rng);
void ShiftBees(vsptr<Bee> bees, float x_amt, float y_amt);
void DrawLeaderLines(sf::RenderWindow & window, sptr<Bee> bee, float scale);

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


int main(int argc, char**argv) {
    
    for(int i = 0; i < argc; i++) {

        if(strcmp(argv[i], "-r")==0) {
            RANDOM_LEADERS = true;
            std::cout << "RANDOM_LEADERS: true\n";
        }

    }
    
    sf::Font font;
    sf::RenderWindow window;
    bool run=true;
    bool pause =true;
    bool leader_lines=false;

    std::random_device rd;
    std::mt19937_64 rng(rd());
    uint64_t time = 0;

    float scale = 1.0;

    vsptr<Bee> bees;

    int show_bee_index = 0;

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    if(!font.openFromFile("resources/saxmono.ttf")) {
        std::cout << "DISPLAY: Unable to load font\n";
    }
    sf::Text text(font);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(15);


    // sf::ContextSettings settings;
    // settings.antialiasingLevel = 4;

    window.create(sf::VideoMode({1000,1000}),
            "TEST SPSP",sf::Style::Titlebar);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sptr<NeuronTemplates> templates = 
            std::make_shared<NeuronTemplates>();
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

    std::uniform_real_distribution<float> rDist(-M_PI,M_PI);
    std::uniform_real_distribution<float> mDist(0.0f,500.0f);
    std::uniform_real_distribution<float> xDist(0.0f,WIDTH);
    std::uniform_real_distribution<float> yDist(0.0f,HEIGHT);

    float x = 0.0f;
    float y = 0.0f;

    for(unsigned i = 0; i < NUM_BEES; i++) {
        /////////////////////////
        // Spawn in circle
        //r = rDist(rng);
        //m = mDist(rng);
        //x = std::cos(r)*m;
        //y = std::sin(r)*m;
        /////////////////////////
        // Spawn in square
        x = xDist(rng);
        y = yDist(rng);

        sptr<Bee> bee = std::make_shared<Bee>(
            x,y, BEE_CIRCLE_SIZE, templates
        );
        bees.push_back(bee);
    }

    RandomizeBeeLeadersTwo(bees,rng);

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

    sf::Clock clock;
    sf::Time dt;
    float time_elapsed;

    while(run) {

        dt = clock.restart();
        time_elapsed = dt.asSeconds();

        //--------------------------------------------------
        // INPUT

        while(const std::optional event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch(keyPressed->scancode) {
                    case sf::Keyboard::Scancode::Q: run=false; break;
                    case sf::Keyboard::Scancode::P: pause=!pause; break;
                    case sf::Keyboard::Scancode::PageDown: scale *= 2.0; break;
                    case sf::Keyboard::Scancode::PageUp: scale /= 2.0; break;
                    case sf::Keyboard::Scancode::Up: ShiftBees(bees,0.0f,SHIFT_AMT); break;
                    case sf::Keyboard::Scancode::Down: ShiftBees(bees,0.0f,-SHIFT_AMT); break;
                    case sf::Keyboard::Scancode::Left: ShiftBees(bees,SHIFT_AMT,0.0f); break;
                    case sf::Keyboard::Scancode::Right: ShiftBees(bees,-SHIFT_AMT,0.0f); break;
                    case sf::Keyboard::Scancode::L: leader_lines=!leader_lines; break;
                    case sf::Keyboard::Scancode::RBracket: show_bee_index=(show_bee_index+1)%NUM_BEES; break;
                    case sf::Keyboard::Scancode::LBracket: show_bee_index=(show_bee_index-1+NUM_BEES)%NUM_BEES; break;
                    default: break;
                }
            }
        }


        //--------------------------------------------------
        // DRAW
        window.clear(sf::Color::Black);

        if(pause) {
            PrintMsg(window,text,"PAUSED",480.0f,10.0f,sf::Color::Red);
        } else {
            for(unsigned i = 0;i < bees.size();i++) {
                bees[i]->Update(time_elapsed,time);
            }
        }

        if(leader_lines) {
            DrawLeaderLines(window,bees[show_bee_index],scale);
        }

        for(unsigned i = 0;i < bees.size();i++) {
            bees[i]->Draw(window,scale);
        }

        window.display();

        //--------------------------------------------------
        // Time
        time++;

        

    }

    return 0;
}

void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y) {
    PrintMsg(win,text,msg,x,y,sf::Color::White);
}
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color) {
    text.setString(msg);
    text.setPosition({x,y});
    text.setFillColor(color);
    win.draw(text);
}

void RandomizeBeeLeadersOne(vsptr<Bee> bees, std::mt19937_64 & rng) {
    //std::shuffle(bees.begin(), bees.end(), rng);
    for(unsigned i = 0; i < bees.size(); i++) {

        sptr<Bee> follower = bees[i];
        sptr<Bee> leader = bees[ (i+1)%bees.size() ];

        follower->SetLeader(leader);

    }
}

void RandomizeBeeLeadersTwo(vsptr<Bee> bees, std::mt19937_64 & rng) {
   // std::shuffle(bees.begin(), bees.end(), rng);
    vec<int> indexes;
    for(int i = 0; i < bees.size(); i++) {
        indexes.push_back(i);
    }

    for(unsigned i = 0; i < bees.size(); i++) {

        vsptr<Bee> leaders;

        if(RANDOM_LEADERS) {
            std::shuffle(indexes.begin(), indexes.end(), rng);
            for(unsigned l = 1, n=0; n <= NUM_LEADERS; l++) {

                int m = indexes[l];
                if(m == i) continue;
                else n++;
                sptr<Bee> leader = bees[ m%bees.size() ];
                leaders.push_back(leader);

            }
        } else {
            for(unsigned l = 1; l <= NUM_LEADERS; l++ ) {

                sptr<Bee> leader = bees[ (i+l)%bees.size() ];
                leaders.push_back(leader);

            }
        }

        
        // for(unsigned l = 1, n=0; n <= NUM_LEADERS; l++) {
        //     if(RANDOM_LEADERS) {
        //         int m = indexes[l];
        //         if(m == i) continue;
        //         else n++;
        //         sptr<Bee> leader = bees[ m%bees.size() ];
        //         // sptr<Bee> leader = bees[ (i+l)%bees.size() ];
        //         leaders.push_back(leader);
        //     }
        //     else {
        //         sptr<Bee> leader = bees[ (i+l)%bees.size() ];
        //         leaders.push_back(leader);
        //     }
        // }

        sptr<Bee> follower = bees[i];
        

        follower->SetLeaders(leaders);

    }
}

void ShiftBees(vsptr<Bee> bees, float x_amt, float y_amt) {
    for(unsigned i = 0; i < bees.size(); i++) {
        bees[i]->ShiftPosition(x_amt,y_amt);
    }
}

void DrawLeaderLines(sf::RenderWindow & window, sptr<Bee> bee, float scale) {
    for(unsigned i = 0; i < bee->leaders.size(); i++) {
        sf::Vertex line[] =
        {
            sf::Vertex({sf::Vector2f(bee->x/scale,bee->y/scale)}),
            sf::Vertex({sf::Vector2f(bee->leaders[i]->x/scale, bee->leaders[i]->y/scale)})
        };
        line[0].color = sf::Color::Cyan;
        line[1].color = sf::Color::Blue;
        window.draw(line,2,sf::PrimitiveType::Lines);
    }
}