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

Pattern::Pattern(std::string str)
{
    used=0;
    name=str;
    basis=true;
};

Pattern::Pattern(std::vector<Pattern*> subPat)
{
    used=0;
	if (subPat.size()==0)
	{
        name="";
		basis=true;
	}
    else
    {
        basis=false;
        subPatterns=subPat;
    }
    
};

std::string Pattern::getName(int k, bool display)
{
    /* Retourne le nom si c'est un pattern de base, donc en gros une lettre ou une syllabe
    Retourne les noms des sous patterns concatenes sinon
    
    k : profondeur de la recursion
    display : s il faut afficher ou nom les données, pour debug

    */
    std::string str;
    if (display)
    {
        std::cout<<"size "<<subPatterns.size()<<std::endl;
        std::cout<<(basis?"true":"false")<<std::endl;
    }
 
    // si le pattern est un pattern de base (une lettre par exemple), afficher simplement le nom

    if (basis)
    {
        str+=name;
    }
    // si non, afficher les noms des subpatterns concatenes, en incrementant la profondeur de recurrence lors du nouvel appel de la fonction
    else
    {
        for (std::vector<Pattern *>::iterator it=subPatterns.begin(); it!=subPatterns.end();it++)
        {
            
            str+=(*it)->getName(k+1,display);
        }
            
    }
    

    return (str);
    
};

void Pattern::init()
{
    // Pour chaque nouveau pattern compose, ajouter le pattern en question a tous ses sous pattern en tant qu'abovePattern juste apres la creation
    for (auto it = subPatterns.begin(); it != subPatterns.end(); it++)
	{
		(*it)->abovePatterns.push_back(this);
	}
};
void Pattern::increment(double x)
{
    used+=x;
    if (!basis)
    {
        for (int i = 0; i < subPatterns.size(); i++)
        {
            subPatterns[i]->increment(x/std::max(subPatterns[i]->used,1.0));
        }
    }
    
    
    
};
PatternHolder::PatternHolder()
{
    batch.insert(std::make_pair("",new Pattern("")));
    failureCount=0;
}

std::vector<Pattern*> PatternHolder::str2pat(std::string str)
{ 
    /* Transforme une string en un vecteur de patterns, ajoute au batch un nouveau pattern si le caractere n'est pas reconnu
    Sinon incrémente les pattern reconnus*/
    std::vector<Pattern*> vec;
    for (int i=0;i<str.size();)
    {
        int j=1;
        bool found=true;
        //Tant qu'on reconnait la sous string de i a i+j comme un pattern connu dans le batch,
        //on incrémente j jusqu'a ce qu'on ne reconnaisse plus le pattern
        while (found&&((i+j)<str.size()))
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
        //Si j vaut 1, le caractere seul n'a pas ete reconnu et on doit l'ajouter comme pattern de base au batch                
        if (j==1)
        {

            Pattern * p = new Pattern(str.substr(i,j));
            batch.insert(std::make_pair(str.substr(i,j),p));
            vec.push_back(p);
        }
        //Si j vaut plus que 1, alors on a reconnu au moins le caractere et eventuellement un groupe de caractere
        //Comme j est la premiere valeur telle que str[i,i+j] ne soit plus reconnu, on decremente j
        //Puis on incremente le compteur d'utilisation du pattern reconnu et on l'ajoute au vecteur qu'on retournera
        else
        {
            j--;
            vec.push_back(batch.at(str.substr(i,j)));
        }
        i+=j;       
    }
    return vec;
};


std::pair<int,int> PatternHolder::findKnownPattern(std::vector<Pattern*> vec)
{
    /* Fonction utilisée pour le debug, parcourt le vecteur et cherche si une concatenation d'au moins deux patterns est connue
    en tant que pattern dans le batch 
    Si plusieurs sont trouvees, retourne celle dont l usage est le plus frequent */

    //Le premier element de foundPättern correspond a l'indice du debut du sous vecteur correspondant a un pattern connu, le second sa longueur
    //On cherche un pattern de longueur au moins deux, donc si le second membre reste a 0, cela signifie qu aucun pattern n a ete trouve
    std::pair<int,int> foundPattern = std::make_pair(0,0);    

    for (int i =0; i<vec.size();i++)
    {
        for(int j=2; i+j<vec.size()+1;j++)
        {
            //On extrait un sous vecteur a partir de l indice i et de longueur j
            std::vector<Pattern*> newFoundVec(vec.begin()+i,vec.begin()+i+j);
            if (batch.find(Pattern(newFoundVec).getName())!=batch.end())
            {
                // S il y a deja un pattern trouve, on compare le pattern trouve precedement et le pattern juste trouve
                // en gardant celui dont l usage est le plus frequent 
                if (foundPattern.second!=0)
                {
                    std::vector<Pattern*> currentFoundPattern(vec.begin()+foundPattern.first,vec.begin()+foundPattern.first+foundPattern.second);
                    if (batch.at(Pattern(currentFoundPattern).getName())->used<batch.at(Pattern(newFoundVec).getName())->used)
                    {
                        foundPattern=std::make_pair(i,j);
                    }
                }
                // Sinon on enregistre seulement les indices du pattern trouve
                else
                {
                    foundPattern=std::make_pair(i,j);
                }                        
            }
        }
    }
    return foundPattern;
};

std::vector<Pattern*> PatternHolder::simplify(std::vector<Pattern*> vec)
{
    /* Regroupe les patterns du debut de vecteur en un seul, puis concatene à ce pattern le vecteur simplifie de la suite du vecteur initial*/
    
    // Fonction recursive, cas de base, si le vecteur est nul, on retourne simplement le vecteur
    if(vec.size()==0)
    {
        return vec;
    }

    int i = 0;
    int j = 0;

    //Meme principe que pour str2pat, cherche un pattern en commencant au debut et en regardant avec une longueur de plus en plus grande
    //conserve la longueur du plus grand pattern trouve
    while (i<vec.size()+1)
    {    
        if(batch.find(Pattern(std::vector<Pattern*>(vec.begin(),vec.begin()+i)).getName())!=batch.end())
        {
            j=i;
        }  
        i++;
    }
    std::vector<Pattern*> maxFoundVec(vec.begin(),vec.begin()+j); 

    std::vector<Pattern*> toReturnVec;
    //On ajoute au vecteur a retourner le pattern correspondant au vecteur trouve
    toReturnVec.push_back(batch.at(Pattern(maxFoundVec).getName()));

    //On appelle la fonction a nouveau sur la suite du vecteur d origine et on insere le resultat a la fin du vecteur a retourner
    std::vector<Pattern*> followingvec=simplify(std::vector<Pattern*>(vec.begin()+j,vec.end()));
    
    toReturnVec.insert(toReturnVec.end(),followingvec.begin(),followingvec.end()); 
    
    return (toReturnVec);
}


void PatternHolder::printMainPatterns(int i)
{
    /*Affiche les pattern utilises plus que le parametre*/
    for (auto it = batch.begin(); it != batch.end(); it++)
    {
        if ((*it).second->used>=i)
        {        
        std::cout << it->first << ":" << it->second->used << ":" << it->second->name << std::endl;            
        }
    }


};
void PatternHolder::input(std::string str)
{
    /*Transforme une string en vecteur de patterns, simplifie le vecteur au maximum
    Ensuite, ajoute au batch des nouveaux patterns correspondant aux suites de deux patterns consecutifs
    A voir par la suite, peut-etre ajouter toutes les possibilites de patterns consecutifs quand le clean fonctionnera
    Incremente au passage l'usage des patterns detectes*/

    // Transforme la string d entree en un vecteur de pattern
    std::vector<Pattern*> vec = str2pat(str);
    // Simplifie le pattern tant que c est possible, arret des que le vecteur et sa simplification sont identiques
    std::vector<Pattern*> simplifiedVec = simplify(vec);
    
    while(simplifiedVec.size()!=vec.size())
    {
        vec=simplifiedVec;
        simplifiedVec=simplify(simplifiedVec);
    }    

    //Pour chaque pattern du vecteur on incremente la valeur d utilisation
    //Ensuite pour chaque element du vecteur, on ajoute au batch le pattern correspondant a ([cet element]-[son successeur])
    //Pour enregister de nouveaux patterns
    //Normalement, apres simplification, les pattern qu on tente d ajouter n existent pas
    //Ainsi on retourne un message lorsqu on les connait deja
    for ( int i=0; i<vec.size()-1;i++)
    {
        vec[i]->increment(1/std::max(vec[i]->used,1.0));        
        std::vector<Pattern*> toConsiderVec(vec.begin()+i,vec.begin()+i+2);
        if (batch.find(Pattern(toConsiderVec).getName())==batch.end())
        {
            Pattern * p = new Pattern(toConsiderVec);
            p->init();
            batch.insert(std::make_pair(p->getName(),p));
        }
        else
        {
            std::cout<<"known pattern when trying to add new"<<std::endl;
            std::cout<<findKnownPattern(toConsiderVec).first<<"-"<<findKnownPattern(toConsiderVec).second<<std::endl;
            std::cout<<batch.at(Pattern(std::vector<Pattern*>(vec.begin()+findKnownPattern(toConsiderVec).first,vec.begin()+findKnownPattern(toConsiderVec).first+findKnownPattern(toConsiderVec).second)).getName())->getName()<<std::endl;
        }
        
    } 
    vec[vec.size()-1]->increment(1/std::max(vec[vec.size()-1]->used,1.0));



};
std::vector<std::string> PatternHolder::parse(std::string str, std::vector<std::string> delimiter)
{
    /*Decoupe une string aux endroits correspondants a differents delimiteur
    D'abord au premier, puis reappelle la fonction sur les sous string avec le vecteur de delimiter moins celui qui a deja ete traite*/
    
    // Si le vecteur de delimiteurs est nul, on retourne simplement la string telle qu elle
    // Cas de base de la recursion
    if(delimiter.size()==0)
    {
        std::vector<std::string> vec;
        vec.push_back(str);
        return vec;
    }
    //Sinon, on utilise le premier element du vecteur comme delimiteur pour parse la string
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
        if(str!="")
        {
            vec.push_back(str);
        }
        
        //On "pop_front" le vecteur de delimiteurs, puis on rappelle la fonction sur chacun des morceaux avec le nouveau vecteur de delimiteurs
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
    /*Fait la moyenne des usages et supprime tous les patterns qui ont un usage inferieur
    
    */

    double sum=0; 
    double count=0;

    //Moyenne de l usage de tous les patterns
    for (auto it = batch.begin(); it != batch.end(); it++)
    {
        
        
        sum+= ((double)((*it).second->used));
        count+=1;
    }
    double mean=sum/count; 
    
    printMainPatterns(mean); 
    std::cout<< "Moyenne use: "<<mean<<std::endl;
    std::cout<< "Taille Batch: "<<batch.size()<<std::endl;  

    // Parcours du batch pour supprimer tous les patterns dont l usage est inferieur a la moyenne
    // Appelle de la fonction checkIfComposedOf pour enlever les apparitions du pointeur dans d autres patterns
    // Pour pouvoir delete le pointeur sans probleme par la suite
    // Stockage des key a retirer de la map dans un vecteur
    // Soustraire la moyenne a tous les autres patterns
    std::vector<std::string> toErase;
        std::cout<<batch.size()<<std::endl;
    for (auto it = batch.begin(); it != batch.end();it++)
    {
          if(it->second->used<(mean/2)&&it->second->getName()!="")
          {             
              checkIfComposedOf(it->second);
              toErase.push_back(it->first);
          }
          else
          {
              (*it).second->used-=mean/2;          
          }
          

    }
    std::cout<<"end of checking above patterns"<<std::endl;
    // Suppression de toutes les keys a retirer
    for (auto it = toErase.begin(); it!= toErase.end(); it++)
    {
        delete(batch.at(*it));
        batch.erase(*it);
    }
    std::cout<<"end of erasing"<<std::endl;
    

}
void PatternHolder::checkIfComposedOf(Pattern * todelete)
{

    /*Check les patterns qui sont composes du pattern a detruire et leur donne un nom et les transforme en pattern de base
    
    */

   //Parcourt tous les patterns de abovePattern, donc tous ceux qui sont composes du pattern a detruire
   //Si leur usage est superieur a la moyenne, on les transforme en pattern de base, ses subpattern devenant inutiles 

              
              
    for (std::vector<Pattern * >::iterator it = todelete->abovePatterns.begin(); it != todelete->abovePatterns.end();it++)
    {

            (*it)->name=(*it)->getName(0,false);
            (*it)->basis=true;
        
    }  
    //Parcourt les subpatterns, et supprime le vecteur a delete de leurs abovepatterns (normalement il y est, mais il y a des erreurs sans la condition)
    for (std::vector<Pattern * >::iterator it = todelete->subPatterns.begin(); it != todelete->subPatterns.end();it++)
    {
        auto iter=std::find((*it)->abovePatterns.begin(),(*it)->abovePatterns.end(), todelete);
        if (iter!=(*it)->abovePatterns.end())
        {
            (*it)->abovePatterns.erase(iter);
        }            
    }
};
PatternHolder::~PatternHolder()
{
    for(auto it=batch.begin();it!=batch.end();it++)
    {
        delete it->second;
    }
};

void PatternHolder::readText(std::string filename)
{
    //Lis un fichier et stock tout le texte dans une string
    std::string str;
    std::string text;
	std::ifstream infile;
	infile.open (filename);
    while(!infile.eof())
    {        
	    getline(infile,str);
        text+=str;
    }

    //Parse le texte et le transforme en un vecteur de string correspondant aux phrases
    std::vector<std::string> vec{"?","!","."};
    std::vector<std::string> sentences=parse(text,vec);
    
    //Input les phrases une par une, et clear le batch lorsque sa taille dépasse une valeur donnée
    int j=0;
    for (int i = 0; i < sentences.size(); i++)    
    {
        input(sentences[i]);
        std::cout<<i<<"/"<<sentences.size()-1<<std::endl;
        if (batch.size()>3000)
        {
            clear();
            j++;
            std::cout<<j<<std::endl;
        }        
    }
    printMainPatterns(1);
    std::cout<<failureCount<<std::endl;
    std::cout<<batch.size()<<std::endl;
	infile.close();
};
