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

#include "Logging.h"


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
	std::pair <double,double>  cpyp::PYPLM<N>::prob_cd(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& cache) const
	{
		Pattern lookup = Pattern(originalContext, 2, 1);

		double p_cd = 0.0;

		//std::map<Pattern,double>::const_iterator i_cd = cache.find(lookup+w);
		auto i_cd = cache.find(lookup+w);
		if(i_cd == cache.end())
		{
			auto it = backoff.backoff.p.find(lookup.reverse());
			if (it == backoff.backoff.p.end())
			{
				p_cd = prob_d(w, is).second;
			} else
			{
				p_cd = it->second.prob(w, prob_d(w, is).second);
			}
//			L_S << "HPYPLM: probS4: fresh   cd " << p_cd << "\n";
			cache[lookup+w] = p_cd;
		} else
		{
			p_cd = i_cd->second;
//			L_S << "HPYPLM: probS4: cache   cd " << p_cd << "\n";
		}

		double w_cd = is->get(lookup);
		L_A << "  CD: " << p_cd << "\n";
		return std::pair <double,double> (w_cd, p_cd);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_b_d(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& cache) const
	{
		Pattern lookup = Pattern(originalContext, 1, 2).addskip(std::pair<int, int>(1,1));

		double p_b_d = 0.0;

		auto i_b_d =  cache.find(lookup+w);
		if(i_b_d == cache.end())
		{
			auto it = backoff.p.find(lookup.reverse());
			if (it == backoff.p.end())
			{
				p_b_d = prob_d(w, is).second;
//				L_A << " B D:           1\n";
			} else
			{
				p_b_d = it->second.prob(w, prob_d(w, is).second);
//				L_A << " B D:           2B\n";
			}
			cache[lookup] = p_b_d;
		} else
		{
			p_b_d = i_b_d->second;
//			L_A << " B D:           3\n";
		}

		double w_b_d = is->get(lookup);
		L_A << " B D: " << p_b_d << "\n";
		return std::pair <double,double> (w_b_d, p_b_d);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_a__d(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& cache) const
	{
		Pattern lookup = originalContext.addskip(std::pair<int, int>(1,2));

		double p_a__d = 0.0;

		auto i_a__d = cache.find(lookup+w);
		if(i_a__d == cache.end())
		{
			auto it = p.find(lookup.reverse());
			if (it == p.end())
			{
				p_a__d = prob_d(w, is).second;
			} else
			{
				p_a__d = it->second.prob(w, prob_d(w, is).second);
			}
			cache[lookup] = p_a__d;
		} else
		{
			p_a__d = i_a__d->second;
		}

		double w_a__d = is->get(lookup);
		L_A << "A  D: " << p_a__d << "\n";
		return std::pair <double,double> (w_a__d, p_a__d);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_bcd(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& cache) const
	{
		Pattern lookup = Pattern(originalContext, 1, 2);

		double p_bcd = 0.0;

		auto i_bcd = cache.find(lookup+w);
		if(i_bcd == cache.end())
		{
			std::pair <double,double> x_cd = prob_cd(w, originalContext, is, cache);
			std::pair <double,double> x_b_d = prob_b_d(w, originalContext, is, cache);
			double backoffProb = (x_cd.first*x_cd.second + x_b_d.first*x_b_d.second)/(x_cd.first + x_b_d.first);

			auto it = backoff.p.find(lookup.reverse());
			if (it == backoff.p.end())
			{
				p_bcd = backoffProb;
			} else
			{
				p_bcd = it->second.prob(w, backoffProb);
			}
			cache[lookup] = p_bcd;
		} else
		{
			p_bcd = i_bcd->second;
		}

		double w_bcd = is->get(lookup);
		L_A << " BCD: " << p_bcd << "\n";
		return std::pair <double,double> (w_bcd, p_bcd);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_a_cd(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& cache) const
	{
		Pattern lookup = originalContext.addskip(std::pair<int, int>(1,1));

		double p_a_cd = 0.0;

		auto i_a_cd = cache.find(lookup+w);
		if(i_a_cd == cache.end())
		{
			std::pair <double,double> x_cd = prob_cd(w, originalContext, is, cache);
			std::pair <double,double> x_a__d = prob_a__d(w, originalContext, is, cache);
			double backoffProb = (x_cd.first*x_cd.second + x_a__d.first*x_a__d.second)/(x_cd.first + x_a__d.first);

			auto it = p.find(lookup.reverse());
			if (it == p.end())
			{
				p_a_cd = backoffProb;
			} else
			{
				p_a_cd = it->second.prob(w, backoffProb);
			}
			cache[lookup] = p_a_cd;
		} else
		{
			p_a_cd = i_a_cd->second;
		}

		double w_a_cd = is->get(lookup);
		L_A << "A CD: " << p_a_cd << "\n";
		return std::pair <double,double> (w_a_cd, p_a_cd);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_ab_d(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& cache) const
	{
		Pattern lookup = originalContext.addskip(std::pair<int, int>(2,1));

		double p_ab_d = 0.0;

		auto i_ab_d = cache.find(lookup +w);
		if(i_ab_d == cache.end())
		{
			std::pair <double,double> x_b_d = prob_b_d(w, originalContext, is, cache);
			std::pair <double,double> x_a__d = prob_a__d(w, originalContext, is, cache);
			double backoffProb = (x_b_d.first*x_b_d.second + x_a__d.first*x_a__d.second)/(x_b_d.first + x_a__d.first);

			auto it = p.find(lookup.reverse());
			if (it == p.end())
			{
				p_ab_d = backoffProb;
			} else
			{
				p_ab_d = it->second.prob(w, backoffProb);
			}
			cache[lookup] = p_ab_d;
		} else
		{
			p_ab_d = i_ab_d->second;
		}

		double w_ab_d = is->get(lookup);
		L_A << "AB D: " << p_ab_d << "\n";
		return std::pair <double,double> (w_ab_d, p_ab_d);
	}

	template<unsigned N>
	std::pair <double,double>  cpyp::PYPLM<N>::prob_abcd(const Pattern& w, const Pattern& context, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& cache) const
	{
		double p_abcd;

		auto i_abcd = cache.find(context+w);
		if(i_abcd == cache.end())
		{
			double backoffProb = 0.0;

			auto it = p.find(context.reverse());
			//if(it == p.end() || it->second.num_customers(w) == 0)
			//{
				std::pair <double,double> x_ab_d = prob_ab_d(w, context, is, cache);
				std::pair <double,double> x_a_cd = prob_a_cd(w, context, is, cache);
				std::pair <double,double> x_bcd = prob_bcd(w, context, is, cache);
				backoffProb = (x_ab_d.first*x_ab_d.second + x_a_cd.first*x_a_cd.second + x_bcd.first*x_bcd.second)/(x_ab_d.first + x_a_cd.first + x_bcd.first);
			//} else
			//{
			//	backoffProb = 0.5;//1.0;        // misschien zit hier iets niet goed
			//}

			if (it == p.end())
			{
				p_abcd = backoffProb;
			} else
			{
				p_abcd = it->second.prob(w, backoffProb);
			}
			cache[context] = p_abcd;
		} else
		{
			p_abcd = i_abcd->second;
		}

		L_A << "ABCD: " << p_abcd << "\n";

		return std::pair <double,double> (1.0, p_abcd);
	}

	template<unsigned N>
	double cpyp::PYPLM<N>::probS4(const Pattern& w, const Pattern& context, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& cache, bool ignoreCache) const
	{

		double p_abcd = prob_abcd(w, context, is, cache).second;
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
	double cpyp::PYPLM<N>::getNormalisationFactor(const Pattern& context, const Pattern& originalContext, crp<Pattern> restaurant, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& probCache, std::unordered_map<Pattern, double>& normalisationCache, unsigned vocabSize)
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
					ps += prob_cd(dish->first, originalContext, is, probCache).second;
				}

				if(contextSize == 2 )
				{
					if(context.isgap(1))
					{
						L_V << "LanguageModel: getNormalisationFactor: 2: \n";
						ps += prob_b_d(dish->first, originalContext, is, probCache).second;
					} else
					{
						L_V << "LanguageModel: getNormalisationFactor: 3: \n";
						ps += prob_bcd(dish->first, originalContext, is, probCache).second;
					}
				}

				if(contextSize == 3 )
				{
					if(context.isgap(1) && !context.isgap(2))
					{
						L_V << "LanguageModel: getNormalisationFactor: 4: \n";
						ps += prob_a_cd(dish->first, originalContext, is, probCache).second;
					} else if(!context.isgap(1) && context.isgap(2))
					{
						L_V << "LanguageModel: getNormalisationFactor: 5: \n";
						ps += prob_ab_d(dish->first, originalContext, is, probCache).second;
					} else if(context.isgap(1) && context.isgap(2))
					{
						L_V << "LanguageModel: getNormalisationFactor: 6: \n";
						ps += prob_a__d(dish->first, originalContext, is, probCache).second;
					} else
					{
						double k = prob_abcd(dish->first, originalContext, is, probCache).second;
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
	double cpyp::PYPLM<N>::probLS4(const Pattern& w, const Pattern& context, SLM::InterpolationStrategy* is, std::unordered_map<Pattern, double>& probCache, std::unordered_map<Pattern, double>& normalisationCache, unsigned vocabSize, bool ignoreCache)
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

			auto i_cd = probCache.find(lookup+w);
			if(i_cd == probCache.end())
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
						p_cd = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, is, probCache, normalisationCache, vocabSize));
					}
					else // we have not seen uw, but seen u -> backoff
					{
						L_S << "LanguageModel: probLS4: c* in model --> backoff \n";
						p_cd = it->second.prob(w, p_d);
					}
				}

		//		if(!ignoreCache) probCache.emplace(lookup+w, p_cd);
			} else
			{
				p_cd = i_cd->second;
			}

			w_cd = is->get(lookup);
		}

		double w_b_d;
		double p_b_d;
		{
			Pattern lookup = Pattern(context, 1, 2).addskip(std::pair<int, int>(1,1));

			auto i_b_d = probCache.find(lookup);
			if(i_b_d == probCache.end())
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
						p_b_d = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, is, probCache, normalisationCache, vocabSize));
					}
					else // we have not seen uw, but seen u -> backoff
					{
						L_S << "LanguageModel: probLS4: b_* in model --> backoff \n";
						p_b_d = it->second.prob(w, p_d);
					}
				}
		//		if(!ignoreCache) probCache[lookup] = p_b_d;
			} else
			{
				p_b_d = i_b_d->second;
			}

			w_b_d = is->get(lookup);
		}

		double w_a__d;
		double p_a__d;
		{
			Pattern lookup = context.addskip(std::pair<int, int>(1,2));

			auto i_a__d = probCache.find(lookup);
			if(i_a__d == probCache.end())
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
						p_a__d = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, is, probCache, normalisationCache, vocabSize));
					}
					else // we have not seen uw, but seen u -> backoff
					{
						L_S << "LanguageModel: probLS4: a__* in model --> backoff \n";
						p_a__d = it->second.prob(w, p_d);
					}
				}
		//		if(!ignoreCache) probCache[lookup] = p_a__d;
			} else
			{
				p_a__d = i_a__d->second;
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

			auto i_bcd = probCache.find(lookup);
			if(i_bcd == probCache.end())
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
						p_bcd = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, is, probCache, normalisationCache, vocabSize));
					}
					else // we have not seen uw, but seen u -> backoff
					{
						L_S << "LanguageModel: probLS4: bc* in model --> backoff \n";
						p_bcd = it->second.prob(w, backoffProb);
					}
				}
		//		if(!ignoreCache) probCache[lookup] = p_bcd;
			} else
			{
				p_bcd = i_bcd->second;
			}

			w_bcd = is->get(lookup);
		}

		double w_a_cd;
		double p_a_cd;
		{
			Pattern lookup = context.addskip(std::pair<int, int>(1,1));

			auto i_a_cd = probCache.find(lookup);
			if(i_a_cd == probCache.end())
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
						p_a_cd = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, is, probCache, normalisationCache, vocabSize));
					}
					else // we have not seen uw, but seen u -> backoff
					{
						L_S << "LanguageModel: probLS4: a_c* in model --> backoff \n";
						p_a_cd = it->second.prob(w, backoffProb);
					}
				}
		//		if(!ignoreCache) probCache[lookup] = p_a_cd;
			} else
			{
				p_a_cd = i_a_cd->second;
			}

			w_a_cd = is->get(lookup);
		}

		double w_ab_d;
		double p_ab_d;
		{
			Pattern lookup = context.addskip(std::pair<int, int>(2,1));

			auto i_ab_d = probCache.find(lookup);
			if(i_ab_d == probCache.end())
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
						p_ab_d = it->second.prob(w, getNormalisationFactor(lookup, context, it->second, is, probCache, normalisationCache, vocabSize));
					}
					else // we have not seen uw, but seen u -> backoff
					{
						L_S << "LanguageModel: probLS4: a_* in model --> backoff \n";
						p_ab_d = it->second.prob(w, backoffProb);
					}
				}
		//		if(!ignoreCache) probCache[lookup] = p_ab_d;
			} else
			{
				p_ab_d = i_ab_d->second;
			}

			w_ab_d = is->get(lookup);
		}
//
//		//////
//
		double p_abcd;
		auto i_abcd =  ignoreCache ? probCache.end() : probCache.find(context+w);
		if(i_abcd == probCache.end())
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
					p_abcd = it->second.prob(w, getNormalisationFactor(context, context, it->second, is, probCache, normalisationCache, vocabSize));
				}
				else // we have not seen uw, but seen u -> backoff
				{
					L_S << "LanguageModel: probLS4: abc* in model --> backoff \n";
					p_abcd = it->second.prob(w, backoffProb);
				}
			}
		//	if(!ignoreCache) probCache[context] = p_abcd;
		} else
		{
			p_abcd = i_abcd->second;
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

Pattern LanguageModel::toPattern(const std::string& patternString)
{
	return classEncoder.buildpattern(patternString);
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

double LanguageModel::getProbS4(const Pattern& focus, const Pattern& context, SLM::InterpolationStrategy* interpolationStrategy, std::unordered_map<Pattern, double>& cache, bool ignoreCache)
{
	return lm.probS4(focus, context, interpolationStrategy, cache, ignoreCache);
}

double LanguageModel::getProbLS4(const Pattern& focus, const Pattern& context, SLM::InterpolationStrategy* interpolationStrategy, std::unordered_map<Pattern, double>& cache, std::unordered_map<Pattern, double>& normalisationCache, bool ignoreCache)
{
	return lm.probLS4(focus, context, interpolationStrategy, cache, normalisationCache, getVocabularySize(), ignoreCache);
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
