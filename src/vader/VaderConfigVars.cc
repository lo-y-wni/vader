/*
 * (C) Copyright 2023  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "oops/util/Logger.h"
#include "vader/VaderConfigVars.h"

namespace vader {

// ------------------------------------------------------------------------------------------------
VaderConfigVars::~VaderConfigVars() {
    oops::Log::trace() << "VaderConfigVars::~VaderConfigVars done" << std::endl;
}
// ------------------------------------------------------------------------------------------------
VaderConfigVars::VaderConfigVars(std::map<std::string, boost::any> configVariables) :
    configVariables_(configVariables) {}

// ------------------------------------------------------------------------------------------------
/*! \brief print
*
* \details **print** this method prints the members of this VaderClientConfig instance to an
           osstream
*
* \param[in] os the stream to print to
*
*/
void VaderConfigVars::print(std::ostream & os) const {
    oops::Log::trace() << "entering VaderConfigVars::print" <<  std::endl;
    // TODO(vahl): Implement this print method
    // std::string cookbookString("VADER Cookbook\ncookbook:\n");
    // for (const auto & cbMember : clientCookbook) {
    //     cookbookString += "  ";  // 2 space indent for yaml
    //     bool firstRecipe = true;
    //     cookbookString += cbMember.first + ": [";
    //     for (const auto & recipe : cbMember.second) {
    //         if (firstRecipe) {
    //             firstRecipe = false;
    //         } else {
    //             cookbookString += ",";
    //         }
    //         cookbookString += recipe->name();
    //     }
    //     cookbookString += "]\n";
    // }
    // os << cookbookString;π
    oops::Log::trace() << "leaving VaderConfigVars::print" <<  std::endl;
}
}  // namespace vader
