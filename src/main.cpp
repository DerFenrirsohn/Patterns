
#include <stdlib.h>

#include <pattern.hpp>
int main(int argc, char *argv[])
{
    
    PatternHolder * ph = new PatternHolder();

    bool printStr2pat=false;
    if (printStr2pat)
    {
        std::vector<Pattern *> vec = ph->str2pat("Hello World!");
        for (int i=0;i<vec.size();i++)
        {
            std::cout<<vec[i]->getName()<<std::endl;
        }
    }

    bool printSimplification=false;
    if (printSimplification)
    {
        std::vector<Pattern *> vec = ph->str2pat("Hello World!");
        Pattern * p=new Pattern(ph->simplify(vec));
        std::cout<<p->getName()<<std::endl;
    }
    

    bool printParse=false;
    if (printParse)
    {    
        std::string str;
        std::string text;
	    std::ifstream infile;
	    infile.open ("../data/(1) The Hunger Games copy.txt");
        while(!infile.eof()) // To get you all the lines.
        {
        
	        getline(infile,str); // Saves the line in STRING.
            text+=str;

        }
        std::vector<std::string> vec{"?",".","!"};
        std::vector<std::string> sentences = ph->parse(text,vec);
        for (int i = 0; i<sentences.size();i++)
        {
            std::cout<<sentences[i]<<std::endl;
        }
    }
    bool readText=true;
    if (readText)
    {
        ph->readText("../data/(1) The Hunger Games.txt");
        delete(ph);
    }
    
    
    

    
    return 0;
}