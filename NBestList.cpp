/*
 * NBestList.cpp
 *
 *  Created on: Feb 3, 2017
 *      Author: louis
 */

#include "NBestList.h"
//#include "utils.h"

#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>

#include <string>
#include <regex>

#include "Logging.h"

#include "Utils.h"

namespace SLM {

NBestItem::NBestItem(const std::string& aSentence, int cRank, double aScore, double lScore, double words) :
        sentence(aSentence), currentRank(cRank), amScore(aScore), lmScore(lScore), nrWords(words), newRank(0)
{

}

void NBestItem::setRescore(double score)
{
        reScore = score;
}
void NBestItem::setRank(int rank)
{
        newRank = rank;
}

double NBestItem::getAmScore() const { return amScore; }
double NBestItem::getLmScore() const { return lmScore; }
double NBestItem::getReScore() const { return reScore; }

std::string NBestItem::toString(bool fileFormat) const
{
        std::stringstream ss;

        if(fileFormat) // http://www.speech.sri.com/projects/srilm/manpages/nbest-format.5.html
        {
                ss << amScore << " " << reScore /*lmScore*/ << " " << nrWords << " " << sentence;
        } else
        {
                ss << "#" << currentRank << "/" << newRank << "\tA:" << amScore << " L:" << lmScore << " R:" << reScore << " (l: " << nrWords << ")";
        }

        return ss.str();
}


std::string FileRenamer::rename(const std::string& original, const std::string& outputDir)
{
//    //0 1    2        3       4        5  6        7   8      9  10       11  12                      13              14
//    //"/esat/spchtemp/scratch/jpeleman/lm/head_map/asr/comp-d/vl/fv701012/sri/fv701012_medthr256_kni4/nbest_fullvocab/fv701012.0.X:0:55291.50000-best.txt"
//    std::vector<std::string> words = ssplit(original, '/');
////      for(auto m: words)
////      {
////              std::cout << m << std::endl;
////      }
//
//                    std::stringstream ss;
//                    ss << outputDir << words[8] << "_" << words[10] << "_" << words[14];
//
////                      std::cout << "Renaming " << original << " to " << ss.str() << std::endl;
//                    return ss.str();
	std::vector<std::string> tokens = delimiterTokeniser(original, '/');


	return outputDir + "/" + tokens.back();
}


NBestList::NBestList() {


}

NBestList::~NBestList() {
        for ( auto item : items ) delete item;
        items.clear();
}


void NBestList::add(NBestItem* item)
{
        items.push_back(item);
}

void NBestList::print(bool fileFormat)
{
        for(NBestItem* nbi : items)
        {
                std::cout << nbi->toString(fileFormat) << std::endl;
        }
}

void NBestList::printToFile(const std::string& originalFileName, const std::string& directory)
{
	L_I << "NBestList: Processing " << originalFileName << std::endl;
        std::string newName = fr.rename(originalFileName, directory);
        L_I << "NBestList:  into " << newName << std::endl;

        std::ofstream myfile;
          myfile.open (newName);
          for(NBestItem* nbi : items)
                {
                  myfile << nbi->toString(true) << std::endl;
                }
          myfile.close();
}


bool comparePtrToNode(NBestItem* a, NBestItem* b) {

        double lhs = a->getAmScore() + a->getReScore();//a->getLmScore();
        double rhs = b->getAmScore() + b->getReScore();//b->getLmScore();

        return lhs < rhs;
//      return (*a < *b);
}

void NBestList::determineNewRanks()
{
        std::sort(items.begin(), items.end(), comparePtrToNode);

        int i = 0;
        for(NBestItem* nbi : items)
        {
                nbi->setRank(++i);
        }
}

} /* namespace SLM */
