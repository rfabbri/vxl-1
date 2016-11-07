#ifndef _STL_UTIL_H
#define _STL_UTIL_H
#pragma warning(disable:4786)

#include <vector>
#include <set>

template<class Tp>
class set_vector:public vector<Tp>{
public:
    inline void insert(Tp &val){::insert(*this,val);}
    inline iterator find(Tp &val){return ::find(*this,val);}
    inline erase(Tp &val){::erase(*this,val);}
};

template<class Tp> bool disjoint(vector<Tp> &s1,set_vector<Tp> &s2); //true if s1 and s2 are disjoint
//vector operations
template<class Tp> void clean_end(vector<Tp *> &v, int nmin); // delete whatever is empty at the end of v
template<class Tp> void delete_element(vector<Tp *> &v,int i);
template<class Tp> void erase_val(vector<Tp> &v, Tp val);
template<class Tp> vector<Tp>::iterator find(vector<Tp> &L, Tp val);
template<class Tp> int find_idx(vector<Tp> *L, const Tp val);
template<class Tp1,class Tp2> vector<pair<Tp1, Tp2> >::iterator find_key(vector<pair<Tp1,Tp2> > &L, Tp1 val);
template<class Tp> void insert(vector<Tp> &L, Tp val);
template<class Tp> void insert_sort(vector<Tp> &L, Tp val);
template<class Tp> bool vect_cmp(vector<Tp> &v1,vector<Tp> &v2);//true if v1==v2
template<class Tp> int rand_elem(vector<Tp *> &v); // random nonzero element

// set operations
template<class Tp> Tp kth_elem(int k, set<Tp> &s);
template<class Tp> Tp rand_elem(set<Tp> &s);
template<class Tp> bool disjoint(set<Tp> &s1,set<Tp> &s2); //true if s1 and s2 are disjoint
template<class Tp> bool set_cmp(set<Tp> &s1,set<Tp> &s2);// true if s1==s2


template<class Tp>
vector<Tp>::iterator find(vector<Tp> &L, Tp val){
    //finds val in vector L, return iterator if found, else returns end
    vector<Tp>::iterator i,e=L.end();
    for (i=L.begin();i!=e;++i){
        if (*i==val) return i;
    }
    return e;
}

template<class Tp>
void insert(vector<Tp> &L, Tp val){
    //finds val in vector L, return iterator if found, else returns end
    vector<Tp>::iterator i,e=L.end();
    for (i=L.begin();i!=e;++i){
        if (*i==val) return;
    }
    L.push_back(val);
}

template<class Tp>
void insert_sort(vector<Tp> &L, Tp val){
    //inserts val in vector L in the right order
    vector<Tp>::iterator i,e=L.end();
    for (i=L.begin();i!=e;++i){
        if (*i>val){
            L.insert(i,val);
            return;
        }
        if (*i==val) return;
    }
    L.push_back(val);
}

template<class Tp>
int find_idx(vector<Tp> *L, const Tp val){
    //finds val in vector L, return index if found, else returns -1
    int i,n=L->size();
    for (i=0;i<n;++i){
        if ((*L)[i]==val) return i;
    }
    return -1;
}

template<class Tp>
inline void erase_val(vector<Tp> &v, Tp val){
    //finds val and erases it
    vector<Tp>::iterator it,ie=v.end();
    for (it=v.begin();it!=ie;++it) {
        if (*it==val){
            v.erase(it);
            return;
        }
    }
}
template<class Tp>
inline void clean_end(vector<Tp *> &v,int nmin){ // delete whatever is empty at the end of v
    // keep at least nmin+1 elements
    int n=v.size();
    while(n>nmin&&v[n-1]==NULL){
        v.pop_back();
        n--;
    }
}
template<class Tp>
void delete_element(vector<Tp *> &v,int i){
    delete v[i];
    v[i]=NULL;
    clean_end(v);
}

template<class Tp>
int rand_elem(vector<Tp *> &v){ // random nonzero element
    clean_end(v);
    int i,n=v.size();
    if (n==0)
        return -1;
    do {
        i=randint(n);
    }while (v[i]==NULL);
    return i;
}


template<class Tp>
inline bool vect_cmp(vector<Tp> &v1,vector<Tp> &v2){//true if v1==v2
    int i,n=v1.size();
    if (v2.size()!=n)
        return false;
    for (i=0;i<n;i++)
        if (v1[i]!=v2[i])
            return false;
    return true;
}
template<class Tp>
inline bool disjoint(vector<Tp> &s1,set_vector<Tp> &s2){ //true if s1 and s2 are disjoint
    vector<Tp>::iterator si,s1e=s1.end(),s2e=s2.end();
    for (si=s1.begin();si!=s1e;++si){
        if (s2.find(*si)!=s2e)
            return false;
    }
    return true;
}
template<class Tp1,class Tp2>
vector<pair<Tp1, Tp2> >::iterator find_key(vector<pair<Tp1,Tp2> > &L, Tp1 val){
    //finds val in vector L, return iterator if found, else returns end
    vector<pair<Tp1,Tp2> >::iterator i,e=L.end();
    for (i=L.begin();i!=e;++i){
        if ((*i).first==val) return i;
    }
    return e;
}

template<class Tp>
inline Tp kth_elem(int k, set<Tp> &s){
    int j;
    set<Tp>::iterator si;
    si=s.begin();for (j=0;j<k;j++) ++si; 
    return *si;
}
template<class Tp>
inline Tp rand_elem(set<Tp> &s){
    return kth_elem(randint(s.size()),s);
}

template<class Tp>
inline bool disjoint(set<Tp> &s1,set<Tp> &s2){ //true if s1 and s2 are disjoint
    set<Tp>::iterator si,s1e=s1.end(),s2e=s2.end();
    for (si=s1.begin();si!=s1e;++si){
        if (s2.find(*si)!=s2e)
            return false;
    }
    return true;
}
template<class Tp>
inline bool set_cmp(set<Tp> &s1,set<Tp> &s2){// true if s1==s2
    set<Tp>::iterator si,sj,s1e=s1.end(),s2e=s2.end();
    if (s1.size()!=s2.size())
        return false;
    sj=s2.begin();
    for (si=s1.begin();si!=s1e;++si,++sj){
        if (*si!=*sj)
            return false;
    }
    return true;
}


#endif

