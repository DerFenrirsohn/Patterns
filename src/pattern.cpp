#include "pattern.hpp"
unsigned int edit_distance(const std::string& s1, const std::string& s2)
{
	const std::size_t len1 = s1.size(), len2 = s2.size();
	std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));

	d[0][0] = 0;
	for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
	for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

	for(unsigned int i = 1; i <= len1; ++i)
		for(unsigned int j = 1; j <= len2; ++j)
                      d[i][j] = std::min( d[i - 1][j] + 1, std::min(d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1)));
	return d[len1][len2];
};
std::string Pattern::getName(int k=0, bool display=false)
{
    std::string str;
    if (display)
    {
        std::cout<<"size "<<subPatterns.size()<<std::endl;
        std::cout<<(basis?"true":"false")<<std::endl;
    }
 
    if (basis)
    {
        str+=name;
    }
    else
    {
        for (std::vector<Pattern *>::iterator it=subPatterns.begin(); it!=subPatterns.end();it++)
        {
            str+=(*it)->getName(k+1,display);
        }
    }
    return (str);
    
};

std::vector<Pattern*> PatternHolder::str2pat(std::string str)
{
    std::vector<Pattern*> vec;
    for (int i=0;i<str.size();i++)
    {
        int j=0;
        bool found=true;
        while (found&&((i+j)<str.size()+1))
        {
            auto it = batch.find(str.substr(i,j+1));
            if (it!=batch.end())
            {
                
                j++;

            }
            else
            {

                found=false;
            }
        }                
        if (j==0)
        {
            Pattern * p = new Pattern(str.substr(i,1));
            batch.insert(std::make_pair(str.substr(i,1),p));
        }
        else
        {
            batch.at(str.substr(i,j))->used++;
            vec.push_back(batch.at(str.substr(i,j)));
        }
        i+=j-1;

        
        
    }
    return vec;
};


std::vector<Pattern*> PatternHolder::simplify(std::vector<Pattern*> vec)
{
    if(vec.size()==0)
    {
        return vec;
    }
    std::vector<Pattern*>::iterator it = vec.begin();
    std::vector<Pattern*> tosearchvec;   
    while (batch.find(Pattern(tosearchvec).getName())!=batch.end()&&it!=vec.end())
    {
        tosearchvec.push_back(*it);
        it++;

    }
    if (it==vec.begin())
    {
        tosearchvec.push_back(*it);
        it++;
    }
    std::vector<Pattern*> followingvec(it,vec.end());
    followingvec=simplify(followingvec);
    tosearchvec.insert(tosearchvec.end(),followingvec.begin(),followingvec.end()); 
    return (tosearchvec);
    

};;
void PatternHolder::printmainpatterns(int i)
{
    
    for (auto it = batch.begin(); it != batch.end(); it++)
    {
        if ((*it).second->used!=0)
        {
        
        std::cout << it->first    // string (key)
                  << ':'
                  << it->second->used   // string's value 
                  << std::endl;            
        }

    }


};
void PatternHolder::input(std::string str)
{

    std::vector<Pattern*> vec = str2pat(str);
    vec = simplify(vec);
    for ( int i=0; i<vec.size();i++)
    {
        for (int j = i; j <vec.size()+1; j++)
        {
            std::vector<Pattern*> toconsidervec(vec.begin()+i,vec.begin()+j);
            auto it = batch.find(Pattern(toconsidervec).getName());
            if (it==batch.end())
            {
                Pattern * p = new Pattern(toconsidervec);
                batch.insert(std::make_pair(p->getName(),p));
            }
        }
    } 



};
std::vector<std::string> PatternHolder::parse(std::string str, std::vector<std::string> delimiter)
{
    if(delimiter.size()==0)
    {
        std::vector<std::string> vec;
        vec.push_back(str);
        return vec;
    }
    else
    {
        std::string delim=delimiter[0];
        size_t pos = 0;
        std::string token;
        std::vector<std::string> vec;

        while ((pos = str.find(delim)) != std::string::npos) 
        {
            token = str.substr(0, pos+ delim.length());
            vec.push_back(token);
            str.erase(0, pos + delim.length());
        }
        vec.push_back(str);
        std::vector<std::string> newdelimiter(delimiter.begin()+1,delimiter.end());
        std::vector<std::string> toReturn;
        for (auto it = vec.begin(); it != vec.end(); it++)
        {
            std::vector<std::string> parsedvec = parse(*it,newdelimiter);
            toReturn.insert(toReturn.end(),parsedvec.begin(),parsedvec.end());
        }
        return toReturn;
        
    }
};
void PatternHolder::clear()
{
    double m=0; 
    double n=0;   
    for (auto it = batch.begin(); it != batch.end(); it++)
    {

        if ((*it).second->used>0)
        {
            m+= ((double)((*it).second->used));
            n+=1;
        }
        
          

    }
    m=m/n;  
    std::cout<< "Moyenne use: "<<m<<std::endl;
    std::cout<< "Taille Batch: "<<batch.size()<<std::endl;  
    std::vector<std::string> toerase;
    for (auto it = batch.begin(); it != batch.end();it++)
    {
          if((*it).second->used<m&&!(*it).second->basis)
          {
              
              
              checkifcomposedof(it->second);
              delete(it->second);
              toerase.push_back(it->first);
          }
          else
          {
              (*it).second->used-=m;
              
          }
          

    }
    for (auto it = toerase.begin(); it!= toerase.end(); it++)
    {
        batch.erase(*it);
    }
    

}
void PatternHolder::checkifcomposedof(Pattern * todelete)
{

    
    for (std::vector<Pattern * >::iterator it = todelete->abovePatterns.begin(); it != todelete->abovePatterns.end();it++)
    {

        std::cout<<"used "<<(*it)->used<<std::endl;
        (*it)->name=(*it)->getName(0,true);

        std::cout<<"ok"<<std::endl;
        (*it)->basis=true;


    }
};
void PatternHolder::readText(std::string filename)
{
    std::string str;
    std::string text;
	std::ifstream infile;
	infile.open (filename);
    while(!infile.eof()) // To get you all the lines.
    {
        
	    getline(infile,str); // Saves the line in STRING.
        text+=str;

    }
    std::vector<std::string> vec;
    vec.push_back("?");
    vec.push_back("!");
    vec.push_back(".");
    std::vector<std::string> sentences=parse(text,vec);
    
    for (int i = 0; i < sentences.size(); i++)    
    {
        input(sentences[i]);
        if (batch.size()>5000)
        {
            clear();
        }
    }

	infile.close();
};