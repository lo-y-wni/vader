/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <map>
#include <string>
#include <vector>
#include <boost/any.hpp>

#include "oops/util/abor1_cpp.h"
#include "oops/util/Logger.h"
#include "oops/util/Printable.h"

namespace vader {

// ------------------------------------------------------------------------------------------------
/*! \brief VaderConfigVars
 *
 *  \details This class is used to store physical constants and other model-specific constants
 *           that might be needed by recipes.
 */

class VaderConfigVars  : public util::Printable {
 public:
    explicit VaderConfigVars(std::map<std::string, boost::any> configVariables = {});
    ~VaderConfigVars();

    template <class T>
    const T getFromConfig(std::string name) const {
        oops::Log::trace() << "VaderConfigVars::getFromConfig Starting" << std::endl;
        if (configVariables_.find(name) == configVariables_.end()) {
            ABORT("VaderConfigVars::getFromConfig: '" + name + "' is not present in Vader's "
                    "configuration variables and is required for a recipe. Set it via the Vader "
                    "constructor.\n");
        }
        T returnValue;
        try {
            returnValue = boost::any_cast<T>(configVariables_.at(name));
        }
        catch (boost::bad_any_cast &e) {
            ABORT("VaderConfigVars::getFromConfig: Unsuccessful retreival of '" + name +
                  "'. Check the data type used in VaderConstructConfig::addToConfig matches the "
                  "type in Vader recipes." + '\n' + e.what() + '\n');
        }
        oops::Log::trace() << "VaderConfigVars::getFromConfig Done" << std::endl;
        return returnValue;
    }

 private:
    const std::map<std::string, boost::any> configVariables_;
    void print(std::ostream &) const;
};

}  // namespace vader
