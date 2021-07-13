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

Pattern::Pattern(std::string str, bool isBasis)
{
    used=0;
    name=str;
    basis=isBasis;
    value=Vec();
};
void Pattern::clear()
{
  
    abovePatterns.clear();
    subPatterns.clear();
    containingSets.clear();

};
Pattern::Pattern(std::vector<std::shared_ptr<Pattern>> subPat)
{
    used=0;
    basis=true;
    int j=0;
    for (int i = 0; i < subPat.size(); i++)
    {
        if (!subPat[i]->basis)
        {
            j++;
            basis=false;
        }
        
    }
    if (j!=0)
    {
        down2up=Network(std::vector<int>{(j+1)*Matrix::spaceDimension,j*10,2*Matrix::spaceDimension});
        up2down=Network(std::vector<int>{2*Matrix::spaceDimension,j*10,(j+1)*Matrix::spaceDimension});
    }
    

    subPatterns=subPat;
    value=Vec();
    createdFrom="Pattern";
    
};
Pattern::Pattern(std::vector<Capsule> subPat)
{
    used=0;
    basis=true;
    int j=0;
    for (int i = 0; i < subPat.size(); i++)
    {
        if (!subPat[i].containedPattern->basis)
        {
            basis=false;
            j++;
        }
        subPatterns.push_back(subPat[i].containedPattern);
        
    }
    
    down2up=Network(std::vector<int>{(j+1)*Matrix::spaceDimension,j*10,2*Matrix::spaceDimension});

    up2down=Network(std::vector<int>{2*Matrix::spaceDimension,j*10,(j+1)*Matrix::spaceDimension});
    createdFrom="Capsule";
    value=Vec();


    
};
int Pattern::getSizeOfDown2UpInput()
{
    if(getType()=="ComposedPattern")
    {
        int i = (*down2up.dimensions_.begin())/Matrix::spaceDimension;
        return i;
    }
    return 0;
};
EquivalenceClassPattern::EquivalenceClassPattern(std::vector<std::shared_ptr<Pattern>> elemnts, std::string str):Pattern(str,false)
{
    elements=std::make_shared<Set>(elemnts);
    elements->init();
    
};

EquivalenceClassPattern::EquivalenceClassPattern()
{
    Pattern("[ECP"+std::to_string(equivalenceClassesNumber)+"]",false);
    elements=std::make_shared<Set>();
};

Set::Set(std::vector<std::shared_ptr<Pattern>> elemnts):elements(elemnts)
{
    
    
};
void Set::init()
{
    for (int i = 0; i < elements.size(); i++)
    {
        elements[i]->containingSets.push_back(std::enable_shared_from_this<Set>::shared_from_this());
    } 
}
std::string Pattern::getName(bool takeInSetValue, Matrix val)
{
    if (subPatterns.size()==0)
    {
        return name;
    }
    else
    {
        std::string str;
        if (!takeInSetValue)
        {
            for (int i = 0; i < subPatterns.size(); i++)
            {
                str+=subPatterns[i]->getName();            
            }
        }
        else
        {
            
            int j=0;
            std::vector<Matrix> outputOfNN = split(up2down.propagate(concatenate(std::vector<Matrix>{val,value})));
            for (int i = 0; i < subPatterns.size(); i++)
            {
                if (subPatterns[i]->getType()=="Pattern")
                {
                    str+=subPatterns[i]->getName();
                }
                else
                {
                    str+=subPatterns[i]->getName(takeInSetValue,outputOfNN[j]);
                    j++;
                }
            }
        }
        return str;
    }      
}

std::string Pattern::getType()
{
    if (basis)
    {
        return("Pattern");
    }
    else
    {
        return("ComposedPattern");
    }
    
    
    
};
std::string EquivalenceClassPattern::getType()
{
    return("EquivalenceClassPattern");
};
void Pattern::init()
{
    // Pour chaque nouveau pattern compose, ajouter le pattern en question a tous ses sous pattern en tant qu'abovePattern juste apres la creation
    for (auto it = subPatterns.begin(); it != subPatterns.end(); it++)
	{
		(*it)->abovePatterns.push_back(std::enable_shared_from_this<Pattern>::shared_from_this());
	}
};
void EquivalenceClassPattern::init()
{
    
    elements->usingPattern=std::enable_shared_from_this<Pattern>::shared_from_this();
    equivalenceClassesNumber++;
};
void Pattern::increment(double x)
{

    used+=x;
    for (int i = 0; i < subPatterns.size(); i++)
    {
            subPatterns[i]->increment(x/std::max(subPatterns[i]->used,1.0));
    }
    
    
    
    
};
Matrix Pattern::getValue(std::vector<Matrix> vec)
{
    if (vec.size()==0)
    {
        return value;
    }
    else
    {
        int i = down2up.dimensions_[0]/Matrix::spaceDimension;
        if (i==vec.size())
        {
            vec.push_back(value);
            std::vector<Matrix> computedValue=split(down2up.propagate(concatenate(vec)));
            value=computedValue[1];
            return computedValue[0];
        }
        else
        {
            int sum=0;
            
            
        }
        

        
    }
    
}
std::string EquivalenceClassPattern::getName(bool takeInSetValue, Matrix val)
{
    if (takeInSetValue)
    {
        int closest=-1;
        double smallestDistance=1.79769e+308;
        for (int i = 0; i < elements->elements.size(); i++)
        {
            double d=distance(value,elements->elements[i]->value);
            if(d<smallestDistance)
            {
                closest=i;
                smallestDistance=d;
            }
        }
        return elements->elements[smallestDistance]->getName(takeInSetValue,val);
    }
    else
    {
        return name;
    }
    
    
}
Matrix EquivalenceClassPattern::getValue(std::vector<Matrix> vec)
{
    if (vec.size()==0)
    {
        return value;
    }
    else
    {
        int closest=-1;
        double smallestDistance=1.79769e+308;
        for (int i = 0; i < elements->elements.size(); i++)
        {
            double d=distance(vec[0],elements->elements[i]->value);
            if(d<smallestDistance)
            {
                closest=i;
                smallestDistance=d;
            }
        }
        return elements->elements[closest]->value;
        
        
    }
    
}
bool Set::contain(std::shared_ptr<Pattern> p)
{
    return(find(elements.begin(),elements.end(),p)!=elements.end());
};

Capsule::Capsule(std::shared_ptr<Pattern> p,std::vector<Matrix> params)
{
    containedPattern=p;
    parameters=params;
};
