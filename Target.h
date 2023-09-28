#ifndef     targetYetIncluded
#define     targetYetIncluded
#define     ANIMATION_SPEED 0.2
#define     TO_SQUARE 2.
#include    "Model.h"
#include    <vector>
#include    <math.h>

class Target{
public:
    Target(Model*fromArg,Model*toArg);
    Model*getFrom();
    Model*getTo();
    int getTotalNrSteps();
    int getStepsDone();
    void incrementStepsDone();
    void resetStepsDone();
private:
    Model*from;
    Model*to;
    int totalNrSteps;
    int stepsDone;
};

#endif      //targetYetIncluded
