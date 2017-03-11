COCO_INCLUDE=$(HOME)/local/include/colibri-core/
COCO_LIB=/home/louis/local/lib

all: asd

asd: RescoreTools.cpp
	$(CXX) -std=c++17 -g -O3 -I.. -o $@ $< ToolProgramOptions.cpp ../Utils.cpp ../TextPreprocessor.cpp ReferenceId.cpp Hypothesis.cpp NBestList.cpp Sorter.cpp FileWriter.cpp ../Logging.cpp ../ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -lstdc++fs -I$(COCO_INCLUDE) -L$(COCO_LIB)
