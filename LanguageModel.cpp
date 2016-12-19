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
	double cpyp::PYPLM<N>::probS4(const Pattern& w, const Pattern& context, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache, bool ignoreCache) const
	{
		double p__ = backoff.backoff.backoff.backoff.p0; // -
		L_S << "HPYPLM: probS4: fresh      " << p__ << "\n";

		double p_d = backoff.backoff.backoff.prob(w, p__); // d
		L_S << "HPYPLM: probS4: fresh    d " << p_d << "\n";

		double w_cd;
		double p_cd;
		{
			Pattern lookup = Pattern(context, 2, 1);

			std::map<Pattern,double>::const_iterator i_cd = ignoreCache ? cache.end() : cache.find(lookup+w);
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

			auto i_b_d =  ignoreCache ? cache.end() : cache.find(lookup+w);
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

			auto i_a__d =  ignoreCache ? cache.end() : cache.find(lookup+w);
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

			auto i_bcd =  ignoreCache ? cache.end() : cache.find(lookup+w);
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

			auto i_a_cd = ignoreCache ? cache.end() :  cache.find(lookup+w);
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

			auto i_ab_d =  ignoreCache ? cache.end() : cache.find(lookup +w);
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
		auto i_abcd =  ignoreCache ? cache.end() : cache.find(context+w);
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

	template<unsigned N>
	double cpyp::PYPLM<N>::probLS4(const Pattern& w, const Pattern& context, const SLM::LanguageModel& lm, SLM::InterpolationStrategy* is, std::map<Pattern, double>& cache) const
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
		double p_abcd;
		auto i_abcd = cache.find(context);
		if(i_abcd == cache.end())
		{
			double backoffProb;
			if(true/*lm.count(context+w)*/)
			{
				backoffProb = 4.0;//normalisationFactor;
			} else
			{
				backoffProb = 3.0;//(w_bcd*p_bcd + w_a_cd*p_a_cd + w_ab_d*p_ab_d)/(w_bcd + w_a_cd + w_ab_d);
			}

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
}


namespace SLM
{

LanguageModel::LanguageModel(const ProgramOptions& programOptions) {
	initialise(programOptions);

}

LanguageModel::~LanguageModel() {
//	delete indexedCorpus;
}

const std::vector<Pattern>& LanguageModel::getVocabulary() const
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
	PatternSet<uint64_t> vocabularySet = patternModel.extractset(1,1);
	vocabulary = std::vector<Pattern>(vocabularySet.begin(), vocabularySet.end());
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

double LanguageModel::getProb(const Pattern& focus, const Pattern& context)
{
	return lm.prob(focus, context);
}

double LanguageModel::getProb4(const Pattern& focus, const Pattern& context)
{
	return lm.prob4(focus, context);
}

double LanguageModel::getProbS4(const Pattern& focus, const Pattern& context, SLM::InterpolationStrategy* interpolationStrategy, std::map<Pattern, double>& cache, bool ignoreCache)
{
	return lm.probS4(focus, context, interpolationStrategy, cache, ignoreCache);
}

double LanguageModel::getProbLS4(const Pattern& focus, const Pattern& context, SLM::InterpolationStrategy* interpolationStrategy, std::map<Pattern, double>& cache)
{
	return lm.probLS4(focus, context, *this, interpolationStrategy, cache);
}

bool LanguageModel::isOOV(const Pattern& word)
{
	auto it = std::find(vocabulary.begin(), vocabulary.end(), word);
	return it == vocabulary.end();
}

unsigned int LanguageModel::count(const Pattern& pattern)
{
	return patternModel.occurrencecount(pattern);
}

} /* namespace SLM */
