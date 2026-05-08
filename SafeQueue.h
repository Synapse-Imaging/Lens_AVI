#pragma once
#include "stdafx.h"
#include <queue>


template <class T>
class CSafeQueue {
	CRITICAL_SECTION m_cs;
public:
	std::queue<T> q;
    
public:

    CSafeQueue() 
	{
		InitializeCriticalSection(&m_cs);
	}

    void push(T elem) {

       EnterCriticalSection(&m_cs);
        //if(elem != NULL) {
            q.push(elem);
        //}
		
        LeaveCriticalSection(&m_cs);

    }

    T next() {

        T elem;

        EnterCriticalSection(&m_cs);
        if(!q.empty()) {
            elem = q.front();
            q.pop();
        }
		else
		{
			q = std::queue<T>();  //20190226 추가 : queue clear
			return nullptr;
		}
        LeaveCriticalSection(&m_cs);

        return elem;

    }

	 bool IsEmpty() {

        if(q.empty()) {
           return true; 
        }
		else
			return false;
    }

	 int size()
	 {
		 return q.size();
	 }
};

//https://codereview.stackexchange.com/questions/149676/writing-a-thread-safe-queue-in-c  에서 발췌
