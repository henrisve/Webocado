//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <algorithm>
#include <QByteArray>
#include <QDebug>
#include <QWebFrame>
#include <QFile>
#include <QList>
#include "BlockExtractor.h"
#include "SeparatorDetector.h"
#include "StructureConstructor.h"
#include "Bento.h"
#include "BentoTree.h"
#include "BentoBlock.h"
using namespace bricolage;
//#####################################################################
// Function computeBentoTree
//#####################################################################
void Bento::computeBentoTree(BentoTree& bentoTree)
{
	QFile jquery(":/assets/jquery.js");
    jquery.open(QIODevice::ReadOnly);
    QString jquery_src(jquery.readAll());
	mBrowserDocument.webFrame()->evaluateJavaScript(jquery_src);

	QFile preprocess(":/assets/preprocess.js");
    preprocess.open(QIODevice::ReadOnly);
    QString preprocess_src(preprocess.readAll());
	mBrowserDocument.webFrame()->evaluateJavaScript(preprocess_src);
	

	QWebElement body = mBrowserDocument.findFirst("body");
	BentoBlock* rootBlock = new BentoBlock(body);

	blockExtractionPass(rootBlock);
	bentoTree.init(rootBlock);

//    for (int i=0; i<bentoTree.mPostOrderList.size(); i++)
//        bentoTree.mPostOrderList[i]->ComputedStyleListPointer=ComputedStyleListPointer;


    for (int i=0; i<bentoTree.mPostOrderList.size()-1; i++)
        reStructure(bentoTree.mPostOrderList[i],bentoTree);



	bentoTree.init(rootBlock);

	int numNodesBefore, numNodesAfter;
    numNodesBefore=numNodesAfter=0;
	    
	do {
		atLeastTwoChildren(rootBlock);
		canvasTagPass(rootBlock);
		bentoTree.init(rootBlock);
        
		for (int i=0; i<bentoTree.mPostOrderList.size(); i++) {
			BentoBlock* bentoBlock = bentoTree.mPostOrderList[i];
			if (bentoBlock->mChildren.size()>0) {
				QSet<BentoBlock*> blockPool = bentoBlock->mChildren.toList().toSet();
				bentoBlock->mChildren.clear();
				SeparatorDetector separatorDetector(mBrowserDocument, blockPool);
				mSeparatorList += separatorDetector.mSeparators;
				StructureConstructor structureConstructor(separatorDetector.mSeparators, blockPool, bentoBlock);
			}
		}
		bentoTree.init(rootBlock);
		
        int numNodesPrev = numNodesBefore;
		numNodesBefore = bentoTree.mPostOrderList.size();
        if (numNodesBefore==numNodesBefore) break;
		removeSameSizeChild(rootBlock);
		bentoTree.init(rootBlock);
		numNodesAfter = bentoTree.mPostOrderList.size();

	} while(numNodesBefore!=numNodesAfter);

    setChildID(bentoTree.mRootBlock);
	
}
//#####################################################################
// Function blockExtractionPass
//#####################################################################
void Bento::blockExtractionPass(BentoBlock* bentoBlock)
{
	BlockExtractor blockExtractor(bentoBlock);
	
	foreach (BentoBlock* blockFromPool, blockExtractor.mBlockPool) {
		bentoBlock->mChildren.append(blockFromPool); blockFromPool->mParent = bentoBlock;
	}
	qStableSort(bentoBlock->mChildren.begin(), bentoBlock->mChildren.end(), BentoBlock::topBottomLeftRight);

	foreach (BentoBlock* bentoBlock, blockExtractor.mBlockPool)
		if (divideFurther(bentoBlock)) blockExtractionPass(bentoBlock);
}
//#####################################################################
// Function reStructure
//#####################################################################
void Bento::reStructure(BentoBlock* bentoBlock, const BentoTree& bentoTree)
{	
    //bentoBlock->rootPage=thisPage;
	QVector<BentoBlock*> potParentList;
	for(int i=0; i<bentoTree.mPostOrderList.size(); i++) {
		BentoBlock* potParent = bentoTree.mPostOrderList[i];
		if(bentoBlock!=potParent && !parentTaboo(bentoBlock, potParent) && contains(bentoBlock, potParent)) potParentList.append(potParent);
	}
	qStableSort(potParentList.begin(), potParentList.end(), BentoBlock::increasingArea);

	if(!potParentList.isEmpty()) {
		BentoBlock* newParent = potParentList[0];
		if(bentoBlock->mParent!=newParent) {
			
			mNumMoves++;

			bentoBlock->mParent->mChildren.remove(bentoBlock->mParent->mChildren.indexOf(bentoBlock));
			bentoBlock->mParent=newParent;
			newParent->mChildren.append(bentoBlock);
			qStableSort(newParent->mChildren.begin(), newParent->mChildren.end(), BentoBlock::topBottomLeftRight);

            //bentoBlock->mParent->mDOMNode.appendInside(bentoBlock->mDOMNode);
		}
	}

	else {
		for (int i=0; i<bentoBlock->mChildren.size(); i++) {
			bentoBlock->mParent->mChildren.append(bentoBlock->mChildren[i]);
			bentoBlock->mChildren[i]->mParent = bentoBlock->mParent;
            bentoBlock->mChildren[i]->mChildID = i;//add ID to each kid..
            //bentoBlock->mChildren[i]->rootPage=thisPage;

		}
		qStableSort(bentoBlock->mParent->mChildren.begin(), bentoBlock->mParent->mChildren.end(), BentoBlock::topBottomLeftRight);
 	}
}
//#####################################################################
// Function divideFurther
//#####################################################################
bool Bento::divideFurther(BentoBlock* bentoBlock) const
{
	QWebElement domNode = bentoBlock->mDOMNode;
	if(DOMUtils::numTextChildren(domNode) > 0) return false;
	return true;
}
//#####################################################################
// Function parentTaboo
//#####################################################################
bool Bento::parentTaboo(BentoBlock* bentoBlock, BentoBlock* potParent) const
{
	for(int i=0; i<mNoChildrenTags.size(); i++) if(potParent->mDOMNode.tagName()==mNoChildrenTags[i]) return true;
	for(int i=0; i<mNoSameTagChildren.size(); i++) if(potParent->mDOMNode.tagName()==mNoSameTagChildren[i] && bentoBlock->mDOMNode.tagName()==mNoSameTagChildren[i]) return true;
	return false;	
}
//#####################################################################
// Function contains
//#####################################################################
bool Bento::contains(BentoBlock* bentoBlock, BentoBlock* potParent) const
{
	QRect bBox = bentoBlock->mGeometry; float area = bBox.width()*bBox.height();
	QRect potParentBBox = potParent->mGeometry; float potParentArea = potParentBBox.width()*potParentBBox.height();
	QRect intersectBBox = potParentBBox.intersect(bBox);
	float percentAreaIntersect = (intersectBBox.width()*intersectBBox.height())/(1.0*area);
	return ((potParentArea>area || (potParentArea==area && potParent->mLevel<bentoBlock->mLevel)) && percentAreaIntersect>=0.5);
}
//#####################################################################
// Function atLeastTwoChildren
//#####################################################################
void Bento::atLeastTwoChildren(BentoBlock* bentoBlock) const
{
	while(true) {
		BentoBlock* temp = NULL;
		if(bentoBlock->mChildren.size() != 1) break;

		if(bentoBlock->mSameSizeContent || (!bentoBlock->mDOMNode.isNull() && (bentoBlock->mDOMNode.styleProperty("background-image", QWebElement::ComputedStyle)!="none" || DOMUtils::numTextChildren(bentoBlock->mDOMNode)>0)) || !bentoBlock->mParent) {
            temp = bentoBlock->mChildren[0];
			bentoBlock->mChildren.clear();
			for(int i=0; i<temp->mChildren.size(); i++) { temp->mChildren[i]->mParent = bentoBlock; bentoBlock->mChildren.append(temp->mChildren[i]);}
		}
		else {
			temp=bentoBlock;
			bentoBlock=bentoBlock->mParent;
			bentoBlock->mChildren.remove(bentoBlock->mChildren.indexOf(temp));
			temp->mChildren[0]->mParent=bentoBlock;
			bentoBlock->mChildren.append(temp->mChildren[0]);
			qStableSort(bentoBlock->mChildren.begin(), bentoBlock->mChildren.end(), BentoBlock::topBottomLeftRight);
		}
		delete temp;
	}
	
	for (int i=0; i<bentoBlock->mChildren.size(); i++) atLeastTwoChildren(bentoBlock->mChildren[i]);

}
//#####################################################################
// Function removeSameSizeChild
//#####################################################################
void Bento::removeSameSizeChild(BentoBlock* bentoBlock) const
{
	QRect bBox = bentoBlock->mGeometry; float area = bBox.width()*bBox.height();
	BentoBlock* temp = NULL;
	for (int i=0; i<bentoBlock->mChildren.size(); i++) {
		QRect childBBox = bentoBlock->mChildren[i]->mGeometry; float childArea = childBBox.width()*childBBox.height();
		QRect intersectBBox = bBox.intersect(childBBox);
		float percentAreaIntersect = (intersectBBox.width()*intersectBBox.height())/(1.0*childArea);
		float relativeArea = childArea/area;
		if (percentAreaIntersect>=0.98 && relativeArea>=0.98) {
			temp = bentoBlock->mChildren[i];
            if(!temp->mDOMNode.isNull() && (DOMUtils::numTextChildren(temp->mDOMNode) /*|| temp->mDOMNode.tagName()=="IMG"*/ || temp->mDOMNode.styleProperty("background-image", QWebElement::ComputedStyle)!="none"))
				bentoBlock->mSameSizeContent = true;
			
			bentoBlock->mChildren.remove(i);
			
			const_cast<Bento *>(this)->mNumRedundant++;
			
			for(int j=0; j<temp->mChildren.size(); j++) { temp->mChildren[j]->mParent = bentoBlock; bentoBlock->mChildren.append(temp->mChildren[j]);}
			i--;
		}
	}
	qStableSort(bentoBlock->mChildren.begin(), bentoBlock->mChildren.end(), BentoBlock::topBottomLeftRight);
	for (int i=0; i<bentoBlock->mChildren.size(); i++) removeSameSizeChild(bentoBlock->mChildren[i]);
}
//#####################################################################
// Function canvasTagPass
//#####################################################################
void Bento::canvasTagPass(BentoBlock* bentoBlock) const
{	
	bool allCanvasChildren = true;
	for (int i=0; i<bentoBlock->mChildren.size(); i++) 		
		if (bentoBlock->mChildren[i]->mDOMNode.tagName()!="CANVAS") { allCanvasChildren=false; break; }
	
	if (allCanvasChildren) { bentoBlock->mChildren.clear(); }
	else for (int i=0; i<bentoBlock->mChildren.size(); i++) canvasTagPass(bentoBlock->mChildren[i]);
}
//#####################################################################
// Function allDOMNodes
//#####################################################################
void Bento::allDOMNodes(BentoBlock* bentoBlock)
{
	QWebElement child = bentoBlock->mDOMNode.firstChild();

	while (!child.isNull())
	{
		BentoBlock* childBlock = new BentoBlock(child);
		childBlock->mParent = bentoBlock;
		bentoBlock->mChildren.append(childBlock);
		allDOMNodes(childBlock);
		
		child = child.nextSibling();
	}
}

//#####################################################################
// Function setChildID
// Set the index under each parent
//#####################################################################
void Bento::setChildID(BentoBlock* bentoBlock)
{
    for (int i=0; i<bentoBlock->mChildren.size(); i++) {
        bentoBlock->mChildren[i]->mChildID = i;
        setChildID(bentoBlock->mChildren[i]);
    }
}
//void Bento::RecomputeBentoTree(BentoTree& bentoTree)
//{


//}
//void Bento::SetHtml(QString* html)
//{
//    mBrowserDocument.webFrame()->setHtml(&html);
//}
