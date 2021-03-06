/*
 * LanguageModel.cpp
 *
 *  Created on: Dec 10, 2016
 *      Author: louis
 */

#include "LanguageModel.h"

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include "cpyp/boost_serializers.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/dynamic_bitset.hpp>

#include "Logging.h"
#include <experimental/optional>
#include "BackoffStrategy.h"


namespace cpyp
{
	// full backoff
	template<unsigned N>
	double cpyp::PYPLM<N>::prob(const Pattern& w, const Pattern& context) const
	{
		const double bo = backoff.prob(w, context);
		return prob(w, context, bo);
	}

	template<unsigned N>
	double cpyp::PYPLM<N>::prob(const Pattern& w, const Pattern& context, double boProb) const
	{
		Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);

		auto it = p.find(lookup);
		if (it == p.end()) { // if the pattern is not in the train data
			return boProb;
		}
		return it->second.prob(w, boProb);
	}

	template<unsigned N>
	double cpyp::PYPLM<N>::prob4(const Pattern& w, const Pattern& context) const
	{

		double p_0 = backoff.backoff.backoff.backoff.p0; // -
		double p_1 = backoff.backoff.backoff.prob(w, p_0); // d

		double p_2_1; // cd
		{
			Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-3);

			auto it = backoff.backoff.p.find(lookup);
			if (it == backoff.backoff.p.end()) { // if the pattern is not in the train data
				p_2_1 = p_1;
			} else
			{
				p_2_1 = it->second.prob(w, p_1);
			}
		}

		double p_3_1; // bcd
		{
			Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-2);

			auto it = backoff.p.find(lookup);
			if (it == backoff.p.end()) { // if the pattern is not in the train data
				p_3_1 = p_2_1;
			} else
			{
				p_3_1 = it->second.prob(w, p_2_1);
			}
		}

		double p_4_1; // abcd
		{
			Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);

			auto it = p.find(lookup);
			if (it == p.end()) { // if the pattern is not in the train data
				p_4_1 = p_3_1;
			} else
			{
				p_4_1 = it->second.prob(w, p_3_1);
			}
		}

		return p_4_1;
	}

	template<unsigned N>
	std::pair <double,double> cpyp::PYPLM<N>::prob_() const
	{
		double p__ = backoff.backoff.backoff.backoff.p0; // -
//		L_S << "HPYPLM: probS4: fresh      " << p__ << "\n";

		L_A << "    : " << p__ << "\n";
		return std::pair <double,double> (1.0, p__);
	}

	template<unsigned N>
	std::pair <double,double> cpyp::PYPLM<N>::prob_d(const Pattern& w, SLM::InterpolationStrategy* is) const
	{
		double p_d = backoff.backoff.backoff.prob(w, prob_().second); // d
//		L_S << "HPYPLM: probS4: fresh    d " << p_d << "\n";

		double w_d = is->get(Pattern());
		L_A << "   D: " << p_d << "\n";
		return std::pair <double,double> (w_d, p_d);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_cd(const Pattern& w, const Pattern& originalContext, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is) const
	{
		Pattern lookup = Pattern(originalContext, 2, 1);

		double p_cd = 0.0;
                
                std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
		if(!optionalValue)
		{
                    auto it = backoff.backoff.p.find(lookup.reverse());
                    if (it == backoff.backoff.p.end())
                    {
                            p_cd = prob_d(w, is).second;
                    } else
                    {
                            p_cd = it->second.prob(w, prob_d(w, is).second);
                    }
                    bs->addToCache(lookup+w, p_cd);
		} else
		{
                    p_cd = optionalValue.value();
		}

		double w_cd = is->get(lookup);
		L_A << "  CD: " << p_cd << "\n";
		return std::pair <double,double> (w_cd, p_cd);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_b_d(const Pattern& w, const Pattern& originalContext, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is) const
	{
		Pattern lookup = Pattern(originalContext, 1, 2).addskip(std::pair<int, int>(1,1));

		double p_b_d = 0.0;

                std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                if(!optionalValue)
		{
                    auto it = backoff.p.find(lookup.reverse());
                    if (it == backoff.p.end())
                    {
                        p_b_d = prob_d(w, is).second;
                    } else
                    {
                        p_b_d = it->second.prob(w, prob_d(w, is).second);
                    }
                    bs->addToCache(lookup+w, p_b_d);
		} else
		{
                    p_b_d = optionalValue.value();
		}

		double w_b_d = is->get(lookup);
		L_A << " B D: " << p_b_d << "\n";
		return std::pair <double,double> (w_b_d, p_b_d);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_a__d(const Pattern& w, const Pattern& originalContext, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is) const
	{
		Pattern lookup = originalContext.addskip(std::pair<int, int>(1,2));

		double p_a__d = 0.0;

		std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                if(!optionalValue)
		{
                    auto it = p.find(lookup.reverse());
                    if (it == p.end())
                    {
                        p_a__d = prob_d(w, is).second;
                    } else
                    {
                        p_a__d = it->second.prob(w, prob_d(w, is).second);
                    }
                    bs->addToCache(lookup+w, p_a__d);
		} else
		{
                    p_a__d = optionalValue.value();
		}

		double w_a__d = is->get(lookup);
		L_A << "A  D: " << p_a__d << "\n";
		return std::pair <double,double> (w_a__d, p_a__d);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_bcd(const Pattern& w, const Pattern& originalContext, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is) const
	{
		Pattern lookup = Pattern(originalContext, 1, 2);

		double p_bcd = 0.0;

		std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                if(!optionalValue)
		{
                    std::pair <double,double> x_cd = prob_cd(w, originalContext, bs, is);
                    std::pair <double,double> x_b_d = prob_b_d(w, originalContext, bs, is);
                    double backoffProb = (x_cd.first*x_cd.second + x_b_d.first*x_b_d.second)/(x_cd.first + x_b_d.first);

                    auto it = backoff.p.find(lookup.reverse());
                    if (it == backoff.p.end())
                    {
                        p_bcd = backoffProb;
                    } else
                    {
                        p_bcd = it->second.prob(w, backoffProb);
                    }
                    bs->addToCache(lookup+w, p_bcd);
		} else
		{
                    p_bcd = optionalValue.value();
		}

		double w_bcd = is->get(lookup);
		L_A << " BCD: " << p_bcd << "\n";
		return std::pair <double,double> (w_bcd, p_bcd);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_a_cd(const Pattern& w, const Pattern& originalContext, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is) const
	{
		Pattern lookup = originalContext.addskip(std::pair<int, int>(1,1));

		double p_a_cd = 0.0;

		std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                if(!optionalValue)
		{
                    std::pair <double,double> x_cd = prob_cd(w, originalContext, bs, is);
                    std::pair <double,double> x_a__d = prob_a__d(w, originalContext, bs, is);
                    double backoffProb = (x_cd.first*x_cd.second + x_a__d.first*x_a__d.second)/(x_cd.first + x_a__d.first);

                    auto it = p.find(lookup.reverse());
                    if (it == p.end())
                    {
                        p_a_cd = backoffProb;
                    } else
                    {
                        p_a_cd = it->second.prob(w, backoffProb);
                    }
                    bs->addToCache(lookup+w, p_a_cd);
		} else
		{
                    p_a_cd = optionalValue.value();
		}

		double w_a_cd = is->get(lookup);
		L_A << "A CD: " << p_a_cd << "\n";
		return std::pair <double,double> (w_a_cd, p_a_cd);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_ab_d(const Pattern& w, const Pattern& originalContext, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is) const
	{
		Pattern lookup = originalContext.addskip(std::pair<int, int>(2,1));

		double p_ab_d = 0.0;

		std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                if(!optionalValue)
		{
                    std::pair <double,double> x_b_d = prob_b_d(w, originalContext, bs, is);
                    std::pair <double,double> x_a__d = prob_a__d(w, originalContext, bs, is);
                    double backoffProb = (x_b_d.first*x_b_d.second + x_a__d.first*x_a__d.second)/(x_b_d.first + x_a__d.first);

                    auto it = p.find(lookup.reverse());
                    if (it == p.end())
                    {
                        p_ab_d = backoffProb;
                    } else
                    {
                        p_ab_d = it->second.prob(w, backoffProb);
                    }
                    bs->addToCache(lookup+w, p_ab_d);
		} else
		{
                    p_ab_d = optionalValue.value();
		}

		double w_ab_d = is->get(lookup);
		L_A << "AB D: " << p_ab_d << "\n";
		return std::pair <double,double> (w_ab_d, p_ab_d);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_abcd(const Pattern& w, const Pattern& context, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is) const
	{
		double p_abcd;

                std::experimental::optional<double> optionalValue = bs->getFromCache(context+w);
                if(!optionalValue)
		{
                    double backoffProb = 0.0;

                    auto it = p.find(context.reverse());
                    std::pair <double,double> x_ab_d = prob_ab_d(w, context, bs, is);
                    std::pair <double,double> x_a_cd = prob_a_cd(w, context, bs, is);
                    std::pair <double,double> x_bcd = prob_bcd(w, context, bs, is);
                    backoffProb = (x_ab_d.first*x_ab_d.second + x_a_cd.first*x_a_cd.second + x_bcd.first*x_bcd.second)/(x_ab_d.first + x_a_cd.first + x_bcd.first);

                    if (it == p.end())
                    {
                        p_abcd = backoffProb;
                    } else
                    {
                        p_abcd = it->second.prob(w, backoffProb);
                    }
                    bs->addToCache(context+w, p_abcd);
		} else
		{
                    p_abcd = optionalValue.value();
		}

		L_A << "ABCD: " << p_abcd << "\n";

		return std::pair <double,double> (1.0, p_abcd);
	}

/////////////////////////////////////////////////////////////////////

//        template<size_t s>
//        struct bitset_comparer
//        {
//            bool operator() (const boost::dynamic_bitset<s>& lhs, const boost::dynamic_bitset<s>& rhs) const
//            {
//                return lhs.to_ulong() < rhs.to_ulong();
//            }
//        };

        
        template<unsigned N>
        double cpyp::PYPLM<N>::getProb(const Pattern& w, const Pattern& context, double bprob, int level) const
        {
            if(level == 0)
            {
                auto it = p.find(context);
                if (it == p.end()) 
                    return bprob;
                return it->second.prob(w, bprob);
            }

            if(N == 1)
            {
                return backoff.p0;
            }

           return backoff.getProb(w, context, bprob, level-1);
        }

        double toBackoffProbs(const std::vector<std::pair<double, double>>& pairs)
        {
            double numerator = 0.0;
            double denominator = 0.0;
            for(std::pair<double, double> pair : pairs)
            {
                numerator += pair.first * pair.second;
                denominator += pair.first;
            }

            return numerator/denominator;
        }
/*
        Pattern maskContext(const Pattern& context, const boost::dynamic_bitset<>& skipPattern)
        {
            Pattern result = context;

            unsigned int iter = 0;
            for(std::size_t i = 0; i < skipPattern.size() && i < context.size(); ++i)
            {
                if(skipPattern.test(i))
                {
                    result.set_skip(result.size() - i);
                }
            }
        }
*/
//
        template<unsigned N>
        std::pair<double, double> cpyp::PYPLM<N>::prob_recursive(const Pattern& w, const Pattern& context, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is, const boost::dynamic_bitset<>& skipPattern) const
        {
            if(skipPattern.none())
            {
                double p = getProb(w, context, 1.0, N);
                return std::pair<double, double> (1.0, p);
            }

            if(skipPattern.to_ulong() == 1)
            {
                double bp = prob_recursive(w, context, bs, is, boost::dynamic_bitset<>()).second;
                double p = getProb(w, Pattern(), bp, N-1);

                double w = is->get(Pattern());
                return std::pair<double, double>( w, p);
            }
//
//            double p;
//
//            Pattern lookup = maskContext(originalContext, skipPattern);
//
//            std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
//            if(!optionalValue)
//            {
//
//                std::vector<std::pair<double,double>> pairs;
//                std::set<std::bitset<s>, bitset_comparer<s>> backoffs;
//
//                std::size_t fi = 0;
//                std::size_t i = 0;
//                for(i = bs.size() - 1; i >= 0; --i)
//                {
//                    if(bs.test(i))
//                    {
//                        fi = i;
//                        break;
//                    }
//                }
//
//                {
//                    std::bitset<s> nbs = bs;
//                    nbs.reset(i);
//                    pairs.push_back(prob_recursive(w, context, bs, is, nbs));
//                }
//                
//                for(fi = i-1; fi >= 1; --fi)
//                {
//                    std::bitset<s> nbs = bs;
//                    nbs.reset(fi);
//                    if(backoffs.count(nbs) == 0 && nbs != bs)
//                    {
//                        pairs.push_back(prob_recursive(w, context, bs, is, nbs));
//                    }
//                    backoffs.insert(nbs);
//                }
//
//                double backoffProb = toBackoffProb(pairs);
//
//                auto it = p.find(context.reverse());
//                if(it == p.end())
//                {
//                    p = backoffProb;
//                } else
//                {
//                    p = it->second.prob(w, backoffProb);
//                }
//                bs->addToCache(context+w, p);
//            } else
//            {
//                p = optionalValue.value();
//            }
//
//            L_A << skipPattern.to_ulong() << "\t" << p << "\n";
//
//            double w = is->get(lookup);
//            return std::pair<double, double> (w, p);
            return std::pair<double, double>(1.0, 1.0);
        }

////////////////////////////////////////////////////////////////////////////////////////////


	template<unsigned N>
	double cpyp::PYPLM<N>::probS4(const Pattern& w, const Pattern& context, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is) 
	{

		double p_abcd = prob_abcd(w, context, bs, is).second;
		return p_abcd;
	}















//	for(auto dish = restaurant.begin(); dish != restaurant.end(); dish++)

	template<unsigned N>
	std::vector<unsigned int> cpyp::PYPLM<N>::getCounts(const Pattern& context) const
	{
		int contextSize = context.size();
		std::vector<unsigned int> returnVector;

		if(contextSize == 0)
		{
			auto it = backoff.backoff.backoff.p.find(context.reverse());
			if (it == backoff.backoff.backoff.p.end())
			{
				return returnVector;
			} else
			{
				for(auto dish = it->second.begin(); dish != it->second.end(); dish++)
				{
					returnVector.push_back(dish->second.num_customers());
				}
			}
		}

		if(contextSize == 1)
		{
			auto it = backoff.backoff.p.find(context.reverse());
			if (it == backoff.backoff.p.end())
			{
				return returnVector;
			} else
			{
				for(auto dish = it->second.begin(); dish != it->second.end(); dish++)
				{
					returnVector.push_back(dish->second.num_customers());
				}
			}
		}

		if(contextSize == 2)
		{
			auto it = backoff.p.find(context.reverse());
			if (it == backoff.p.end())
			{
				return returnVector;
			} else
			{
				for(auto dish = it->second.begin(); dish != it->second.end(); dish++)
				{
					returnVector.push_back(dish->second.num_customers());
				}
			}
		}

		if(contextSize == 3)
		{
			auto it = p.find(context.reverse());
			if (it == p.end())
			{
				return returnVector;
			} else
			{
				for(auto dish = it->second.begin(); dish != it->second.end(); dish++)
				{
					returnVector.push_back(dish->second.num_customers());
				}
			}
		}

		return returnVector;
	}

	template<unsigned N>
	unsigned int cpyp::PYPLM<N>::getCount(const Pattern& w, const Pattern& context) const
	{
		int contextSize = context.size();

		if(contextSize == 0)
		{
			auto it = backoff.backoff.backoff.p.find(context.reverse());
			if (it == backoff.backoff.backoff.p.end())
			{
				return 0;
			} else
			{
				return it->second.num_customers(w);
			}
		}

		if(contextSize == 1)
		{
			auto it = backoff.backoff.p.find(context.reverse());
			if (it == backoff.backoff.p.end())
			{
				return 0;
			} else
			{
				return it->second.num_customers(w);
			}
		}

		if(contextSize == 2)
		{
			auto it = backoff.p.find(context.reverse());
			if (it == backoff.p.end())
			{
				return 0;
			} else
			{
				return it->second.num_customers(w);
			}
		}

		if(contextSize == 3)
		{
			auto it = p.find(context.reverse());
			if (it == p.end())
			{
				return 0;
			} else
			{
				return it->second.num_customers(w);
			}
		}

		return 0;
	}

	template<unsigned N>
	double cpyp::PYPLM<N>::getNormalisationFactor(const Pattern& context, const Pattern& originalContext, crp<Pattern> restaurant, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& normalisationCache, unsigned vocabSize)
	{
		L_S << "LanguageModel: getNormalisationFactor: context length: "  << " " << context.size() << "\n";
		int contextSize = context.size();

		auto iter = normalisationCache.find(context);
		if(iter == normalisationCache.end())
		{
			L_S << "LanguageModel: getNormalisationFactor: generating fresh: \n";

			int i  = 0;
			double ps = 0.0;
			for(auto dish = restaurant.begin(); dish != restaurant.end(); dish++)
			{
				++i;
				if(contextSize == 1 )
				{
					L_V << "LanguageModel: getNormalisationFactor: 1: \n";
					ps += prob_cd(dish->first, originalContext, bs, is).second;
				}

				if(contextSize == 2 )
				{
					if(context.isgap(1))
					{
						L_V << "LanguageModel: getNormalisationFactor: 2: \n";
						ps += prob_b_d(dish->first, originalContext, bs, is).second;
					} else
					{
						L_V << "LanguageModel: getNormalisationFactor: 3: \n";
						ps += prob_bcd(dish->first, originalContext, bs, is).second;
					}
				}

				if(contextSize == 3 )
				{
					if(context.isgap(1) && !context.isgap(2))
					{
						L_V << "LanguageModel: getNormalisationFactor: 4: \n";
						ps += prob_a_cd(dish->first, originalContext, bs, is).second;
					} else if(!context.isgap(1) && context.isgap(2))
					{
						L_V << "LanguageModel: getNormalisationFactor: 5: \n";
						ps += prob_ab_d(dish->first, originalContext, bs, is).second;
					} else if(context.isgap(1) && context.isgap(2))
					{
						L_V << "LanguageModel: getNormalisationFactor: 6: \n";
						ps += prob_a__d(dish->first, originalContext, bs, is).second;
					} else
					{
						double k = prob_abcd(dish->first, originalContext, bs, is).second;
						L_V << "LanguageModel: getNormalisationFactor: 7: " << k << "\n";
						ps += k;
					}
				}
			}

			L_S << "LanguageModel: getNormalisationFactor: number of words after context: " << i << "(of " << vocabSize << ")\n";
			L_S << "LanguageModel: getNormalisationFactor: with probability sum: " << ps << "\n";

			ps = std::min(ps, 1.0);

			if(ps > 1.0)
				L_S << "LanguageModel: getNormalisationFactor: [" << context.size() << "]!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";


			double rv = 0;
			if(ps > 0.0)
				rv = (1.0 - ps)/(vocabSize - i);

			normalisationCache.emplace(context, rv);

			return rv;


		} else
		{
			return iter->second;
		}
	}

	template<unsigned N>
	double cpyp::PYPLM<N>::probLS4(const Pattern& w, const Pattern& context, SLM::BackoffStrategy* bs, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& normalisationCache, unsigned vocabSize)
	{

		double p__ = backoff.backoff.backoff.backoff.p0; // -
		L_S << "HPYPLM: probLS4: fresh      " << p__ << "\n";

		double p_d = backoff.backoff.backoff.prob(w, p__); // d   ---  p__ = 1.0?
		if(!getCount(w, Pattern()))
			L_V << "HPYPLM: probLS4: fresh     " << p_d << "\n";
		else
			L_V << "HPYPLM: probLS4: fresh    d " << p_d << "\n";

		double w_cd;
		double p_cd;
		{
                    Pattern lookup = Pattern(context, 2, 1);
                    
                    std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                    if(!optionalValue)
                    {
                        auto it = backoff.backoff.p.find(lookup.reverse());
                        if (it == backoff.backoff.p.end())
                        {
                            L_S << "LanguageModel: probLS4: c NOT in model --> backoff \n";
                            p_cd = p_d;
                        } else
                        {
                            if(it->second.num_customers(w)) // we have seen the pattern uw -> no backoff
                            {
                                L_S << "LanguageModel: probLS4: cd in model --> no backoff \n";
                                p_cd = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, bs, is, normalisationCache, vocabSize));
                            }
                            else // we have not seen uw, but seen u -> backoff
                            {
                                L_S << "LanguageModel: probLS4: c* in model --> backoff \n";
                                p_cd = it->second.prob(w, p_d);
                            }
                        }
                        bs->addToCache(lookup+w, p_cd);
                    } else
                    {
                        p_cd = optionalValue.value();
                    }

                    w_cd = is->get(lookup);
		}

		double w_b_d;
		double p_b_d;
		{
                    Pattern lookup = Pattern(context, 1, 2).addskip(std::pair<int, int>(1,1));

                    std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                    if(!optionalValue)
                    {
                        auto it = backoff.p.find(lookup.reverse());
                        if (it == backoff.p.end())
                        {
                            L_S << "LanguageModel: probLS4: b_ NOT in model --> backoff \n";
                            p_b_d = p_d;
                        } else
                        {
                            if(it->second.num_customers(w)) // we have seen the pattern uw -> no backoff
                            {
                                L_S << "LanguageModel: probLS4: b_d in model --> no backoff \n";
                                p_b_d = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, bs, is, normalisationCache, vocabSize));
                            }
                            else // we have not seen uw, but seen u -> backoff
                            {
                                L_S << "LanguageModel: probLS4: b_* in model --> backoff \n";
                                p_b_d = it->second.prob(w, p_d);
                            }
                        }
                        bs->addToCache(lookup+w, p_b_d);
                    } else
                    {
                        p_b_d = optionalValue.value();
                    }

                    w_b_d = is->get(lookup);
		}

		double w_a__d;
		double p_a__d;
		{
                    Pattern lookup = context.addskip(std::pair<int, int>(1,2));

                    std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                    if(!optionalValue)
                    {
                        auto it = p.find(lookup.reverse());
                        if (it == p.end())
                        {
                            L_S << "LanguageModel: probLS4: a__ NOT in model --> backoff \n";
                            p_a__d = p_d;
                        } else
                        {
                            if(it->second.num_customers(w)) // we have seen the pattern uw -> no backoff
                            {
                                L_S << "LanguageModel: probLS4: a__d in model --> no backoff \n";
                                p_a__d = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, bs, is, normalisationCache, vocabSize));
                            }
                            else // we have not seen uw, but seen u -> backoff
                            {
                                L_S << "LanguageModel: probLS4: a__* in model --> backoff \n";
                                p_a__d = it->second.prob(w, p_d);
                            }
                        }
                        bs->addToCache(lookup+w, p_a__d);
                    } else
                    {
                        p_a__d = optionalValue.value();
                    }

                    w_a__d = is->get(lookup);
		}
//
//		//////
//
		double w_bcd;
		double p_bcd;
		{
                    Pattern lookup = Pattern(context, 1, 2);

                    std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                    if(!optionalValue)
                    {
                        double backoffProb = (w_cd*p_cd + w_b_d*p_b_d)/(w_cd + w_b_d);

                        auto it = backoff.p.find(lookup.reverse());
                        if (it == backoff.p.end())
                        {
                            L_S << "LanguageModel: probLS4: bc NOT in model --> backoff \n";
                            p_bcd = backoffProb;
                        } else
                        {
                            if(it->second.num_customers(w)) // we have seen the pattern uw -> no backoff
                            {
                                    L_S << "LanguageModel: probLS4: bcd in model --> no backoff \n";
                                    p_bcd = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, bs, is, normalisationCache, vocabSize));
                            }
                            else // we have not seen uw, but seen u -> backoff
                            {
                                    L_S << "LanguageModel: probLS4: bc* in model --> backoff \n";
                                    p_bcd = it->second.prob(w, backoffProb);
                            }
                        }
                        bs->addToCache(lookup+w, p_bcd);
                    } else
                    {
                        p_bcd = optionalValue.value();
                    }

                    w_bcd = is->get(lookup);
		}

		double w_a_cd;
		double p_a_cd;
		{
                    Pattern lookup = context.addskip(std::pair<int, int>(1,1));

                    std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                    if(!optionalValue)
                    {
                        double backoffProb = (w_cd*p_cd + w_a__d*p_a__d)/(w_cd + w_a__d);

                        auto it = p.find(lookup.reverse());
                        if (it == p.end())
                        {
                            L_S << "LanguageModel: probLS4: a_c NOT in model --> backoff \n";
                            p_a_cd = backoffProb;
                        } else
                        {
                            if(it->second.num_customers(w)) // we have seen the pattern uw -> no backoff
                            {
                                L_S << "LanguageModel: probLS4: a_cd in model --> no backoff \n";
                                p_a_cd = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, bs, is, normalisationCache, vocabSize));
                            }
                            else // we have not seen uw, but seen u -> backoff
                            {
                                L_S << "LanguageModel: probLS4: a_c* in model --> backoff \n";
                                p_a_cd = it->second.prob(w, backoffProb);
                            }
                        }
                        bs->addToCache(lookup+w, p_a_cd);
                    } else
                    {
                        p_a_cd = optionalValue.value();
                    }

                    w_a_cd = is->get(lookup);
		}

		double w_ab_d;
		double p_ab_d;
		{
                    Pattern lookup = context.addskip(std::pair<int, int>(2,1));

                    std::experimental::optional<double> optionalValue = bs->getFromCache(lookup+w);
                    if(!optionalValue)
                    {
                        double backoffProb = (w_b_d*p_b_d + w_a__d*p_a__d)/(w_b_d + w_a__d);

                        auto it = p.find(lookup.reverse());
                        if (it == p.end())
                        {
                            L_S << "LanguageModel: probLS4: ab_ NOT in model --> backoff \n";
                            p_ab_d = backoffProb;
                        } else
                        {
                            if(it->second.num_customers(w)) // we have seen the pattern uw -> no backoff
                            {
                                L_S << "LanguageModel: probLS4: ab_d in model --> no backoff \n";
                                p_ab_d = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, bs, is, normalisationCache, vocabSize));
                            }
                            else // we have not seen uw, but seen u -> backoff
                            {
                                L_S << "LanguageModel: probLS4: a_* in model --> backoff \n";
                                p_ab_d = it->second.prob(w, backoffProb);
                            }
                        }
                        bs->addToCache(lookup+w, p_ab_d);
                    } else
                    {
                        p_ab_d = optionalValue.value();
                    }

                    w_ab_d = is->get(lookup);
		}
//
//		//////
//
		double p_abcd;
		std::experimental::optional<double> optionalValue = bs->getFromCache(context+w);
                if(!optionalValue)
		{
                    double backoffProb = (w_bcd*p_bcd + w_a_cd*p_a_cd + w_ab_d*p_ab_d)/(w_bcd + w_a_cd + w_ab_d);

                    auto it = p.find(context.reverse());
                    if (it == p.end())
                    {
                        L_S << "LanguageModel: probLS4: abc NOT in model --> backoff \n";
                        p_abcd = backoffProb;
                    } else
                    {
                        if(it->second.num_customers(w)) // we have seen the pattern uw -> no backoff
                        {
                            L_S << "LanguageModel: probLS4: abcd in model --> no backoff \n";
                            p_abcd = it->second.prob(w, getNormalisationFactor(context, context, it->second, bs, is, normalisationCache, vocabSize));
                        }
                        else // we have not seen uw, but seen u -> backoff
                        {
                            L_S << "LanguageModel: probLS4: abc* in model --> backoff \n";
                            p_abcd = it->second.prob(w, backoffProb);
                        }
                    }
                    bs->addToCache(context+w, p_abcd);
		} else
		{
                    p_abcd = optionalValue.value();
		}

		return p_abcd;
	}
}


namespace SLM
{

LanguageModel::LanguageModel(const ProgramOptions& programOptions) {
	initialise(programOptions);

}

LanguageModel::~LanguageModel() {
//	delete indexedCorpus;
}

const PatternSet<uint64_t>& LanguageModel::getVocabulary() const
{
	return vocabulary;
}

Pattern LanguageModel::toPattern(const std::string& patternString, bool allowUnknown)
{
	return classEncoder.buildpattern(patternString, allowUnknown);
}

std::string LanguageModel::toString(const Pattern& pattern)
{
	return pattern.tostring(classDecoder);
}

void LanguageModel::initialise(const ProgramOptions& programOptions)
{
	L_I << "LanguageModel: Intialisation\n";

	L_V << "LanguageModel: Loading language model options\n";
	defaultPatternModelOptions();

	L_V << "LanguageModel: Loading vocabulary\n";
	classEncoder.load(programOptions.getTrainVocabulary());

	L_V << "LanguageModel: Loading class decoder\n";
	classDecoder.load(programOptions.getTrainVocabulary());

	L_V << "LanguageModel: Loading indexed corpus\n";
	indexedCorpus = new IndexedCorpus(programOptions.getTrainCorpus());

	L_V << "LanguageModel: Training pattern model\n";
	patternModel = PatternModel<uint32_t>(programOptions.getTrainPatternModel(), patternModelOptions, nullptr, indexedCorpus);

	L_V << "LanguageModel: Extending the class encoder\n";
	extendClassEncoder(programOptions.getTestInputFiles(), programOptions.getTestCorpus());

	L_V << "LanguageModel: Loading language model\n";
	loadLanguageModel(programOptions.getTrainLanguageModel());

	L_V << "LanguageModel: Initialise vocabulary\n";
	vocabulary = patternModel.extractset(1,1);
//	vocabulary = std::unordered_set<Pattern>(vocabularySet.begin(), vocabularySet.end());
	L_V << "LanguageModel: Vocabulary contains " << vocabulary.size() << " items\n";
}

void LanguageModel::loadLanguageModel(const std::string& inputFile)
{
	std::ifstream ifs(inputFile, std::ios::binary);
	if(!ifs.good())
	{
		std::cerr << "Something went wrong with loading the language model: " << inputFile << std::endl;
	}
	boost::archive::binary_iarchive ia(ifs);

	ia& lm;
}

void LanguageModel::defaultPatternModelOptions()
{
	patternModelOptions.MAXLENGTH = 4;
	patternModelOptions.MINLENGTH = 1;
	patternModelOptions.DOSKIPGRAMS_EXHAUSTIVE = true;
	patternModelOptions.DOREVERSEINDEX = true;
	patternModelOptions.QUIET = false;
	patternModelOptions.MINTOKENS = 1;
}

void LanguageModel::extendClassEncoder(const std::vector<std::string>& inputFiles, const std::string& outputFile)
{
	for(auto& i: inputFiles)
	{
		classEncoder.encodefile(i, outputFile, 1, 1, 0, 1);
	}

	classEncoder.save(outputFile);
	classDecoder.load(outputFile);
}

unsigned int LanguageModel::getCount(const Pattern& focus, const Pattern& context) const
{
	return lm.getCount(focus, context);
}

std::vector<unsigned int> LanguageModel::getCounts(const Pattern& context) const
{
	return lm.getCounts(context);
}

double LanguageModel::getProb(const Pattern& focus, const Pattern& context)
{
	return lm.prob(focus, context);
}

double LanguageModel::getProb4(const Pattern& focus, const Pattern& context)
{
	return lm.prob4(focus, context);
}

double LanguageModel::getProbS4(const Pattern& focus, const Pattern& context, SLM::BackoffStrategy* backoffStrategy, SLM::InterpolationStrategy* interpolationStrategy)
{
	return lm.probS4(focus, context, backoffStrategy, interpolationStrategy);
}

double LanguageModel::getProbLS4(const Pattern& focus, const Pattern& context, SLM::BackoffStrategy* backoffStrategy, SLM::InterpolationStrategy* interpolationStrategy, std::unordered_map<Pattern, double>& normalisationCache)
{
	return lm.probLS4(focus, context, backoffStrategy, interpolationStrategy, normalisationCache, getVocabularySize());
}

unsigned LanguageModel::getVocabularySize()
{
	if(vocabularySize == 0)
	{
		vocabularySize = vocabulary.size();
	}
	return vocabularySize;
}

bool LanguageModel::isOOV(const Pattern& word)
{
//	auto it = std::find(vocabulary.begin(), vocabulary.end(), word);
//	return it == vocabulary.end();
	return !vocabulary.has(word);
}

unsigned int LanguageModel::count(const Pattern& pattern)
{
	return patternModel.occurrencecount(pattern);
}

} /* namespace SLM */
