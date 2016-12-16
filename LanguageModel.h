/*
 * LanguageModel.h
 *
 *  Created on: Dec 10, 2016
 *      Author: louis
 */

#ifndef LANGUAGEMODEL_H_
#define LANGUAGEMODEL_H_

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

//#include "hpyplm.h"

#include "ProgramOptions.h"




#include <vector>
#include <unordered_map>
#include <map>
#include <set>

#include "cpyp/m.h"
#include "cpyp/random.h"
#include "cpyp/crp.h"
#include "cpyp/tied_parameter_resampler.h"

#include "uvector.h"
#include "uniform_vocab.h"

//#include <classdecoder.h>

#include <pattern.h>

#include "Logging.h"
#include "InterpolationStrategy.h"


namespace SLM {

class InterpolationStrategy;
class LanguageModel;



} /* namespace SLM */



//#include "LanguageModel.h"

// A not very memory-efficient implementation of an N-gram LM based on PYPs
// as described in Y.-W. Teh. (2006) A Hierarchical Bayesian Language Model
// based on Pitman-Yor Processes. In Proc. ACL.

namespace cpyp {

template<unsigned N> struct PYPLM;

template<> struct PYPLM<0> : public UniformVocabulary {
	PYPLM(unsigned vs, double a, double b, double c, double d) :
			UniformVocabulary(vs, a, b, c, d) {
	}
};

// represents an N-gram LM
template<unsigned N> struct PYPLM {
	PYPLM() :
			backoff(0, 1, 1, 1, 1), tr(1, 1, 1, 1, 0.8, 0.0) {
	}
	explicit PYPLM(unsigned vs, double da = 1.0, double db = 1.0, double ss = 1.0, double sr = 1.0) :
			backoff(vs, da, db, ss, sr), tr(da, db, ss, sr, 0.8, 0.0) {
	}
	template<typename Engine>
	void increment(const Pattern& w, const Pattern& context, Engine& eng) {
		const double bo = backoff.prob(w, context);

                Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);

		auto it = p.find(lookup);
		if (it == p.end()) {
			it = p.insert(make_pair(lookup, crp<Pattern>(0.8, 0))).first;
			tr.insert(&it->second); // add to resampler
		}

		if (it->second.increment(w, bo, eng)) {
			backoff.increment(w, context, eng);
		}
	}

	template<typename Engine>
	void decrement(const Pattern& w, const Pattern& context, Engine& eng) {
                Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);

		auto it = p.find(lookup);
		assert(it != p.end());

		if (it->second.decrement(w, eng)) {
			backoff.decrement(w, context, eng);
		}
	}

	// full backoff
	double prob(const Pattern& w, const Pattern& context) const
	{
			const double bo = backoff.prob(w, context);
	        return prob(w, context, bo);
	}


	double prob(const Pattern& w, const Pattern& context, double boProb) const
	{
		Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);

		auto it = p.find(lookup);
		if (it == p.end()) { // if the pattern is not in the train data
			return boProb;
		}
		return it->second.prob(w, boProb);
	}



	double prob4(const Pattern& w, const Pattern& context) const
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

	double probS4(const Pattern& w, const Pattern& context, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const
	{
		double p__ = backoff.backoff.backoff.backoff.p0; // -
		L_S << "HPYPLM: probS4: fresh      " << p__ << "\n";

		double p_d = backoff.backoff.backoff.prob(w, p__); // d
		L_S << "HPYPLM: probS4: fresh    d " << p_d << "\n";

		double w_cd;
		double p_cd;
		{
			Pattern lookup = Pattern(context, 2, 1);

			std::map<Pattern,double>::const_iterator i_cd = cache.find(lookup+w);
			if(i_cd == cache.end())
			{
				auto it = backoff.backoff.p.find(lookup.reverse());
				if (it == backoff.backoff.p.end())
				{
					p_cd = p_d;
				} else
				{
					p_cd = it->second.prob(w, p_d);
				}
				L_S << "HPYPLM: probS4: fresh   cd " << p_cd << "\n";
				cache.emplace(lookup+w, p_cd);
			} else
			{
				p_cd = i_cd->second;
				L_S << "HPYPLM: probS4: cache   cd " << p_cd << "\n";
			}

			w_cd = is->get(lookup);
		}

		double w_b_d;
		double p_b_d;
		{
			Pattern lookup = Pattern(context, 1, 2).addskip(std::pair<int, int>(1,1));

			auto i_b_d = cache.find(lookup);
			if(i_b_d == cache.end())
			{
				auto it = backoff.backoff.p.find(lookup.reverse());
				if (it == backoff.backoff.p.end())
				{
					p_b_d = p_d;
				} else
				{
					p_b_d = it->second.prob(w, p_d);
				}
				cache[lookup] = p_b_d;
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

			auto i_a__d = cache.find(lookup);
			if(i_a__d == cache.end())
			{
				auto it = backoff.backoff.p.find(lookup.reverse());
				if (it == backoff.backoff.p.end())
				{
					p_a__d = p_d;
				} else
				{
					p_a__d = it->second.prob(w, p_d);
				}
				cache[lookup] = p_a__d;
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

			auto i_bcd = cache.find(lookup);
			if(i_bcd == cache.end())
			{
				double backoffProb = (w_cd*p_cd + w_b_d*p_b_d)/(w_cd + w_b_d);

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

			w_bcd = is->get(lookup);
		}

		double w_a_cd;
		double p_a_cd;
		{
			Pattern lookup = context.addskip(std::pair<int, int>(1,1));

			auto i_a_cd = cache.find(lookup);
			if(i_a_cd == cache.end())
			{
				double backoffProb = (w_cd*p_cd + w_a__d*p_a__d)/(w_cd + w_a__d);

				auto it = backoff.p.find(lookup.reverse());
				if (it == backoff.p.end())
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

			w_a_cd = is->get(lookup);
		}

		double w_ab_d;
		double p_ab_d;
		{
			Pattern lookup = context.addskip(std::pair<int, int>(2,1));

			auto i_ab_d = cache.find(lookup);
			if(i_ab_d == cache.end())
			{
				double backoffProb = (w_b_d*p_b_d + w_a__d*p_a__d)/(w_b_d + w_a__d);

				auto it = backoff.p.find(lookup.reverse());
				if (it == backoff.p.end())
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

			w_ab_d = is->get(lookup);
		}
//
//		//////
//
		double p_abcd;
		auto i_abcd = cache.find(context);
		if(i_abcd == cache.end())
		{
			double backoffProb = (w_bcd*p_bcd + w_a_cd*p_a_cd + w_ab_d*p_ab_d)/(w_bcd + w_a_cd + w_ab_d);

			auto it = p.find(context.reverse());
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

		return p_abcd;
	}

	double probLS4(const Pattern& w, const Pattern& context, const SLM::LanguageModel& lm, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const
	{
		/*
		double p__ = backoff.backoff.backoff.backoff.p0; // -
		L_S << "HPYPLM: probS4: fresh      " << p__ << "\n";

		double p_d = backoff.backoff.backoff.prob(w, p__); // d
		L_S << "HPYPLM: probS4: fresh    d " << p_d << "\n";

		double w_cd;
		double p_cd;
		{
			Pattern lookup = Pattern(context, 2, 1);

			std::map<Pattern,double>::const_iterator i_cd = cache.find(lookup+w);
			if(i_cd == cache.end())
			{
				auto it = backoff.backoff.p.find(lookup.reverse());
				if (it == backoff.backoff.p.end())
				{
					p_cd = p_d;
				} else
				{
					p_cd = it->second.prob(w, p_d);
				}
				L_S << "HPYPLM: probS4: fresh   cd " << p_cd << "\n";
				cache.emplace(lookup+w, p_cd);
			} else
			{
				p_cd = i_cd->second;
				L_S << "HPYPLM: probS4: cache   cd " << p_cd << "\n";
			}

			w_cd = is->get(lookup);
		}

		double w_b_d;
		double p_b_d;
		{
			Pattern lookup = Pattern(context, 1, 2).addskip(std::pair<int, int>(1,1));

			auto i_b_d = cache.find(lookup);
			if(i_b_d == cache.end())
			{
				auto it = backoff.backoff.p.find(lookup.reverse());
				if (it == backoff.backoff.p.end())
				{
					p_b_d = p_d;
				} else
				{
					p_b_d = it->second.prob(w, p_d);
				}
				cache[lookup] = p_b_d;
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

			auto i_a__d = cache.find(lookup);
			if(i_a__d == cache.end())
			{
				auto it = backoff.backoff.p.find(lookup.reverse());
				if (it == backoff.backoff.p.end())
				{
					p_a__d = p_d;
				} else
				{
					p_a__d = it->second.prob(w, p_d);
				}
				cache[lookup] = p_a__d;
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

			auto i_bcd = cache.find(lookup);
			if(i_bcd == cache.end())
			{
				double backoffProb = (w_cd*p_cd + w_b_d*p_b_d)/(w_cd + w_b_d);

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

			w_bcd = is->get(lookup);
		}

		double w_a_cd;
		double p_a_cd;
		{
			Pattern lookup = context.addskip(std::pair<int, int>(1,1));

			auto i_a_cd = cache.find(lookup);
			if(i_a_cd == cache.end())
			{
				double backoffProb = (w_cd*p_cd + w_a__d*p_a__d)/(w_cd + w_a__d);

				auto it = backoff.p.find(lookup.reverse());
				if (it == backoff.p.end())
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

			w_a_cd = is->get(lookup);
		}

		double w_ab_d;
		double p_ab_d;
		{
			Pattern lookup = context.addskip(std::pair<int, int>(2,1));

			auto i_ab_d = cache.find(lookup);
			if(i_ab_d == cache.end())
			{
				double backoffProb = (w_b_d*p_b_d + w_a__d*p_a__d)/(w_b_d + w_a__d);

				auto it = backoff.p.find(lookup.reverse());
				if (it == backoff.p.end())
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

			w_ab_d = is->get(lookup);
		}
//
//		//////
//
		*/
//		double p_abcd;
//		auto i_abcd = cache.find(context);
//		if(i_abcd == cache.end())
//		{
//			double backoffProb;
//			if(count uw > 0 limited)
//			{
//				backoffProb = normalisationFactor;
//			} else
//			{
//				backoffProb = (w_bcd*p_bcd + w_a_cd*p_a_cd + w_ab_d*p_ab_d)/(w_bcd + w_a_cd + w_ab_d);
//			}
//
//			auto it = p.find(context.reverse());
//			if (it == p.end())
//			{
//				p_abcd = backoffProb;
//			} else
//			{
//				p_abcd = it->second.prob(w, backoffProb);
//			}
//
//			cache[context] = p_abcd;
//		} else
//		{
//			p_abcd = i_abcd->second;
//		}

//		return p_abcd;
		return 0.4;
	}


	double log_likelihood() const {
		return backoff.log_likelihood() + tr.log_likelihood();
	}

	template<typename Engine>
	void resample_hyperparameters(Engine& eng) {
		tr.resample_hyperparameters(eng);
		backoff.resample_hyperparameters(eng);
	}

	template<class Archive> void serialize(Archive& ar, const unsigned int version) {
		backoff.serialize(ar, version);
		//ar & tr;
                ar & p;
	}

	PYPLM<N - 1> backoff;
	tied_parameter_resampler<crp<Pattern>> tr;
	std::unordered_map<Pattern, crp<Pattern>> p;  // .first = context .second = CRP
};

} // namespace cpyp

namespace SLM
{
class LanguageModel {
public:
	LanguageModel(const SLM::ProgramOptions& programOptions);
	virtual ~LanguageModel();

	Pattern toPattern(const std::string& patternString);
	std::string toString(const Pattern& pattern);

	const std::vector<Pattern>& getVocabulary() const;
	bool isOOV(const Pattern& word);

	double getProb(const Pattern& focus, const Pattern& context);
	double getProb4(const Pattern& focus, const Pattern& context);
	double getProbS4(const Pattern& focus, const Pattern& context, SLM::InterpolationStrategy* interpolationStrategy, std::map<Pattern, double>& cache);
	double getProbLS4(const Pattern& focus, const Pattern& context, SLM::InterpolationStrategy* interpolationStrategy, std::map<Pattern, double>& cache);
private:
	void initialise(const ProgramOptions& programOptions);
	void defaultPatternModelOptions();
	void extendClassEncoder(const std::vector<std::string>& inputFiles, const std::string& outputFile);
	void loadLanguageModel(const std::string& inputFile);

	ClassEncoder classEncoder;
	ClassDecoder classDecoder;
	IndexedCorpus* indexedCorpus;
	PatternModel<uint32_t> patternModel;
	PatternModelOptions patternModelOptions;

	std::vector<Pattern> vocabulary;
	::cpyp::PYPLM<4> lm;
};
} // namespace SLM


#endif /* LANGUAGEMODEL_H_ */
