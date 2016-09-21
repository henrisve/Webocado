//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#ifndef _BLOCK_EXTRACTOR_H_
#define _BLOCK_EXTRACTOR_H_

#include <QSet>
#include <QVarLengthArray>
#include <QWebElement>

namespace bricolage{

class BentoBlock;

//#####################################################################
// Class BlockExtractor
//#####################################################################
class BlockExtractor
{
public:
	QSet<BentoBlock*> mBlockPool;
        BentoBlock* mRootBlock; //test, should be protected
protected:
	enum {DIVIDE, NOT_DIVIDE, MOVE_ON, DONE};


public:	
	~BlockExtractor()
	{}

public:
	BlockExtractor(BentoBlock* rootBlock);

protected:
	void divideDOMTree(QWebElement& domNode);
	int isDividable(QWebElement& domNode);
	
	int ruleIsRootBlock(const QWebElement& domNode) const;
	int ruleIsHiddenOverflow(QWebElement& domNode) const;
	int ruleBackgroundImage(QWebElement& domNode) const;
	int ruleTextChildren(QWebElement& domNode) const;
	int ruleNoValidChildren(QWebElement& domNode) const;
	int ruleOneValidChild(QWebElement& domNode) const;
	int ruleNoChildrenInBBox(QWebElement& domNode) const;
	int ruleAllChildrenVTextNodes(QWebElement& domNode) const;
	int ruleLineBreakChild(const QWebElement& domNode) const;
	int ruleHRChild(const QWebElement& domNode) const;
	int ruleDiffBackgroundColorChild(const QWebElement& domNode);
	int ruleSameSizeAsRoot(QWebElement& domNode) const;
	int ruleSiblingNotDivided(const QWebElement& domNode) const;
	int ruleNotDivide() const;

//#####################################################################
};
	
//#####################################################################
} // namespace bricolage

#endif //  _BLOCK_EXTRACTOR_H_
