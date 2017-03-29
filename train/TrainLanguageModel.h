/*
 * TrainLanguageModel.h
 *
 *  Created on: Mar 28, 2017
 *      Author: louis
 */

#ifndef TRAIN_TRAINLANGUAGEMODEL_H_
#define TRAIN_TRAINLANGUAGEMODEL_H_

#include <pattern.h>
#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <unordered_set>

#include "cpyp/m.h"
#include "cpyp/random.h"
#include "cpyp/crp.h"
#include "cpyp/tied_parameter_resampler.h"
#include "uvector.h"
#include "uniform_vocab.h"

#include "TrainProgramOptions.h"

namespace SLM
{
	class TrainLanguageModel;

	class PatternContainer
	{
	public:
		PatternContainer()
		{

		}

		PatternContainer(Pattern focus, Pattern context, int sentenceNumber, int patternNumber)
			: focus(focus), context(context), sentenceNumber(sentenceNumber), patternNumber(patternNumber)
		{

		}

		int sentenceNumber;
		int patternNumber;
		Pattern focus;
		Pattern context;
	};
} /* namespace SLM */

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

	double prob(const Pattern& w, const Pattern& context) const;
	double prob(const Pattern& w, const Pattern& context, double boProb) const;

	PYPLM<N - 1> backoff;
	tied_parameter_resampler<crp<Pattern>> tr;
	std::unordered_map<Pattern, crp<Pattern>> p;  // .first = context .second = CRP
};

} // namespace cpyp

namespace SLM
{

class TrainLanguageModel {
public:
	TrainLanguageModel(SLM::TrainProgramOptions& trainProgramOptions);
	virtual ~TrainLanguageModel();

	Pattern toPattern(const std::string& patternString);
	std::string toString(const Pattern& pattern);

	const PatternSet<uint64_t>& getVocabulary() const;
	bool isOOV(const Pattern& word);

	PatternContainer* getNextPattern();

	void increment(const Pattern& w, const Pattern& context);
	void decrement(const Pattern& w, const Pattern& context);

	void resample_hyperparameters();
	double log_likelihood() const;
private:
	void initialise(TrainProgramOptions& trainProgramOptions);
	PatternModelOptions fromTrainProgramOptions(const TrainProgramOptions& trainProgramOptions);

	cpyp::MT19937 _eng;
protected:
	PatternModelOptions patternModelOptions;

	ClassEncoder classEncoder;
	ClassDecoder classDecoder;
	IndexedCorpus* indexedCorpus;
	PatternModel<uint32_t> patternModel;
	PatternSet<uint64_t> vocabulary;
	::cpyp::PYPLM<4> lm;

	PatternContainer* patternContainer = new PatternContainer();
	IndexedCorpus::iterator indexedCorpusIter;
	std::unordered_set<PatternPointer>::iterator patternPointerIter;
	std::unordered_set<PatternPointer> reverseIndex;
	uint64_t patternNumber = 0;
	uint64_t sentenceNumber = 0;
};

} /* namespace SLM */

#endif /* TRAIN_TRAINLANGUAGEMODEL_H_ */
