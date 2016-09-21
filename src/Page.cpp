//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#include "Page.h"
#include "Bento.h"
#include "BentoBlock.h"
//#include <QCoreApplication>
//class Window;
//#include <QDebug>
using namespace bricolage;

//#####################################################################
// Function Page
//#####################################################################
Page::Page(QWebPage& webPage, int pageID, QString url)
:mPageID(pageID),mURL(url),mBentoTree(new BentoTree())
{
    //qDebug() << url;
    webpageP = &webPage;
    setDOMNodes(webPage.mainFrame()->documentElement());
    Bento bento;

    bento.init(webPage.mainFrame()->documentElement());
    bento.computeBentoTree(*mBentoTree);

}
//#####################################################################
// Function setDOMNodes
//#####################################################################
void Page::setDOMNodes(const QWebElement& domNode) {
    QWebElement domChild = domNode.firstChild();
    while (!domChild.isNull()) {
        setDOMNodes(domChild);
        domChild = domChild.nextSibling();
    }
    mDOMNodes.append(domNode);
}
//#####################################################################
// Function UpdatePage  BentoBlock* bentoBlock
//#####################################################################
void Page::updatePage(Page &newPage,QWebElement newpart,int location){

   // newPage.webpageP->mainFrame()->documentElement().
}

void Page::updatePage(Page &newPage){

    webpageP->mainFrame()->setHtml(newPage.getHtml());

    int prevs = mDOMNodes.size(); //temp
    mDOMNodes.clear();//We migth need to wait here?

    setDOMNodes(webpageP->mainFrame()->documentElement());
    //Now all the DOMNodes are updated to the new one
    mBentoTree->mHeight=newPage.mBentoTree->mHeight;
    mBentoTree->mNumNodes=newPage.mBentoTree->mNumNodes;
    mColor=newPage.mColor;
    mPageID=newPage.mPageID;
    mURL=newPage.mURL;
    ComputedStyleList=newPage.ComputedStyleList;

    copyPage(mBentoTree->mRootBlock,newPage.mBentoTree->mRootBlock);

}

//should be under BentoBlock.h (or .cpp)??
void Page::copyPage(BentoBlock* copyBlock,BentoBlock* orgBlock){

    copyBlock->mBentoID=orgBlock->mBentoID; //This is probably pointers, soo maybe need the data??
    copyBlock->mbgColor=orgBlock->mbgColor;
    copyBlock->mborderColor=orgBlock->mborderColor;
    copyBlock->mChildID=orgBlock->mChildID;
    copyBlock->mDomNodeID=orgBlock->mDomNodeID;
    copyBlock->mGeometry=orgBlock->mGeometry;
    copyBlock->mLevel=orgBlock->mLevel;
    copyBlock->mlinkActiveColor=orgBlock->mlinkActiveColor;
    copyBlock->mlinkColor=orgBlock->mlinkColor;
    copyBlock->mlinkHoverColor=orgBlock->mlinkHoverColor;
    copyBlock->mlinkVisitColor=orgBlock->mlinkVisitColor;
    copyBlock->moutlineColor=orgBlock->moutlineColor;
    copyBlock->mSameSizeContent=orgBlock->mSameSizeContent;
    copyBlock->mtColor=orgBlock->mtColor;
    copyBlock->mComputedStyles=orgBlock->mComputedStyles;

    if(copyBlock->mDomNodeID>0){ // why
        copyBlock->mDOMNode = mDOMNodes[copyBlock->mDomNodeID];
    }


    copyBlock->mChildren.clear(); //should already be clear, except for maybe first time!

    for (int i=0; i<orgBlock->mChildren.size(); i++) {
        copyBlock->mChildren.append(new BentoBlock);
        copyBlock->mChildren[i]->mParent=copyBlock;

        copyPage(copyBlock->mChildren[i],orgBlock->mChildren[i]);
    }


//    qDebug()<< "-----------------------this is where --------------------------------" << orgBlock->mBentoID << this->mPageID;;

//    QList<QString> keys = ComputedStyleList.keys();
//    foreach (QString key, keys) {
//        qDebug() << endl<< "the key "<< key << "= ";
//        foreach(QString value, ComputedStyleList.value(key)){
//            qDebug() << value << ", ";
//        }
//    }
//    qDebug()<< "-------------------------------------------------------------------";
//    QList<QString> keys2 = orgBlock->mComputedStyles.keys();
//    foreach (QString key, keys2) {
//        qDebug() <<endl<< key << "is" << orgBlock->mComputedStyles.value(key) ;
//    }
//    qDebug()<< "-------------------------------------------------------------------";
//    QList<QString> keys3 = copyBlock->mComputedStyles.keys();
//    foreach (QString key, keys3) {
//        qDebug() <<endl<< key << "is" << copyBlock->mComputedStyles.value(key) ;
//    }
}



void Page::updateStyleList(BentoBlock* bentoBlock){
    QHash<QString, QString> ComputedStyles = bentoBlock->getStyles();
    QList<QString> keys = ComputedStyles.keys();
    //Qlist<QString> temp;
    foreach (QString key, keys) {
        QString styleValue = ComputedStyles.value(key);
        int index=addStyles(styleValue,key);

        int size=ComputedStyleList.value(key).size();
        bentoBlock->setStyles(index,key,size);
    }
    for(int i=0; i<bentoBlock->mChildren.size(); i++) {
        updateStyleList(bentoBlock->mChildren[i]);
    }
}
int Page::addStyles(QString styleValue, QString key){
    int index = -1;
    if(ComputedStyleList.contains(key)){
        index = ComputedStyleList.value(key).indexOf(styleValue);
    }
    if(index < 0){
        int size =ComputedStyleList.value(key).size();
        //
        //
        for(int i=0;i<size;i++){
            bool lesser;
            QList<double> al=getNumberFromQString(styleValue);
            QList<double> bl=getNumberFromQString(ComputedStyleList.value(key)[i]);

            if(!al.isEmpty() && !bl.isEmpty()){
                int a=getNumberFromQString(styleValue)[0];
                int b=getNumberFromQString(ComputedStyleList.value(key)[i])[0];
                lesser = a<=b;
            }else{
                lesser = styleValue<=ComputedStyleList.value(key)[i];
            }
            if(lesser){
                ComputedStyleList[key].insert(i,styleValue);
                return i;
            }
        }
        ComputedStyleList[key].append(styleValue);
        index=size;
    }
    return index;
    //QMap<QString, QVector<QString> > ComputedStyleList;
}

void Page::updateStyles(BentoBlock* bentoBlock){

        //somtings wronh
    bentoBlock->updateStyles(&ComputedStyleList);

    for(int i=0; i<bentoBlock->mChildren.size(); i++) {
        updateStyles(bentoBlock->mChildren[i]);
    }
}

void Page::printList(const QWebElement& domNode,int g_x) {
    //for(uint j=0; j<g_x;j++) qDebug().nospace() << "     ";
    QWebElement domChild = domNode.firstChild();
    qDebug() << "{" << endl;
    g_x++;
    int qq=0;
    while (!domChild.isNull()) {
        qq++;
        for(uint j=0; j<g_x;j++) qDebug() << ".    ";
        qDebug() << "kid" << qq << "("<< domChild.tagName() << ")";
        printList(domChild,g_x);
        domChild = domChild.nextSibling();
    }
    g_x--;
    for(uint j=0; j<g_x;j++) qDebug() << ".    ";
    qDebug()   << "}"<< endl;
}

//void Page::crossOverBasic(){
////    if(!pRand(BasicCrossProba)) return;
////    foreach(QString key,getKeys()){
////        if(!pRand(BasicCrossProbaKey) || !ComputedStyleList.contains(key)) continue;
////        foreach (QString value, ComputedStyleList[key]) {

////        }
////    }

//    //ComputedStyleList

//}
