/*
 * (C) Copyright 2023  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "oops/util/Logger.h"
#include "vader/VaderConstructConfig.h"

namespace vader {

// ------------------------------------------------------------------------------------------------
VaderConstructConfig::~VaderConstructConfig() {
    oops::Log::trace() << "VaderConstructConfig::~VaderConstructConfig done" << std::endl;
}
// ------------------------------------------------------------------------------------------------
VaderConstructConfig::VaderConstructConfig(const cookbookConfigType & cookbook_,
                                            std::map<std::string, boost::any> configVariables) :
    cookbook(cookbook_),
    configVariables_(configVariables) {}

// ------------------------------------------------------------------------------------------------
/*! \brief print
*
* \details **print** this method prints the members of this VaderConstructConfig instance to an
           osstream
*
* \param[in] os the stream to print to
*
*/
void VaderConstructConfig::print(std::ostream & os) const {
    oops::Log::trace() << "entering VaderConstructConfig::print" <<  std::endl;
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
    oops::Log::trace() << "leaving VaderConstructConfig::print" <<  std::endl;
}
}  // namespace vader
