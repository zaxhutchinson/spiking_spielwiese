#include<iostream>
#include<string>
#include<chrono>
#include<thread>
#include<cmath>
#include<random>
#include<fstream>

#include<SFML/Graphics.hpp>

#include"spspdef.hpp"
#include"Neuron.hpp"
#include"Synapse.hpp"
#include"NTemplate.hpp"

using namespace spsp;

/* NOTES:
    Spare firing with large holes.
    Exc_weight = 1000
    Alphabase = 1.0


*/

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
const int GRID_SIZE_X=100;
const int GRID_SIZE_Y=100;
const float CONN_PROB=1.0;
const double INH_WEIGHT=-200.0;
const double EXC_WEIGHT=540.0;//1015.0;
const int NUM_PLAYERS=1;
const int PLAYER_INPUT_DURATION=1000;
const double STARTING_INPUT=400.0;
const double ALPHABASE=2.0;
const float SQUARE_SIZE=7;
const int COLOR_MIN=200;
const int COLOR_MAX=210;
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
struct Coord {
    int x,y;
    Coord(int x,int y) {this->x=x;this->y=y;}
    bool operator==(const Coord & rhs) {
        return x==rhs.x && y==rhs.y;
    }
};
struct Cell {
    bool conn[4];
    Cell() {conn[0]=conn[1]=conn[2]=conn[3]=true;}
    int NumConns() {
        int conns=0;
        if(conn[0])conns++;
        if(conn[1])conns++;
        if(conn[2])conns++;
        if(conn[3])conns++;
        return conns;
    }
};
struct Player {
    int index;
    vec<Coord> bases;
    vsptr<Synapse> synapses;
    vec<double> inputs;
    Player(int index) {this->index = index;}
};
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void GenerateGrid(vec<vec<Cell>> & grid, std::mt19937_64 & rng);
void GenerateNetworkFromGrid(vec<vec<Cell>> & grid,
                            vec<vec<vsptr<Neuron>>> & network,
                            NT * nt,
                            std::mt19937_64 & rng);
void DrawNetwork(sf::RenderWindow & window,
                            vec<vec<vsptr<Neuron>>> & network,
                            vec<vec<Cell>> & grid,
                            vec<sf::Color> colors,
                            int player);
void DrawNetwork2(sf::RenderWindow & window,
                            vec<vec<vsptr<Neuron>>> & network,
                            vec<vec<Cell>> & grid,
                            vec<sf::Color> colors,
                            int player);
void AddPlayerToNetwork(vec<vec<vsptr<Neuron>>> & network,
                            sptr<Player> player,
                            int player_index);
void StartPlayer(sptr<Player> player);
void StopPlayer(sptr<Player> player);
void UpdateNetwork(vec<vec<vsptr<Neuron>>> & network,
                            std::uniform_int_distribution<int> & dist,
                            std::mt19937_64 & rng,
                            vec<double> & energy,
                            uint64_t time);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color);
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
    std::uniform_int_distribution<int> playerOrderDist(0,NUM_PLAYERS-1);
    int wait_time=0;
    int player_index=0;
    vsptr<Player> players;
    vec<sf::Color> player_colors;
    std::uniform_int_distribution<int> xDist(0,GRID_SIZE_X-1);
    std::uniform_int_distribution<int> yDist(0,GRID_SIZE_Y-1);
    std::uniform_int_distribution<int> colorDist(COLOR_MIN,COLOR_MAX);
    std::ofstream fout("energy.out", std::ios::out);
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    

    for(int i = 0; i < NUM_PLAYERS; i++) {
        sptr<Player> p = std::make_shared<Player>(i);
        p->bases.push_back(Coord(xDist(rng),yDist(rng)));
        players.push_back(p);
        player_colors.push_back(sf::Color(255,255,255));
    }

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
    vec<vec<Cell>> grid;
    GenerateGrid(grid,rng);

    vec<vec<vsptr<Neuron>>> network;
    GenerateNetworkFromGrid(grid,network,
                    templates->GetNeuronTemplate("RegularSpiking"),
                    rng);

    for(int i = 0; i < NUM_PLAYERS; i++) {
        AddPlayerToNetwork(network,players[i],i);
        StartPlayer(players[i]);
    }
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

    while(run) {
        
        //--------------------------------------------------------
        // INPUT
        while(window.pollEvent(event)) {
            if(event.type==sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Q: run=false; break;
                    case sf::Keyboard::Left: 
                        player_index=(player_index-1+NUM_PLAYERS)%NUM_PLAYERS; break;
                    case sf::Keyboard::Right: 
                        player_index=(player_index+1)%NUM_PLAYERS; break;
                    case sf::Keyboard::Up:
                        wait_time+=10; break;
                    case sf::Keyboard::Down:
                        if(wait_time>0) {wait_time-=10;} break;
                    default: break;
                }
            }
        }

        //--------------------------------------------------
        // UPDATE
        vec<double> energy;

        for(int i = 0; i < NUM_PLAYERS; i++) energy.push_back(0.0);

        if(time==PLAYER_INPUT_DURATION) {
            for(int i = 0; i < NUM_PLAYERS; i++) {
                StopPlayer(players[i]);
                
            }
        }
        UpdateNetwork(network,playerOrderDist,rng,energy,time);

        //--------------------------------------------------
        // DRAW

        window.clear(sf::Color::Black);

        DrawNetwork2(window,network,grid,player_colors,player_index);
        PrintMsg(window,text,"TIME: "+std::to_string(time),10.0f,10.0f);
        PrintMsg(window,text,"WAIT TIME: "+std::to_string(wait_time),10.0f,30.0f);
        PrintMsg(window,text,"PLAYER: "+std::to_string(player_index),10.0f,50.0f);

        for(int i = 0; i < NUM_PLAYERS; i++) {
            //energy[i] = energy[i] / (GRID_SIZE_X*GRID_SIZE_Y); // Normalize by number of neurons
            PrintMsg(window,text,"Energy [" + std::to_string(i) + "]  " + std::to_string(energy[i]),600.0f,10.0f+i*15.0f);
        }

        window.display();

        time++;

        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
    }

    fout.close();
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    return 0;
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void GenerateGrid(vec<vec<Cell>> & grid, std::mt19937_64 & rng) {
    std::uniform_real_distribution<float> dice(0.0f,1.0f);
    std::uniform_int_distribution<int> connDist(0,3);

    for(int x = 0; x < GRID_SIZE_X; x++) {
        vec<Cell> v;
        for(int y = 0; y < GRID_SIZE_Y; y++) {
            v.push_back(Cell());
        }
        grid.push_back(v);
    }

    // N,S,E,W
    int xoff[4]={0,0,1,-1};
    int yoff[4]={-1,1,0,0};
    int to[4]={0,1,2,3};
    int from[4]={1,0,3,2};

    for(int x = 0; x < GRID_SIZE_X; x++) {
        for(int y = 0; y < GRID_SIZE_Y; y++) {
            //int start=connDist(rng);
           //for(int i = start; i < (start+3)%4; i=(i+1)%4) {
            for(int i = 0; i < 4; i++) {
                if(grid[x][y].conn[to[i]]) {

                    int nx = (x+xoff[i]+GRID_SIZE_X)%GRID_SIZE_X;
                    int ny = (y+yoff[i]+GRID_SIZE_Y)%GRID_SIZE_Y;

                    if(dice(rng)>CONN_PROB && grid[nx][ny].NumConns() > 2 && grid[x][y].NumConns() > 2) {
                        grid[x][y].conn[to[i]]=false;
                        grid[nx][ny].conn[from[i]]=false;
                    } 
                }
            }
        }
    }
}
void GenerateNetworkFromGrid(vec<vec<Cell>> & grid,
                            vec<vec<vsptr<Neuron>>> & network,
                            NT * nt,
                            std::mt19937_64 & rng) {

    // N,S,E,W
    int xoff[4]={0,0,1,-1};
    int yoff[4]={-1,1,0,0};

    // Generate the neurons and inhibitory connections
    for(int x = 0; x < GRID_SIZE_X; x++) {
        vec<vsptr<Neuron>> v;
        for(int y = 0; y < GRID_SIZE_Y; y++) {
            vsptr<Neuron> point;

            for(int i = 0; i < NUM_PLAYERS; i++) {
                sptr<Neuron> n = std::make_shared<Neuron>(nt);
                n->SetAlphaBase(ALPHABASE);
                n->EnableNoise(NoiseType::Normal,rng(),0.0,1.0);
                point.push_back(n);
            }

            for(int i = 0; i < NUM_PLAYERS; i++) {
                sptr<Synapse> in = std::make_shared<SimpleSynapse>(INH_WEIGHT,2);
                point[i]->AddOutputSynapse(in);
                for(int j = 0; j < NUM_PLAYERS; j++) {
                    if(i==j) continue;
                    else {
                        point[j]->AddInputSynapse(in);
                    }
                }
            }

            v.push_back(point);
        }
        network.push_back(v);
    }

    for(int x = 0; x < GRID_SIZE_X; x++) {
        for(int y = 0; y < GRID_SIZE_Y; y++) {
            for(int i = 0; i < 4; i++) {

                double exc_weight = EXC_WEIGHT;
                
                // WAVE PATTERN
                if(i==2) exc_weight*=1.5;
                else exc_weight*=0.85;

                if(grid[x][y].conn[i]) {
                    int nx = (x+xoff[i]+GRID_SIZE_X)%GRID_SIZE_X;
                    int ny = (y+yoff[i]+GRID_SIZE_Y)%GRID_SIZE_Y;

                    for(int j = 0; j < NUM_PLAYERS; j++) {
                        sptr<Synapse> syn = std::make_shared<SimpleSynapse>(exc_weight,2);
                        network[x][y][j]->AddOutputSynapse(syn);
                        network[nx][ny][j]->AddInputSynapse(syn);
                    }
                }
            }
        }
    }
}

void DrawNetwork(sf::RenderWindow & window, 
                    vec<vec<vsptr<Neuron>>> & network, 
                    vec<vec<Cell>> & grid,
                    vec<sf::Color> colors,
                    int player) {
    sf::CircleShape cs(5);
    sf::Color color = colors[player];
    sf::Vertex line[2];
    sf::Color grid_color(70,70,70);

    cs.setOutlineThickness(1);
    cs.setOutlineColor(grid_color);
    line[0].color=grid_color;
    line[1].color=grid_color;
    for(int x = 0; x < GRID_SIZE_X; x++) {
        for(int y = 0; y < GRID_SIZE_Y; y++) {

            float nx = 100.0f+x*20.0f;
            float ny = 100.0f+y*20.0f;

            cs.setPosition(nx,ny);
            int alpha = static_cast<int>(255*network[x][y][player]->GetCurrentOutputNormalized());
            if(alpha > 255) alpha=255;
            color.a=( alpha );
            cs.setFillColor(color);
            window.draw(cs);

            // Draw North
            if(grid[x][y].conn[0]) {
                line[0].position=sf::Vector2f(nx+5.0f,ny);
                line[1].position=sf::Vector2f(nx+5.0f,ny-10.0f);
                window.draw(line,2,sf::Lines);
            }
            // Draw South
            if(grid[x][y].conn[1]) {
                line[0].position=sf::Vector2f(nx+5.0f,ny+10.0f);
                line[1].position=sf::Vector2f(nx+5.0f,ny+20.0f);
                window.draw(line,2,sf::Lines);
            }
            // Draw East
            if(grid[x][y].conn[2]) {
                line[0].position=sf::Vector2f(nx+10.0f,ny+5.0f);
                line[1].position=sf::Vector2f(nx+20.0f,ny+5.0f);
                window.draw(line,2,sf::Lines);
            }
            // Draw West
            if(grid[x][y].conn[3]) {
                line[0].position=sf::Vector2f(nx,ny+5.0f);
                line[1].position=sf::Vector2f(nx-10.0f,ny+5.0f);
                window.draw(line,2,sf::Lines);
            }
        }
    }
}

void DrawNetwork2(sf::RenderWindow & window,
                            vec<vec<vsptr<Neuron>>> & network,
                            vec<vec<Cell>> & grid,
                            vec<sf::Color> colors,
                            int player) {
    sf::RectangleShape rs(sf::Vector2f(SQUARE_SIZE,SQUARE_SIZE));

    for(int x = 0; x < GRID_SIZE_X; x++) {
        for(int y = 0; y < GRID_SIZE_Y; y++) {
            for(int i = 0; i < NUM_PLAYERS; i++) {
                float px=x*SQUARE_SIZE+100.0f;
                float py=y*SQUARE_SIZE+100.0f;
                rs.setPosition(px,py);
                int alpha = static_cast<int>(255*network[x][y][i]->GetCurrentOutputNormalized());
                if(alpha > 255) alpha=255;
                colors[i].a=( alpha );
                rs.setFillColor(colors[i]);
                window.draw(rs);
            }
        }
    }
}

void AddPlayerToNetwork(vec<vec<vsptr<Neuron>>> & network,
                            sptr<Player> player,
                            int player_index) {

    for(unsigned i = 0; i < player->bases.size(); i++) {

        sptr<Synapse> syn = std::make_shared<SimpleSynapse>(1.0,1);
        player->synapses.push_back(syn);
        player->inputs.push_back(STARTING_INPUT);

        int x = player->bases[i].x;
        int y = player->bases[i].y;

        network[x][y][player_index]->AddInputSynapse(syn);

    }
}
void StartPlayer(sptr<Player> player) {
    for(unsigned i = 0; i < player->synapses.size(); i++) {
        player->synapses[i]->SetSignal(0,player->inputs[i]);
    }
}
void StopPlayer(sptr<Player> player) {
    for(unsigned i = 0; i < player->synapses.size(); i++) {
        player->synapses[i]->SetSignal(0,0.0);
    }
}
void UpdateNetwork(vec<vec<vsptr<Neuron>>> & network,
                            std::uniform_int_distribution<int> & dist,
                            std::mt19937_64 & rng,
                            vec<double> & energy,
                            uint64_t time) {

    
    #pragma omp parallel for shared(energy)
    
    for(int x = GRID_SIZE_X-1; x >= 0; x--) {
        for(int y = GRID_SIZE_Y-1; y >= 0; y--) {
            //int first = dist(rng);
            //for(int i = first; i < (first+NUM_PLAYERS-1)%NUM_PLAYERS; i=(i+1)%NUM_PLAYERS) {
            for(int i = 0; i < NUM_PLAYERS; i++) {
                network[x][y][i]->Update(time);
                #pragma omp atomic
                energy[i] += network[x][y][i]->GetCurrentOutputNormalized();
                
            }
        }
    }
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

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////