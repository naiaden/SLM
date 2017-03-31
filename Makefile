COCO_INCLUDE=$(HOME)/local/include/colibri-core/
COCO_LIB=/home/louis/local/lib

all: ppl rescore

ppl: SLM-ppl.cpp
	$(CXX) -std=c++1y -g -O0 -I.. -o $@ $< PerplexityInterpolationStrategy.cpp NprefInterpolationStrategy.cpp RandomInterpolationStrategy.cpp CountInterpolationStrategy.cpp EntropyInterpolationStrategy.cpp MLEInterpolationStrategy.cpp Timer.cpp LimitedBackoffStrategy.cpp UniformInterpolationStrategy.cpp InterpolationStrategy.cpp FullBackoffStrategy.cpp Utils.cpp NgramBackoffStrategy.cpp BackoffStrategy.cpp BackoffStrategies.cpp LanguageModel.cpp Logging.cpp ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -I$(COCO_INCLUDE) -L$(COCO_LIB)

rescore: SLM-rescore.cpp
	$(CXX) -std=c++1y -g -O3 -I.. -o $@ $< TextPreprocessor.cpp RescoreModule.cpp NprefInterpolationStrategy.cpp RandomInterpolationStrategy.cpp CountInterpolationStrategy.cpp EntropyInterpolationStrategy.cpp MLEInterpolationStrategy.cpp NBestList.cpp Timer.cpp LimitedBackoffStrategy.cpp UniformInterpolationStrategy.cpp InterpolationStrategy.cpp FullBackoffStrategy.cpp Utils.cpp NgramBackoffStrategy.cpp BackoffStrategy.cpp BackoffStrategies.cpp LanguageModel.cpp Logging.cpp ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -I$(COCO_INCLUDE) -L$(COCO_LIB)
