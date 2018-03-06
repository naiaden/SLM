COCO_INCLUDE=$(HOME)/local/include/colibri-core/
COCO_LIB=/home/louis/local/lib

all: asd

asd: RescoreTools.cpp
	$(CXX) -std=c++17 -g -O3 -I.. -o $@ $< ../NgramBackoffStrategy.cpp ../BackoffStrategy.cpp  ../BackoffStrategies.cpp ../FullBackoffStrategy.cpp ../ValueInterpolationStrategy.cpp ../PerplexityInterpolationStrategy.cpp ../NprefInterpolationStrategy.cpp ../RandomInterpolationStrategy.cpp ../CountInterpolationStrategy.cpp ../EntropyInterpolationStrategy.cpp ../MLEInterpolationStrategy.cpp ../LimitedBackoffStrategy.cpp ../UniformInterpolationStrategy.cpp ../InterpolationStrategy.cpp ToolUtils.cpp FileReader.cpp ToolProgramOptions.cpp ../Utils.cpp ../TextPreprocessor.cpp ReferenceId.cpp Hypothesis.cpp Hypotheses.cpp Sorter.cpp ../LanguageModel.cpp FileWriter.cpp ../Logging.cpp ../ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -lstdc++fs -I$(COCO_INCLUDE) -L$(COCO_LIB)

woco: WordCounter.cpp
	$(CXX) -std=c++17 -g -O3 -I.. -o $@ $< WordList.cpp ToolUtils.cpp FileReader.cpp ToolProgramOptions.cpp ../Utils.cpp ../TextPreprocessor.cpp ReferenceId.cpp Hypothesis.cpp Hypotheses.cpp Sorter.cpp FileWriter.cpp ../Logging.cpp ../ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -lstdc++fs -I$(COCO_INCLUDE) -L$(COCO_LIB)
	
lt: ltt.cpp
	$(CXX) -std=c++17 -g -O3 -I.. -o $@ $< ../Utils.cpp

