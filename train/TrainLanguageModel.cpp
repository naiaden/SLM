/*
 * TrainLanguageModel.cpp
 *
 *  Created on: Mar 28, 2017
 *      Author: louis
 */

#include "TrainLanguageModel.h"

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
}

namespace SLM {

TrainLanguageModel::TrainLanguageModel(SLM::TrainProgramOptions& trainProgramOptions) {
	initialise(trainProgramOptions);

}

TrainLanguageModel::~TrainLanguageModel() {
	// TODO Auto-generated destructor stub
}

const PatternSet<uint64_t>& TrainLanguageModel::getVocabulary() const
{
	return vocabulary;
}

Pattern TrainLanguageModel::toPattern(const std::string& patternString)
{
	return classEncoder.buildpattern(patternString);
}

std::string TrainLanguageModel::toString(const Pattern& pattern)
{
	return pattern.tostring(classDecoder);
}

PatternModelOptions TrainLanguageModel::fromTrainProgramOptions(const TrainProgramOptions& trainProgramOptions)
{
	patternModelOptions.MAXLENGTH = 4;
	patternModelOptions.MINLENGTH = 1;

	patternModelOptions.DOSKIPGRAMS_EXHAUSTIVE = trainProgramOptions.doSkipgrams();
	patternModelOptions.DOREVERSEINDEX = true;
	patternModelOptions.QUIET = false;

	patternModelOptions.MINTOKENS = trainProgramOptions.getNgramThreshold();
	patternModelOptions.MINTOKENS_SKIPGRAMS = trainProgramOptions.getSkipgramThreshold();
	patternModelOptions.MINTOKENS_UNIGRAMS = trainProgramOptions.getUnigramThreshold();
	patternModelOptions.PRUNENONSUBSUMED = trainProgramOptions.getPruneThreshold();

	return patternModelOptions;
}

void TrainLanguageModel::initialise(TrainProgramOptions& trainProgramOptions)
{
	L_I << "TrainLanguageModel: Intialisation\n";

	L_V << "TrainLanguageModel: Loading language model options\n";
	fromTrainProgramOptions(trainProgramOptions);

	L_V << "TrainLanguageModel: Loading vocabulary\n";
	if(trainProgramOptions.getTrainVocabularyFile().empty())
	{
		L_V << "TrainLanguageModel: Training vocabulary from\n";
		std::vector<std::string> inputFiles = trainProgramOptions.getInputFiles();
		for(auto i : inputFiles)
		{
			L_V << "     " << i << "\n";
		}
		classEncoder.build(inputFiles, true);
		std::string vocabularyOutputFile = trainProgramOptions.constructTrainVocabularyFile();
		L_V << "TrainLanguageModel: Writing vocabulary to " << vocabularyOutputFile << "\n";
		classEncoder.save(vocabularyOutputFile);

		trainProgramOptions.setTrainVocabularyFile(vocabularyOutputFile);
	} else
	{
		L_V << "TrainLanguageModel: Loading vocabulary from " << trainProgramOptions.getTrainVocabularyFile() << "\n";
		std::string vocabularyInputFile = trainProgramOptions.getTrainVocabularyFile();
		classEncoder.load(vocabularyInputFile);
	}

	L_V << "TrainLanguageModel: Loading class decoder\n";
	if(trainProgramOptions.getTrainCorpusFile().empty())
	{
		L_V << "TrainLanguageModel: Training corpus from\n";
		std::vector<std::string> inputFiles = trainProgramOptions.getInputFiles();
		for(auto i : inputFiles)
		{
			L_V << "     " << i << "\n";
		}
		int append = 0;
		std::string corpusOutputFile = trainProgramOptions.constructTrainCorpusFile();
		for(auto i : inputFiles)
		{
			classEncoder.encodefile(i, corpusOutputFile, 0, 0, append, 0);
			append = 1;
		}
		std::string vocabularyInputFile = trainProgramOptions.getTrainVocabularyFile();
		L_V << "TrainLanguageModel: Loading corpus from " << vocabularyInputFile << "\n";
		classDecoder.load(vocabularyInputFile);

		trainProgramOptions.setTrainCorpusFile(corpusOutputFile);
	} else
	{
		// nothing
	}

	L_V << "TrainLanguageModel: Loading indexed corpus\n";
	{
		std::string corpusInputFile = trainProgramOptions.getTrainCorpusFile();
		indexedCorpus = new IndexedCorpus(corpusInputFile);
	}

	L_V << "TrainLanguageModel: Training pattern model from indexed corpus\n";
	{
		patternModel = PatternModel<uint32_t>(indexedCorpus);
	}

	if(trainProgramOptions.getExtendModel().empty())
	{
		if(trainProgramOptions.getPatternModelFile().empty())
		{
			L_V << "TrainLanguageModel: Training pattern model from " << trainProgramOptions.getTrainCorpusFile() << "\n";
			patternModel.train(trainProgramOptions.getTrainCorpusFile(), patternModelOptions);
			std::string patternModelOutputFile = trainProgramOptions.constructPatternModelFile();
			L_V << "TrainLanguageModel: Writing pattern model to " << patternModelOutputFile << "\n";
			patternModel.write(patternModelOutputFile);
		} else
		{
			L_V << "TrainLanguageModel: Load pattern model from " << trainProgramOptions.getPatternModelFile() << "\n";
			patternModel.train(trainProgramOptions.getPatternModelFile(), patternModelOptions);
		}
	} else
	{
		if(trainProgramOptions.getPatternModelFile().empty())
		{
			L_V << "CCI: Load existing model to extend from " << trainProgramOptions.getExtendModel() << "\n";
			std::string extendModel = trainProgramOptions.getExtendModel();
			patternModel.load(extendModel, patternModelOptions);
            L_V << "CCI: Train the extended model from " << trainProgramOptions.getTrainCorpusFile() << std::endl;
            patternModel.train(trainProgramOptions.getTrainCorpusFile(), patternModelOptions, nullptr, nullptr, true, 1, true);
            L_V << "CCI: Write extended pattern model to " << trainProgramOptions.getPatternModelFile() << std::endl;
            patternModel.write(trainProgramOptions.getPatternModelFile());
		} else
		{
			L_V << "TrainLanguageModel: Load pattern modelfrom " << trainProgramOptions.getTrainCorpusFile() << "\n";
			patternModel.train(trainProgramOptions.getPatternModelFile(), patternModelOptions);
		}
	}

	L_V << "TrainLanguageModel: Initialise vocabulary\n";
	vocabulary = patternModel.extractset(1,1);
	L_V << "TrainLanguageModel: Vocabulary contains " << vocabulary.size() << " items\n";


	indexedCorpusIter = indexedCorpus->begin();
	reverseIndex = patternModel.getreverseindex(indexedCorpusIter.index(), 0, 0, 4);
	patternPointerIter = reverseIndex.begin();

	std::cout << "Size of revidx: " << reverseIndex.size() << std::endl;
}

PatternContainer* TrainLanguageModel::getNextPattern()
{
	L_V << "TrainLanguageModel: next pattern\n";

	if(patternPointerIter == reverseIndex.end())
	{
		do
		{
			L_V << "TrainLanguageModel: End of patternPointerIter\n";
			++indexedCorpusIter;
			++sentenceNumber;

			if(indexedCorpusIter != indexedCorpus->end())
			{
				reverseIndex = patternModel.getreverseindex(indexedCorpusIter.index(), 0, 0, 4);
				patternPointerIter = reverseIndex.begin();
			} else
			{
				break;
			}

		} while (patternPointerIter == reverseIndex.end());


	}

//	L_V << "TrainLanguageModel: next pattern (1)\n";

	if(indexedCorpusIter == indexedCorpus->end())
	{
		L_V << "TrainLanguageModel: End of indexedCorpusIter\n";

//		focus = nullptr;
//		context = nullptr;
		sentenceNumber = 0;
		patternNumber = 0;

		indexedCorpusIter = indexedCorpus->begin();
		reverseIndex = patternModel.getreverseindex(indexedCorpusIter.index(), 0, 0, 4);
		patternPointerIter = reverseIndex.begin();

		return nullptr;
	}

//	L_V << "TrainLanguageModel: next pattern (2)\n";

//	if(!(*patternPointerIter)) std::cout << "HELP" << std::endl;
	PatternPointer pp = *patternPointerIter;

//	L_V << "TrainLanguageModel: next pattern (3)\n";

	Pattern pattern(pp);

//	L_V << "TrainLanguageModel: next pattern (4)\n";

	patternContainer->context = Pattern(pattern, 0, 4 - 1);
	patternContainer->focus = Pattern(pattern, 4-1, 1);

//	L_V << "TrainLanguageModel: next pattern (5)\n";

	patternContainer->patternNumber = ++patternNumber;
	patternContainer->sentenceNumber = sentenceNumber;

	++patternPointerIter;

//	L_V << "TrainLanguageModel: next pattern (6)\n";

	return patternContainer;
}

void TrainLanguageModel::increment(const Pattern& w, const Pattern& context)
{
	lm.increment(w, context, _eng);
}
void TrainLanguageModel::decrement(const Pattern& w, const Pattern& context)
{
	lm.decrement(w, context, _eng);
}

void TrainLanguageModel::resample_hyperparameters()
{
	lm.resample_hyperparameters(_eng);
}

double TrainLanguageModel::log_likelihood() const
{
	return lm.log_likelihood();
}

//void TrainLanguageModel::loadLanguageModel(const std::string& inputFile)
//{
//	std::ifstream ifs(inputFile, std::ios::binary);
//	if(!ifs.good())
//	{
//		std::cerr << "Something went wrong with loading the language model: " << inputFile << std::endl;
//	}
//	boost::archive::binary_iarchive ia(ifs);
//
//	ia& lm;
//}

} /* namespace SLM */
