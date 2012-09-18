#ifndef _INXOBJLIST_H
#define _INXOBJLIST_H

#pragma once
#include "Porting_Classes/INXObject.h"

struct __INXPOSITION {};
typedef __INXPOSITION* INXPOSITION;

typedef INXObject* object; // set data type for object
typedef unsigned char byte;

struct INXPlex{ /* @todo this is legacy code that needs reviewing */
	INXPlex* pNext;
	void* data() {
		return this+1;
	};
	static INXPlex*  Create(INXPlex*& pHead, unsigned int nMax, unsigned int cbElement) {
		INXPlex* p = (INXPlex*) new byte[sizeof(INXPlex) + nMax * cbElement];   
		p->pNext = pHead;   
		pHead = p;    
		return p;   
	};
	void FreeDataChain(){
		INXPlex* p = this;   
		while (p != NULL)   {   
			byte* _bytes = (byte*) p;   
			INXPlex* pNext = p->pNext;   
			delete[] _bytes;   
			p = pNext;   
		}   
	};      
};

struct INXNode{
	INXNode* pNext;
	INXNode* pPrev;
	object data;
};

class INXObjList
{
protected:

	INXNode* nodeHead; 
	INXNode* nodeTail; 
	int count;
	INXNode* nodeFree;
	struct INXPlex* blocks; 
	int blockSize; 

	INXNode* NewNode(INXNode* pPrev, INXNode* pNext){
		if (nodeFree == NULL){
			// add another block
			INXPlex* pNewBlock = INXPlex::Create(blocks, blockSize,
					sizeof(INXNode));

			// chain them into free list
			INXNode* pNode = (INXNode*) pNewBlock->data();
			// free in reverse order to make it easier to debug
			pNode += blockSize - 1;
			for (int i = blockSize-1; i >= 0; i--, pNode--){
				pNode->pNext = nodeFree;
				nodeFree = pNode;
			}
		}
		INXNode* pNode = nodeFree;
		nodeFree = nodeFree->pNext;
		pNode->pPrev = pPrev;
		pNode->pNext = pNext;
		count++;
		pNode->data = 0; // start with zero

		return pNode;
	}
	void FreeNode(INXNode* pNode){ /* @todo review if this should free the data mem.  */
		pNode->pNext = nodeFree;
		nodeFree = pNode;
		count--;		
		// if no more elements, cleanup completely
		if (count == 0)
			RemoveAll();
	};

public:
	INXObjList(int BlockSize = 10){
		count = 0;
		nodeHead = nodeTail = nodeFree = NULL;
		blocks = NULL;
		blockSize = BlockSize;
	}
	~INXObjList(void){
		RemoveAll();
	};

	// add before head or after tail
	INXPOSITION AddHead(object newElement){
		INXNode* pNewNode = NewNode(NULL, nodeHead);
		pNewNode->data = newElement;
		if (nodeHead != NULL)
			nodeHead->pPrev = pNewNode;
		else
			nodeTail = pNewNode;
		nodeHead = pNewNode;
		return (INXPOSITION) pNewNode;
	};
	INXPOSITION AddTail(object newElement){
		INXNode* pNewNode = NewNode(nodeTail, NULL);
		pNewNode->data = newElement;
		if (nodeTail != NULL)
			nodeTail->pNext = pNewNode;
		else
			nodeHead = pNewNode;
		nodeTail = pNewNode;
		return (INXPOSITION) pNewNode;
	};
	
	int GetCount(void){
		return count;
	};
	int GetSize(void){
		return count;
	};
	bool IsEmpty(void){
		if(count == 0) 
			return true;
		else return false;
	};

	// peek at head or tail
	object& GetHead(){
		return nodeHead->data; 
	};
	object& GetTail(){
		return nodeTail->data;
	};
// Operations
	object RemoveHead(){
		INXNode* pOldNode = nodeHead;
		object returnValue = pOldNode->data;

		nodeHead = pOldNode->pNext;
		if (nodeHead != NULL)
			nodeHead->pPrev = NULL;
		else
			nodeTail = NULL;
		FreeNode(pOldNode);
		return returnValue;
	};
	object RemoveTail(){	
		INXNode* pOldNode = nodeTail;
		object returnValue = pOldNode->data;

		nodeTail = pOldNode->pPrev;
		if (nodeTail != NULL)
			nodeTail->pNext = NULL;
		else
			nodeHead = NULL;
		FreeNode(pOldNode);
		return returnValue;
	};
	// iteration
	INXPOSITION GetHeadPosition(){
		return (INXPOSITION) nodeHead;
	};
	INXPOSITION GetTailPosition(){
		return (INXPOSITION) nodeTail;
	};
	object& GetNext(INXPOSITION& rPosition){
		INXNode* pNode = (INXNode*) rPosition;
		rPosition = (INXPOSITION) pNode->pNext;
		return pNode->data; 
	}; // return *Position++
	/*object GetNext(INXPOSITION& rPosition){
		INXNode* pNode = (INXNode*) rPosition;
		rPosition = (INXPOSITION) pNode->pNext;
		return pNode->data; 
	}; */
	//const CObject* GetNext(POSITION& rPosition) const; // return *Position++
	object& GetPrev(INXPOSITION& rPosition){
		INXNode* pNode = (INXNode*) rPosition;
		rPosition = (INXPOSITION) pNode->pPrev;
		return pNode->data;
	}; // return *Position--
	//const CObject* GetNext(POSITION& rPosition) const; // return *Position++
	/*object GetPrev(INXPOSITION& rPosition){
		INXNode* pNode = (INXNode*) rPosition;
		rPosition = (INXPOSITION) pNode->pPrev;
		return pNode->data;
	};*/
	//const CObject* GetPrev(POSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	object& GetAt(INXPOSITION position){
		INXNode* pNode = (INXNode*) position;
		return pNode->data; 
	};
	/*const object GetAt(INXPOSITION position){
		INXNode* pNode = (INXNode*) position;
		return pNode->data; 
	};*/
	void SetAt(INXPOSITION pos, object newElement){
		INXNode* pNode = (INXNode*) pos;
		pNode->data = newElement; 
	};
	INXPOSITION InsertAfter(INXPOSITION position, object newElement){

		if (position == NULL)
			return AddTail(newElement); // insert after nothing -> tail of the list
	// Insert it before position
		INXNode* pOldNode = (INXNode*) position;
		INXNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
		pNewNode->data = newElement;

		if (pOldNode->pNext != NULL){
			pOldNode->pNext->pPrev = pNewNode;
		}else{
			nodeTail = pNewNode;
		}
		pOldNode->pNext = pNewNode;
		return (INXPOSITION) pNewNode;
	}
	void RemoveAll(){
		count = 0;
		nodeHead = nodeTail = nodeFree = NULL;
		blocks->FreeDataChain();
		blocks = NULL;
	};
	void RemoveAt(INXPOSITION position){
		INXNode* pOldNode = (INXNode*) position;
		if (pOldNode == nodeHead){
			nodeHead = pOldNode->pNext;
		}
		else{
			pOldNode->pPrev->pNext = pOldNode->pNext;
		}
		if (pOldNode == nodeTail){
			nodeTail = pOldNode->pPrev;
		}
		else{
			pOldNode->pNext->pPrev = pOldNode->pPrev;
		}
		FreeNode(pOldNode);
	};

	// inserting before or after a given position
	//POSITION InsertBefore(POSITION position, CObject* newElement);
	//POSITION InsertAfter(POSITION position, CObject* newElement);
};

#endif