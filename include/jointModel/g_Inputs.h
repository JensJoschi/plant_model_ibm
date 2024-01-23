//This file is not meant for sharing. If you have received the file in error, please email us immediately at info@ecolopes.org

// --------------------------------------------------------------------------
// TEMPORARY. PLANT MODEL SHOULD NOT INCLUDE JOINT MODEL CODE IN FOLDER SRC. 
// --------------------------------------------------------------------------



 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // JJ
 //--------------------------------------------------------------------------

 /*!
 * \file g_Inputs.h
 * \brief Inputs template class
 * \details  Contains all inputs, including global simulation parameters and data. 
 * \version 1.0
 * \date 2023
 */

#ifndef GSP_H
#define GSP_H

#include "g_GSP_BASE.h"
#include "g_Data_BASE.h"

/** @cond */
#include <nlohmann/json.hpp>
#include <string>
#include "a_generalFunctions_vmvc.h"
#include "easylogging++.h"
/** @endcond */

/**
 * \brief stores inputs
 * \details to be used with GSP_BASE and Data_BASE and their derived classes
 * 
 * \tparam gsp_T GSP of type GSP_BASE, or any derived class
 * \tparam data_T Data of type Data_BASE, or any derived class
 * \note this template class does not check whether types provided match (e.g. GSP_BASE and Data_ANIMAL)
 * \note one could also use gsp_BASE and data_BASE member variables, which would accept any derived type (i.e. not a template function). However,
 * "Inputs("filename.json")" would not work, because the compiler would not know which derived type to use. Making it a template function allows 
 * us to write "Inputs<DerivedGSP, DerivedData>("filename.json")" and the compiler will know which derived types to use.
 */
template<class gsp_T, class data_T>
struct Inputs{
    gsp_T config;
    data_T data;

    explicit Inputs(const std::string& inputsFile){
         LOG(DEBUG) << "Create Inputs from file " << inputsFile << ".";
        nlohmann::json j = generalFunctions::readJsonFile(inputsFile);
         LOG(DEBUG) << "--GSP";
        config  = gsp_T(j.at("GlobSimulFile"));
         LOG(DEBUG) << "--DATA";
        data = data_T(j.at("DATA"), config);
        assert(config.simulDuration > 0); //makes sure GSP was not default-constructed
         LOG(DEBUG) << "***Inputs created.";
    }
    bool checkKeys() const{
        return data.checkKeys(config);
    }
};
#endif //GSP_H