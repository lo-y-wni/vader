/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <map>
#include <vector>

#include "oops/util/Logger.h"
#include "vader/recipes/TestRecipes.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initializations
const char Test_VarA_from_B::Name[] = "Test_VarA_from_B";
const char Test_VarA_from_C_D::Name[] = "Test_VarA_from_C_D";
const char Test_VarB_from_E::Name[] = "Test_VarB_from_E";
const char Test_VarB_from_A::Name[] = "Test_VarB_from_A";
const char Test_VarC_from_F::Name[] = "Test_VarC_from_F";

// Register the makers
static RecipeMaker<Test_VarA_from_B> makerTestVarA_A_(Test_VarA_from_B::Name);
static RecipeMaker<Test_VarA_from_C_D> makerTestVarA_B_(Test_VarA_from_C_D::Name);
static RecipeMaker<Test_VarB_from_E> makerTestVarB_A_(Test_VarB_from_E::Name);
static RecipeMaker<Test_VarB_from_A> makerTestVarB_B_(Test_VarB_from_A::Name);
static RecipeMaker<Test_VarC_from_F> makerTestVarC_A_(Test_VarC_from_F::Name);

Test_VarA_from_B::Test_VarA_from_B(const Parameters_ & params,
                                   const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "Test_VarA_from_B::Test_VarA_from_B(params)" << std::endl;
}
Test_VarA_from_C_D::Test_VarA_from_C_D(const Parameters_ & params,
                                       const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "Test_VarA_from_C_D::Test_VarA_from_C_D(params)" << std::endl;
}
Test_VarB_from_E::Test_VarB_from_E(const Parameters_ & params,
                                   const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "Test_VarB_from_E::Test_VarB_from_E(params)" << std::endl;
}
Test_VarB_from_A::Test_VarB_from_A(const Parameters_ & params,
                                   const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "Test_VarB_from_A::Test_VarB_from_A(params)" << std::endl;
}
Test_VarC_from_F::Test_VarC_from_F(const Parameters_ & params,
                                   const VaderConfigVars & configVariables)
{
    oops::Log::trace() << "Test_VarC_from_F::Test_VarC_from_F(params)" << std::endl;
}

std::string Test_VarA_from_B::name() const
{
    return Test_VarA_from_B::Name;
}
std::string Test_VarA_from_C_D::name() const
{
    return Test_VarA_from_C_D::Name;
}
std::string Test_VarB_from_E::name() const
{
    return Test_VarB_from_E::Name;
}
std::string Test_VarB_from_A::name() const
{
    return Test_VarB_from_A::Name;
}
std::string Test_VarC_from_F::name() const
{
    return Test_VarC_from_F::Name;
}

void Test_VarA_from_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering/leaving empty Test_VarA_from_B::executeNL function"
        << std::endl;
}
void Test_VarA_from_C_D::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering/leaving empty Test_VarA_from_C_D::executeNL function"
        << std::endl;
}
void Test_VarB_from_E::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering/leaving empty Test_VarB_from_E::executeNL function"
        << std::endl;
}
void Test_VarB_from_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering/leaving empty Test_VarB_from_A::executeNL function"
        << std::endl;
}
void Test_VarC_from_F::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering/leaving empty Test_VarC_from_F::executeNL function"
        << std::endl;
}

}  // namespace vader
