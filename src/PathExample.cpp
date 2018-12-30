#include<iostream>
#include<string>
#include<cmath>
#include<random>
#include<algorithm>
#include<limits>

#include<SFML/Graphics.hpp>

#include"spspdef.hpp"
#include"Neuron.hpp"
#include"Synapse.hpp"
#include"NTemplate.hpp"
#include"bresenham.hpp"

using namespace spsp;

#define _USE_MATH_DEFINES

struct Goal;
struct Agent;
struct Obstacle;
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
const float WIDTH=1000.0f;
const float HEIGHT=1000.0f;
const double FORWARD_IMPULSE_WEIGHT=100.0;
const double FORWARD_EYE_WEIGHT=-50.0;
const double LEFT_EYE_WEIGHT=100.0;
const double RIGHT_EYE_WEIGHT=100.0;
const double GOAL_LEFT_WEIGHT=100.0;
const double GOAL_RIGHT_WEIGHT=100.0;
const double INH_FL_WEIGHT=-50.0;
const double INH_FR_WEIGHT=-50.0;
const float LEFT_EYE_OFFSET = -M_PI/3.0;
const float RIGHT_EYE_OFFSET = M_PI/3.0;
const float MAX_SIGHT = 1000.0f;
const float EYE_INPUT_WEIGHT = 1.0f;
const float AGENT_SPEED = 20.0f;
const int NUM_OBSTACLES = 15;
const double ALPHABASE = 20.0;
const float SHIFT_AMT = 100.0f;
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color);
void ShiftObjects(float x, float y, Goal & goal, Agent & agent, vec<Obstacle> & obstacles);
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
struct Line {
    Point a;
    Point b;
    Line() {}
    Line(Point _a, Point _b) {
        a=_a; b=_b;
    }
    bool CollisionWith(Point c, Point d, Point & col) {
        float denom = (
            (a.x-b.x) * (c.y-d.y) -
            (a.y-b.y) * (c.x-d.x)
        );
        if(denom==0) {
            return false;
        } else {
            float t_num = (
                (a.x-c.x) * (c.y-d.y) -
                (a.y-c.y) * (c.x-d.x)
            );
            float u_num = (
                (a.x-b.x) * (a.y-c.y) -
                (a.y-b.y) * (a.x-c.x)
            );
            float t = t_num / denom;
            float u = -u_num / denom;

            if( t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
                col.x = (a.x + t * (b.x-a.x));
                col.y = (a.y + t * (b.y-a.y));
                return true;
            // } else if(u >= 0.0f && u <= 1.0f) {
            //     col.x = (c.x + u * (d.x-c.x));
            //     col.y = (c.y + u * (d.y-c.y));
            //     return true;
            } else {
                return false;
            }
        }
    }
};
struct Rect : sf::RectangleShape {
    Point tl;
    Point br;
    Line top, bottom, left, right;
    Rect(Point topleft, Point bottomright)
        : sf::RectangleShape(sf::Vector2f(bottomright.x-topleft.x, bottomright.y-topleft.y)), 
            tl(topleft), br(bottomright) 
    {
        setPosition(sf::Vector2f(tl.x,tl.y));
        SetLines();
    }
    void SetLines() {
        top = Line(tl,Point(br.x,tl.y));
        bottom = Line(Point(tl.x,br.y), br);
        left = Line(tl, Point(tl.x,br.y));
        right = Line(Point(br.x,tl.y),br);
    }
    void ShiftPosition(float x, float y) {
        tl.x+=x; tl.y+=y;
        br.x+=x; br.y+=y;
        setPosition(sf::Vector2f(tl.x,tl.y));
        SetLines();
    }
    void Draw(sf::RenderWindow & window, float scale) {
        setSize(sf::Vector2f((br.x-tl.x)/scale, (br.y-tl.y)/scale));
        setPosition(tl.x/scale,tl.y/scale);
        window.draw(*this);
    }

};
struct Obstacle : public Rect {
    
    Obstacle(Point topleft, Point bottomright)
        : Rect(topleft, bottomright)
    {}

    bool CheckCollision(Point origin, Point end, Point & collision) {
        float distance = MAX_SIGHT;

        Point tempCollision;
        bool collides = false;

        if(top.CollisionWith(origin,end,tempCollision)) {
            if(Distance(origin,tempCollision) < distance ) {
                distance = Distance(origin,tempCollision);
                collision = tempCollision;
                collides = true;
            }
        } 
        tempCollision = Point();
        if(bottom.CollisionWith(origin,end,tempCollision)) {
            if(Distance(origin,tempCollision) < distance) {
                distance = Distance(origin,tempCollision);
                collision = tempCollision;
                collides = true;
            }
        } 
        tempCollision = Point();
        if(left.CollisionWith(origin, end, tempCollision)) {
            if(Distance(origin,tempCollision) < distance) {
                distance = Distance(origin,tempCollision);
                collision = tempCollision;
                collides = true;
            }
        } 
        tempCollision = Point();
        if(right.CollisionWith(origin,end,tempCollision)) {
            if(Distance(origin,tempCollision) < distance) {
                distance = Distance(origin,tempCollision);
                collision = tempCollision;
                collides = true;
            }
        }
        return collides;
    }
    
};
struct Goal : public sf::CircleShape {
    Point loc;
    Goal() {}
    Goal(Point p) : sf::CircleShape(16.0f) {
        loc=p;
        setFillColor(sf::Color::Red);
        setPosition(sf::Vector2f(p.x,p.y));
    }
    void ShiftPosition(float x, float y) {
        loc.x+=x; loc.y+=y;
        setPosition(sf::Vector2f(loc.x,loc.y));
    }
    void Draw(sf::RenderWindow & window, float scale) {
        setRadius(16.0f/scale);
        setPosition(sf::Vector2f(loc.x/scale, loc.y/scale));
        window.draw(*this);
    }
};
struct Eye {
    float angle;
    sptr<SimpleSynapse> nerve;
};
struct Agent : public sf::CircleShape {
    Point loc;
    float heading;
    float heading_correction;
    float angle_to_goal;
    float radius;
    float vx;
    float vy;
    Eye eye_forward;
    Eye eye_left;
    Eye eye_right;

    sptr<Synapse> forward_impulse;

    sptr<Synapse> goal_left;
    sptr<Synapse> goal_right;

    sptr<Synapse> inh_forward_left;
    sptr<Synapse> inh_forward_right;

    uptr<Neuron> move_forward;
    uptr<Neuron> turn_left;
    uptr<Neuron> turn_right;

    Point eye_left_max;
    Point eye_right_max;
    Point eye_forward_max;

    Agent(std::mt19937_64 & rng, float _x, float _y, float _radius,
            NeuronTemplates & nt) 
        :sf::CircleShape(_radius), radius(_radius)
    {
        loc = Point(_x,_y);
        //setOrigin(loc.x,loc.y);
        setPosition(sf::Vector2f(loc.x,loc.y));
        setFillColor(sf::Color::Blue);
        
        eye_forward.angle=0.0f;
        Turn(0.0f);

        forward_impulse = std::make_shared<SimpleSynapse>(FORWARD_IMPULSE_WEIGHT);
        forward_impulse->SetSignal(1.0);

        eye_forward.nerve = std::make_shared<SimpleSynapse>(FORWARD_EYE_WEIGHT);
        eye_left.nerve = std::make_shared<SimpleSynapse>(LEFT_EYE_WEIGHT);
        eye_right.nerve = std::make_shared<SimpleSynapse>(RIGHT_EYE_WEIGHT);

        goal_left = std::make_shared<SimpleSynapse>(GOAL_LEFT_WEIGHT);
        goal_right = std::make_shared<SimpleSynapse>(GOAL_RIGHT_WEIGHT);

        inh_forward_left = std::make_shared<SimpleSynapse>(INH_FL_WEIGHT);
        inh_forward_right = std::make_shared<SimpleSynapse>(INH_FR_WEIGHT);

        move_forward = std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking"));
        turn_left = std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking"));
        turn_right = std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking"));

        move_forward->SetAlphaBase(40.0);
        turn_left->SetAlphaBase(20.0);
        turn_right->SetAlphaBase(20.0);

        turn_left->EnableNoise(NoiseType::Uniform,0.0,1.0,rng());
        turn_right->EnableNoise(NoiseType::Uniform,0.0,1.0,rng());

        move_forward->AddInputSynapse(forward_impulse);
        move_forward->AddInputSynapse(eye_forward.nerve);
        move_forward->AddOutputSynapse(inh_forward_left);
        move_forward->AddOutputSynapse(inh_forward_right);

        turn_left->AddInputSynapse(eye_left.nerve);
        turn_left->AddInputSynapse(inh_forward_left);
        turn_left->AddInputSynapse(goal_left);

        turn_right->AddInputSynapse(eye_right.nerve);
        turn_right->AddInputSynapse(inh_forward_right);
        turn_right->AddInputSynapse(goal_right);
    }

    void Turn(float amt) {
        eye_forward.angle += amt;
        if(eye_forward.angle < -M_PI) eye_forward.angle+=2.0*M_PI;
        if(eye_forward.angle > M_PI) eye_forward.angle-=2.0*M_PI;
        eye_left.angle = eye_forward.angle+LEFT_EYE_OFFSET;
        if(eye_left.angle < -M_PI) eye_left.angle+=2.0*M_PI;
        if(eye_left.angle > M_PI) eye_left.angle-=2.0*M_PI;
        eye_right.angle = eye_forward.angle+RIGHT_EYE_OFFSET,M_PI;
        if(eye_right.angle < -M_PI) eye_right.angle+=2.0*M_PI;
        if(eye_right.angle > M_PI) eye_right.angle-=2.0*M_PI;
        heading = eye_forward.angle;
    }

    void Update(vec<Obstacle> & obstacles, Goal goal, float dt, uint64_t time) {
        eye_left_max = Point(
            loc.x+std::cos(eye_left.angle)*MAX_SIGHT,
            loc.y+std::sin(eye_left.angle)*MAX_SIGHT
        );

        eye_right_max = Point(
            loc.x+std::cos(eye_right.angle)*MAX_SIGHT,
            loc.y+std::sin(eye_right.angle)*MAX_SIGHT
        );
        eye_forward_max = Point(
            loc.x+std::cos(eye_forward.angle)*MAX_SIGHT,
            loc.y+std::sin(eye_forward.angle)*MAX_SIGHT
        );

        float dist_left=MAX_SIGHT;
        float dist_right=MAX_SIGHT;
        float dist_forward=MAX_SIGHT;

        Point point_of_collision;

        for(vec<Obstacle>::iterator it = obstacles.begin(); it != obstacles.end();it++) {
            Point point_of_collision;
            if(it->CheckCollision(loc,eye_left_max,point_of_collision)) {
                if(Distance(loc,point_of_collision)<dist_left) {
                    dist_left = Distance(loc,point_of_collision);
                    eye_left_max = point_of_collision;
                }
            }
            point_of_collision=Point();
            if(it->CheckCollision(loc,eye_right_max,point_of_collision)) {
                if(Distance(loc,point_of_collision)<dist_right) {
                    dist_right = Distance(loc,point_of_collision);
                    eye_right_max = point_of_collision;
                }
            }
            point_of_collision=Point();
            if(it->CheckCollision(loc,eye_forward_max,point_of_collision)) {
                if(Distance(loc,point_of_collision)<dist_forward) {
                    dist_forward = Distance(loc,point_of_collision);
                    eye_forward_max = point_of_collision;
                }
            }
        }

        float left_signal = ((1.0-dist_right/MAX_SIGHT)+(dist_left/MAX_SIGHT))*EYE_INPUT_WEIGHT;
        eye_left.nerve->SetSignal(left_signal);
        float right_signal = ((1.0-dist_left/MAX_SIGHT)+(dist_right/MAX_SIGHT))*EYE_INPUT_WEIGHT;
        eye_right.nerve->SetSignal(right_signal);
        eye_forward.nerve->SetSignal( (1.0-(dist_forward/MAX_SIGHT)) *EYE_INPUT_WEIGHT);

        angle_to_goal = std::atan2(goal.loc.y-loc.y, goal.loc.x-loc.x);
        heading_correction = angle_to_goal-heading;
        if(heading_correction > 0) goal_right->SetSignal(heading_correction);
        else if(heading_correction < 0) goal_left->SetSignal(-heading_correction);

        

        turn_left->Update(time);
        turn_right->Update(time);
        

        Turn((turn_right->GetCurrentOutput()-turn_left->GetCurrentOutput())*dt);
        
        float speed = move_forward->GetCurrentOutput()*dt*AGENT_SPEED;
        loc.x += std::cos(heading)*speed;
        loc.y += std::sin(heading)*speed;

        move_forward->Update(time);

        this->setPosition(sf::Vector2f(loc.x,loc.y));
    }
    void ShiftPosition(float x, float y) {
        loc.x+=x; loc.y+=y;
        setPosition(sf::Vector2f(loc.x,loc.y));
    }
    void Draw(sf::RenderWindow & window, float scale) {
        setRadius(radius/scale);
        setPosition(sf::Vector2f(loc.x/scale,loc.y/scale));
        window.draw(*this);
    }
};



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


    Agent agent(rng,2000.0f,2000.0f,16.0f,templates);
    Goal goal(Point(950.0f,950.0f));
    vec<Obstacle> obstacles;
    std::uniform_real_distribution<float> xDist(150.0f,850.0f);
    std::uniform_real_distribution<float> yDist(400.0f,850.0f);
    for(int i = 0; i < NUM_OBSTACLES; i++) {
        float x = xDist(rng);
        float y = yDist(rng);
        Point tl(x-25.0f,y-25.0f);
        Point br(x+25.0f,y+25.0f);
        obstacles.push_back(Obstacle(tl,br));
    }

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
                    case sf::Keyboard::Up: ShiftObjects(0.0f,SHIFT_AMT,goal,agent,obstacles); break;
                    case sf::Keyboard::Down: ShiftObjects(0.0f,-SHIFT_AMT,goal,agent,obstacles); break;
                    case sf::Keyboard::Left: ShiftObjects(SHIFT_AMT,0.0f,goal,agent,obstacles); break;
                    case sf::Keyboard::Right: ShiftObjects(-SHIFT_AMT,0.0f,goal,agent,obstacles); break;
                }
            }
        }


        //--------------------------------------------------
        // DRAW
        window.clear(sf::Color::Black);

        if(pause) {
            PrintMsg(window,text,"PAUSED",480.0f,10.0f,sf::Color::Red);
        } else {
            agent.Update(obstacles,goal,time_elapsed,time);
        }

        PrintMsg(window,text,"AGENT LOC: "+std::to_string(agent.loc.x)+","+std::to_string(agent.loc.y),
            600.0f,2.0f);
        PrintMsg(window,text,"AGENT LOC: "+std::to_string(goal.loc.x)+","+std::to_string(goal.loc.y),
            600.0f,20.0f);   

        PrintMsg(window,text,"TIME:    "+std::to_string(time),400.0f,2.0f);
        PrintMsg(window,text,"HEADING: "+std::to_string(agent.heading),400.0f,20.0f);
        PrintMsg(window,text,"GOAL   : "+std::to_string(agent.angle_to_goal),400.0f,40.0f);
        PrintMsg(window,text,"A:LEFT   : "+std::to_string(agent.eye_left.nerve->GetSignal()),400.0f,60.0f);
        PrintMsg(window,text,"A:RIGHT  : "+std::to_string(agent.eye_right.nerve->GetSignal()),400.0f,80.0f);
        PrintMsg(window,text,"A:FORWARD: "+std::to_string(agent.eye_forward.nerve->GetSignal()),400.0f,100.0f);
        PrintMsg(window,text,"N:LEFT   : "+std::to_string(agent.turn_left->GetCurrentOutput()),400.0f,120.0f);
        PrintMsg(window,text,"N:RIGHT  : "+std::to_string(agent.turn_right->GetCurrentOutput()),400.0f,140.0f);
        PrintMsg(window,text,"N:FORWARD: "+std::to_string(agent.move_forward->GetCurrentOutput()),400.0f,160.0f);
        PrintMsg(window,text,"H:CORRECT: "+std::to_string(agent.heading_correction),400.0f,180.0f);

        sf::Vertex left[] = {
            sf::Vertex(sf::Vector2f(agent.loc.x/scale,agent.loc.y/scale)),
            sf::Vertex(sf::Vector2f(agent.eye_left_max.x/scale, agent.eye_left_max.y/scale))  
        };
        sf::Vertex right[] = {
            sf::Vertex(sf::Vector2f(agent.loc.x/scale,agent.loc.y/scale)),
            sf::Vertex(sf::Vector2f(agent.eye_right_max.x/scale, agent.eye_right_max.y/scale))  
        };
        sf::Vertex forward[] = {
            sf::Vertex(sf::Vector2f(agent.loc.x/scale,agent.loc.y/scale)),
            sf::Vertex(sf::Vector2f(agent.eye_forward_max.x/scale, agent.eye_forward_max.y/scale))  
        };
        left[0].color=sf::Color::Red;
        right[0].color=sf::Color::Red;
        forward[0].color=sf::Color::Red;
        window.draw(left,2,sf::Lines);
        window.draw(right,2,sf::Lines);
        window.draw(forward,2,sf::Lines);

        for(int i = 0; i < NUM_OBSTACLES; i++) {
            obstacles[i].Draw(window,scale);
            //window.draw(obstacles[i]);
        }
        goal.Draw(window,scale);
        agent.Draw(window,scale);
        //window.draw(goal);
        //window.draw(agent);

       // for(int i = 0;i < bees.size();i++) {
        //    bees[i]->Draw(window,scale);
        //}

        window.display();

        //--------------------------------------------------
        // Time
        if(!pause) {
            time++;
        }

        

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
void ShiftObjects(float x, float y, Goal & goal, Agent & agent, vec<Obstacle> & obstacles) {
    goal.ShiftPosition(x,y);
    agent.ShiftPosition(x,y);
    for(int i = 0; i < obstacles.size(); i++) {
        obstacles[i].ShiftPosition(x,y);
    }
}