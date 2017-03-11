//============================================================================
// Name        : RescoreTools.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <string>
#include <iostream>
#include <vector>
#include <experimental/filesystem>

#include <unordered_map>

#include "Utils.h"
#include "ReferenceId.h"
#include "Sorter.h"
#include "FileWriter.h"
#include "FileReader.h"
#include "TextPreprocessor.h"
#include "ToolProgramOptions.h"

// grid search optimal parameters
// best hyp
// oracle ceiling

std::unordered_map<std::string, SLM::ReferenceId> collectReferenceIds(const std::string& path)
{
	std::unordered_map<std::string, SLM::ReferenceId> referenceIds;

	for (auto & p : std::experimental::filesystem::directory_iterator(path))
	{
		std::string fileName = delimiterTokeniser(p.path(), '/').back();
		std::string fileId = delimiterTokeniser(fileName, '.').front();

		referenceIds.emplace (fileId, SLM::ReferenceId(fileId));
	}

	std::cout << referenceIds.size() << " references found" << std::endl;

	return referenceIds;
}

SLM::ReferenceId collectNBestListsForReferenceId(SLM::ReferenceId& rId, const std::string& path)
{
	std::cout << rId.getId() << std::endl;

	for (auto & p : std::experimental::filesystem::directory_iterator(path))
	{
		std::string fileName = delimiterTokeniser(p.path(), '/').back();
		if(startsWith(fileName, rId.getId()))
		{
			std::cout << " " << fileName << std::endl;
			rId.add(new SLM::NBestList(fileName, p.path()));
		}
	}

	std::cout << rId.getNBestLists().size() << std::endl;

	return rId;
}

// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B
double WER(const std::vector<std::string> &ref, const std::vector<std::string> &hyp)
{
	// To change the type this function manipulates and returns, change
	// the return type and the types of the two variables below.
	int s1len = ref.size();
	int s2len = hyp.size();

	auto column_start = (decltype(s1len))1;

	auto column = new decltype(s1len)[s1len + 1];
	std::iota(column + column_start, column + s1len + 1, column_start);

	for (auto x = column_start; x <= s2len; x++) {
		column[0] = x;
		auto last_diagonal = x - column_start;
		for (auto y = column_start; y <= s1len; y++) {
			auto old_diagonal = column[y];
			auto possibilities = {
				column[y] + 1,
				column[y - 1] + 1,
				last_diagonal + (ref[y - 1] == hyp[x - 1]? 0 : 1)
			};
			column[y] = std::min(possibilities);
			last_diagonal = old_diagonal;
		}
	}
	auto result = column[s1len];
	delete[] column;
	return 100* (result/(double) s1len);
}


enum PROGRAM_MODE { GRIDSEARCH, SELECTBEST, CEILING};

int main(int argc, char** argv)
{
	SLM::ToolProgramOptions po(argc, argv);


    PROGRAM_MODE pm = PROGRAM_MODE::SELECTBEST;

//	SLM::AcousticSorter sorter;
    SLM::WeightedSorter sorter(1, 1);
	SLM::CGNTextPreprocessor tpp;

    std::unordered_map<std::string, SLM::ReferenceId> referenceIds = collectReferenceIds(po.getInputPath());

    for (auto & r : referenceIds)
    {
    	collectNBestListsForReferenceId(r.second, po.getInputPath());

    	// if get best hypothesis per nbest file
		{
    		SLM::ReferenceFileWriter fw(po.getOutputPath(), r.first + "-" + sorter.getName());
    		SLM::ReferenceFileReader fr(po.getReferencePath(), r.first + ".stm");

    		std::vector<std::string> reference = fr.getTokens();
    		std::vector<std::string> hypothesisTokens;


    		for(auto & nbl : r.second.getNBestLists())
    		{
    			std::cout << "!!!\t\t" << nbl->getFileName() << "\t\t" << sorter.sort(*nbl).getAcousticScore() << "\n" << std::endl;
    			std::vector<std::string> f = sorter.sort(*nbl).getTokens();
    			hypothesisTokens.insert(std::end(hypothesisTokens), std::begin(f), std::end(f));
    		}
//    		;
    		std::cout << WER(reference, tpp.removeFillers(hypothesisTokens, true)) << std::endl;

    		std::cout << join(reference, " ") << std::endl;
    		std::cout << join(tpp.removeFillers(hypothesisTokens, true), " ") << std::endl;
		}
    }

    std::vector<std::string> ref = whitespaceTokeniser("maar Sun Dance riep mevrouw Brogen lachend hoe kun je zo harteloos zijn ik ben niet harteloos vond Sun Dance het is gewoon waar Man zegt altijd dat hij het leuk vindt om ziek te zijn op dat ogenblik werd er geklopt en stak meneer Robinson zijn hoofd om de woonkamerdeur kom binnen kom binnen riep mevrouw Brogen het hindert niet dat je lekt ik ben eigenlijk te nat zei meneer Robinson vermoeid maar hij stapte toch naar binnen hij zag er vreselijk afgemat uit en het leek wel of hij net was aangespoeld u bent toch niet zo nat geworden op die paar meter door de tuin heen riep Robin uit dat heb je goed gezien zei meneer Robinson hij schudde het natte haar uit z'n ogen en blies op z'n vingers ik ben op het strand geweest om die verd die vervl die krankzinnige hond van ons te roepen een half uur lang en begin nu niet gelijk tegen me te jammeren Mier niemand heeft hem naar buiten gestuurd het was z'n eigen idee hij glipte langs me heen toen ik probeerde de brievenbus dicht te maken zodat de regen niet naar binnen zou waaien en ik ben er net achter gekomen dat je moeder op de drempel naar hem heeft staan roepen in die wind hoe gaat het met haar onderbrak mevrouw Brogen hem nog min of meer hetzelfde dank je antwoordde meneer Robinson hij bukte zich om een stapel cadeautjes onder de kerstboom te leggen ik moest je hartelijk bedanken dat je het hele stel vanmiddag hier wilde hebben en je een gelukkig kerstfeest wensen en je moest je geen zorgen maken je krijgt bezoek morgen is het niet alleen Robins vriend Dani<EB>l en z'n vader en moeder antwoordde mevrouw Brogen dus als ik wat kan doen wat dan ook zeg het dan en zeg tegen haar dat ze veel te veel heeft meegegeven zoveel pakjes dat zegt ze van jou ook je kan nooit te veel Kerstmis hebben zei Robin en Ouwe Lap komt heus gauw weer terug hij weet echt nog wel wat hij doet");
    std::vector<std::string> hyp = whitespaceTokeniser("maar samen Beyns riep mevrouw belopen lachend hoe kun je zo harteloos zijn ik ben niet harteloos Handzame mens het is gewoon waren waarom zegt altijd dat het leuk vindt om ziek te zijn op dat ogenblik werd er geklopt een sterk Mr Robinson zijn hoofd om de bomen raakte kom binnen kom binnen ik ben verbroken het hindert niet uitgelekt in een eigen lijkt en dat zij Mr Robinson vermoeid maar hij stapte toch naar binnen hijsen haar vreselijke afgemaakt bij en het leek wel of hij net was aangespoeld ik ben toch niet zo nat geworden op die paar meter door De Deyne ��n Een probleem uit dat het je goed gezien zij mengen Robinson hij scheidde het natte haren uit zijn ogen en blies op zijn vingers ik ben beschaamd geweest om leverde die vervoeren die krankzinnige om vanalles te roepen een halfuur lang en begin nu niet gelijk tegen Wetteren Jan Renier niemand heeft hem naar buiten gestuurd het was zijn eigen idee hij glipte langs me heen toen ik probeerde de brievenbus dicht te maken zodat de regen niet naar binnen zou waaien en ik ben er net achter gekomen dat je moeder op de drempel naar hem heeft samenroepen in die wind hoe gaat het met haar onderbuik mevrouw berokkent hem toch min of meer hetzelfde drankje antwoordde meneer mensen hij beukte zich om een stapel cadeautjes onder de kerstboom te leggen ik moest je hartelijk bedanken dat je het hele stel vanmiddag geen wilde hebben en je een gelukkig kerstfeest wensen en je moest je geen zorgen maak ik het bezoek morgen is het niet alleen Robbins vriendin Danielle en zijn vader en moeder antwoordde mevrouw brokken dus als ik wat kan doen wat dan ook zeggen dan en zeg tegen haar dat ze veel te veel heeft meegegeven hoeveel pakjes dat zegt ze van jou ook je kan nooit teveel Kerstmis hebben zei Robin en ouwe rat komt wel eens gauw weer terug hij weet echt nog wel wat hij doet");

    std::cout << WER(ref, hyp) << std::endl;


}
