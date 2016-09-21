//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#ifndef _BENTO_TREE_H_
#define _BENTO_TREE_H_

#include <QVarLengthArray>

namespace bricolage{

class BentoBlock;

//#####################################################################
// Class BentoTree
//#####################################################################
class BentoTree
{
	
public:
	BentoBlock* mRootBlock;
	QVarLengthArray<BentoBlock*, 50> mPostOrderList;
    uint mNumNodes, mHeight;

public:
	BentoTree():mRootBlock(NULL),mNumNodes(0),mHeight(0){}
	
	void init(BentoBlock* rootBlock) 
	{
		mRootBlock=rootBlock; 
		mNumNodes=0; mHeight=0; mPostOrderList.clear();  
		setPostOrderList(mRootBlock); computeHeight(mRootBlock); 
	}
	
public:
	~BentoTree();
	
protected:
	void setPostOrderList(BentoBlock* bentoBlock);
    void computeHeight(BentoBlock* bentoBlock,uint level=0);

//#####################################################################
};
	
//#####################################################################
} // namespace bricolage

#endif // _BENTO_TREE_H_
