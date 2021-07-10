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
#include <fstream>
#include <iomanip>
#include <algorithm>



class Pattern
{
	public:
	bool basis;
	double used;
	std::string name;
	std::vector<Pattern *> subPatterns;
	std::vector<Pattern *> abovePatterns;


	Pattern(std::string str);
	Pattern(std::vector<Pattern*> subPat);
	~Pattern(){};
	std::string getName(int k=0,bool display=false);
	void increment(double x);
	void init();
};

class PatternHolder
{
	public:
	std::map<std::string,Pattern*> batch;
	int failureCount;

	PatternHolder();
	~PatternHolder();
	void input(std::string str);
	void readText(std::string filename);
	void checkIfComposedOf(Pattern * todelete);
	std::pair<int,int> findKnownPattern(std::vector<Pattern*> vec);
	std::vector<std::string> parse(std::string str, std::vector<std::string> delimiter);
	std::vector<Pattern*> str2pat(std::string str);
	std::vector<Pattern*> simplify(std::vector<Pattern*> vec);
	void clear();
	void printMainPatterns(int i);
};
#endif