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

#include "oops/util/Logger.h"
#include "oops/util/Printable.h"

// Recipe headers
#include "recipes/AirPotentialTemperature.h"
#include "recipes/AirPressureThickness.h"
#include "recipes/AirTemperature.h"
#include "recipes/AirVirtualTemperature.h"
#include "recipes/DryAirDensityLevelsMinusOne.h"
#include "recipes/uwind_at_10m.h"
#include "recipes/vwind_at_10m.h"

namespace vader {

typedef std::map<std::string, std::vector<std::string>> cookbookConfigType;

// ------------------------------------------------------------------------------------------------
/*! \brief VaderConstructConfig
 *
 *  \details This struct can optionally be be passed as a parameter to the Vader constructor.
 *
 *           It is designed to contain configuration of Vader that will likely be unchanging
 *           for a particular client of Vader. Using this struct (instead of VaderParameters)
 *           makes it easier for a Vader client to define these options in code instead of in YAML,
 *           which reduces YAML duplication and maintenance.
 *
 *           Typically a Vader client who wants to construct a Vader object will first construct
 *           a VaderConstructConfig struct using a desired cookbook. Then they will call the 
 *           'addToConfig` method once for each constant that is needed by the recipes they use.
 *           Then pass the VaderConstructConfig object to the Vader constructor.
 *           (If unsure which constants are needed, they can just construct Vader without any
 *           constants and see what constants cause errors.)
 */

struct VaderConstructConfig  : public util::Printable {
 public:
    VaderConstructConfig(const cookbookConfigType & cookbook_ = {
            // Default VADER cookbook definition
            {"potential_temperature",  {AirPotentialTemperature_A::Name}},
            {"virtual_temperature",    {AirVirtualTemperature_A::Name}},
            {"air_temperature",        {AirTemperature_A::Name, AirTemperature_B::Name}},
            {"uwind_at_10m",           {uwind_at_10m_A::Name}},
            {"vwind_at_10m",           {vwind_at_10m_A::Name}},
            {"dry_air_density_levels_minus_one",  {DryAirDensityLevelsMinusOne_A::Name}}
        },
        std::map<std::string, boost::any> configVariables = {});
    ~VaderConstructConfig();
    const std::map<std::string, boost::any> & getConfigVars() const { return configVariables_; }
    const cookbookConfigType & cookbook;

    template <class T>
    void addToConfig(std::string name, T data) {
        oops::Log::trace() << "VaderConstructConfig::addToConfig Starting" << std::endl;
        configVariables_[name] = data;
        oops::Log::trace() << "VaderConstructConfig::addToConfig Done" << std::endl;
    }

 private:
    std::map<std::string, boost::any> configVariables_;
    void print(std::ostream &) const;
};

}  // namespace vader
