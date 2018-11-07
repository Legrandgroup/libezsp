/**
 * 
 */

#include <iterator>
#include <algorithm>

#include "observer.h"

using namespace std;
/*
void CObservateur::Update(const CObservable* observable) const
{
  //on affiche l'état de la variable
  //cout<<observable->Statut()<<endl;
}
*/ 
CObservateur::~CObservateur()
{
       //pour chaque objet observé, 
        //on lui dit qu'on doit supprimer l'observateur courant
       const_iterator ite=m_list.end();
       
       for(iterator itb=m_list.begin();itb!=ite;++itb)
       {
               (*itb)->DelObs(this);
       }
}
 
void CObservateur::AddObs( CObservable* obs)
{
    m_list.push_back(obs);
}
    
void CObservateur::DelObs(CObservable* obs)
{
    //on enlève l'objet observé.
   iterator it= std::find(m_list.begin(),m_list.end(),obs);
    if(it != m_list.end())
       m_list.erase(it);
}
 
void CObservable::AddObs( CObservateur* obs)
{
    //on ajoute l'observateur à notre liste 
    m_list.push_back(obs);

    //et on lui donne un nouvel objet observé.
    obs->AddObs(this);
}
    
void CObservable::DelObs(CObservateur* obs)
{
  //même chose que dans Observateur::DelObs
   iterator it= find(m_list.begin(),m_list.end(),obs);
    if(it != m_list.end())
       m_list.erase(it);
}
 
 
CObservable::~CObservable()
{
       //même chose qu'avec Observateur::~Observateur
       iterator itb=m_list.begin();
       const_iterator ite=m_list.end();
       
       for(;itb!=ite;++itb)
       {
               (*itb)->DelObs(this);
       }
}
 
void CObservable::Notify(void)
{
        //on prévient chaque observateur que l'on change de valeur
        iterator itb=m_list.begin();
        const_iterator ite=m_list.end();
       
       for(;itb!=ite;++itb)
       {
               (*itb)->Update(this);
       }
}