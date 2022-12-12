/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <vector>

#include "oops/util/Logger.h"
#include "vader/recipes/TestRecipes.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char TestVarA_A::Name[] = "TestVarA_A";

// Register the maker
static RecipeMaker<TestVarA_A> makerTestVarA_A_(TestVarA_A::Name);

TestVarA_A::TestVarA_A(const Parameters_ &params)
{
    oops::Log::trace() << "TestVarA_A::TestVarA_A(params)" << std::endl;
}

std::string TestVarA_A::name() const
{
    return TestVarA_A::Name;
}

bool TestVarA_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering/leaving empty TestVarA_A::executeNL function"
        << std::endl;

    return true;
}

}  // namespace vader
