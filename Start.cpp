#include "Projection.h"
#include "Reader.h"
#include "Transformation.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <unistd.h>
#include <stdlib.h>             //srand(),rand()
#include <functional>           //std::reference_wrapper
#include <math.h>               //fabs()

#define CAM_ALPHA               0.005//0.0025
#define zFar                    (-100000.0)
#define ROT_ALPHA               0.25

Projection *projection;
Transformation *transformation;
std::vector<Model> modelList;
std::vector<Anim> animList;
std::vector<struct Color>colorList;

bool isFS=false;
void keyboardFunc(unsigned char key, int x, int y);
void displayCallback();
void animation();

int main(int argc,char*argv[]) {
    transformation=new Transformation();
    projection=new Projection(transformation);
    Reader*r=new Reader();
    r->readData(&modelList,&colorList);
    r->readAnim(&animList,&modelList,transformation);
    r->readTrafo(&animList,transformation);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);                                    //to select depth buffer,float buffered window,RGBA mode window    
    glutCreateWindow("Visualization");                                                                                                         //Creates a Window
    glutKeyboardFunc(keyboardFunc);                                                                                              //Determine the keyboardfunction
    glutDisplayFunc(displayCallback);                                                                            //Detremine displays callback for current window
    glClearColor(1.0, 1.0, 1.0, 1.0);                                                                            //Specifies blank-values for colorbuffer [white]
    glColor3f(0.0, 0.0, 0.0);                                                                                                        //Sets current color to draw
    gluOrtho2D(WIDTH/-2, WIDTH_HALF, HEIGHT/-2, HEIGHT_HALF);                                                //Scales visible field, in proportion to screen-resolution
    glutReshapeWindow(WIDTH_HALF, HEIGHT_HALF);                                                                                  //Adjusts the windows size/its zBuffer
    glutPositionWindow(100, 100);                                                                                                  //Adjusts the windows position
    glutMainLoop();
    return 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void keyboardFunc(unsigned char key, int x, int y) {
    std::vector<float>Y_AXIS_RET={0,0,0};
    switch (key) {
        case 27:    exit (0);                                                                                           break;                            /*Esc*/
        case 102:   if(isFS)    {glutReshapeWindow((WIDTH_HALF), (HEIGHT_HALF));} else {glutFullScreen();}  isFS=!isFS;       break;                              /*f*/
        case 122:   transformation->rotateCam((-CAM_ALPHA),'x',&modelList,&animList,&projection->camPos);               
                    transformation->rotatePoint((-CAM_ALPHA),'x',projection->getCoordCrossY(),&Y_AXIS_RET);
                    projection->setCoordCrossY(Y_AXIS_RET);
                    break;                                                                                                                                  /*z*/
        case 104:   transformation->rotateCam(CAM_ALPHA,'x',&modelList,&animList,&projection->camPos);                  
                    transformation->rotatePoint(CAM_ALPHA,'x',projection->getCoordCrossY(),&Y_AXIS_RET);
                    projection->setCoordCrossY(Y_AXIS_RET);
                    break;                                                                                                                                  /*h*/
        case 103:   transformation->rotateCam((-CAM_ALPHA),'y',&modelList,&animList,&projection->camPos);               
                    transformation->rotatePoint((-CAM_ALPHA),'y',projection->getCoordCrossY(),&Y_AXIS_RET);
                    projection->setCoordCrossY(Y_AXIS_RET);
                    break;                                                                                                                                  /*g*/
        case 106:   transformation->rotateCam(CAM_ALPHA,'y',&modelList,&animList,&projection->camPos);                  
                    transformation->rotatePoint(CAM_ALPHA,'y',projection->getCoordCrossY(),&Y_AXIS_RET);
                    projection->setCoordCrossY(Y_AXIS_RET);
                    break;                                                                                                                                  /*j*/
        case 119:   transformation->moveCamAlongAlignment (&modelList,'f');                                             break;                              /*w*/
        case 115:   transformation->moveCamAlongAlignment (&modelList,'b');                                             break;                              /*s*/
        case 97:    transformation->moveCamAlongAlignment (&modelList,'l');                                             break;                              /*a*/
        case 100:   transformation->moveCamAlongAlignment (&modelList,'r');                                             break;                              /*d*/
        case 99:    transformation->moveCamAlongAlignment (&modelList,'d');                                             break;                              /*c*/
        case 32:    transformation->moveCamAlongAlignment (&modelList,'u');                                             break;                          /*space*/
        default:                                                                                                        break;
    }               
}
#pragma GCC diagnostic pop

void displayCallback() {
    glutPostRedisplay();    
    glClearColor (1,1,1,0);
    glClear (GL_COLOR_BUFFER_BIT);
    glBegin (GL_POINTS);

//NOTE animateNextFrameBEGIN
    for(auto &anim:animList) {
        switch(anim.getCategory()) {
            case 108:                                                                                                                                         //l
                if(anim.focussedTarget->getStepsDone() < (anim.focussedTarget->getTotalNrSteps())) {
                    transformation->move(anim.getModel(),anim.getDirection());
                    anim.focussedTarget->incrementStepsDone();
                }else{
                    anim.focussedTarget->resetStepsDone();
                    anim.focusNextTarget();
                    transformation->align(anim.getModel(),anim.focussedTarget->getTo());
                }
                break;
                
            case 114:                                                                                                                                         //r
                transformation->rotateFigure(ROT_ALPHA,'z',anim.getModel());
                break;
                
            case 109:                                                                                                                                         //m
                if(anim.focussedTarget->getStepsDone() < (anim.focussedTarget->getTotalNrSteps())) {
                    //NOTE hasnt reached its <Target>
                    transformation->move(anim.getModel(),anim.getDirection());
                    anim.focussedTarget->incrementStepsDone();
                }else{
                    //NOTE <Target> got reached
                    anim.focussedTarget->resetStepsDone();
                    anim.focusNextTarget();
                    std::vector<float>fPRE={anim.getModel()->getAlignmentsLineVec(2)->at(0),
                                            anim.getModel()->getAlignmentsLineVec(2)->at(1),
                                            anim.getModel()->getAlignmentsLineVec(2)->at(2)};
                    transformation->align(anim.getModel(),anim.focussedTarget->getTo());
                    std::vector<float>fPOST={anim.getModel()->getAlignmentsLineVec(2)->at(0),
                                             anim.getModel()->getAlignmentsLineVec(2)->at(1),
                                             anim.getModel()->getAlignmentsLineVec(2)->at(2)};
                    
                    //anim.getModel() soll nach anim.focussedTarget ausgerichtet werden
                    anim.getModel()->rotateModel(transformation->focussing(&anim,fPRE,fPOST,*projection->getCoordCrossY()),*projection->getCoordCrossY());
                }
                
                //to indicate alternately movement
                if(anim.getStep()<2) {
                    anim.setStep(anim.getStep()+1);
                }else {
                    anim.setStep(-1);
                    anim.setIndicator(!anim.getIndicator());                 // -1,0,1,2 -> 4 Frequenzen eines Schritts bis gewechselt wird
                }
                
                if(anim.getIndicator()) {
                    anim.getModel()->rotateJoint(ROT_ALPHA,0);      // 0:=jointID-figureRightLeg
                    anim.getModel()->rotateJoint((-1)*ROT_ALPHA,1); // 1:=jointID-figureLeftLeg
                    anim.getModel()->rotateJoint((-1)*ROT_ALPHA,2); // 2:=jointID-figureRightArm
                    anim.getModel()->rotateJoint(ROT_ALPHA,3);      // 3:=jointID-figureLeftArm
                }else {
                    anim.getModel()->rotateJoint((-1)*ROT_ALPHA,0);
                    anim.getModel()->rotateJoint(ROT_ALPHA,1);
                    anim.getModel()->rotateJoint(ROT_ALPHA,2);
                    anim.getModel()->rotateJoint((-1)*ROT_ALPHA,3);
                }
                break;
                
            case 106:                                                                                                                                       //j
                //NOTE hierbei braucht <Anim> trotzdem min 2 <Target>s(koennen 2 mal die selben sein) obwohl es keine linear Transformation abbildet.
                
                //to indicate alternately movement
                if(anim.getStep()<2) {
                    anim.setStep(anim.getStep()+1);
                }else {
                    anim.setStep(-1);
                    anim.setIndicator(!anim.getIndicator());                 // -1,0,1,2 -> 4 Frequenzen eines Schritts bis gewechselt wird
                }
                
                if(anim.getIndicator()) {
                    anim.getModel()->rotateJoint(ROT_ALPHA,0);      // 0:=jointID-figureRightLeg
                    anim.getModel()->rotateJoint((-1)*ROT_ALPHA,1); // 1:=jointID-figureLeftLeg
                    anim.getModel()->rotateJoint((-1)*ROT_ALPHA,2); // 2:=jointID-figureRightArm
                    anim.getModel()->rotateJoint(ROT_ALPHA,3);      // 3:=jointID-figureLeftArm
                }else {
                    anim.getModel()->rotateJoint((-1)*ROT_ALPHA,0);
                    anim.getModel()->rotateJoint(ROT_ALPHA,1);
                    anim.getModel()->rotateJoint(ROT_ALPHA,2);
                    anim.getModel()->rotateJoint((-1)*ROT_ALPHA,3);
                }
                break;
            default: 
                break;
        }
    }
//NOTE animateNextFrameEND

    for(auto m:modelList) {     //hier kann ein Attribut <getsProjected()> geprueft werden
        if(m.getGetsProjected()) {      //to separate joint-ModelParts from real Models
            projection->map(&m);
        }
    }
    
    for(unsigned int y=0;y<HEIGHT;y++) {
        for(unsigned int x=0;x<WIDTH;x++) {
            Color *const currentColor=&projection->zBuffer[x][y].c;
            if(currentColor->isColored) {
                glColor3f(currentColor->red, currentColor->green, currentColor->blue);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
                glVertex2i((x-(WIDTH_HALF)),(y-(HEIGHT_HALF)));
#pragma GCC diagnostic pop
                *currentColor=nullColor;
                projection->zBuffer[x][y].z=zFar;
            }
        }
    }
    glEnd();
    glutSwapBuffers();
    
    usleep(20000);
}
