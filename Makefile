COCO_INCLUDE=$(HOME)/local/include/colibri-core/
COCO_LIB=$(HOME)/local/lib

all: newppl rescore

newppl: SLM-ppl.cpp
	$(CXX) -Wno-deprecated-declarations -std=c++1y -g -O3 -I.. -o $@ $< PerplexityInterpolationStrategy.cpp NprefInterpolationStrategy.cpp RandomInterpolationStrategy.cpp CountInterpolationStrategy.cpp EntropyInterpolationStrategy.cpp MLEInterpolationStrategy.cpp Timer.cpp LimitedBackoffStrategy.cpp UniformInterpolationStrategy.cpp InterpolationStrategy.cpp FullBackoffStrategy.cpp Utils.cpp NgramBackoffStrategy.cpp BackoffStrategy.cpp BackoffStrategies.cpp LanguageModel.cpp Logging.cpp ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -I$(COCO_INCLUDE) -L$(COCO_LIB) -L/home/lonrust/Software/boost_1_56_0/stage/lib -I/home/lonrust/Software/boost_1_56_0

rescore: SLM-rescore.cpp
	$(CXX) -std=c++1y -g -O3 -I.. -o $@ $< TextPreprocessor.cpp RescoreModule.cpp NprefInterpolationStrategy.cpp RandomInterpolationStrategy.cpp CountInterpolationStrategy.cpp EntropyInterpolationStrategy.cpp MLEInterpolationStrategy.cpp NBestList.cpp Timer.cpp LimitedBackoffStrategy.cpp UniformInterpolationStrategy.cpp InterpolationStrategy.cpp FullBackoffStrategy.cpp Utils.cpp NgramBackoffStrategy.cpp BackoffStrategy.cpp BackoffStrategies.cpp LanguageModel.cpp Logging.cpp ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -I$(COCO_INCLUDE) -L$(COCO_LIB)
