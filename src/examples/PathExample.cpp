#include<iostream>
#include<string>
#include<cmath>
#include<random>
#include<algorithm>
#include<limits>

#include<SFML/Graphics.hpp>

#include"spspdef.hpp"
#include"Point.hpp"
#include"Neuron.hpp"
#include"Synapse.hpp"
#include"NTemplate.hpp"

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
const double FORWARD_IMPULSE_WEIGHT=400.0;
const double FORWARD_IMPACT_WEIGHT=1000.0;
const double FORWARD_EYE_WEIGHT=-300.0;
const double LEFT_EYE_WEIGHT=400.0;
const double RIGHT_EYE_WEIGHT=400.0;
const double GOAL_LEFT_WEIGHT=200.0;
const double GOAL_RIGHT_WEIGHT=200.0;
const double INH_FL_WEIGHT=-200.0;
const double INH_FR_WEIGHT=-200.0;
const float MAX_SIGHT = 1000.0f;
const float EYE_INPUT_WEIGHT = 1.0f;
const float AGENT_SPEED = 500.0f;
const int NUM_OBSTACLES = 250;
const double ALPHABASE = 1.0;
const float SHIFT_AMT = 100.0f;
const int NUM_SIDE_EYES=12;
const float LEFT_EYE_SPACING = -M_PI/36.0; // 5 degrees
const float RIGHT_EYE_SPACING = M_PI/36.0; // 5 degrees
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y);
void PrintMsg(sf::RenderWindow & win, sf::Text & text, std::string msg, float x, float y, sf::Color color);
void ShiftObjects(float x, float y, Goal & goal, Agent & agent, vec<Obstacle> & obstacles);
float GetMaxSight(int position);

void ObstacleScenario000(std::mt19937_64 & rng, vec<Obstacle> & obstacles);
void ObstacleScenario001(std::mt19937_64 & rng, vec<Obstacle> & obstacles);
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
        setPosition({tl.x/scale,tl.y/scale});
        window.draw(*this);
    }
    bool ContainsPoint(Point point) {
        if(tl.x <= point.x && tl.y <= point.y &&
            br.x >= point.x && br.y >= point.y) return true;
        else return false;
    }
};
struct Obstacle : public Rect {
    
    Obstacle(Point topleft, Point bottomright)
        : Rect(topleft, bottomright)
    {}

    bool CheckCollision(Point origin, Point end, Point & collision, float distance) {

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
    bool close;
    Eye eye_forward;
    vec<Eye> eyes_left;
    vec<Eye> eyes_right;

    vec<float> left_eye_offsets;
    vec<float> right_eye_offsets;

    sptr<Synapse> forward_impulse;
    sptr<Synapse> forward_impact_left;
    sptr<Synapse> forward_impact_right;

    vsptr<Synapse> goals_left;
    vsptr<Synapse> goals_right;

    uptr<Neuron> neuron_forward;
    vuptr<Neuron> neurons_left;
    vuptr<Neuron> neurons_right;

    vec<Point> eyes_left_max;
    vec<Point> eyes_right_max;
    Point eye_forward_max;

    Agent(std::mt19937_64 & rng, float _x, float _y, float _radius,
            NeuronTemplates & nt) 
        :sf::CircleShape(_radius), radius(_radius)
    {
        loc = Point(_x,_y);
        setOrigin({8.0f,8.0f});
        setPosition(sf::Vector2f(loc.x,loc.y));
        setFillColor(sf::Color::Blue);
        
        close = false;

        forward_impulse = std::make_shared<SimpleSynapse>(FORWARD_IMPULSE_WEIGHT);
        forward_impulse->SetSignal(0,1.0);

        forward_impact_left = std::make_shared<SimpleSynapse>(0.0);
        forward_impact_right = std::make_shared<SimpleSynapse>(0.0);

        eye_forward.nerve = std::make_shared<SimpleSynapse>(FORWARD_EYE_WEIGHT);
        float leye_offset = LEFT_EYE_SPACING;
        float reye_offset = RIGHT_EYE_SPACING;
        for(unsigned i = 0; i < NUM_SIDE_EYES; i++) {

            // MAKE LEFT EYE
            Eye le;
            le.nerve = std::make_shared<SimpleSynapse>(LEFT_EYE_WEIGHT/(i+1));
            eyes_left.push_back(le);

            sptr<SimpleSynapse> lgoal_syn = std::make_shared<SimpleSynapse>(GOAL_LEFT_WEIGHT/(NUM_SIDE_EYES-i));
            goals_left.push_back(lgoal_syn);

            neurons_left.push_back(std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking")));
            neurons_left[i]->SetAlphaBase(ALPHABASE);
            //neurons_left[i]->EnableNoise(NoiseType::Uniform,0.0,1.0,rng());

            neurons_left[i]->AddInputSynapse(eyes_left[i].nerve);
            neurons_left[i]->AddInputSynapse(goals_left[i]);
            neurons_left[i]->AddInputSynapse(forward_impact_left);

            left_eye_offsets.push_back(leye_offset);
            leye_offset += LEFT_EYE_SPACING;

            eyes_left_max.push_back(Point(
                loc.x+std::cos(eyes_left[i].angle)*GetMaxSight(i),
                loc.y+std::sin(eyes_left[i].angle)*GetMaxSight(i)
            ));

            // MAKE RIGHT EYE
            Eye re;
            re.nerve = std::make_shared<SimpleSynapse>(RIGHT_EYE_WEIGHT/(i+1));
            eyes_right.push_back(re);

            sptr<SimpleSynapse> rgoal_syn = std::make_shared<SimpleSynapse>(GOAL_RIGHT_WEIGHT/(NUM_SIDE_EYES-i));
            goals_right.push_back(rgoal_syn);

            neurons_right.push_back(std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking")));
            neurons_right[i]->SetAlphaBase(ALPHABASE);
            //neurons_right[i]->EnableNoise(NoiseType::Uniform,0.0,1.0,rng());

            neurons_right[i]->AddInputSynapse(eyes_right[i].nerve);
            neurons_right[i]->AddInputSynapse(goals_right[i]);
            neurons_right[i]->AddInputSynapse(forward_impact_right);

            right_eye_offsets.push_back(reye_offset);
            reye_offset += RIGHT_EYE_SPACING;

            eyes_right_max.push_back(Point(
                loc.x+std::cos(eyes_right[i].angle)*GetMaxSight(i),
                loc.y+std::sin(eyes_right[i].angle)*GetMaxSight(i)
            ));
        }


        neuron_forward = std::make_unique<Neuron>(nt.GetNeuronTemplate("RegularSpiking"));
        neuron_forward->SetAlphaBase(ALPHABASE);

        neuron_forward->AddInputSynapse(forward_impulse);
        neuron_forward->AddInputSynapse(eye_forward.nerve);

        eye_forward.angle=0.0f;
        Turn(M_PI/4.0f);
    }

    void Turn(float amt) {
        //std::cout << amt << std::endl;
        eye_forward.angle += amt;
        //if(eye_forward.angle < -M_PI) eye_forward.angle+=2.0*M_PI;
        //if(eye_forward.angle > M_PI) eye_forward.angle-=2.0*M_PI;

        for(unsigned i = 0; i < NUM_SIDE_EYES; i++) {
            eyes_left[i].angle = eye_forward.angle+left_eye_offsets[i];
            //if(eyes_left[i].angle < -M_PI) eyes_left[i].angle+=2.0*M_PI;
            //if(eyes_left[i].angle > M_PI) eyes_left[i].angle-=2.0*M_PI;

            eyes_right[i].angle = eye_forward.angle+right_eye_offsets[i];
            //if(eyes_right[i].angle < -M_PI) eyes_right[i].angle+=2.0*M_PI;
            //if(eyes_right[i].angle > M_PI) eyes_right[i].angle-=2.0*M_PI;
        }
        
        heading = eye_forward.angle;
    }

    void Update(vec<Obstacle> & obstacles, Goal goal, float dt, uint64_t time) {

        double left_sum=0.0;
        double right_sum=0.0;

        for(unsigned i = 0; i < NUM_SIDE_EYES; i++) {
            eyes_left_max[i] = Point(
                loc.x+std::cos(eyes_left[i].angle)*GetMaxSight(i),
                loc.y+std::sin(eyes_left[i].angle)*GetMaxSight(i)
            );

            eyes_right_max[i] = Point(
                loc.x+std::cos(eyes_right[i].angle)*GetMaxSight(i),
                loc.y+std::sin(eyes_right[i].angle)*GetMaxSight(i)
            );

            float ldist=GetMaxSight(i);
            float rdist=GetMaxSight(i);
            for(vec<Obstacle>::iterator it = obstacles.begin(); it != obstacles.end();it++) {
                Point lpoint_of_collision;
                Point rpoint_of_collision;

                if(it->CheckCollision(loc,eyes_left_max[i],lpoint_of_collision,GetMaxSight(i))) {
                    if(Distance(loc,lpoint_of_collision)<ldist) {
                        ldist = Distance(loc,lpoint_of_collision);
                        eyes_left_max[i] = lpoint_of_collision;
                    }
                }

                if(it->CheckCollision(loc,eyes_right_max[i],rpoint_of_collision,GetMaxSight(i))) {
                    if(Distance(loc,rpoint_of_collision)<rdist) {
                        rdist = Distance(loc,rpoint_of_collision);
                        eyes_right_max[i] = rpoint_of_collision;
                    }
                }
            }

            float left_signal = ((1.0-rdist/GetMaxSight(i))+(ldist/GetMaxSight(i)))*EYE_INPUT_WEIGHT;
            eyes_left[i].nerve->SetSignal(time,left_signal);

            float right_signal = ((1.0-ldist/GetMaxSight(i))+(rdist/GetMaxSight(i)))*EYE_INPUT_WEIGHT;
            eyes_right[i].nerve->SetSignal(time,right_signal);

            left_sum+=left_signal;
            right_sum+=right_signal;

            angle_to_goal = std::atan2(goal.loc.y-loc.y, goal.loc.x-loc.x);
            heading_correction = angle_to_goal-heading;
            if(heading_correction > 0) goals_right[i]->SetSignal(time,heading_correction);
            else if(heading_correction < 0) goals_left[i]->SetSignal(time,-heading_correction);

        }

       

        eye_forward_max = Point(
            loc.x+std::cos(eye_forward.angle)*GetMaxSight(0),
            loc.y+std::sin(eye_forward.angle)*GetMaxSight(0)
        );

        float dist_forward=GetMaxSight(0);

        Point point_of_collision;

        for(vec<Obstacle>::iterator it = obstacles.begin(); it != obstacles.end();it++) {
            Point point_of_collision;

            point_of_collision=Point();
            if(it->CheckCollision(loc,eye_forward_max,point_of_collision,GetMaxSight(0))) {
                if(Distance(loc,point_of_collision)<dist_forward) {
                    dist_forward = Distance(loc,point_of_collision);
                    eye_forward_max = point_of_collision;
                }
            }
        }

        if(dist_forward < 50.0f) {
            close = true;
            dynamic_cast<SimpleSynapse*>(forward_impact_left.get())->SetWeight(FORWARD_IMPACT_WEIGHT);
            dynamic_cast<SimpleSynapse*>(forward_impact_right.get())->SetWeight(-FORWARD_IMPACT_WEIGHT);
        } else if(close) {
            close = false;
            dynamic_cast<SimpleSynapse*>(forward_impact_left.get())->SetWeight(0.0);
            dynamic_cast<SimpleSynapse*>(forward_impact_right.get())->SetWeight(0.0);
        }

        forward_impact_left->SetSignal( time,(1.0-(dist_forward/GetMaxSight(0))) );
        forward_impact_right->SetSignal( time,(1.0-(dist_forward/GetMaxSight(0))) );
        
        eye_forward.nerve->SetSignal( time,(1.0-(dist_forward/GetMaxSight(0))) *EYE_INPUT_WEIGHT);

        float left_turn_sum = 0.0f;
        float right_turn_sum = 0.0f;

        for(unsigned i = 0; i < NUM_SIDE_EYES; i++) {
            neurons_left[i]->Update(time);
            neurons_right[i]->Update(time);

            left_turn_sum += neurons_left[i]->GetCurrentOutput();
            right_turn_sum += neurons_right[i]->GetCurrentOutput();    
        }
        
        //std::cout << right_turn_sum << " " << left_turn_sum << std::endl;
        Turn((right_turn_sum-left_turn_sum)*dt);
        
        float speed = neuron_forward->GetCurrentOutput()*dt*AGENT_SPEED;

        float new_x = std::cos(heading)*speed;
        float new_y = std::sin(heading)*speed;
        Point p(new_x+loc.x,new_y+loc.y);
        bool collision = false;
        for(vec<Obstacle>::iterator it = obstacles.begin(); it != obstacles.end();it++) {
            if(it->ContainsPoint(p)) {
                collision=true; break;
                
            }
        }


        neuron_forward->Update(time);

        if(!collision)
            this->setPosition(sf::Vector2f(loc.x+=new_x,loc.y+=new_y));
    }
    void ShiftPosition(float x, float y) {
        loc.x+=x; loc.y+=y;
        setPosition(sf::Vector2f(loc.x,loc.y));
    }
    void Draw(sf::RenderWindow & window, float scale) {
        setRadius(radius/scale);
        setOrigin({radius/(2.0f*scale),radius/(2.0f*scale)});
        setPosition(sf::Vector2f(loc.x/scale,loc.y/scale));
        window.draw(*this);
    }
};



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
int main(int argc, char**argv) {
    
    
    sf::Font font;
    sf::RenderWindow window;

    bool run=true;
    bool pause =true;

    std::random_device rd;
    std::mt19937_64 rng(rd());
    uint64_t time = 0;

    float scale = 1.0;

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
    NeuronTemplates templates;
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////


    Agent agent(rng,50.0f,50.0f,16.0f,templates);
    Goal goal(Point(10000.0f,10000.0f));
    vec<Obstacle> obstacles;
    ObstacleScenario000(rng,obstacles);
    //ObstacleScenario001(rng,obstacles);
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
                    case sf::Keyboard::Scancode::Up: ShiftObjects(0.0f,SHIFT_AMT,goal,agent,obstacles); break;
                    case sf::Keyboard::Scancode::Down: ShiftObjects(0.0f,-SHIFT_AMT,goal,agent,obstacles); break;
                    case sf::Keyboard::Scancode::Left: ShiftObjects(SHIFT_AMT,0.0f,goal,agent,obstacles); break;
                    case sf::Keyboard::Scancode::Right: ShiftObjects(-SHIFT_AMT,0.0f,goal,agent,obstacles); break;
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
            agent.Update(obstacles,goal,time_elapsed,time);
        }

        // PrintMsg(window,text,"AGENT LOC: "+std::to_string(agent.loc.x)+","+std::to_string(agent.loc.y),
        //     600.0f,2.0f);
        // PrintMsg(window,text,"AGENT LOC: "+std::to_string(goal.loc.x)+","+std::to_string(goal.loc.y),
        //     600.0f,20.0f);   

        PrintMsg(window,text,"TIME:    "+std::to_string(time),400.0f,2.0f);
        PrintMsg(window,text,"HEADING: "+std::to_string(agent.heading),400.0f,20.0f);
        PrintMsg(window,text,"GOAL   : "+std::to_string(agent.angle_to_goal),400.0f,40.0f);
        //PrintMsg(window,text,"A:LEFT   : "+std::to_string(agent.eye_left.nerve->GetSignal()),400.0f,60.0f);
        //PrintMsg(window,text,"A:RIGHT  : "+std::to_string(agent.eye_right.nerve->GetSignal()),400.0f,80.0f);
        PrintMsg(window,text,"A:FORWARD: "+std::to_string(agent.eye_forward.nerve->GetSignal(time)),400.0f,100.0f);
        //PrintMsg(window,text,"N:LEFT   : "+std::to_string(agent.turn_left->GetCurrentOutput()),400.0f,120.0f);
        //PrintMsg(window,text,"N:RIGHT  : "+std::to_string(agent.turn_right->GetCurrentOutput()),400.0f,140.0f);
        PrintMsg(window,text,"N:FORWARD: "+std::to_string(agent.neuron_forward->GetCurrentOutput()),400.0f,160.0f);
        PrintMsg(window,text,"H:CORRECT: "+std::to_string(agent.heading_correction),400.0f,180.0f);

        for(unsigned i = 0; i < NUM_SIDE_EYES; i++) {

            sf::Vertex left[] = {
                sf::Vertex({sf::Vector2f(agent.loc.x/scale,agent.loc.y/scale)}),
                sf::Vertex({sf::Vector2f(agent.eyes_left_max[i].x/scale, agent.eyes_left_max[i].y/scale)})  
            };
            sf::Vertex right[] = {
                sf::Vertex({sf::Vector2f(agent.loc.x/scale,agent.loc.y/scale)}),
                sf::Vertex({sf::Vector2f(agent.eyes_right_max[i].x/scale, agent.eyes_right_max[i].y/scale)})  
            };

            left[0].color=sf::Color::Red;
            right[0].color=sf::Color::Red;

            window.draw(left,2,sf::PrimitiveType::Lines);
            window.draw(right,2,sf::PrimitiveType::Lines);
        }
        sf::Vertex forward[] = {
            sf::Vertex({sf::Vector2f(agent.loc.x/scale,agent.loc.y/scale)}),
            sf::Vertex({sf::Vector2f(agent.eye_forward_max.x/scale, agent.eye_forward_max.y/scale)})  
        };
        
        forward[0].color=sf::Color::Red;
        
        window.draw(forward,2,sf::PrimitiveType::Lines);

        for(unsigned i = 0; i < obstacles.size(); i++) {
            obstacles[i].Draw(window,scale);
            //window.draw(obstacles[i]);
        }
        goal.Draw(window,scale);
        agent.Draw(window,scale);
        //window.draw(goal);
        //window.draw(agent);

       // for(unsigned i = 0;i < bees.size();i++) {
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
    text.setPosition({x,y});
    text.setFillColor(color);
    win.draw(text);
}
void ShiftObjects(float x, float y, Goal & goal, Agent & agent, vec<Obstacle> & obstacles) {
    goal.ShiftPosition(x,y);
    agent.ShiftPosition(x,y);
    for(unsigned i = 0; i < obstacles.size(); i++) {
        obstacles[i].ShiftPosition(x,y);
    }
}
float GetMaxSight(int position) {
    return (NUM_SIDE_EYES-position)*( MAX_SIGHT / NUM_SIDE_EYES);
}

void ObstacleScenario000(std::mt19937_64 & rng, vec<Obstacle> & obstacles) {
    std::uniform_real_distribution<float> xDist(500.0f,9500.0f);
    std::uniform_real_distribution<float> yDist(500.0f,9500.0f);
    for(unsigned i = 0; i < NUM_OBSTACLES; i++) {
        float x = xDist(rng);
        float y = yDist(rng);
        Point tl(x-100.0f,y-100.0f);
        Point br(x+100.0f,y+100.0f);
        obstacles.push_back(Obstacle(tl,br));
    }
}

void ObstacleScenario001(std::mt19937_64 & rng, vec<Obstacle> & obstacles) {
    Point tl1(2000.0f,500.0f); Point br1(2100.0f,3500.0f);
    Point tl2(500.0f,2000.0f); Point br2(3500.0f,2100.0f);
    obstacles.push_back(Obstacle(tl1,br1));
    obstacles.push_back(Obstacle(tl2,br2));
}