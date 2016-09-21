//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#ifndef _SEPARATOR_DETECTOR_H_
#define _SEPARATOR_DETECTOR_H_

#include <QRect>
#include <QSet>
#include <QVarLengthArray>
#include <QVector>
#include <QWebElement>
#include "Separator.h"

namespace bricolage{
	
class BentoBlock;

//#####################################################################
// Class SeparatorDetector
//#####################################################################
class SeparatorDetector
{
public:
	QVector<Separator> mHorizontalSeparators;
	QVector<Separator> mVerticalSeparators;
	QVector<Separator> mSeparators;
	
protected:
	enum {HORIZONTAL, VERTICAL, CONTAINED, CROSSING1, CROSSING2, COVERING, DISTANT};
	QWebElement mBrowserDocument;
	QRect mBoundingBlock;
	const QSet<BentoBlock*>& mBlockPool;
	int mBaseWeight;
	QVarLengthArray<QSet<BentoBlock*>, 10> subBlockPools;

public:
	SeparatorDetector(const QWebElement browserDocument, const QSet<BentoBlock*>& blockPool, int baseWeight=10); // TODO: REPLACE W/ A CONSTANT

protected:
	void computeSeparators(int orientation);
	int separatorBlockRelation(const Separator& separator, const BentoBlock* bentoBlock, int orientation) const;
	void split(uint separatorIndex, BentoBlock* bentoBlock, int orientation);
	void crossingUpdate(uint separatorIndex, BentoBlock* bentoBlock, int orientation, int relation);
	void removeSeparator(uint separatorIndex, int orientation);
	void removeBorders();
	void addBlocksToSeparators();
	void computeSeparatorWeights();
	void subdivideBlockPool();
	
//#####################################################################
};
	
//#####################################################################
} // namespace bricolage

#endif // _SEPARATOR_DETECTOR_H_
