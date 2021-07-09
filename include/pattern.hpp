#ifndef PATTERN_HPP
#define PATTERN_HPP
#include "list"
#include "string"
#include "vector"
#include "map"
#include <stdlib.h>
#include <time.h> 
#include <random>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <iomanip>



class Pattern
{
	public:
	Pattern(std::string str):name(str),basis(true){};
	Pattern(std::vector<Pattern*> subPat)
	{
		subPatterns=subPat;
		if (subPat.size()==0)
		{
			basis=true;
		}
	};
	std::string getName(int k=0,bool display=false);
	void init();
	bool basis=false;
	double used=0;
	std::string name="";
	std::vector<Pattern *> subPatterns{};
	std::vector<Pattern *> abovePatterns{};
};

class PatternHolder
{
	public:
	PatternHolder(){batch.insert(std::make_pair("",new Pattern("")));};
	void input(std::string str);
	void readText(std::string filename);
	void checkIfComposedOf(Pattern * todelete,double mean);
	std::pair<int,int> findKnownPattern(std::vector<Pattern*> vec);
	std::vector<std::string> parse(std::string str, std::vector<std::string> delimiter);
	std::vector<Pattern*> str2pat(std::string str);
	std::vector<Pattern*> simplify(std::vector<Pattern*> vec);
	void clear();
	void printMainPatterns(int i);
	std::map<std::string,Pattern*> batch;
	int failureCount=0;
};
#endif