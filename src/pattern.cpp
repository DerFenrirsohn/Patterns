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

PatternHolder::PatternHolder()
{
    batch.insert(std::make_pair("",new Pattern("")));
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
            batch.at(str.substr(i,j))->used++;
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
        for(int j=2; i+j<vec.size();j++)
        {
            //On extrait un sous vecteur a partir de l indice i et de longueur j
            std::vector<Pattern*> toSearchVec(vec.begin()+i,vec.begin()+i+j);
            if (batch.find(Pattern(toSearchVec).getName())!=batch.end())
            {
                // S il y a deja un pattern trouve, on compare le pattern trouve precedement et le pattern juste trouve
                // en gardant celui dont l usage est le plus frequent 
                if (foundPattern.second!=0)
                {
                    std::vector<Pattern*> currentFoundPattern(vec.begin()+foundPattern.first,vec.begin()+foundPattern.first+foundPattern.second);
                    if (batch.at(Pattern(currentFoundPattern).getName())->used<batch.at(Pattern(toSearchVec).getName())->used)
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
    std::vector<Pattern*> toSearchVec; 

    //Meme principe que pour str2pat, cherche un pattern en commencant au debut et en regardant avec une longueur de plus en plus grande
    while (batch.find(Pattern(toSearchVec).getName())!=batch.end()&&i<vec.size())
    {        
        toSearchVec.push_back(vec[i]);
        i++;
    }
    //Puis decrementer l indice et pop le vecteur, parce qu il correspond au premier pattern non trouve 
    //On le fait seulement si le pattern correspondant au vec a chercher n est pas reconnu 
    //(s il l est c est qu on a atteint la fin du vec dans le while)
    //Et si le vecteur a chercher a une taille d au moins 2, ainsi il ne sera pas nul apres la decrementation 
    if (batch.find(Pattern(toSearchVec).getName())==batch.end()&&i>1)
    {
        i--;
        toSearchVec.pop_back();
    }

    std::vector<Pattern*> toReturnVec;
    //On ajoute au vecteur a retourner le pattern correspondant au vecteur trouve
    toReturnVec.push_back(batch.at(Pattern(toSearchVec).getName()));

    //On appelle la fonction a nouveau sur la suite du vecteur d origine et on insere le resultat a la fin du vecteur a retourner
    std::vector<Pattern*> followingvec=simplify(std::vector<Pattern*>(vec.begin()+i,vec.end()));
    
    toReturnVec.insert(toReturnVec.end(),followingvec.begin(),followingvec.end()); 
    
    return (toReturnVec);
}


void PatternHolder::printMainPatterns(int i)
{
    /*Affiche les pattern utilises plus que le parametre*/
    std::cout << std::endl;
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
    //Ainsi on retourne un message "Fail" lorsqu on les connait deja
    for ( int i=0; i<vec.size()-1;i++)
    {
        vec[i]->used++;        
        std::vector<Pattern*> toConsiderVec(vec.begin()+i,vec.begin()+i+2);
        Pattern * p = new Pattern(toConsiderVec);
        p->init();
        if (batch.find(p->getName())==batch.end())
        {
            batch.insert(std::make_pair(p->getName(),p));
        }
        else
        {
            std::cout<<"Fail "<< p->getName() <<std::endl;
            std::cout<<findKnownPattern(p->subPatterns).first<<"-"<< findKnownPattern(p->subPatterns).second <<std::endl;
            for (int i = findKnownPattern(p->subPatterns).first; i < findKnownPattern(p->subPatterns).first+findKnownPattern(p->subPatterns).second; i++)
            {
                std::cout<<p->subPatterns[i]->getName()<<std::endl;
            }
            failureCount++;
            delete p;
        }      
    } 
    vec[vec.size()-1]->used++;



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
    /*Fait la moyenne des usages non nuls et supprime tous les patterns qui ont un usage inferieur
    
    NE FONCTIONNE PAS
    
    segfault lors de l'appelle de la fonction getname dans checkIfComposedOf*/

    double sum=0; 
    double count=0;

    //Moyenne de l usage de tous les patterns dont l usage est superieur a zero
    for (auto it = batch.begin(); it != batch.end(); it++)
    {
        if (it->second->used>0)
        {
            sum+= ((double)((*it).second->used));
            count+=1;
        }
    }
    double mean=sum/count;  
    std::cout<< "Moyenne use: "<<mean<<std::endl;
    std::cout<< "Taille Batch: "<<batch.size()<<std::endl;  

    // Parcours du batch pour supprimer tous les patterns dont l usage est inferieur a la moyenne
    // Appelle de la fonction checkIfComposedOf pour enlever les apparition du pointeur dans d autres patterns
    // Pour pouvoir delete le pointeur sans probleme par la suite
    // Stockage des key a retirer de la map dans un vecteur
    // Soustraire la moyenne a tous les autres patterns
    std::vector<std::string> toErase;
    for (auto it = batch.begin(); it != batch.end();it++)
    {
          if(it->second->used<mean&&!it->second->basis)
          {             
              checkIfComposedOf(it->second,mean);
              delete(it->second);
              toErase.push_back(it->first);
          }
          else
          {
              (*it).second->used-=mean;              
          }
          

    }
    std::cout<<"end of checking above patterns"<<std::endl;
    // Suppression de toutes les keys a retirer
    for (auto it = toErase.begin(); it!= toErase.end(); it++)
    {
        batch.erase(*it);
    }
    std::cout<<"end of erasing"<<std::endl;
    

}
void PatternHolder::checkIfComposedOf(Pattern * todelete, double mean)
{

    /*Check les patterns qui sont composes du pattern a detruire et leur donne un nom et les transforme en pattern de base
    
    NE FONCTIONNE PAS
    
    */

   //Parcourt tous les patterns de abovePattern, donc tous ceux qui sont composes du pattern a detruire
   //Si leur usage est superieur a la moyenne, on les transforme en pattern de base, ses subpattern devenant inutiles 
    for (std::vector<Pattern * >::iterator it = todelete->abovePatterns.begin(); it != todelete->abovePatterns.end();it++)
    {
        if ((*it)->used>=mean)
        {
            (*it)->name=(*it)->getName(0,false);
            (*it)->basis=true;
        }
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
    for (int i = 0; i < sentences.size(); i++)    
    {
        input(sentences[i]);
    //printMainPatterns(-1);
        if (batch.size()>10000)
        {
            clear();
        }        
    }
    clear();
    std::cout<<failureCount<<std::endl;
    std::cout<<batch.size()<<std::endl;
	infile.close();
};