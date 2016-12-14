#ifndef HPYPLM_H_
#define HPYPLM_H_

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

#include <classdecoder.h>

#include <pattern.h>

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








//
//	double limitedNaiveOccurs(const Pattern& pattern, PatternCounts* _patternCounts) const
//	{
//		return _patternCounts->get(pattern) > 0 ? true : false;
//	}
//
//	double probLimitedNaiveHelper(const Pattern& w, const Pattern& context, const Pattern& pattern, double p0, double S, ContextCounts* contextCounts, LimitedInformation* limitedInformation, CoCoInitialiser * const cci = nullptr) const
//	{
//		if(pattern.size() != N)
//		{
//			if(cci) std::cout << "My level is " << N << " but the pattern has length " << pattern.size() << std::endl;
//			return backoff.probFullNaiveHelper(w, context, pattern, p0, cci);
//		}
//
//		if(cci) std::cout << "Doing something with co" << N << "gram " << pattern.tostring(cci->classDecoder) << std::endl;
//		auto it = p.find(context.reverse());
//		if(it == p.end())
//		{
//			return p0;
//		} else
//		{
//			return it->second.probNaive(context, w, p0, S);
//
//		}
//
//	}
//
//
//
//		double probLimitedNaive(const Pattern& w, const Pattern& context,cpyp::PYPLM<kORDER>& _lm, PatternCounts* _patternCounts,
//						ContextCounts* contextCounts, ContextValues* contextValues, LimitedCountsCache * limitedCounts,
//						CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
//		{
//
//			PLNCache plnCache(w, context, _patternCounts, contextCounts, contextValues, limitedCounts, cci);
//
//			std::cout << "PLN!!!!!!!!!!!!!!!! " << _patternCounts->get(context) << std::endl;
//
//
//			Debug::getInstance() << DebugLevel::SUBPATTERN << "\\/=============\n\n";
//
//			plnCache.xxxx->compute(p);
//			plnCache.xxxd->compute(backoff.backoff.backoff.p);
//			plnCache.xxcd->compute(backoff.backoff.p);
//			plnCache.xbxd->compute(backoff.p);
//			plnCache.axxd->compute(p);
//			plnCache.xbcd->compute(backoff.p);
//			plnCache.axcd->compute(p);
//			plnCache.abxd->compute(p);
//
//			Debug::getInstance() << DebugLevel::SUBPATTERN << "  =============\n\n";
////			return plnCache.abcd->compute(p);
//			double rv = plnCache.abcd->compute(p);
//			Debug::getInstance() << DebugLevel::SUBPATTERN << "/\\=============\n\n";
//			return rv;
//
//
////			return plnCache.abcd->compute();
//
//		}
//
//
//	double probFullNaiveHelper(const Pattern& w, const Pattern& context, const Pattern& pattern, double p0, CoCoInitialiser * const cci = nullptr) const
//	{
//		if(pattern.size() != N)
//		{
//			if(cci) std::cout << "My level is " << N << " but the pattern has length " << pattern.size() << std::endl;
//			return backoff.probFullNaiveHelper(w, context, pattern, p0, cci);
//		}
//
//		if(N == 1)
//		{
//			if(cci) std::cout << "Doing something with unigram" << pattern.tostring(cci->classDecoder) << std::endl;
//			auto it = p.find(Pattern());
//			if (it == p.end()) { // if the pattern is not in the train data
//				if(cci) std::cout << "It's not in the training data, but whatever" << std::endl;;
//				return p0;
//			}
//
//			return it->second.prob(w, p0);
//		}
//
//
//		if(cci) std::cout << "Doing something with co" << N << "gram " << pattern.tostring(cci->classDecoder) << std::endl;
//		auto it = p.find(context);
//		if (it == p.end()) { // if the pattern is not in the train data
//			return p0;
//		}
//
//		return it->second.prob(w, p0);
//
//
//	}
//
//	double probFullNaive(const Pattern& w, const Pattern& context,
//					ContextCounts* contextCounts, ContextValues* contextValues,
//					CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
//	{
//		//remove std::cout << "###:" << contextValues->name() << std::endl;
//
//		Debug::getInstance() << DebugLevel::ALL << "Testing for contextValues... " << std::endl;
//		if(contextValues)
//		{
//			Debug::getInstance() << DebugLevel::ALL << "\tit's not null..." << std::endl;
//			Debug::getInstance() << DebugLevel::ALL << "Testing for access to the object" << std::endl;
//			Debug::getInstance() << DebugLevel::ALL << contextValues->name() << std::endl;
//		}
//		else
//		{
//			Debug::getInstance() << DebugLevel::ALL << "\tand it's null... damn" << std::endl;
//		}
//
//		Pattern pattern = context + w;
//		if(pattern.size() != 4)
//		{
//			std::cerr << "Do something: Pattern length is not 4" << std::endl;
//		}
//
//		//std::cout << "### Using " << contextValues->name() << std::endl;
//
//		// 4 content words
//		// a b c d
//		Pattern abcd = pattern;
//
//		// 3 content words
//		// a b   d
//		Pattern abxd = abcd.addskip(std::pair<int, int>(1,1));
//		// a   c d
//		Pattern axcd = abcd.addskip(std::pair<int, int>(2,1));
//		//   b c d
//		Pattern xbcd = Pattern(abcd,1,3);
//
//		// 2 content words
//		// a     d
//		Pattern axxd = abcd.addskip(std::pair<int, int>(1,2));
//		//   b   d
//		Pattern xbxd = xbcd.addskip(std::pair<int, int>(1,1));
//		//     c d
//		Pattern xxcd = Pattern(xbcd,1,2);
//
//		// 1 content word
//		//       d
//		Pattern xxxd = Pattern(xxcd,1,1);
//
//		// 0 content words
//		//
//		Pattern xxxx = Pattern();
//
//		CoCoInitialiser* temp_cciPtr = nullptr;
//
//		// -----------------------------
//		// 0
//		double xxxx_prob = probFullNaiveHelper(Pattern(), Pattern(), xxxx, 0, temp_cciPtr);
//		Debug::getInstance() << DebugLevel::ALL << "xxxx p: " << xxxx_prob << std::endl;
//
//		// 1
//		double xxxd_prob = probFullNaiveHelper(xxxd, Pattern(), xxxd, xxxx_prob, temp_cciPtr);
//		Debug::getInstance() << DebugLevel::ALL << "xxxd p: " << xxxd_prob << std::endl;
//
//		// 2
//		double axxd_prob = probFullNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, xxxd_prob, temp_cciPtr);
//		double axxd_weight = contextValues->get(axxd);
//		Debug::getInstance() << DebugLevel::ALL << "axxd p: " << axxd_prob << " with weight: " << axxd_weight << std::endl;
//		double xbxd_prob = probFullNaiveHelper(xxxd, Pattern(xbxd, 0, 2), xbxd, xxxd_prob, temp_cciPtr);
//		double xbxd_weight = contextValues->get(xbxd);
//		Debug::getInstance() << DebugLevel::ALL << "xbxd p: " << xbxd_prob << " with weight: " << xbxd_weight << std::endl;
//		double xxcd_prob = probFullNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxxd_prob, temp_cciPtr);
//		double xxcd_weight = contextValues->get(xxcd);
//		Debug::getInstance() << DebugLevel::ALL << "xxcd p: " << xxcd_prob << " with weight: " << xxcd_weight << std::endl;
//
//		double c2_prob = (axxd_prob * axxd_weight + xbxd_prob * xbxd_weight + xxcd_prob * xxcd_weight) / (axxd_weight + xbxd_weight + xxcd_weight);
//
//		// 3
//		double abxd_prob = probFullNaiveHelper(xxxd, Pattern(abxd, 0, 3), abxd, c2_prob, temp_cciPtr);
//		double abxd_weight = contextValues->get(abxd);
//		Debug::getInstance() << DebugLevel::ALL << "abxd p: " << abxd_prob << " with weight: " << abxd_weight << std::endl;
//		double axcd_prob = probFullNaiveHelper(xxxd, Pattern(axcd, 0, 3), axcd, c2_prob, temp_cciPtr);
//		double axcd_weight = contextValues->get(axcd);
//		Debug::getInstance() << DebugLevel::ALL << "axcd p: " << axcd_prob << " with weight: " << axcd_weight << std::endl;
//		double xbcd_prob = probFullNaiveHelper(xxxd, Pattern(xbcd, 0, 2), xbcd, c2_prob, temp_cciPtr);
//		double xbcd_weight = contextValues->get(xbcd);
//		Debug::getInstance() << DebugLevel::ALL << "xbcd p: " << xbcd_prob << " with weight: " << xbcd_weight << std::endl;
//
//		double c3_prob = (abxd_prob * abxd_weight + axcd_prob * axcd_weight + xbcd_prob * xbcd_weight) / (abxd_weight + axcd_weight + xbcd_weight);
//
//		// 4
//		double abcd_prob = probFullNaiveHelper(xxxd, Pattern(abcd, 0, 3), abcd, c3_prob, temp_cciPtr);
//		double abcd_weight = 1.0;
//		Debug::getInstance() << DebugLevel::ALL << "abcd p: " << abcd_prob << " with weight: " << abcd_weight << std::endl;
//
////		return 0.5;
//		return abcd_prob;
//
//	}
//
//	double probFullNaive1(const Pattern& w, const Pattern& context,
//							ContextCounts* contextCounts, ContextValues* contextValues,
//							CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
//			{
//		Debug::getInstance() << DebugLevel::ALL << "PFN1" << std::endl;
//
//				Pattern pattern = context + w;
//				if(pattern.size() != 1)
//				{
//					std::cerr << "[ProbFullNaive1]\tDo something: Pattern length is not 1" << std::endl;
//				}
//
//				// 1 word
//				Pattern xxxd = pattern;
//				// 0 words
//				Pattern xxxx = Pattern();
//
//				CoCoInitialiser* temp_cciPtr = nullptr;
//
//				// -----------------------------
//				// 0
//				double xxxx_prob = probFullNaiveHelper(Pattern(), Pattern(), xxxx, 0, temp_cciPtr);
//				Debug::getInstance() << DebugLevel::ALL << "xxxx p: " << xxxx_prob << std::endl;
//
//				// 1
//				double xxxd_prob = probFullNaiveHelper(xxxd, Pattern(), xxxd, xxxx_prob, temp_cciPtr);
//				Debug::getInstance() << DebugLevel::ALL << "xxxd p: " << xxxd_prob << std::endl;
//
//				return xxxd_prob;
//			}
//
//	double probFullNaive2(const Pattern& w, const Pattern& context,
//						ContextCounts* contextCounts, ContextValues* contextValues,
//						CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
//		{
//
//		Debug::getInstance() << DebugLevel::ALL << "PFN2" << std::endl;
//
//			Pattern pattern = context + w;
//			if(pattern.size() != 2)
//			{
//				std::cerr << "[ProbFullNaive2]\tDo something: Pattern length is not 2" << std::endl;
//			}
//
//			// 2 words
//			Pattern xxcd = pattern;
//			// 1 word
//			Pattern xxxd = Pattern(xxcd,1,1);
//			// 0 words
//			Pattern xxxx = Pattern();
//
//			CoCoInitialiser* temp_cciPtr = nullptr;
//
//			// -----------------------------
//			// 0
//			double xxxx_prob = probFullNaiveHelper(Pattern(), Pattern(), xxxx, 0, temp_cciPtr);
//			Debug::getInstance() << DebugLevel::ALL << "xxxx p: " << xxxx_prob << std::endl;
//
//			// 1
//			double xxxd_prob = probFullNaiveHelper(xxxd, Pattern(), xxxd, xxxx_prob, temp_cciPtr);
//			Debug::getInstance() << DebugLevel::ALL << "xxxd p: " << xxxd_prob << std::endl;
//
//			// 2
//			double xxcd_prob = probFullNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxxd_prob, temp_cciPtr);
//			double xxcd_weight = contextValues->get(xxcd);
//			Debug::getInstance() << DebugLevel::ALL << "xxcd p: " << xxcd_prob << " with weight: " << xxcd_weight << std::endl;
//
//			double c2_prob = xxcd_prob;// * xxcd_weight / xxcd_weight;
//			return c2_prob;
//		}
//
//	double probFullNaive3(const Pattern& w, const Pattern& context,
//						ContextCounts* contextCounts, ContextValues* contextValues,
//						CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
//		{
//		Debug::getInstance() << DebugLevel::ALL << "PFN3" << std::endl;
//
//			Pattern pattern = context + w;
//			if(pattern.size() != 3)
//			{
//				std::cerr << "[ProbFullNaive3]\tDo something: Pattern length is not 3" << std::endl;
//			}
//
//
//			// 3 words
//			Pattern xbcd = pattern;
//			// 2 words
//			Pattern xbxd = xbcd.addskip(std::pair<int, int>(1,1));
//			Pattern xxcd = Pattern(xbcd,1,2);
//			// 1 word
//			Pattern xxxd = Pattern(xxcd,1,1);
//			// 0 words
//			Pattern xxxx = Pattern();
//
//			CoCoInitialiser* temp_cciPtr = nullptr;
//
//			// -----------------------------
//			// 0
//			double xxxx_prob = probFullNaiveHelper(Pattern(), Pattern(), xxxx, 0, temp_cciPtr);
//			Debug::getInstance() << DebugLevel::ALL << "xxxx p: " << xxxx_prob << std::endl;
//
//			// 1
//			double xxxd_prob = probFullNaiveHelper(xxxd, Pattern(), xxxd, xxxx_prob, temp_cciPtr);
//			Debug::getInstance() << DebugLevel::ALL << "xxxd p: " << xxxd_prob << std::endl;
//
//			// 2
//			double xbxd_prob = probFullNaiveHelper(xxxd, Pattern(xbxd, 0, 2), xbxd, xxxd_prob, temp_cciPtr);
//			double xbxd_weight = contextValues->get(xbxd);
//			Debug::getInstance() << DebugLevel::ALL << "xbxd p: " << xbxd_prob << " with weight: " << xbxd_weight << std::endl;
//			double xxcd_prob = probFullNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxxd_prob, temp_cciPtr);
//			double xxcd_weight = contextValues->get(xxcd);
//			Debug::getInstance() << DebugLevel::ALL << "xxcd p: " << xxcd_prob << " with weight: " << xxcd_weight << std::endl;
//
//			double c2_prob = (xbxd_prob * xbxd_weight + xxcd_prob * xxcd_weight) / (xbxd_weight + xxcd_weight);
//
//			// 3
//			double xbcd_prob = probFullNaiveHelper(xxxd, Pattern(xbcd, 0, 2), xbcd, c2_prob, temp_cciPtr);
//			double xbcd_weight = contextValues->get(xbcd);
//			Debug::getInstance() << DebugLevel::ALL << "xbcd p: " << xbcd_prob << " with weight: " << xbcd_weight << std::endl;
//
//			double c3_prob = xbcd_prob;// * xbcd_weight / xbcd_weight;
//			return c3_prob;
//
//		}




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

		// backoff 3
		// backoff.backoff 2
		// backoff.backoff.backoff 1
		// backoff.backoff.backoff.backoff 0

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

		/*
	    //
	    {0} UniformVocabulary -> p0 = 1.0/vocabularySize

	    //     d
	    {1} prob(d) = cpyp::prob(d, {0})

	    //   c d
	    {2.1.1}  c prob(cd) = cpyp::prob(d, {1})
	    {2.1.2} !c prob(cd) = {1}

	    //  b* d
	    {2.2.1}  b* prob(b*d) = cpyp::prob(d, {1})
	    {2.2.2} !b* prob(b*d) = {1}

	    // a** d
	    {2.3.1}  a** prob(a**d) = cpyp::prob(d, {1})
	    {2.3.2} !a** prob(a**d) = {1}


	    //  bc d
	    {3.1.1}  bc prob(bcd) = cpyp::prob(d, ({2.1.-}+{2.2.-})/2)
	    {3.1.2} !bc prob(bcd) = ({2.1.-}+{2.2.-})/2

	    // a*c d
	    {3.2.1}  a*c prob(a*cd) = cpyp::prob(d, ({2.1.-}+{2.3.-})/2)
	    {3.2.2} !a*c prob(a*cd) = ({2.1.-}+{2.3.-})/2

	    // ab* d
	    {3.3.1}  ab* prob(ab*d) = cpyp::prob(d, ({2.2.-}+{2.3.-})/2)
	    {3.3.2} !ab* prob(ab*d) = ({2.2.-}+{2.3.-})/2


	    // abc d
	    {4.1}  abc prob(abcd) = cpyp::prob(d, ({3.1.-}+{3.2.-}+{3.3.-})/3)
	    {4.2} !abc prob(abcd) = ({3.1.-}+{3.2.-}+{3.3.-})/3

	    return {4.-}
	    */
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

}

#endif

