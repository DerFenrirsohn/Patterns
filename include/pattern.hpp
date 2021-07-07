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
			name="";
		}
		else
		{
			
			
			
			for (auto it = subPatterns.begin(); it != subPatterns.end(); it++)
			{
				(*it)->abovePatterns.push_back(this);
			}
			
		}
		
		
	};
	std::string getName(int k=0,bool display=false);
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
	void checkIfComposedOf(Pattern * todelete);
	std::vector<std::string> parse(std::string str, std::vector<std::string> delimiter);
	std::vector<Pattern*> str2pat(std::string str);
	std::vector<Pattern*> simplify(std::vector<Pattern*> vec);
	void clear();
	void printMainPatterns(int i);
	std::map<std::string,Pattern*> batch;
};
#endif