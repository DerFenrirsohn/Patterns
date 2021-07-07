#include "pattern.hpp"
unsigned int edit_distance(const std::string& s1, const std::string& s2)
{
    /* Fonction inutile pour l'instant pas la peine de regarder*/
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
std::string Pattern::getName(int k, bool display)
{
    /* Retourne le nom si c'est un pattern de base, donc en gros une lettre ou une syllabe
    Retourne les noms des sous patterns concatenes sinon*/
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
    /* Transforme une string en un vecteur de patterns, ajoute au batch un nouveau pattern si le caractere n'est pas reconnu
    Sinon incrémente les pattern reconnus*/
    std::vector<Pattern*> vec;
    for (int i=0;i<str.size();i++)
    {
        int j=1;
        bool found=true;
        while (found&&((i+j)<str.size()+1))
        {
            auto it = batch.find(str.substr(i,j));
            if (it!=batch.end())
            {
                
                j++;

            }
            else
            {

                found=false;
            }
        }                
        if (j==1)
        {
            Pattern * p = new Pattern(str.substr(i,j));
            batch.insert(std::make_pair(str.substr(i,j),p));
            vec.push_back(p);
        }
        else
        {
            j--;
            batch.at(str.substr(i,j))->used++;
            vec.push_back(batch.at(str.substr(i,j)));
        }
        i+=j-1;

        
        
    }
    return vec;
};


std::vector<Pattern*> PatternHolder::simplify(std::vector<Pattern*> vec)
{
    /* Regroupe les patterns du debut de vecteur en un seul, puis concatene à ce pattern le vecteur simplifie de la suite du vecteur initial*/
    if(vec.size()==0)
    {
        return vec;
    }

    int i = 1;
    std::vector<Pattern*> toSearchVec;  
    toSearchVec.push_back(vec[0]);
    while (batch.find(Pattern(toSearchVec).getName())!=batch.end()&&i<vec.size())
    {
        toSearchVec.push_back(vec[i]);
        i++;

    }
    if (batch.find(Pattern(toSearchVec).getName())==batch.end())
    {
        toSearchVec.pop_back();
        i--;
    }


    std::vector<Pattern*> toReturnVec;
    toReturnVec.push_back(batch.at(Pattern(toSearchVec).getName()));

    std::vector<Pattern*> followingvec(vec.begin()+i,vec.end());
    followingvec=simplify(followingvec);
    toReturnVec.insert(toReturnVec.end(),followingvec.begin(),followingvec.end()); 
    return (toReturnVec);
    

};;
void PatternHolder::printMainPatterns(int i)
{
    /*Affiche les pattern utilises plus que le parametre*/
    std::cout << std::endl;
    for (auto it = batch.begin(); it != batch.end(); it++)
    {
        if ((*it).second->used>=i)
        {
        
        std::cout << it->first    // string (key)
                  << ":"
                  << it->second->used   // string's value 
                  
                  << std::endl;            
        }

    }


};
void PatternHolder::input(std::string str)
{
    /*Transforme une string en vecteur de patterns, simplifie le vecteur au maximum
    Ensuite, ajoute au batch des nouveaux patterns correspondant aux suites de deux patterns consecutifs
    A voir par la suite, peut-etre ajouter toutes les possibilites de patterns consecutifs quand le clean fonctionnera
    Incremente au passage l'usage des patterns detectes*/
    std::vector<Pattern*> vec = str2pat(str);
    std::vector<Pattern*> simplifiedVec = simplify(vec);
    while(simplifiedVec!=vec)
    {
        vec=simplifiedVec;
        simplifiedVec=simplify(simplifiedVec);
    }
    
    for ( int i=0; i<vec.size()-1;i++)
    {
        vec[i]->used++;
        
        std::vector<Pattern*> toConsiderVec(vec.begin()+i,vec.begin()+i+2);
        Pattern * p = new Pattern(toConsiderVec);
        batch.insert(std::make_pair(p->getName(),p));
    } 
    vec[vec.size()-1]->used++;



};
std::vector<std::string> PatternHolder::parse(std::string str, std::vector<std::string> delimiter)
{
    /*Decoupe une string aux endroits correspondants a differents delimiteur
    D'abord au premier, puis reappelle la fonction sur les sous string avec le vecteur de delimiter moins celui qui a deja ete traite*/
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
        std::vector<std::string> newDelimiter(delimiter.begin()+1,delimiter.end());
        std::vector<std::string> toReturn;
        for (auto it = vec.begin(); it != vec.end(); it++)
        {
            std::vector<std::string> parsedvec = parse(*it,newDelimiter);
            toReturn.insert(toReturn.end(),parsedvec.begin(),parsedvec.end());
        }
        return toReturn;
        
    }
};
void PatternHolder::clear()
{
    /*Fait la moyenne des usages non nuls et supprime tous les patterns qui ont un usage inferieur
    
    NE FONCTIONNE PAS
    
    segfault lors de l'appelle de la fonction getname dans checkIfComposedOf*/
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
    printMainPatterns(10*m);
    std::cout<< "Moyenne use: "<<m<<std::endl;
    std::cout<< "Taille Batch: "<<batch.size()<<std::endl;  
    std::vector<std::string> toErase;
    for (auto it = batch.begin(); it != batch.end();it++)
    {
          if((*it).second->used<m&&!(*it).second->basis)
          {
              
              
              checkIfComposedOf(it->second);
              delete(it->second);
              toErase.push_back(it->first);
          }
          else
          {
              (*it).second->used-=m;
              
          }
          

    }
    for (auto it = toErase.begin(); it!= toErase.end(); it++)
    {
        batch.erase(*it);
    }
    

}
void PatternHolder::checkIfComposedOf(Pattern * todelete)
{

    /*Check les patterns qui sont composes du pattern a detruire et leur donne un nom et les transforme en pattern de base
    
    NE FONCTIONNE PAS
    
    */
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
    /*Lis un text et le transforme en un vecteur de string*/
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
        if (batch.size()>500000)
        {
            clear();
        }
    }
    clear();
	infile.close();
};