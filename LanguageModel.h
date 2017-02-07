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

	std::pair <double,double> prob_() const;
	std::pair <double,double> prob_d(const Pattern& w, SLM::InterpolationStrategy* is) const;
	std::pair <double,double> prob_cd(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const;
	std::pair <double,double> prob_b_d(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const;
	std::pair <double,double> prob_a__d(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const;
	std::pair <double,double> prob_bcd(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const;
	std::pair <double,double> prob_a_cd(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const;
	std::pair <double,double> prob_ab_d(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const;
	std::pair <double,double> prob_abcd(const Pattern& w, const Pattern& originalContext, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const;

	unsigned int getCount(const Pattern& w, const Pattern& context) const;
	std::vector<unsigned int> getCounts(const Pattern& context) const;

	double prob(const Pattern& w, const Pattern& context) const;

	double prob(const Pattern& w, const Pattern& context, double boProb) const;

	double prob4(const Pattern& w, const Pattern& context) const;

	double probS4(const Pattern& w, const Pattern& context, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache, bool ignoreCache) const;

	double probLS4(const Pattern& w, const Pattern& context, SLM::InterpolationStrategy* is, std::map<Pattern, double>& probCache, std::map<Pattern, double>& normalisationCache);

	double getNormalisationFactor(const Pattern& context, crp<Pattern> restaurant, SLM::InterpolationStrategy* is, std::map<Pattern, double>& probCache, std::map<Pattern, double>& normalisationCache);

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
	unsigned int count(const Pattern& pattern);

	unsigned int getCount(const Pattern& focus, const Pattern& context) const;
	std::vector<unsigned int> getCounts(const Pattern& context) const;

	double getProb(const Pattern& focus, const Pattern& context);
	double getProb4(const Pattern& focus, const Pattern& context);
	double getProbS4(const Pattern& focus, const Pattern& context, SLM::InterpolationStrategy* interpolationStrategy, std::map<Pattern, double>& cache, bool ignoreCache);
	double getProbLS4(const Pattern& focus, const Pattern& context, SLM::InterpolationStrategy* interpolationStrategy, std::map<Pattern, double>& cache, std::map<Pattern, double>& normalisationCache);
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
