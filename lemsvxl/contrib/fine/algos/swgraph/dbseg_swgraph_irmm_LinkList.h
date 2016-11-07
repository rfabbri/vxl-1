////////////////////////////////////////////////////
// LinkList
////////////////////////////////////////////////////
#ifndef _IRMM_LINKLIST_H
#define _IRMM_LINKLIST_H
 
#include <memory>
#include <cstdlib>
#include <cmath>
//#include <new>
#include <cassert>
#include <cstdlib>
#include <cstdio>

#include "Matrix.h"

class MCMC_Point
{
public:
    int x,y;

    bool operator==(const MCMC_Point &pt)
    {
        if (pt.x==x && pt.y==y)
            return true;
        else
            return false;
    }
    bool operator!=(const MCMC_Point &pt)
    {
        if (pt.x==x && pt.y==y)
            return false;
        else
            return true;
    }
    MCMC_Point& operator=(const MCMC_Point &pt)
    {
        x = pt.x;
        y = pt.y;
        return *this;
    }
};


template <class Tp>
class ListNode
{
public:
    ListNode<Tp>    *next;
    Tp                data;

public:
    // construction & destruction
    ListNode(){next=NULL;};
    ~ListNode(){next=NULL;};
    
    // operators
    ListNode& operator=(const ListNode<Tp> &x);

    // operations
    Tp        Get(void);
    void    Set(Tp &x);
};

template <class Tp>
ListNode<Tp>& ListNode<Tp>::operator=(const ListNode<Tp> &x)
{
    data = x.Get();
}

template <class Tp>
Tp ListNode<Tp>::Get(void)
{
    return data;
}

template <class Tp>
void ListNode<Tp>::Set(Tp &x)
{
    data = x;
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////
template <class Tp>
class LinkList
{
public:
    // construction & destruction
    LinkList();
    ~LinkList();
    void FreeAll(void);
    
    // operators
    LinkList<Tp>& operator=(LinkList<Tp> &x);

    // operations
    void    Insert(Tp &x);
    void    Insert_unique(Tp &x);
    bool    Delete(const Tp &x);
    bool    GetFirst(Tp &x);
    bool    GetLast(Tp &x);
    Tp*        GetFirstPos(void);
    bool    GetCurrent(Tp &x);
    Tp*        GetCurrentPos(void);
    bool    GetNext(Tp &x);
    Tp*        GetNextPos(void);
    Tp*        GetPos(const Tp &x);
    bool    IsIn(const Tp &x);
    void    Pop(void);
    inline    int Count(void) const {return count;};
    bool    IsEnd(void){if (move==tail) return true; else return false;};
    void    ToMatrix(Matrix<Tp> &mx);

private:
    ListNode<Tp>    *head,*move,*tail;
    int                count;
};

template <class Tp>
LinkList<Tp>::LinkList()
{
    head = NULL;
    move = NULL;
    tail = NULL;
    count = 0;
}

template <class Tp>
LinkList<Tp>::~LinkList()
{
    FreeAll();
}


template <class Tp>
LinkList<Tp>& LinkList<Tp>::operator=(LinkList<Tp> &x)
{
    if (this!=&x)
    {
        FreeAll();

        Tp        y;
        bool    bGo;
        bGo = x.GetFirst(y);
        while (bGo)
        {
            Insert(y);
            bGo = x.GetNext(y);
        }
    }
    return *this;
}

template <class Tp>
void LinkList<Tp>::FreeAll(void)
{
    while (head != NULL)
    {
        move = head;
        head = head->next;
        delete move;
    }
    head = NULL;
    move = NULL;
    tail = NULL;
    count = 0;
}

template <class Tp>
bool LinkList<Tp>::GetFirst(Tp &x)
{
    move = head;
    if (move == NULL)
        return false;
    else
    {
        x = move->Get();
        return true;
    }
}

template <class Tp>
bool LinkList<Tp>::GetLast(Tp &x)
{
    move = tail;
    if (move == NULL)
        return false;
    else
    {
        x = move->Get();
        return true;
    }
}

template <class Tp>
Tp*    LinkList<Tp>::GetFirstPos(void)
{
    Tp x;
    
    GetFirst(x);
    if (move != NULL)
        return &(move->data);
    else
        return NULL;
}

template <class Tp>
Tp*    LinkList<Tp>::GetCurrentPos(void)
{
    if (move != NULL)
        return &(move->data);
    else
        return NULL;
}

template <class Tp>
bool LinkList<Tp>::GetCurrent(Tp &x)
{
    if (move != NULL)
    {
        x = move->data;
        return true;
    }
    else
        return false;
}

template <class Tp>
bool LinkList<Tp>::GetNext(Tp &x)
{
    if (move == NULL)
        return false;
    else
    {
        move = move->next;
        if (move == NULL)
            return false;
        else
        {
            x = move->Get();
            return true;
        }
    }
}

template <class Tp>
Tp*    LinkList<Tp>::GetNextPos(void)
{
    Tp x;
    
    GetNext(x);
    if (move != NULL)
        return &(move->data);
    else
        return NULL;
}

template <class Tp>
void LinkList<Tp>::Insert(Tp &x)
{
    ListNode<Tp> *newNode;
    
    newNode = new ListNode<Tp>;
    if (newNode==NULL)
        exit(0);

    newNode->Set(x);
    if (head == NULL)
    {
        // empty link list
        head = newNode;
        tail = head;
    }
    else
    {
        tail->next = newNode;
        tail = newNode;
    }
    count++;
}

template <class Tp>
void LinkList<Tp>::Pop(void)
{
    if (head != NULL)
    {
        if (tail == head)
        {
            tail = NULL;
            delete head;
            head = NULL;
        }
        else
        {
            ListNode<Tp> *temp;
            temp = head;
            head = head->next;
            delete temp;
        }
        count--;
    }
}

template <class Tp>
bool LinkList<Tp>::Delete(const Tp &x)
{
    Tp             y;
    ListNode<Tp> *temp;
    
    move = head;
    while (move != NULL)
    {
        y = move->Get();
        if (y==x && move==head)
        {
            head = move->next;
            if (tail == move)
                tail = head;
            delete move;
            return true;
        }
        else
        {
            if (move->next != NULL)
            {
                y = move->next->Get();
                if (y==x)
                {
                    temp = move->next;
                    move->next = move->next->next;
                    if (tail==temp)
                        tail = move;
                    delete temp;
                    return true;
                }
            }
        }
        move = move->next;
    }
    return false;
}


template <class Tp>
bool LinkList<Tp>::IsIn(const Tp &x)
{
    bool bGo;
    Tp     y;

    bGo = GetFirst(y);
    while (bGo)
    {
        if (y==x)
            return true;
        bGo = GetNext(y);
    }

    return false;
}

template <class Tp>
void LinkList<Tp>::Insert_unique(Tp &x)
{
    if (!IsIn(x))
        Insert(x);
}


template <class Tp>
Tp*    LinkList<Tp>::GetPos(const Tp &x)
{
    bool bGo;
    Tp     y;

    bGo = GetFirst(y);
    while (bGo)
    {
        if (y==x)
            return &(move->data);
        bGo = GetNext(y);
    }
    return NULL;
}

template <class Tp>
void LinkList<Tp>::ToMatrix(Matrix<Tp> &mx)
{
    bool    bGo;
    Tp        x;
    int        i;

    mx.SetDimension(Count(),1);
    i=0;
    bGo = GetFirst(x);
    while (bGo)
    {
        mx(i++) = x;
        bGo = GetNext(x);
    }

}

#endif

