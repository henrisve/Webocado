//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#ifndef _BENTO_BLOCK_H_
#define _BENTO_BLOCK_H_

#include <QRect>
#include <QSet>
#include <QVector>
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#include "DOMUtils.h"
#include <Page.h>


//TODO, MAKE A BentoBlock.cpp


namespace bricolage {
//class Page;
//#####################################################################
// Class BentoBlock
//#####################################################################
class BentoBlock{

private:

    
public:


    uint mBentoID;
    QWebElement mDOMNode;
    BentoBlock* mParent;
    QVector<BentoBlock*> mChildren;
    QRect mGeometry;
    uint mLevel;
    uint mChildID; //to keep track of sibling ID. todo, fix varning=0 // Keep this?
    int mDomNodeID; //maybe can remove
    //QVector<int> mOrgDomStructurePath;

    int mbgColor; //bg color, points to the page var "mColor"
    int mtColor;
    int mlinkColor;
    int mlinkVisitColor;
    int mlinkHoverColor;//TODOCOLOR
    int mlinkActiveColor;
    int mborderColor;
    int moutlineColor;

    //With these we can remowe the colors above!
    //QMap<QString, QString> mAttributes; // TODO: change to hash?
    QHash<QString, QPair<int, int> > mComputedStyles; // <key, <index, size> >

    //create a bentoblock.cpp?
    QHash<QString, QString> getStyles();
    void setStyles(int keyIndex, QString key,int size);


    void updateStyles(const QHash<QString, QVector<QString> >* styleList, QSet<QString> UpdatedKeys);
    void buildBlockColorList(QVector<QColor>* colorList);
    void addColor(int &colorBlock,QVector<QColor>* colorList, QColor newColor);
    int colorIndex(QVector<QColor>* colorList, QColor color);
    void updateBlockColor(QVector<QColor>* colorList);



    bool mSameSizeContent; // TODO: keep this?

	BentoBlock()
	:mParent(NULL)
	{}
	
        BentoBlock(QWebElement domNode)
        :mDOMNode(domNode),mParent(NULL),mGeometry(DOMUtils::getGeometry(domNode)),mSameSizeContent(false)
	{
		bool result;
		QRect clipRect = DOMUtils::isPartiallyHidden(domNode, result);
		if (result) mGeometry = clipRect;

	}
	
	BentoBlock(QSet<BentoBlock*>& blockPool)
	:mParent(NULL),mGeometry(boundingRectangle(blockPool)),mSameSizeContent(false)
	{}
	
	~BentoBlock(){}
	
	static QRect boundingRectangle(const QSet<BentoBlock*>& unionBlocks)
	{
		QRect bounding;
		foreach (BentoBlock* BentoBlock, unionBlocks) bounding = bounding.united(BentoBlock->mGeometry);
		return bounding;
	}
	
	static bool topBottomLeftRight(const BentoBlock* lhs, const BentoBlock* rhs) 
	{ if(lhs->mGeometry.top() == rhs->mGeometry.top()) return lhs->mGeometry.left() < rhs->mGeometry.left();
	else return lhs->mGeometry.top() < rhs->mGeometry.top();
	} 
	
	static bool leftRightTopBottom(const BentoBlock* lhs, const BentoBlock* rhs) 
	{ if(lhs->mGeometry.left() == rhs->mGeometry.left()) return lhs->mGeometry.top() < rhs->mGeometry.top();
	else return lhs->mGeometry.left() < rhs->mGeometry.left();
	} 
	
	static bool increasingArea(const BentoBlock* lhs, const BentoBlock* rhs) 
	{  
		return (lhs->mGeometry.width()*lhs->mGeometry.height()) < (rhs->mGeometry.width()*rhs->mGeometry.height());
	} 



        inline bool pRand(double probability=0.5){ return (double)rand()/RAND_MAX < probability;}//Move to a common function file


//#####################################################################
};


//#####################################################################
} // namespace bricolage

#endif // _BENTO_BLOCK_H_
