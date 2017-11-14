/*
 * ValueInterpolationStrategy.h
 *
 * Created on: Nov 14, 2017
 *     Author: louis
 */

#ifndef VALUEINTERPOLATIONSTRATEGY_H_
#define VALUEINTERPOLATIONSTRATEGY_H_

#include "InterpolationStrategy.h"

namespace SLM {

class ValueInterpolationWeights
{
    public:
        std::string toString() const;

        double w_d = 1.0;
        double w_cd = 1.0;
        double w_bcd = 1.0;
        double w_b_d = 1.0;
        double w_abcd = 1.0;
        double w_a_cd = 1.0;
        double w_ab_d = 1.0;
        double w_a__d = 1.0;
};

class ValueInterpolationStrategy: public InterpolationStrategy
{
    public:
        ValueInterpolationStrategy(SLM::ValueInterpolationWeights& viw);
        virtual ~ValueInterpolationStrategy();

        double get(const Pattern& context);
        std::string name() const;
    protected:
        SLM::ValueInterpolationWeights& weights;


}; // class

} // namespace

#endif // 
