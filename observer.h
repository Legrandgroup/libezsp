#ifndef OBSERVER_H
#define OBSERVER_H

#include <list>

class CObservable;

class CObservateur
{
 protected:
    std::list<CObservable*> m_list;
   typedef std::list<CObservable*>::iterator iterator; 
   typedef std::list<CObservable*>::const_iterator const_iterator;
   virtual ~CObservateur() = 0;
 public:
    virtual void Update(const CObservable* observable) const = 0;

    void AddObs(CObservable* obs);
    void DelObs(CObservable* obs);
};

class CObservable
{
    std::list<CObservateur*> m_list;

   typedef std::list<CObservateur*>::iterator iterator; 
   typedef std::list<CObservateur*>::const_iterator const_iterator;

 public:
    void AddObs( CObservateur* obs);
    void DelObs(CObservateur* obs);
 
    virtual uint32_t Statut(void) const =0;
    
    virtual ~CObservable();
 protected:
    void Notify(void);

};

#endif // OBSERVER_H