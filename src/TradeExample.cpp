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
const double STARTING_POPULATION=200.0;
const float SHIFT_AMT=100.0f;
const float WIDTH = 500.0f;
const float HEIGHT = 500.0f;
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
struct Planet;
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color);
void ShiftDisplay(vsptr<Planet> planets, float x_amt, float y_amt);
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
struct Planet {
    std::string name;
    float x;
    float y;
    sptr<double> population;
    sptr<double> work;
    vsptr<double> goods;
    vec<std::string> goods_names;
    int number_of_goods;

    sptr<Synapse> syn_pop_works;
    sptr<Synapse> syn_workforce_feedback;
    sptr<Synapse> syn_work_prod;
    sptr<Synapse> syn_work_cons;
    sptr<Synapse> syn_factory_input;
    
    vsptr<Synapse> syn_good_prods;
    vsptr<Synapse> syn_good_feedback;
    vsptr<Synapse> syn_surplus_input;
    vsptr<Synapse> syn_deficit_input;
    vsptr<Synapse> syn_surplus_effects;
    vsptr<Synapse> syn_deficit_effects;
    vsptr<Synapse> syn_goods_cons;
    
    uptr<Neuron> work_force;
    uptr<Neuron> factory;
    vuptr<Neuron> surplus_effects;
    vuptr<Neuron> deficit_effects;

    Planet(std::string _name,
            float _x, float _y,
            NeuronTemplates & nt, 
            vec<std::string> & _goods_names) {

        name = _name;
        x = _x; y = _y;
        population = std::make_shared<double>(STARTING_POPULATION);
        work = std::make_shared<double>(0.0);
        number_of_goods = _goods_names.size();
        goods_names = _goods_names;

        for(int i = 0; i < number_of_goods; i++) {
            goods.push_back(std::make_shared<double>(0.0));
        }

        syn_pop_works = std::make_shared<SimpleSynapse>(1.0);
        syn_workforce_feedback = std::make_shared<SimpleSynapse>(-50.0);
        syn_work_prod = std::make_shared<PCSynapse>(1.0,work);
        syn_work_cons = std::make_shared<PCSynapse>(1.0,work);
        syn_factory_input = std::make_shared<SimpleSynapse>(100.0);

        work_force = std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking"));
        factory = std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking"));

        work_force->SetAlphaBase(50.0);
        factory->SetAlphaBase(50.0);
        
        for(int i = 0; i < number_of_goods; i++) {
            syn_good_prods.push_back(std::make_shared<PCSynapse>(2.0,goods[i]));
            syn_good_feedback.push_back(std::make_shared<SimpleSynapse>(-50.0));
            factory->AddOutputSynapse(syn_good_prods[i]);
            factory->AddInputSynapse(syn_good_feedback[i]);

            surplus_effects.push_back(std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking")));
            deficit_effects.push_back(std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking")));

            surplus_effects[i]->SetAlphaBase(50.0);
            deficit_effects[i]->SetAlphaBase(50.0);

            syn_surplus_input.push_back(std::make_shared<SimpleSynapse>(10.0));
            syn_deficit_input.push_back(std::make_shared<SimpleSynapse>(-10.0));
            syn_surplus_effects.push_back(std::make_shared<PCSynapse>(1.0,population));
            syn_deficit_effects.push_back(std::make_shared<PCSynapse>(1.0,population));

            surplus_effects[i]->AddInputSynapse(syn_surplus_input[i]);
            surplus_effects[i]->AddOutputSynapse(syn_surplus_effects[i]);
            deficit_effects[i]->AddInputSynapse(syn_deficit_input[i]);
            deficit_effects[i]->AddInputSynapse(syn_deficit_effects[i]);

            syn_goods_cons.push_back(std::make_shared<PCSynapse>(1.0,goods[i]));

            work_force->AddInputSynapse(syn_goods_cons[i]);

        }

        work_force->AddInputSynapse(syn_pop_works);
        work_force->AddOutputSynapse(syn_work_prod);
        work_force->AddInputSynapse(syn_workforce_feedback);
        factory->AddInputSynapse(syn_work_cons);
        factory->AddInputSynapse(syn_factory_input);
    }

    void Update(uint64_t time) {

        //dynamic_cast<PCSynapse*>(syn_work_prod.get())->SetWeight(*population);
        //dynamic_cast<PCSynapse*>(syn_work_cons.get())->SetWeight(*population);

        syn_pop_works->SetSignal(*population);
        syn_factory_input->SetSignal(*work);
        syn_workforce_feedback->SetSignal(*work);

        for(int i = 0; i < number_of_goods; i++) {
            syn_surplus_input[i]->SetSignal(*(goods[i]));
            syn_deficit_input[i]->SetSignal(*(goods[i]));
            syn_good_feedback[i]->SetSignal(*(goods[i]));
        }

        work_force->Update(time);
        factory->Update(time);
        for(int i = 0; i < number_of_goods; i++) {
            surplus_effects[i]->Update(time);
            deficit_effects[i]->Update(time);
        }
    }
    void ShiftPosition(float x, float y) {
        this->x += x;
        this->y += y;
    }

    void Draw(sf::RenderWindow & window, sf::Text & text) {
        PrintMsg(window,text,"POP:  "+std::to_string(*population),x,y);
        PrintMsg(window,text,"WORK: "+std::to_string(*work),x,y+15.0f);
        for(int i = 0; i < number_of_goods; i++) {
            PrintMsg(window,text,goods_names[i]+": "+std::to_string( *(goods[i]) ), x, y+30.0f+15.0f*i);
        }
    }
};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
int main(int argc, char**argv) {

    sf::Text text;
    sf::Font font;
    sf::RenderWindow window;
    sf::Event event;
    bool run=true;
    bool pause =true;
    bool leader_lines=false;

    std::random_device rd;
    std::mt19937_64 rng(rd());
    uint64_t time = 0;

    float scale = 1.0;

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

    window.create(sf::VideoMode(1000,1000),
            "TEST SPSP",sf::Style::Titlebar,settings);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    NeuronTemplates templates;
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

    std::uniform_real_distribution<float> rDist(-M_PI,M_PI);
    std::uniform_real_distribution<float> mDist(0.0f,500.0f);
    std::uniform_real_distribution<float> xDist(0.0f,WIDTH);
    std::uniform_real_distribution<float> yDist(0.0f,HEIGHT);

    float x = 0.0f;
    float y = 0.0f;
    float r = 0.0f;
    float m = 0.0f;

    vec<std::string> goods_names = {"Food"};
    vsptr<Planet> planets;
    planets.push_back(std::make_shared<Planet>("Earth",500.0f,500.0f,templates,goods_names));

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
        while(window.pollEvent(event)) {
            if(event.type==sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Q: run=false; break;
                    case sf::Keyboard::P: pause=!pause; break;
                    case sf::Keyboard::PageDown: scale *= 2.0; break;
                    case sf::Keyboard::PageUp: scale /= 2.0; break;
                    case sf::Keyboard::Up: ShiftDisplay(planets,0.0f,SHIFT_AMT); break;
                    case sf::Keyboard::Down: ShiftDisplay(planets,0.0f,-SHIFT_AMT); break;
                    case sf::Keyboard::Left: ShiftDisplay(planets,SHIFT_AMT,0.0f); break;
                    case sf::Keyboard::Right: ShiftDisplay(planets,-SHIFT_AMT,0.0f); break;
                }
            }
        }


        //--------------------------------------------------
        // DRAW
        window.clear(sf::Color::Black);

        if(pause) {
            PrintMsg(window,text,"PAUSED",480.0f,10.0f,sf::Color::Red);
        } else {
            for(int i = 0; i < planets.size(); i++) {
                planets[i]->Update(time);
            }
        }

        for(int i = 0; i < planets.size(); i++) {
            planets[i]->Draw(window, text);
        }

        window.display();

        //--------------------------------------------------
        // Time
        time++;

    }


    return 0;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y) {
    PrintMsg(win,text,msg,x,y,sf::Color::White);
}
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color) {
    text.setString(msg);
    text.setPosition(x,y);
    text.setFillColor(color);
    win.draw(text);
}
void ShiftDisplay(vsptr<Planet> planets, float x_amt, float y_amt) {
    for(int i = 0; i < planets.size(); i++) {
        planets[i]->ShiftPosition(x_amt,y_amt);
    }
}