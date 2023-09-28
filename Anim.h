#ifndef     animYetIncluded
#define     animYetIncluded
#include    "Model.h"
#include    "Target.h"

class Anim {
public:
    Anim(Model*m,const char categoryArg);
    Model*getModel();
    void addTarget(Target*newTarget);
    unsigned int getNrTargets();
    Target*getTarget(unsigned int i);
    Target*focussedTarget;
    char getDirection();
    char getCategory();
    void focusNextTarget();
    void setStep(int i);
    int getStep();
    void setIndicator(bool b);
    bool getIndicator();
private:
    Model*m;
    char direction,category;
    std::vector<Target*>targets;
    unsigned int targetPointingOn;
    int step=0;
    bool indicator=true;
};
    
#endif      //animYetIncluded
