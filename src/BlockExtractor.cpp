//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#include <algorithm>
#include <QDebug>
#include "BlockExtractor.h"
#include "BentoBlock.h"
using namespace bricolage;
//#####################################################################
// BlockExtractor
//#####################################################################
BlockExtractor::BlockExtractor(BentoBlock* rootBlock)
:mRootBlock(rootBlock)
{
	divideDOMTree(mRootBlock->mDOMNode);
}
//#####################################################################
// Function divideDOMTree
//#####################################################################
void BlockExtractor::divideDOMTree(QWebElement& domNode)
{
    //qDebug() << domNode.tagName() << " " << domNode.attribute("id") << " " << domNode.attribute("class");
	int result = isDividable(domNode);
	if (result == DIVIDE) {
		QWebElement child = domNode.firstChild();
		while(!child.isNull()) { divideDOMTree(child); child = child.nextSibling(); }
	}
	else if (result == NOT_DIVIDE)
		mBlockPool.insert(new BentoBlock(domNode)); 
}
//#####################################################################
// Function isDividable
//#####################################################################
int BlockExtractor::isDividable(QWebElement& domNode)
{	
	if (!DOMUtils::isValidNode(domNode)) return DONE;
	
	int result; QString tag = domNode.tagName();

	
    //if(tag=="IMG") return DIVIDE;  //TODO, did I do this??

	result=ruleIsRootBlock(domNode); if (result!=MOVE_ON) return result;
	result=ruleIsHiddenOverflow(domNode); if (result!=MOVE_ON) return result;
    result=ruleBackgroundImage(domNode); if (result!=MOVE_ON) return result;    // try uncomment this latel, should be there?
	result=ruleTextChildren(domNode); if (result!=MOVE_ON) return result;

	if(tag=="TABLE" || tag=="TR") {		
		result=ruleNoValidChildren(domNode); if (result!=MOVE_ON) return result;
		result=ruleOneValidChild(domNode); if (result!=MOVE_ON) return result;
		result=ruleNoChildrenInBBox(domNode); if (result!=MOVE_ON) return result;
		result=ruleDiffBackgroundColorChild(domNode); if (result!=MOVE_ON) return result;
		result=ruleSameSizeAsRoot(domNode); if (result!=MOVE_ON) return result;
	}
	else if(tag=="TD") {
		result=ruleNoValidChildren(domNode); if (result!=MOVE_ON) return result;
		result=ruleOneValidChild(domNode); if (result!=MOVE_ON) return result;
		result=ruleNoChildrenInBBox(domNode); if (result!=MOVE_ON) return result;
		result=ruleAllChildrenVTextNodes(domNode); if (result!=MOVE_ON) return result;
		result=ruleSameSizeAsRoot(domNode); if (result!=MOVE_ON) return result;
		result=ruleSiblingNotDivided(domNode); if (result!=MOVE_ON) return result;
	}
    else if(tag!="IMG" && tag!="IFRAME" && tag!="AREA" && tag!="INPUT" && tag!="SELECT" && tag!="CANVAS") {
		result=ruleNoValidChildren(domNode); if (result!=MOVE_ON) return result;
		result=ruleOneValidChild(domNode); if (result!=MOVE_ON) return result;
		result=ruleNoChildrenInBBox(domNode); if (result!=MOVE_ON) return result;
		result=ruleAllChildrenVTextNodes(domNode); if (result!=MOVE_ON) return result;
        if (DOMUtils::isInlineNode(domNode) || tag == "P") {
            result=ruleLineBreakChild(domNode); if (result!=MOVE_ON) return result; }
		result=ruleHRChild(domNode); if (result!=MOVE_ON) return result;
		result=ruleSameSizeAsRoot(domNode); if (result!=MOVE_ON) return result;
	}
	
	return ruleNotDivide();
}
//#####################################################################
// Function ruleIsRootBlock
//#####################################################################
int BlockExtractor::ruleIsRootBlock(const QWebElement& domNode) const
{
	if(domNode == mRootBlock->mDOMNode) { 
        //qDebug() << "DIVIDE (ruleIsRootBlock)";
		return DIVIDE; 
	}
	return MOVE_ON;
}
//#####################################################################
// Function ruleIsHiddenOverflow
//#####################################################################
int BlockExtractor::ruleIsHiddenOverflow(QWebElement& domNode) const
{	
	if(DOMUtils::isHiddenOverflow(domNode)) { 
        //qDebug() << "DIVIDE (ruleIsHiddenOverflow)";
		return DIVIDE; 
	}
	return MOVE_ON;
}
//#####################################################################
// Function ruleBackgroundImage
//#####################################################################
int BlockExtractor::ruleBackgroundImage(QWebElement& domNode) const
{
	if(domNode.styleProperty("background-image", QWebElement::ComputedStyle)!="none") {
		QRect bBox = DOMUtils::getGeometry(domNode);
        if (DOMUtils::hasValidDimensions(bBox)) { /*qDebug() << "NOT_DIVIDE (ruleBackgroundImage)";*/ return NOT_DIVIDE; }
        else { /*qDebug() << "DIVIDE (ruleBackgroundImage)";*/ return DIVIDE; }
	}
	return MOVE_ON;
}
//#####################################################################
// Function ruleHasTextChildren
//#####################################################################
int BlockExtractor::ruleTextChildren(QWebElement& domNode) const
{
	if(DOMUtils::numTextChildren(domNode)>0) {
		if (DOMUtils::hasOnlyTextChildren(domNode)) {
			QRect bBox = DOMUtils::getGeometry(domNode);
			int left = DOMUtils::parsePixelFeature(domNode.styleProperty("text-indent", QWebElement::ComputedStyle)) + bBox.left() + bBox.width();
            if (left<0) { /*qDebug() << "DONE (ruleTextChildren)";*/ return DONE; }
		}
        /*qDebug() << "NOT_DIVIDE (ruleTextChildren)";*/ return NOT_DIVIDE;
	}
 	return MOVE_ON;
}
//#####################################################################
// Function ruleNoValidChildren
//#####################################################################
int BlockExtractor::ruleNoValidChildren(QWebElement& domNode) const
{
	if(DOMUtils::numTextChildren(domNode) > 0) return MOVE_ON;
	QWebElement child = domNode.firstChild();
	while(!child.isNull()) {
		if(DOMUtils::isValidNode(child)) return MOVE_ON;
		child = child.nextSibling(); 
	}
    //qDebug() << "DONE (ruleNoValidChildren)";
	return DONE;
}
//#####################################################################
// Function ruleOneValidChild
//#####################################################################
int BlockExtractor::ruleOneValidChild(QWebElement& domNode) const
{
	if(DOMUtils::numTextChildren(domNode) > 0) return MOVE_ON;
	uint validChildCount = 0;
	QWebElement child = domNode.firstChild();
	while(!child.isNull()) {
		if(DOMUtils::isValidNode(child)) validChildCount++; 
		child = child.nextSibling(); 
	}
	if(validChildCount==1) { 
        //qDebug() << "DIVIDE (ruleOneValidChild)";
		return DIVIDE; 
	}
	return MOVE_ON;
}
//#####################################################################
// Function ruleNoChildrenInBBox
//#####################################################################
int BlockExtractor::ruleNoChildrenInBBox(QWebElement& domNode) const
{	
	QRect bBox = DOMUtils::getGeometry(domNode);
	if(bBox.width()*bBox.height()>0) {
		if(DOMUtils::numTextChildren(domNode)>0) return MOVE_ON;
		bBox.setRect(bBox.left()-1, bBox.top()-1, bBox.width()+2, bBox.height()+2);
		
		QVector<QWebElement> descList;
		QWebElement child = domNode.firstChild();
		while (!child.isNull()) { 
			if(DOMUtils::isValidNode(child)) descList.append(child); 
			child = child.nextSibling();
		}
				
		while (!descList.isEmpty()) {
			QWebElement currChild = descList.first(); descList.pop_front();
			QRect childBBox = DOMUtils::getGeometry(currChild);
			if (childBBox.width()*childBBox.height()==0) {
				child = currChild.firstChild();
				if (!child.isNull() && DOMUtils::isValidNode(child)) { descList.append(child); child = child.nextSibling(); }
			}
			else if(bBox.contains(childBBox)) return MOVE_ON;
		}
	}
    //qDebug() << "DIVIDE (ruleNoChildrenInBBox)";
	return DIVIDE;
}
//#####################################################################
// Function ruleAllChildrenVTextNodes
//#####################################################################
int BlockExtractor::ruleAllChildrenVTextNodes(QWebElement& domNode) const
{	
	QWebElement child = domNode.firstChild();
	if (!child.isNull()) {
		if (!DOMUtils::isVirtualTextNode(child)) return MOVE_ON;
		child = child.nextSibling();
	}
    //qDebug() << "NOT_DIVIDE (ruleAllChildrenVTextNodes)";
	return NOT_DIVIDE;
}
//#####################################################################
// Function ruleLineBreakChild
//#####################################################################
int BlockExtractor::ruleLineBreakChild(const QWebElement& domNode) const
{
	QWebElement child = domNode.firstChild();
	if (!child.isNull()) {
		if (DOMUtils::isLineBreakNode(child)) { 
            //qDebug() << "DIVIDE (ruleLineBreakChild)";
			return DIVIDE; 
		}
		child = child.nextSibling();}
	return MOVE_ON;
}
//#####################################################################
// Function ruleHRChild
//#####################################################################
int BlockExtractor::ruleHRChild(const QWebElement& domNode) const
{
	QWebElement child = domNode.firstChild();
	if (!child.isNull()) {
		if (child.tagName()=="HR") { 
            //qDebug() << "DIVIDE (ruleHRChild)";
			return DIVIDE; 
		}
	child = child.nextSibling(); }
	return MOVE_ON;
}
//#####################################################################
// Function ruleDiffBackgroundColorChild
//#####################################################################
int BlockExtractor::ruleDiffBackgroundColorChild(const QWebElement& domNode)
{	QColor parentColor, childColor;
	parentColor = DOMUtils::parseColorFeature(domNode.styleProperty("background-color", QWebElement::ComputedStyle));
	QSet<QWebElement*> differingChildren;
	QWebElement child = domNode.firstChild();
	while(!child.isNull()) {
		childColor = DOMUtils::parseColorFeature(child.styleProperty("background-color", QWebElement::ComputedStyle));
		if (parentColor!=childColor) differingChildren.insert(&child);
		child = child.nextSibling();
	}
	if (differingChildren.size()==0) return MOVE_ON;
	
	child = domNode.firstChild();
	while(!child.isNull()) {
		if(differingChildren.contains(&child)) mBlockPool.insert(new BentoBlock(child));
		else divideDOMTree(child);
		child = child.nextSibling();
	}
    //qDebug() << "NOT_DIVIDED " << QString::number(differingChildren.size()) << " (ruleDiffBackgroundColorChild)";
	return DONE;
}
//#####################################################################
// Function ruleSameSizeAsRoot
//#####################################################################
int BlockExtractor::ruleSameSizeAsRoot(QWebElement& domNode) const
{
	QRect domRect = DOMUtils::getGeometry(domNode);
	float domArea = domRect.width()*domRect.height();
	float rootArea = mRootBlock->mGeometry.width()*mRootBlock->mGeometry.height();
    if (domArea/(1.0*rootArea)>0.95/*0.85*/) {
        //qDebug() << "DIVIDE (SameSizeAsRoot)";
		return DIVIDE; 
	}
	return MOVE_ON;
}
//#####################################################################
// Function ruleSiblingNotDivided
//#####################################################################
int BlockExtractor::ruleSiblingNotDivided(const QWebElement& domNode) const
{ 
	foreach (BentoBlock* bentoBlock, mBlockPool)
		if(domNode.previousSibling()==bentoBlock->mDOMNode) { 
            //qDebug() << "NOT_DIVIDE (ruleSiblingNotDivided)";
			return NOT_DIVIDE; 
		}
	return MOVE_ON;
}
//#####################################################################
// Function ruleNotDivide
//#####################################################################
int BlockExtractor::ruleNotDivide() const
{
    //qDebug() << "NOT_DIVIDE (ruleNotDivide)";
	return NOT_DIVIDE;
}
