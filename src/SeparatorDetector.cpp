//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#include "DOMUtils.h"
#include "SeparatorDetector.h"
#include "BentoBlock.h"
using namespace bricolage;
//#####################################################################
// Function SeparatorDetector
//#####################################################################
SeparatorDetector::SeparatorDetector(const QWebElement browserDocument, const QSet<BentoBlock*>& blockPool, int baseWeight)
:mBrowserDocument(browserDocument), mBlockPool(blockPool), mBaseWeight(baseWeight)
{ 
	mBoundingBlock = BentoBlock::boundingRectangle(mBlockPool);	
	mHorizontalSeparators.append(Separator(mBoundingBlock.left(), mBoundingBlock.top(), mBoundingBlock.width(), mBoundingBlock.height(), baseWeight)); 	computeSeparators(HORIZONTAL);
	mVerticalSeparators.append(Separator(mBoundingBlock.left(), mBoundingBlock.top(), mBoundingBlock.width(), mBoundingBlock.height(), mBaseWeight)); computeSeparators(VERTICAL);

	removeBorders();
	addBlocksToSeparators();
	computeSeparatorWeights();
	subdivideBlockPool();

	if (subBlockPools.size() > 1) 
		for (int i=0; i<subBlockPools.size(); i++)
			if (subBlockPools[i].size() > 1) { 
				SeparatorDetector subDetector(mBrowserDocument, subBlockPools[i], mBaseWeight-3); 
				mHorizontalSeparators += subDetector.mHorizontalSeparators;
				mVerticalSeparators += subDetector.mVerticalSeparators;
			}
	
	mSeparators = mHorizontalSeparators + mVerticalSeparators;
}
//#####################################################################
// Function computeSeparators
//#####################################################################
void SeparatorDetector::computeSeparators(int orientation)
{
	QVector<Separator>& separatorList=(orientation==HORIZONTAL)?mHorizontalSeparators:mVerticalSeparators;
	foreach (BentoBlock* bentoBlock, mBlockPool) {
		int j=0; int separatorListSize=separatorList.size();
		while(j<separatorListSize) {
			int relation = separatorBlockRelation(separatorList[j], bentoBlock, orientation);
			switch (relation) {
				case CONTAINED: split(j, bentoBlock, orientation); break;
				case CROSSING1:
				case CROSSING2: crossingUpdate(j, bentoBlock, orientation, relation); break;
				case COVERING: 
						removeSeparator(j, orientation); 
					separatorListSize--; j--; break;
				default: break;
			}
			j++;
		}
	}	
}	
//#####################################################################
// Function separatorBlockRelation
//#####################################################################
int SeparatorDetector::separatorBlockRelation(const Separator& separator, const BentoBlock* bentoBlock, int orientation) const 
{
	int relation = DISTANT;
	int separatorTopLeft = (orientation==HORIZONTAL)?separator.top():separator.left(); 
	int separatorBottomRight = (orientation==HORIZONTAL)?separator.bottom():separator.right();
	int blockTopLeft = (orientation==HORIZONTAL)?bentoBlock->mGeometry.top():bentoBlock->mGeometry.left();
	int blockBottomRight = (orientation==HORIZONTAL)?bentoBlock->mGeometry.bottom():bentoBlock->mGeometry.right();
	
	if((separatorTopLeft <= blockTopLeft) && (separatorBottomRight >= blockBottomRight)) relation = CONTAINED;
	else if((separatorTopLeft <= blockTopLeft) && (separatorBottomRight >= blockTopLeft) &&
			(separatorBottomRight < blockBottomRight)) relation = CROSSING1;
	else if((separatorTopLeft > blockTopLeft) && (separatorTopLeft <= blockBottomRight) &&
			(separatorBottomRight >= blockBottomRight)) relation = CROSSING2;
	else if((separatorTopLeft >= blockTopLeft) && (separatorBottomRight <= blockBottomRight)) relation = COVERING;

	return relation;
}
//#####################################################################
// Function split
//#####################################################################
void SeparatorDetector::split(uint separatorIndex, BentoBlock* bentoBlock, int orientation)
{
	if(orientation==HORIZONTAL) {
		mHorizontalSeparators.append(Separator(mHorizontalSeparators[separatorIndex].left(), bentoBlock->mGeometry.bottom(), 
											   mHorizontalSeparators[separatorIndex].width(), mHorizontalSeparators[separatorIndex].bottom()-bentoBlock->mGeometry.bottom()+1, mBaseWeight));
		mHorizontalSeparators[separatorIndex].setBottom(bentoBlock->mGeometry.top());
	}
	else {
		mVerticalSeparators.append(Separator(bentoBlock->mGeometry.right(), mVerticalSeparators[separatorIndex].top(), 
											 mVerticalSeparators[separatorIndex].right()-bentoBlock->mGeometry.right()+1, mVerticalSeparators[separatorIndex].height(), mBaseWeight));
		mVerticalSeparators[separatorIndex].setRight(bentoBlock->mGeometry.left());
	}
}
//#####################################################################
// Function crossingUpdate
//#####################################################################
void SeparatorDetector::crossingUpdate(uint separatorIndex, BentoBlock* bentoBlock, int orientation, int relation)
{
	if (orientation==HORIZONTAL && relation==CROSSING1) mHorizontalSeparators[separatorIndex].setBottom(bentoBlock->mGeometry.top());
	else if (orientation==HORIZONTAL) mHorizontalSeparators[separatorIndex].setTop(bentoBlock->mGeometry.bottom());
	else if (relation==CROSSING1) mVerticalSeparators[separatorIndex].setRight(bentoBlock->mGeometry.left());
	else mVerticalSeparators[separatorIndex].setLeft(bentoBlock->mGeometry.right());
}
//#####################################################################
// Function removeSeparator
//#####################################################################
void SeparatorDetector::removeSeparator(uint separatorIndex, int orientation)
{
	QVector<Separator>& separatorList=(orientation==HORIZONTAL)?mHorizontalSeparators:mVerticalSeparators;
	separatorList.remove(separatorIndex);
}
//#####################################################################
// Function removeSeparator
//#####################################################################
void SeparatorDetector::removeBorders()
{
	for (int i=0; i<mHorizontalSeparators.size(); i++)
		if ((mHorizontalSeparators[i].top() == mBoundingBlock.top()) ||
			(mHorizontalSeparators[i].bottom() == mBoundingBlock.bottom()))
			{ removeSeparator(i, HORIZONTAL); i--; }
	for (int i=0; i<mVerticalSeparators.size(); i++)
		if ((mVerticalSeparators[i].left() == mBoundingBlock.left()) ||
			(mVerticalSeparators[i].right() == mBoundingBlock.right()))		
			{ removeSeparator(i, VERTICAL); i--; }
}
//#####################################################################
// Function addBlocksToSeparators
//#####################################################################
void SeparatorDetector::addBlocksToSeparators()
{
	qStableSort(mHorizontalSeparators.begin(), mHorizontalSeparators.end(), Separator::lessThanTop);
	foreach(BentoBlock* bentoBlock, mBlockPool)
	for(int i=0; i<mHorizontalSeparators.size(); i++)
		if(bentoBlock->mGeometry.bottom() <= mHorizontalSeparators[i].top()) {mHorizontalSeparators[i].mBlocks.insert(bentoBlock); break;}
	
	qStableSort(mHorizontalSeparators.begin(), mHorizontalSeparators.end(), Separator::greaterThanBottom);
	foreach(BentoBlock* bentoBlock, mBlockPool)
	for(int i=0; i<mHorizontalSeparators.size(); i++)
		if(bentoBlock->mGeometry.top() >= mHorizontalSeparators[i].bottom()) {mHorizontalSeparators[i].mBlocks.insert(bentoBlock); break;}
	
	qStableSort(mVerticalSeparators.begin(), mVerticalSeparators.end(), Separator::lessThanLeft);
	foreach(BentoBlock* bentoBlock, mBlockPool)
	for(int i=0; i<mVerticalSeparators.size(); i++)
		if(bentoBlock->mGeometry.right() <= mVerticalSeparators[i].left()) {mVerticalSeparators[i].mBlocks.insert(bentoBlock); break;}
	
	qStableSort(mVerticalSeparators.begin(), mVerticalSeparators.end(), Separator::greaterThanRight);
	foreach(BentoBlock* bentoBlock, mBlockPool)
	for(int i=0; i<mVerticalSeparators.size(); i++)
		if(bentoBlock->mGeometry.left() >= mVerticalSeparators[i].right()) {mVerticalSeparators[i].mBlocks.insert(bentoBlock); break;} 
	
}
//#####################################################################
// Function addBlocksToSeparators
//#####################################################################
void SeparatorDetector::computeSeparatorWeights()
{
	QWebElementCollection allHRNodes = mBrowserDocument.findAll("hr");
	for(int i=0; i<mHorizontalSeparators.size(); i++) {
		QList<BentoBlock*> blockList = mHorizontalSeparators[i].mBlocks.toList();
		qStableSort(blockList.begin(), blockList.end(), BentoBlock::topBottomLeftRight);

		for (int j=1; j<blockList.size(); j++) {
			
			if (blockList[j-1]->mGeometry.bottom() <= blockList[j]->mGeometry.top()) 
				if(DOMUtils::numTextChildren(blockList[j-1]->mDOMNode)>0 && DOMUtils::numTextChildren(blockList[j]->mDOMNode)>0
					&& DOMUtils::parsePixelFeature(blockList[j]->mDOMNode.styleProperty("font-size", QWebElement::ComputedStyle))>DOMUtils::parsePixelFeature(blockList[j-1]->mDOMNode.styleProperty("font-size", QWebElement::ComputedStyle))) {
					mHorizontalSeparators[i].mWeight++; break; 
				}
			foreach(QWebElement hrNode, allHRNodes) 
				if (mHorizontalSeparators[i].intersects(hrNode.geometry())) {
					mHorizontalSeparators[i].mWeight++; break; 
				}
		}
	}
}
//#####################################################################
// Function subdivideBlockPool
//#####################################################################
void SeparatorDetector::subdivideBlockPool()
{
	qStableSort(mHorizontalSeparators.begin(), mHorizontalSeparators.end(), Separator::lessThanBottom);
	qStableSort(mVerticalSeparators.begin(), mVerticalSeparators.end(), Separator::lessThanRight);
	int horizontalSeparatorWidth=0, verticalSeparatorHeight=0;
	if(mHorizontalSeparators.size()>0) horizontalSeparatorWidth=mHorizontalSeparators[0].width();
	if(mVerticalSeparators.size()>0) verticalSeparatorHeight=mVerticalSeparators[0].height();
	QVector<Separator>& separatorList = horizontalSeparatorWidth>=verticalSeparatorHeight ? mHorizontalSeparators : mVerticalSeparators;
	subBlockPools.resize(separatorList.size()+1);

	foreach (BentoBlock* bentoBlock, mBlockPool) 
		for(int i=0; i<=separatorList.size(); i++) 
			if (i==separatorList.size()
				|| (horizontalSeparatorWidth>=verticalSeparatorHeight && bentoBlock->mGeometry.bottom()<=separatorList[i].bottom())
				|| (horizontalSeparatorWidth<verticalSeparatorHeight && bentoBlock->mGeometry.right()<=separatorList[i].right()))
				{subBlockPools[i].insert(bentoBlock); break; }
	
}	