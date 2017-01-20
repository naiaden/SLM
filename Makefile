COCO_INCLUDE=$(HOME)/local/include/colibri-core/
COCO_LIB=/home/louis/local/lib


all: SLM.cpp
	$(CXX) -std=c++1y -g -O3 -I.. $< Timer.cpp LimitedBackoffStrategy.cpp UniformInterpolationStrategy.cpp InterpolationStrategy.cpp FullBackoffStrategy.cpp Utils.cpp NgramBackoffStrategy.cpp BackoffStrategy.cpp BackoffStrategies.cpp LanguageModel.cpp Logging.cpp ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -I$(COCO_INCLUDE) -L$(COCO_LIB)
