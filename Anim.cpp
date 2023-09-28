#include "Anim.h"

Anim::Anim(Model*mArg,const char categoryArg) {
    m=mArg;
    category=categoryArg;
    direction='f';
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wshadow"
    std::vector<Target*>targets(1);
    #pragma GCC diagnostic pop
    targetPointingOn=0;
    focussedTarget=targets.at(targetPointingOn);
}

void Anim::setStep(int i) {
    step=i;
}

int Anim::getStep() {
    return step;
}

void Anim::setIndicator(bool b) {
    indicator=b;
}

bool Anim::getIndicator() {
    return indicator;
}

void Anim::addTarget(Target*newTarget) {
    targets.push_back(newTarget);
}

unsigned int Anim::getNrTargets() {
    return targets.size();
}

char Anim::getCategory() {
    return category;
}

Target*Anim::getTarget(unsigned int i) {
    return targets[i];
}

Model*Anim::getModel() {
    return m;
}

char Anim::getDirection() {
    return direction;
}

void Anim::focusNextTarget() {
    targetPointingOn+=1;
    if (targetPointingOn==getNrTargets()) {
        targetPointingOn=1;
    }
    focussedTarget=targets.at(targetPointingOn);
}
