#ifndef _INXOBJARRAY_H
#define _INXOBJARRAY_H

#pragma once
#include <iostream>
#include <vector>
using namespace std;

template<class T>
class INXObjArray:public vector<T>{
public:
	// add object to a vector
	void Append(vector arr){
		insert(end(),arr.begin(),arr.end());
	};
	void Add(T obj){
		push_back(obj);
	};
	// get object from a vector
	T GetAt(unsigned int index) const{
		return at(index);
	};
	int GetCount() const{
		return size();
	};
	int GetUpperBound(){
		return (int)size() - 1;	
	};
	int GetSize(){
		return size();
	};
	void InsertAt(unsigned int index, T obj){
		insert(begin() + index, obj);
	};
	bool IsEmpty(){
		return empty();
	};
	void SetAt(unsigned int index, T obj){
		at(index) = obj;
	};
	void SetAtGrow(unsigned int index,T obj){
		if(GetUpperBound() < (int)index){
			// resize it
			resize(index + 1);
			// add the item
			at(index) = obj;
		}else at(index) = obj;	
	};
	void SetSize(int newSize, int growBy = -1){
		resize(newSize);
	};
	void RemoveAt(unsigned int index){
		erase(begin() + index);
	};
	void RemoveAll(){
		erase(begin(), end());
	};
};

#endif;