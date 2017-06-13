COCO_INCLUDE=$(HOME)/local/include/colibri-core/
COCO_LIB=/home/louis/local/lib

all: asd

train: TrainModel.cpp
	$(CXX) -std=c++17 -g -O3 -I.. -o $@ $< TrainProgressPrinter.cpp TrainLanguageModel.cpp TrainProgramOptions.cpp ../Utils.cpp ../TextPreprocessor.cpp ../Logging.cpp ../ProgramOptions.cpp -lcolibricore -lbz2 -lboost_serialization -lstdc++fs -I$(COCO_INCLUDE) -L$(COCO_LIB) -L/home/lonrust/Software/boost_1_56_0/stage/lib -I/home/lonrust/Software/boost_1_56_0
