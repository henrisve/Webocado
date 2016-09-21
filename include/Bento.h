//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#ifndef _BENTO_H_
#define _BENTO_H_

#include <QVarLengthArray>
#include <QVector>
#include <QWebElement>
#include "DOMUtils.h"
#include "Separator.h"

namespace bricolage{

class BentoTree;
class Page;
//#####################################################################
// Class Bento
//#####################################################################
class Bento
{
public:
	QVector<Separator> mSeparatorList;
	QVector<QString> mNoChildrenTags, mNoSameTagChildren;

protected:
	QWebElement mBrowserDocument;


public:
	Bento() : mNumMoves(0), mNumRedundant(0)
	{}
	
	void init(QWebElement browserDocument) {
		mBrowserDocument=browserDocument; 
                mNoChildrenTags << "INPUT" << "OPTION" << "OPTGROUP" << "IMG" << "AREA" << "PARAM" << "BR" << "HR" << "PRE" << "CODE" << "TEXTAREA" << "SELECT" << "IFRAME" << "NOFRAME" << "STYLE" << "SCRIPT"/* << "LINK"*/;
                mNoSameTagChildren /*<< "P" << "FORM" << "H1" << "H2" << "H3" << "H4" << "H5" << "H6" << "STRONG" << "B" << "I"*/ << "SMALL" << "DEL" << "INS" << "OBJECT" << "FIELDSET" << "LABEL";
	}
	
public:
        void computeBentoTree(BentoTree& bentoTree);

protected:	
	void blockExtractionPass(BentoBlock* bentoBlock);
	void reStructure(BentoBlock* bentoBlock, const BentoTree& bentoTree);
	void atLeastTwoChildren(BentoBlock* bentoBlock) const;
	void removeSameSizeChild(BentoBlock* bentoBlock) const;
	void canvasTagPass(BentoBlock* bentoBlock) const;
	bool divideFurther(BentoBlock* bentoBlock) const;
	bool parentTaboo(BentoBlock* bentoBlock, BentoBlock* potParent) const;
	bool contains(BentoBlock* bentoBlock, BentoBlock* potParent) const;
	void allDOMNodes(BentoBlock* bentoBlock);
        void setChildID(BentoBlock* bentoBlock);

public:
	int mNumMoves;
	int mNumRedundant;
	
//#####################################################################
};

//#####################################################################
} // namespace bricolage

#endif // _BENTO_H_
