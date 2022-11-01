#include "sample_module.hpp"

SampleModule::SampleModule(int number) : number(number)
{
}

int SampleModule::GetNumber()
{
    return number;
}