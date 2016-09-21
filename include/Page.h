//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#ifndef _PAGE_H_
#define _PAGE_H_

#include <QList>
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#include "BentoTree.h"

namespace bricolage {

class BentoBlock; //why??
    
//#####################################################################
// Class Page
//#####################################################################
class Page
{
    
public:
    uint mPageID;
    QString mURL;
    QList<QWebElement> mDOMNodes;
    BentoTree* mBentoTree;
    QVector<QColor> mColor;

    QHash<QString, QVector<QString> > ComputedStyleList;
	
public:
    inline bool pRand(double probability=0.5){ return (double)rand()/RAND_MAX < probability;}//Move to a common function file?

    QList<QString> getKeys(){
        return ComputedStyleList.keys();
    }
    QList<QString> getKeys(QList<QString> keys){
        if(keys.empty()) return ComputedStyleList.keys();
        QList<QString> keyOut;
        foreach (QString key, keys) {
            if(ComputedStyleList.keys().contains(key)){
                keyOut.append(key);
            }
        }
        return keyOut;
    }
    //Because we now using sorted list it dont make sense to swap
//    void swapStyle(QString key){
//        int size= ComputedStyleList[key].size();
//        if(size > 1){
//            int a = qrand()%size;
//            int b = qrand()%size;
//            if(a!=b) swapStyle(key,a,b);
//        }
//    }
//    void swapStyle(QString key,int a,int b){
//         std::swap(ComputedStyleList[key][a],ComputedStyleList[key][b]);
//    }


    //Temp
    void printList(const QWebElement& domNode,int g_x);

    inline void updateStyleList(){
        ComputedStyleList.clear();
        updateStyleList(mBentoTree->mRootBlock);
        updateStyleList(mBentoTree->mRootBlock);
    }

    inline void updateStyles(){updateStyles(mBentoTree->mRootBlock);}
    void updateStyleList(BentoBlock* bentoBlock);
    void updateStyles(BentoBlock* bentoBlock);

    void updatePage(Page &newPage);
    void updatePage(Page &newPage, QWebElement newpart, int location);//for the copy of pages!
    QString getHtml(){return webpageP->mainFrame()->toHtml();}
    int addStyles(QString styleValue, QString key);

    Page(QWebPage& webPage=*new QWebPage, int pageID=0, QString url="");
    ~Page() { delete mBentoTree; }
    QWebPage* webpageP;





private:
    //for the copy of pages!
    void copyPage(BentoBlock* copyBlock,BentoBlock* orgBlock);

    void setDOMNodes(const QWebElement& domNode);

    QList<double> getNumberFromQString(const QString &str){ //Move to a common file..
        QList<double> list;
        int pos = 0;
        QRegExp rx("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");
        while ((pos = rx.indexIn(str, pos)) != -1) {
            list << rx.cap(1).toDouble();
            pos += rx.matchedLength();
        }
        return list;
    }


//#####################################################################
};

//#####################################################################
} // namespace bricolage.database




#endif // _PAGE_H_



