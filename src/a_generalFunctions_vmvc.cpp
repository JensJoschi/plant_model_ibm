//This file is not meant for sharing. If you have received the file in error, please email us immediately at info@ecolopes.org

// --------------------------------------------------------------------------
// TEMPORARY. PLANT MODEL SHOULD NOT INCLUDE JOINT MODEL CODE IN FOLDER SRC. 
// ALSO, THE FUNCTIONS NOT COMMENTED OUT HERE WILL LIKELY MOVE ELSEWHERE.
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Authors and contributors to this file:
// Victoria Culshaw (functions that are commented out)
// Jens Joschinski (all other functions)
// --------------------------------------------------------------------------


//---------------------------------------------------------------------------
// cpp file
// General Functions class. These static functions are used in multiple classes.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <math.h>
#include <random>
#include <string>
#include <vector>
#include <charconv>
#include "easylogging++.h"
#include "nlohmann/json.hpp"
#include "a_generalFunctions_vmvc.h"
// #include "a_homeRange_vmvc.h"

using std::string;
using std::vector;
using std::ifstream;
using std::cout;
using std::endl;
using std::ofstream;
using std::ios;
using std::map;


// //// General functions.
// double generalFunctions::pythagorasTheorem_vmvc(int x1, int y1, int x2, int y2){
//   return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
// }

// double generalFunctions::z01_cb_vmvc(){
//   // random number generator [0,1]
//   double z;
//   z=(double)rand()/(RAND_MAX+1.0);
//   return z;
// }

// vector<double> generalFunctions::findCoordinateOnACircle_vmvc(double r, double Pa){
// // This function returns the coordinate at radian angle 
// // Pa from the origin (0,0) on the circle with radius n.
//   double x = r * cos(Pa);
//   double y = r * sin(Pa);
  
//   return {x, y};
// }

// int generalFunctions::randomNumberInRange_vmvc(int minN, int maxN){
//   return rand()%((int)maxN-(int)minN+1);
// }


// //// DistanceList functions.
// bool generalFunctions::sortcol_vmvc(vector<double>& v1, vector<double>& v2) {
//   // Driver function to sort the 2D vector
//   // on basis of a particular column in 
//   // descending order.
//   // https://www.geeksforgeeks.org/sorting-2d-vector-in-c-set-2-in-descending-order-by-row-and-column
//   // https://stackoverflow.com/questions/26131112/why-stdsort-requires-static-compare-function
//   return v1[0] < v2[0];
// }

// //// Input and output functions.
// double** generalFunctions::readInLandscape_cb_vmvc_double(string readInfolderName, string readInfileName, int NXY, int sea){
//   //// Read in a NXY by NXY resources landscapes.
//   int seaNXY = NXY + 2 * sea;
// //   vector<vector<double>> resource (*seaNXY, vector<double> (*seaNXY, 0));
//   double cap=0;
  
//   //// Create and put the zero sea into the resource landscape.
//   double** resource;
//   resource = new double* [seaNXY];
//   for (int i = 0; i < seaNXY; i ++){
//     resource[i] = new double[seaNXY];
    
//     for (int j = 0; j < seaNXY; j ++)
//       resource[i][j] = 0;
//   }
  
  
//   for (int i=0;i<seaNXY;++i)
//     for (int j=0;j<seaNXY;++j)
//       resource[i][j] = 0;
  
  
//   //// Upload resource landscape from file.
//   string fileName1 = readInfolderName + readInfileName;
//   char const *filename=fileName1.c_str();      // filename https://stackoverflow.com/questions/29270202/c-cannot-initialize-a-variable-of-type-char-with-an-rvalue-of-type-char
//   ifstream landscape_file;
//   landscape_file.open (filename);
//   if(!landscape_file.is_open()) {
//     std::cout<<"Unable to open the file " << filename <<std::endl;
//     exit(EXIT_FAILURE);
//   }
  
//   for (int i=sea;i<(NXY+sea);++i){ // for(int i=0;i<*NXY;i++)
//     for (int j=sea;j<(NXY+sea);++j){ // for(int j=0;j<*NXY;j++){
//       landscape_file>>cap;
      
//       resource[i][j]=cap;
      
//     }
//   }
//   landscape_file.close();
  
//   return resource;
  
// //     cout << endl;
// //     for (int i=0;i<seaNXY;++i){
// //       for (int j=0;j<seaNXY;++j)
// //         cout << resource[i][j] << "	";
// //       cout << endl;
// //     }
// //     cout << endl;
  
// }

// int** generalFunctions::readInLandscape_cb_vmvc(string readInfolderName, string readInfileName, int NXY, int sea){
//   //// Read in a NXY by NXY landscape
//   int cap=0;
//   int seaNXY = NXY + 2 * sea;
//   int** resource; //   vector<vector<int>> resource (seaNXY, vector<int> (seaNXY, 0));
//   resource = new int* [seaNXY];
//   for (int i = 0; i < seaNXY; i ++){
//     resource[i] = new int[seaNXY];
    
//     for (int j = 0; j < seaNXY; j ++)
//       resource[i][j] = 0;
//   }
  
  
//   //// Upload resource landscape from file.
//   string fileName1 = readInfolderName + readInfileName;
//   char const *filename=fileName1.c_str();      // filename https://stackoverflow.com/questions/29270202/c-cannot-initialize-a-variable-of-type-char-with-an-rvalue-of-type-char
//   ifstream landscape_file;
//   landscape_file.open (filename);
//   if(!landscape_file.is_open()) {
//     cout<<"Unable to open the file " << filename <<endl;
//     exit(EXIT_FAILURE);
//   }
  
  
//   for (int i=sea;i<(NXY+sea);++i){ // for(int i=0;i<*NXY;i++)
//     for (int j=sea;j<(NXY+sea);++j){ // for(int j=0;j<*NXY;j++){
//       landscape_file>>cap;                          // with small landscapes cap is int 0-100
       
       
//       //// 1. read in of integers
//       resource[i][j]=cap;
//     }
//   }
//   landscape_file.close();
  
// //     cout << endl;
// //     for (int i=0;i<seaNXY;++i){
// //       for (int j=0;j<seaNXY;++j)
// //         cout << resource[i][j] << "	";
// //       cout << endl;
// //     }
// //     cout << endl;
    
//   return resource;
  
// }

// void generalFunctions::readOutLandscape_vmvc(double** resource, int NXY, int sea, string readOutfolderName, string readOutfileName, bool printOut, bool readOut){
//   int seaNXY = NXY + 2 * sea;
  
//   //// creates and writes result file.
//   if(readOut){
//     ofstream writeanimal;
//     string filename1 = readOutfolderName + readOutfileName;
//     char const *filename2=filename1.c_str(); // https://stackoverflow.com/questions/29270202/c-cannot-initialize-a-variable-of-type-char-with-an-rvalue-of-type-char
    
//     writeanimal.open(filename2, ios::trunc); // ios::app adds information to end of file, ios::trunc overwrite.
//     for (int i=0;i<seaNXY;++i){ // for (int i=0;i<*NXY;++i)
//       for (int j=0;j<seaNXY;++j){ // for (int j=0;j<*NXY;++j){
//         writeanimal << resource[i][j] << "	";
//       }
//     }
//     writeanimal << endl;
//     writeanimal.close();
//   }
  
  
//   //// prints result onto the terminal.
//   if(printOut){
//     for (int i=0;i<seaNXY;++i){ // for (int i=0;i<*NXY;++i){
//       for (int j=0;j<seaNXY;++j){ // for (int j=0;j<*NXY;++j){
//         cout << resource[i][j] << "	";
//       }
//       cout << endl;
//     }
//   }
// }

// void generalFunctions::readOutLandscape_vmvc(int** resource, int NXY, int sea, string readOutfolderName, string readOutfileName, bool printOut, bool readOut){
//   int seaNXY = NXY + 2 * sea;
  
//   //// creates and writes result file.
//   if(readOut){
//     ofstream writeanimal;
//     string filename1 = readOutfolderName + readOutfileName;
//     char const *filename2=filename1.c_str(); // https://stackoverflow.com/questions/29270202/c-cannot-initialize-a-variable-of-type-char-with-an-rvalue-of-type-char
    
//     writeanimal.open(filename2, ios::trunc); // ios::app adds information to end of file, ios::trunc overwrite.
//     for (int i=0;i<seaNXY;++i) // for (int i=0;i<*NXY;++i)
//       for (int j=0;j<seaNXY;++j){ // for (int j=0;j<*NXY;++j){
//         writeanimal << resource[i][j] << "	";
//       }
//       writeanimal << endl;
//       writeanimal.close();
//   }
  
  
//   //// prints result onto the terminal.
//   if(printOut){
//     for (int i=0;i<seaNXY;++i){ // for (int i=0;i<*NXY;++i){
//       for (int j=0;j<seaNXY;++j){ // for (int j=0;j<*NXY;++j){
//         cout << resource[i][j] << "	";
//       }
//       cout << endl;
//     }
//   }
// }

// void generalFunctions::readOutAnimalHRFileDuringRun_cb_vmvc(map<string, map<int, homeRange* >>* homeRangeList, string fg, int kID, string readOutfolderName, string readOutfileName, bool write, bool append){
//   //// writes a line in results file for each successful animal any line in results file:
//   ////   ID animal AFG mass_kg x y area_ha hrDistanceListCount howDied age timeStepBorn timeStepDied
//   //// 
//   //// creates and opens result file.
//   ofstream writeanimal;                                      // to get it independent from opening and writing.
//   string filename1=readOutfolderName+readOutfileName;
//   char const *filename2=filename1.c_str();                   // https://stackoverflow.com/questions/29270202/c-cannot-initialize-a-variable-of-type-char-with-an-rvalue-of-type-char
  
//   if(write){
//     writeanimal.open(filename2, ios::trunc);                 // ios::app adds information to end of file, ios::trunc overwrite.
//     writeanimal 
//     << "ID"
//     << '	' << "animal"
//     << '	' << "AFG"
//     << '	' << "mass_kg"
//     << '	' << "x"
//     << '	' << "y"
//     << '	' << "area_ha"
//     << '	' << "hrDistanceListCount"
//     << '	' << "howDied"
//     << '	' << "age"
//     << '	' << "timeStepBorn"
//     << '	' << "timeStepDied"
//     << endl;
//   }
  
//   if(append){
//     writeanimal.open(filename2, ios::app);                   // ios::app adds information to end of file, ios::trunc overwrite.
    
    
//     //// Write row in the result file.
//     writeanimal
//     << kID
//     << '	' << (*homeRangeList)[fg][kID]->getTaxClass_vmvc()
//     << '	' << fg
//     << '	' << (*homeRangeList)[fg][kID]->getMass_vmvc() 
//     << '	' << (*homeRangeList)[fg][kID]->getCentrePoint_vmvc()[0]
//     << '	' << (*homeRangeList)[fg][kID]->getCentrePoint_vmvc()[1]
//     << '	' << (*homeRangeList)[fg][kID]->getArea_vmvc()
//     << '	' << (*homeRangeList)[fg][kID]->getHrDistanceListCount_vmvc()
//     << '	' << (*homeRangeList)[fg][kID]->getHowDied_vmvc()
//     << '	' << (*homeRangeList)[fg][kID]->getAge_vmvc()
//     << '	' << (*homeRangeList)[fg][kID]->getTimeStepBorn_vmvc()
//     << '	' << (*homeRangeList)[fg][kID]->getTimeStepDied_vmvc()
//     << endl;
//   }
  
//   writeanimal.close();
// }

nlohmann::json generalFunctions::readJsonFile(const std::string& paramSimulFile){
  ifstream f(paramSimulFile.c_str());
  if (!f.good()) LOG(FATAL) << "the file " << paramSimulFile << " cannot be opened. Please check!";
  nlohmann::json j = nlohmann::json::parse(f);
  f.close();
  if (j.empty()) LOG(FATAL) << "the file " << paramSimulFile << " is empty. Please check!";
  return j;
}


std::vector<int> generalFunctions::stringToVector(std::string_view key){
  std::vector<int> result;
  assert(key.front() == '(' && key.back() == ')');
  key.remove_prefix(1); //remove brackets
  key.remove_suffix(1);
  assert(std::all_of(key.begin(), key.end(), [](char c){return c == ',' || c== ' ' || std::isdigit(c);})); //asserts that
  //the string only contains digits, space and commas. Does not catch all bugs, e.g. "" or "(4,,3)"

  while (!key.empty()) { //find region before next ", " place as int in result and shorten key accordingly
    size_t commaPos = key.find(',');
    if (commaPos == std::string_view::npos) {
      commaPos = key.size();
    }
    int value;
    std::from_chars(key.data(), key.data() + commaPos, value);
    result.push_back(value);
    if (commaPos != key.size()) {
      key.remove_prefix(commaPos + 2);  //this includes the space after the comma
    } else {
      key.remove_prefix(commaPos);
    }
  }
  return result;
}

