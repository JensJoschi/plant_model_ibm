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
//---------------------------------------------------------------------------
// Header file
// General Functions namespace. These functions are used in multiple classes.

/* SHORT DESCRIPTION:
 * This class contains only static functions that are used by all animal model class objects.
 * There are some print functions I want to place in here once I have time.
*/

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifndef GENERALFUNCTIONSCLASS_H
#define GENERALFUNCTIONSCLASS_H

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

// #include "a_homeRange_vmvc.h"
// #include "a_animalFunctionalGroupProgram_vmvc.h"


//@viky, seeing that all functions are static, I think you could move them all into one header file, into a namespace instead of a class?

// https://stackoverflow.com/questions/23841169/c-using-static-class-member-in-another-classs-non-static-function
// https://stackoverflow.com/questions/9346076/static-template-functions-in-a-class
class generalFunctions {
  public:
    // //// For coding templates: https://www.reddit.com/r/cpp_questions/comments/i2u4tt/linker_error_when_using_templates/g06z4ld
    // template<typename TK, typename TV> // to be used with function "extract_keys_vmvc".
    //   static std::vector<TK> extract_keys_vmvc(std::map<TK, TV> & input_map, bool print=false) {
    //     // https://www.codegrepper.com/code-examples/cpp/get+all+keys+in+map+cpp
    //     // https://www.lonecpluspluscoder.com/2015/08/13/an-elegant-way-to-extract-keys-from-a-c-map/
    //     std::vector<TK> retval;
    //     for (auto & element: input_map) {
    //       if (print) std::cout << element.first << ". ";
    //       retval.push_back(element.first);
    //      }
         
    //      if(print) std::cout << std::endl;
    //     return retval;
    //   }
      
template<typename TK>
static std::vector<std::string> extract_keys_vmvc_JJ(TK& input_map) {
  std::vector<std::string> retval;
  for (const auto& [key, value] : input_map) {
    retval.push_back(key);
  }
  return retval;
}
    
    // template<typename Iter, typename RandomGenerator>                        // to be used with function "selectRandomly_vmvc".
    //   static Iter selectRandomly_vmvc(Iter start, Iter end, RandomGenerator& g) {
    //     // https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
    //     // https://gist.github.com/cbsmith/5538174
    //     std::uniform_int_distribution<> dis(0, distance(start, end) - 1);
    //     advance(start, dis(g));
    //     return start;
    //   }
    
    // template<typename Iter> // to be used with function "selectRandomly_vmvc".
    //   static Iter selectRandomly_vmvc(Iter start, Iter end) {
    //     // https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
    //     // https://gist.github.com/cbsmith/5538174
    //     static std::random_device rd;
    //     static std::mt19937 gen(rd());
    //     return selectRandomly_vmvc(start, end, gen);
    //   }
    
    // static double pythagorasTheorem_vmvc(int x1, int y1, int x2, int y2);
    



    // static double z01_cb_vmvc();
    
    // static std::vector<double> findCoordinateOnACircle_vmvc(double r, double Pa);
    
    // static int randomNumberInRange_vmvc(int minN, int maxN);
    
    
    // //// DistanceList functions.
    // /**
    //  * \brief Cond function for a std::sort() function.
    //  * \details This function ....
    // */
    // static bool sortcol_vmvc(std::vector<double>& v1, std::vector<double>& v2);
    
    
    // //// Input and output functions.
    // static double** readInLandscape_cb_vmvc_double(std::string readInfolderName, std::string readInfileName, int NXY, int sea); // !!!! turn functions into templetes for int** and double**
    // static int** readInLandscape_cb_vmvc(std::string readInfolderName, std::string readInfileName, int NXY, int sea);
    
    // static void readOutLandscape_vmvc(double** resource, int NXY, int sea, std::string readOutfolderName="", std::string readOutfileName="readOutLandscape.txt", bool printOut=false, bool readOut=false); // !!!! turn functions into templetes for int** and double**
    // static void readOutLandscape_vmvc(int** resource, int NXY, int sea, std::string readOutfolderName="", std::string readOutfileName="readOutLandscape.txt", bool printOut=false, bool readOut=false);
    // static void readOutAnimalHRFileDuringRun_cb_vmvc(std::map<std::string, std::map<int, homeRange* >>* homeRangeList, std::string fg, int kID, std::string readOutfolderName, std::string readOutfileName, bool write, bool append);
    
    /**
     * \brief read json file into a json object
     * \details This function reads a file and stores the result in a json object
     * 
     * \param paramSimulFile Filename
     * @return nlohmann::json resulting json object
     */
    static nlohmann::json readJsonFile(const std::string& paramSimulFile);

    /**
     * \brief turn a key into a vector
     * \details This function works with keys of form "(0, 4, 72)", and supports any number of dimensions (usually 3, but can also be 2). 
     * Note that it will break when keys are negative, do not contain the extra space after the comma, or contain any other characters than numbers, commas and spaces.
     * Note that the function will NOT break on all posisble cases, e.g. "(0, 4, 72,)" or "(0,, 0)". It is up to the caller to do proper error checks. 
     */
    static std::vector<int> stringToVector(std::string_view key);

//     static void readInAnimalHRFile_vmvc(string readInfolderName, string readInfileName, map<string, map<int, homeRange* >>* homeRangeList , animalFunctionalGroupProgram* TraitsFGs){
//       //// Reading a list of home ranges from a file. Function should be used in tandem with "readInLandscape_cb_vmvc()"
//       //// or "homeRangeProgram::depleteResourceMapForReadInHomeRange_vmvc"so that you have the corresponding depleted
//       //// resource landscape(s) that the home ranges live upon.
//       int fileindex;              // "ID". We will modify this by: -1 * ("ID" + 1) to avoid 0s and not to mess around with the HR initialisation function.
//       string animal;              // "animal"
//       string AFG;                 // "AFG"
//       double mass;                // "mass_kg"
//       int x;                      // "x"
//       int y;                      // "y"
//       double area;                // "area_ha"
//       int hrDLCount;              // "hrDistanceListCount"
//       string howDied;             // "howDied"
//       int age;                    // "age"
//       int tBorn;                  // "timeStepBorn"
//       int tDied;                  // "timeStepDied"
//       
//       
//       //// Upload home ranges from file.
//       string fileName1 = readInfolderName + readInfileName;
//       char const *filename=fileName1.c_str();                         // filename https://stackoverflow.com/questions/29270202/c-cannot-initialize-a-variable-of-type-char-with-an-rvalue-of-type-char
//       ifstream infile;
//       infile.open (filename);
//       
//       while (infile >> fileindex >> animal >> AFG >> mass >> x >> y >> area >> hrDLCount >> howDied >> age >> tBorn >> tDied){
//         (*homeRangeList)[AFG].insert(make_pair(-(fileindex+1), new homeRange(TraitsFGs->getTraitsAnimalFunctionalGroups_vmvc(AFG), mass, {x, y}, area, hrDLCount, tBorn, "notzero")));
//         (*homeRangeList)[AFG][fileindex]->setHowDied_vmvc(howDied);   // "howDied"
//         (*homeRangeList)[AFG][fileindex]->setAge_vmvc(age);           // "age"
//         (*homeRangeList)[AFG][fileindex]->setTimeStepDied_vmvc(tDied);// "timeStepDied"
//       }
//       
//       infile.close();
//     }
  
};

#endif

