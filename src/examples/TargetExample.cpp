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
const int SCREEN_W = 1100;
const int SCREEN_H = 1000;
const float LEADER_SPEED_Y = 500.0f;
const float LEADER_SPEED_X = 500.0f;
const float FOLLOWER_SPEED_Y = 50.0f;
const float FOLLOWER_SPEED_X = 50.0f;
const int num_followers = 3;
const float ACCELERATION = 50.0;
const float PROJECTILE_VEL = 400.0f;
const int RELOAD_TIME = 100;
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
    sptr<Synapse> x_pos_sig;
    sptr<Synapse> x_neg_sig;
    sptr<Synapse> y_pos_sig;
    sptr<Synapse> y_neg_sig;
    Leader(float x, float y) 
        : sf::CircleShape(10.0) {
            this->x=x; this->y=y;
            this->vx = 0.0f; this->vy = 0.0f;
            this->setPosition(x,y);
            this->setFillColor(sf::Color::Blue);
            x_pos = std::make_shared<SimpleSynapse>(1.0);
            x_neg = std::make_shared<SimpleSynapse>(-1.0);
            y_pos = std::make_shared<SimpleSynapse>(1.0);
            y_neg = std::make_shared<SimpleSynapse>(-1.0);
            x_pos_sig = std::make_shared<SimpleSynapse>(50.0);
            x_neg_sig = std::make_shared<SimpleSynapse>(-50.0);
            y_pos_sig = std::make_shared<SimpleSynapse>(50.0);
            y_neg_sig = std::make_shared<SimpleSynapse>(-50.0);
    }
    virtual bool Update(float dt, uint64_t time) {
        x += vx*dt;
        y += vy*dt;

        x_pos->SetSignal(time,x);
        x_neg->SetSignal(time,x);
        y_pos->SetSignal(time,y);
        y_neg->SetSignal(time,y);
        x_pos_sig->SetSignal(time,x);
        x_neg_sig->SetSignal(time,x);
        y_pos_sig->SetSignal(time,y);
        y_neg_sig->SetSignal(time,y);

        this->setPosition(x,y);
        return true;
    }
    float X() { return x; }
    float Y() { return y; }
    void MoveE(float dt) { vx += ACCELERATION; }//x += LEADER_SPEED_X*dt;}
    void MoveW(float dt) { vx -= ACCELERATION; }//x -= LEADER_SPEED_X*dt;}
    void MoveN(float dt) { vy -= ACCELERATION; }//y -= LEADER_SPEED_Y*dt;}
    void MoveS(float dt) { vy += ACCELERATION; }//y += LEADER_SPEED_Y*dt;}
};
class Turret : public Leader {
public:
    uptr<Neuron> E;
    uptr<Neuron> W;
    uptr<Neuron> S;
    uptr<Neuron> N;
    int reload_time;
    float tx;
    float ty;
    float mag;
    Turret(float x, float y, sptr<NeuronTemplates> templates, sptr<Leader> leader, int init_reload)
        : Leader(x,y) {
            this->setRadius(5);
            this->setFillColor(sf::Color::Yellow);
            E = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
            W = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
            N = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
            S = std::make_unique<Neuron>(templates->GetNeuronTemplate("RegularSpiking"));
            E->SetAlphaBase(20.0);
            W->SetAlphaBase(20.0);
            N->SetAlphaBase(20.0);
            S->SetAlphaBase(20.0);

            x_pos = std::make_shared<SimpleSynapse>(50.0);
            x_neg = std::make_shared<SimpleSynapse>(-50.0);
            y_pos = std::make_shared<SimpleSynapse>(50.0);
            y_neg = std::make_shared<SimpleSynapse>(-50.0);

            E->AddInputSynapse(leader->x_pos_sig);
            E->AddInputSynapse(this->x_neg);
            W->AddInputSynapse(leader->x_neg_sig);
            W->AddInputSynapse(this->x_pos);
            N->AddInputSynapse(this->y_pos);
            N->AddInputSynapse(leader->y_neg_sig);
            S->AddInputSynapse(this->y_neg);
            S->AddInputSynapse(leader->y_pos_sig);

            reload_time=init_reload;
    } 
    virtual bool Update(float dt, uint64_t time) override {
        x_pos->SetSignal(time,x);
        x_neg->SetSignal(time,x);
        y_pos->SetSignal(time,y);
        y_neg->SetSignal(time,y);

        E->Update(time); W->Update(time); S->Update(time); N->Update(time);
        tx = (E->GetCurrentOutput()-W->GetCurrentOutput());
        ty = (S->GetCurrentOutput()-N->GetCurrentOutput());
        
        mag = std::sqrt(std::pow(tx,2.0)+std::pow(ty,2.0));
        if(mag > 0.0) {
            tx /= mag;
            ty /= mag;
        }
        //x += tx;
        //y += ty;

        this->setPosition(x,y);

        reload_time--;
        if(reload_time==0) {
            reload_time=RELOAD_TIME;
            return true;
        }
        else return false;
    }
    float TX() { return tx; }
    float TY() { return ty; }
};

class Follower : public Leader {
public:
    uptr<Neuron> E;
    uptr<Neuron> W;
    uptr<Neuron> S;
    uptr<Neuron> N;
    sptr<Turret> turret;
    Follower(float x, float y, sptr<NeuronTemplates> templates, sptr<Leader> leader, int init_reload)
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

            turret = std::make_shared<Turret>(x,y,templates,leader,init_reload);
    } 
    virtual bool Update(float dt, uint64_t time) override {
        x_pos->SetSignal(time,x);
        x_neg->SetSignal(time,x);
        y_pos->SetSignal(time,y);
        y_neg->SetSignal(time,y);

        E->Update(time); W->Update(time); S->Update(time); N->Update(time);
        float dx = (E->GetCurrentOutput()-W->GetCurrentOutput());
        float dy = (S->GetCurrentOutput()-N->GetCurrentOutput());
        x += dx*dt*FOLLOWER_SPEED_X;
        y += dy*dt*FOLLOWER_SPEED_Y;

        this->setPosition(x,y);

        turret->x = x;
        turret->y = y;
        return turret->Update(dt, time);
    }
    sptr<Turret> GetTurret() { return turret; }
};
class Projectile : public sf::CircleShape {
public:
    float x;
    float y;
    float vel;
    float dx;
    float dy;
    Projectile(float x, float y, float dx, float dy, float v) 
        : sf::CircleShape(2.0f) {
        setFillColor(sf::Color::Red);
        this->x = x;
        this->y = y;
        this->dx = dx;
        this->dy = dy;
        this->vel = v;
    }
    void Update(float dt) {
        x += dx*vel*dt;
        y += dy*vel*dt;
        setPosition(x,y);
    }
    float X() { return x; }
    float Y() { return y; }
};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color);
bool OffScreen(float x, float y);
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
int main() {
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sf::Text text;
    sf::Font font;
    sf::RenderWindow window;
    sf::Event event;
    bool run=true;
    uint64_t time=0;
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_real_distribution<float> dist(200.0f,600.0f);
    int wait_time=0;
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sptr<NeuronTemplates> templates = 
            std::make_shared<NeuronTemplates>();
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

    window.create(sf::VideoMode(SCREEN_W,SCREEN_H),
            "TEST SPSP",sf::Style::Titlebar,settings);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    run = true;

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sptr<Leader> leader = std::make_shared<Leader>(500.0f,500.0f);
    
    std::uniform_int_distribution<int> reloadDist(0,RELOAD_TIME);

    vsptr<Follower> followers;
    for(int i = 0; i < num_followers; i++) {
        followers.push_back(std::make_shared<Follower>(
            dist(rng),dist(rng),templates,leader,
            reloadDist(rng)
        ));
    }
    vsptr<Projectile> projectiles;
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sf::Clock clock;
    sf::Time dt;
    float time_elapsed;
    while(run) {

        dt = clock.restart();
        time_elapsed = dt.asSeconds();
        
        //--------------------------------------------------------
        // INPUT
        while(window.pollEvent(event)) {
            if(event.type==sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Q: run=false; break;
                    case sf::Keyboard::Up:
                        wait_time+=10; break;
                    case sf::Keyboard::Down:
                        if(wait_time>0) wait_time-=10; break;
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
        for(int i = 0; i < num_followers; i++) {
            bool fired = followers[i]->Update(time_elapsed, time);
            if(fired) {
                sptr<Turret> turret = followers[i]->GetTurret();
                
                if(turret->TX() != 0.0 || turret->TY() != 0.0) {
                    sptr<Projectile> projectile = std::make_shared<Projectile>(
                        followers[i]->X(), followers[i]->Y(),
                        turret->TX(), turret->TY(),
                        PROJECTILE_VEL
                    );
                    projectiles.push_back(projectile);
                }
                
            }
        }
        for(vsptr<Projectile>::iterator it = projectiles.begin();
                it != projectiles.end(); ) {
            (*it)->Update(time_elapsed);
            if(OffScreen((*it)->X(), (*it)->Y())) {
                it = projectiles.erase(it);
            } else {
                it++;
            }
        }
        //--------------------------------------------------
        // DRAW

        window.clear(sf::Color::Black);

        for(int i = 0; i < num_followers; i++) {
            window.draw(*(followers[i]));
            window.draw(*(followers[i]->turret));
        }
        for(int i = 0; i < projectiles.size(); i++) {
            window.draw(*projectiles[i]);
        }
        window.draw(*leader);

        PrintMsg(window,text,std::to_string(followers[0]->GetTurret()->TX()),10.0f,10.0f);
        PrintMsg(window,text,std::to_string(followers[0]->GetTurret()->TY()),10.0f,30.0f);

        window.display();

        time++;

        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
    }
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////



    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

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
bool OffScreen(float x, float y)  {
    if(x < 0.0f || x > SCREEN_W || y < 0.0f || y > SCREEN_H) {
        return true;
    } return false;
}