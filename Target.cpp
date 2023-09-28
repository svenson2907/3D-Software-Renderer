#include    "Target.h"
Target::Target(Model*fromArg,Model*toArg) {
    from=fromArg;
    to=toArg;
    stepsDone=0;
    totalNrSteps=sqrt(pow(toArg->pos[0]-fromArg->pos[0],TO_SQUARE)+
                      pow(toArg->pos[1]-fromArg->pos[1],TO_SQUARE)+
                      pow(toArg->pos[2]-fromArg->pos[2],TO_SQUARE))/ANIMATION_SPEED;
}

Model*Target::getFrom() {
    return from;
}

Model*Target::getTo() {
    return to;
}

int Target::getTotalNrSteps() {
    return totalNrSteps;
}

int Target::getStepsDone() {
    return stepsDone;
}

void Target::incrementStepsDone() {
    stepsDone++;
}

void Target::resetStepsDone() {
    stepsDone=0;
}
