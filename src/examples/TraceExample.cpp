#include<iostream>
#include<string>
#include<chrono>
#include<thread>
#include<cmath>

#include<SFML/Graphics.hpp>

#include"spspdef.hpp"
#include"Neuron.hpp"
#include"Synapse.hpp"
#include"NTemplate.hpp"

using namespace spsp;

struct Network {
    uptr<Neuron> n1;
    //uptr<Neuron> n2;
    sptr<Synapse> s1;
    sptr<Synapse> s2;
    //sptr<Synapse> s3;
};

void BuildNetwork(sptr<Network> network, NeuronTemplates & templates, int nt_index);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color);
void PrintTrace(sf::RenderWindow & win, sf::CircleShape & dot, double output[], double v[]);
void PrintGrid(sf::RenderWindow & win, sf::Text & text);
void PrintHeader(sf::RenderWindow & win, sf::Text & text);


///////////////////////////////////////////////////////////////////
const int num_neuron_types = 7;
std::string neuron_types[] = {
    "RegularSpiking",
    "Chattering",
    "MediumSpiny",
    "Relay",
    "FastSpiking",
    "ReticularThalamic",
    "IntrinsicBursting"
};

sf::Vertex borders[] = {
    sf::Vertex(sf::Vector2f(50,100)),       // LEFT
    sf::Vertex(sf::Vector2f(50,900)),
    sf::Vertex(sf::Vector2f(1050,100)),     // RIGHT
    sf::Vertex(sf::Vector2f(1050,900)),
    sf::Vertex(sf::Vector2f(50,100)),       // TOP
    sf::Vertex(sf::Vector2f(1050,100)),
    sf::Vertex(sf::Vector2f(50,900)),       // BOTTOM
    sf::Vertex(sf::Vector2f(1050,900))
};
sf::Vertex hmarkers[] = {
    sf::Vertex(sf::Vector2f(50,800)),       // ONE
    sf::Vertex(sf::Vector2f(1050,800)),
    sf::Vertex(sf::Vector2f(50,700)),       // TWO
    sf::Vertex(sf::Vector2f(1050,700)),
    sf::Vertex(sf::Vector2f(50,600)),       // THREE
    sf::Vertex(sf::Vector2f(1050,600)),
    sf::Vertex(sf::Vector2f(50,500)),       // FOUR
    sf::Vertex(sf::Vector2f(1050,500)),
    sf::Vertex(sf::Vector2f(50,400)),       // FIVE
    sf::Vertex(sf::Vector2f(1050,400)),     
    sf::Vertex(sf::Vector2f(50,300)),       // SIX
    sf::Vertex(sf::Vector2f(1050,300)),
    sf::Vertex(sf::Vector2f(50,200)),       // SEVEN
    sf::Vertex(sf::Vector2f(1050,200))
};


int main(int argc, char**argv) {

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    sf::Text text;
    sf::Font font;
    sf::RenderWindow window;
    sf::Event event;
    bool run;
    uint64_t time=0;
    sf::CircleShape dot(2);
    dot.setFillColor(sf::Color(255,255,0));
    double output[1000] = {0.0};
    double v[1000] = {0.0};
    double u[1000] = {0.0};
    int nt_index = 0;

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

    run = true;
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

    for(int i = 0; i < 14; i++) {
        hmarkers[i].color = sf::Color(100,100,100);
    }

    double exin = 0.0;
    double out = 0.0;

    NeuronTemplates templates;

    sptr<Network> network = std::make_shared<Network>();
    BuildNetwork(network,templates,nt_index);

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

    while(run) {
        //--------------------------------------------------
        // INPUT
        while(window.pollEvent(event)) {
            if(event.type==sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Q: run=false; break;
                    case sf::Keyboard::Up: exin+=1.0; break;
                    case sf::Keyboard::Down: exin-=1.0; break;
                    case sf::Keyboard::Right: 
                        nt_index=(nt_index+1)%num_neuron_types;
                        BuildNetwork(network,templates,nt_index);
                        break;
                    case sf::Keyboard::Left: 
                        nt_index=(nt_index-1+num_neuron_types)%num_neuron_types;
                        BuildNetwork(network,templates,nt_index);
                        break;
                }
            }
        }
        //--------------------------------------------------
        // UPDATE
        network->s1->SetSignal(exin);
        network->n1->Update(time);
        v[time%1000] = network->n1->V();
        output[time%1000] = network->s2->GetSignal();
        //--------------------------------------------------
        // DRAW
        window.clear(sf::Color::Black);

        PrintMsg(window,text,"NTYPE:  "+neuron_types[nt_index],5.0f,5.0f);
        PrintMsg(window,text,"INPUT:  "+std::to_string(exin),5.0f,20.0f);
        PrintMsg(window,text,"OUTPUT: "+std::to_string(output[time%1000]),5.0f,35.0f,sf::Color::Yellow);
        PrintMsg(window,text,"V:      "+std::to_string(v[time%1000]),5.0f,50.0f,sf::Color::Cyan);
        
        PrintHeader(window,text);
        PrintGrid(window,text);
        PrintTrace(window,dot,output,v);

        window.display();

        time++;
        std::this_thread::sleep_for(std::chrono::milliseconds(33));

    }
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    return 0;
}

void BuildNetwork(sptr<Network> network, NeuronTemplates & templates, int nt_index) {
    network->n1 = std::make_unique<Neuron>(templates.GetNeuronTemplate(neuron_types[nt_index]));
    //network->n2 = std::make_unique<Neuron>(templates.GetNeuronTemplate(neuron_types[nt_index]));

    network->s1 = std::make_shared<SimpleSynapse>(1.0);
    network->s2 = std::make_shared<SimpleSynapse>(1.0);

    network->n1->AddInputSynapse(network->s1);
    network->n1->AddOutputSynapse(network->s2);
    //network->n2->AddInputSynapse(network->s2);
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

void PrintTrace(sf::RenderWindow & win, sf::CircleShape & dot, double output[], double v[]) {

    sf::Vertex oline[2];
    sf::Vertex vline[2];
    

    for(int i = 1; i < 1000; i++) {

        oline[0] = sf::Vector2f(static_cast<float>(i+50),(-output[i-1])*100.0f+900.0f); 
        oline[1] = sf::Vector2f(static_cast<float>(i+51),(-output[i])*100.0f+900.0f); 
        oline[0].color = sf::Color::Yellow;
        oline[1].color = sf::Color::Yellow; 

        vline[0] = sf::Vector2f(static_cast<float>(i+50),(-v[i-1])*2.0f+500.0f);
        vline[1] = sf::Vector2f(static_cast<float>(i+51),(-v[i])*2.0f+500.0f);
        vline[0].color = sf::Color::Cyan;
        vline[1].color = sf::Color::Cyan;
        
        win.draw(oline,2,sf::Lines);
        win.draw(vline,2,sf::Lines);
    }
}

void PrintGrid(sf::RenderWindow & win, sf::Text & text) {

    win.draw(borders,8,sf::Lines);
    win.draw(hmarkers,14,sf::Lines);

    PrintMsg(win,text,"0.0",25.0f,890.0f,sf::Color::Yellow);
    PrintMsg(win,text,"1.0",25.0f,790.0f,sf::Color::Yellow);
    PrintMsg(win,text,"2.0",25.0f,690.0f,sf::Color::Yellow);
    PrintMsg(win,text,"3.0",25.0f,590.0f,sf::Color::Yellow);
    PrintMsg(win,text,"4.0",25.0f,490.0f,sf::Color::Yellow);
    PrintMsg(win,text,"5.0",25.0f,390.0f,sf::Color::Yellow);
    PrintMsg(win,text,"6.0",25.0f,290.0f,sf::Color::Yellow);
    PrintMsg(win,text,"7.0",25.0f,190.0f,sf::Color::Yellow);
    PrintMsg(win,text,"TIME (1 SEC IN MILLIS)",450.0f,910.0f);

    text.rotate(-90.0f);
    PrintMsg(win,text,"OUTPUT",5.0f,525.0f,sf::Color::Yellow);
    text.rotate(90.0f);

    PrintMsg(win,text,"150",1055.0f,190.0f,sf::Color::Cyan);
    PrintMsg(win,text,"100",1055.0f,290.0f,sf::Color::Cyan);
    PrintMsg(win,text,"50",1055.0f,390.0f,sf::Color::Cyan);
    PrintMsg(win,text,"0",1055.0f,490.0f,sf::Color::Cyan);
    PrintMsg(win,text,"-50",1055.0f,590.0f,sf::Color::Cyan);
    PrintMsg(win,text,"-100",1055.0f,690.0f,sf::Color::Cyan);
    PrintMsg(win,text,"-150",1055.0f,790.0f,sf::Color::Cyan);
    PrintMsg(win,text,"-200",1055.0f,890.0f,sf::Color::Cyan);

    text.rotate(90.0f);
    PrintMsg(win,text,"V (pA)",1095.0f,490.0f,sf::Color::Cyan);
    text.rotate(-90.0f);
}

void PrintHeader(sf::RenderWindow & win, sf::Text & text) {
    PrintMsg(win,text,"[UP]:    Increase Input",400.0f,5.0f);
    PrintMsg(win,text,"[Down]:  Decrease Input",400.0f,20.0f);
    PrintMsg(win,text,"[LEFT]:  Cycle Neuron Types",400.0f,35.0f);
    PrintMsg(win,text,"[P]:     Pause",400.0f,50.0f);
}