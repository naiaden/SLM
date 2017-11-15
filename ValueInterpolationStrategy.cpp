/*
 * ValueInterpolationStrategy.cpp
 *
 *  Created on: Nov 14, 2017
 *      Author: louis
 */

#include "ValueInterpolationStrategy.h"

namespace SLM
{

// d - cd - bcd - b_d - abcd - a_cd - ab_d - a__d
std::string ValueInterpolationWeights::toString() const
{
    std::stringstream ss;
    ss << w_d << "-" << w_cd << "-" << w_bcd << "-" << w_b_d << "-"
       << w_abcd << "-" << w_a_cd << "-" << w_ab_d << "-" << w_a__d;
    return ss.str();
}

ValueInterpolationStrategy::ValueInterpolationStrategy(ValueInterpolationWeights& viw) : weights(viw)
{}

ValueInterpolationStrategy::~ValueInterpolationStrategy()
{}

double ValueInterpolationStrategy::get(const Pattern& context)
{
    //std::cout << "\t\t" << context.size() << "-" << weights.toString() << std::endl;
    int contextSize = context.size();

    if(contextSize == 0)
    {
        return weights.w_d;
    }

    if(contextSize == 1)
    {
        return weights.w_cd;
    }

    if(contextSize == 2)
    {
        if(context.isgap(1))
            return weights.w_b_d;
        return weights.w_bcd;
    }

    if(contextSize == 3)
    {
        if(context.isgap(1) && !context.isgap(2))
            return weights.w_a_cd;
        if(context.isgap(2) && !context.isgap(1))
            return weights.w_ab_d;
        if(context.isgap(1) && context.isgap(2))
            return weights.w_a__d;
        return weights.w_abcd;
    }
        
}

std::string ValueInterpolationStrategy::name() const
{
    return "value-" + weights.toString();
}

}
