COCO_INCLUDE=$(HOME)/local/include/colibri-core/
COCO_LIB=/home/louis/local/lib


all: SLM.cpp
	$(CXX) -std=c++1y -g -O0 -I.. $< LanguageModel.cpp Logging.cpp ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -I$(COCO_INCLUDE) -L$(COCO_LIB)
