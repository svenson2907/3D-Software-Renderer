#ifndef     readerYetIncluded
#define     readerYetIncluded
#include    <fstream>                                                                                        /*fileoperation ifstream(lesen),ofstream(schreiben)*/
#include    <cstring>
#include    "Anim.h"
#include    "Model.h"
#include    "Transformation.h"
#include    <iostream>
#include    <vector>

#define     SCENEINIT_READ  " sceneInit.txt loaded\n"
#define     ANIMINIT_READ   " animInit.txt loaded\n"
#define     TRAFOINIT_READ  " trafoInit.txt loaded\n"

#define     SCENEINIT_DATA  "sceneInit.txt"
#define     ANIMINIT_DATA   "animInit.txt"
#define     TRAFOINIT_DATA  "trafoInit.txt"

class Reader {
public:
    void readData   (std::vector<Model>*const modelList,std::vector<struct Color>*const colorList);
    void readAnim   (std::vector<Anim>*const animList,std::vector<Model>*const modelList,Transformation*transformation);
    void readTrafo  (std::vector<Anim>*const animList,Transformation*transformation);
private:
    Model*readData (const char *filesPath);                                                       /*Reads the file's data, seperates them to sequences, 
                                                                                                   *writes them well-ordered to allocated datafields and 
                                                                                                   *instantiates with the read data a new Model instance.
                                                                                                   */
    std::fstream fs;                                                                                                                     //fstream Object
    std::string s;                                                                                                                    //filestream String
    std::string name;                                                                                                         //To store the model's name
    void allocPntrs();                                                                        //Allocates enought memory fpr the the needed datasequences
    unsigned int amtPnts;                                                                                                              //Number of points
    unsigned int amtFaces;                                                                                                              //Number of faces
    std::vector<unsigned int>pointsPerFace;                                                                          //Number of points per face sequence
    std::vector<std::vector<float>>alignment;
    std::vector<float>pos;
    std::vector<std::vector<float>> points;
    std::vector<std::vector<unsigned short>>faces;
};

#endif
