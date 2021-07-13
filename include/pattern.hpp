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
#include <neuralNetwork.hpp>
#include <memory>

class Set;
class Capsule;


class Pattern : public std::enable_shared_from_this<Pattern>
{
	public:
	bool basis;
	double used;
	std::string name;
	std::vector<std::shared_ptr<Pattern>> subPatterns;
	std::vector<std::shared_ptr<Pattern>> abovePatterns;
	std::vector<std::shared_ptr<Set>> containingSets;
	Matrix value;
	Network down2up;
	Network up2down;
	std::string createdFrom;

	
	Pattern(){};
	Pattern(std::string str,bool basis=true);
	Pattern(std::vector<Capsule> subPat);
	Pattern(std::vector<std::shared_ptr<Pattern>> subPat);
	~Pattern(){};
	virtual void clear();
	virtual std::string getType();
	virtual std::string getName(bool takeInSetValue=false, Matrix val=Matrix());
	virtual void increment(double x);
	virtual void init();
	int getSizeOfDown2UpInput();
	virtual Matrix getValue(std::vector<Matrix> vec);
};

class Set: public std::enable_shared_from_this<Set>
{
	public:
	std::vector<std::shared_ptr<Pattern>> elements;
	std::shared_ptr<Pattern> usingPattern;
	std::vector<std::string> patternTemplate;

	Set(){};
	Set(std::vector<std::shared_ptr<Pattern>> elemnts);
	void init();
	bool contain(std::shared_ptr<Pattern> p);

};

class EquivalenceClassPattern : public Pattern 
{
	public:
	std::shared_ptr<Set> elements;

	EquivalenceClassPattern(std::vector<std::shared_ptr<Pattern>> elemnts, std::string str);
	EquivalenceClassPattern();
	void init() override;
	std::string getType() override;
	virtual Matrix getValue(std::vector<Matrix> vec) override;
	std::string getName(bool takeInSetValue=false, Matrix val=Matrix()) override;
	static int equivalenceClassesNumber;
};

class Capsule
{
	public:
	Capsule(std::shared_ptr<Pattern> p,std::vector<Matrix> params);
	std::shared_ptr<Pattern> containedPattern;
	std::vector<Matrix> parameters;
};

class PatternHolder
{
	public:
	std::map<std::string,std::shared_ptr<Pattern>> batch;
	std::vector<std::shared_ptr<Set>> sets;
	int failureCount;

	PatternHolder();
	~PatternHolder();
	void input(std::string str);
	void readText(std::string filename);
	void clear();
	void sleep(double mean);
	void fusionSets(){};
	void checkIfComposedOf(std::shared_ptr<Pattern> todelete);
	std::pair<int,int> findKnownPattern(std::vector<std::shared_ptr<Pattern>> vec);
	std::vector<std::string> parse(std::string str, std::vector<std::string> delimiter);
	std::vector<std::shared_ptr<Pattern>> str2pat(std::string str);
	std::vector<Capsule> str2caps(std::string str);
	std::vector<std::shared_ptr<Pattern>> simplify(std::vector<std::shared_ptr<Pattern>> vec);
	std::vector<Capsule> simplify(std::vector<Capsule> vec,int i=0);
	void createNewPattern(std::vector<Capsule> data);
	void printMainPatterns(double i);
};
#endif