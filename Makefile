COCO_INCLUDE=$(HOME)/local/include/colibri-core/
COCO_LIB=/home/louis/local/lib


all: SLM.cpp
	$(CXX) -std=c++1y -g -O0 -I.. $< FullBackoffStrategy.cpp Utils.cpp NgramBackoffStrategy.cpp BackoffStrategy.cpp BackoffStrategies.cpp LanguageModel.cpp Logging.cpp ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -I$(COCO_INCLUDE) -L$(COCO_LIB) -L/users/spraak/onrust/software/boost_1_55_0/stage/lib/
