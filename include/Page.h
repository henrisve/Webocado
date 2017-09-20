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
#include <QSet>

namespace bricolage {

class BentoBlock; //why??
    
//#####################################################################
// Class Page
//#####################################################################
class Page
{
    
public:
    //public variables
    QString pID; //uniqe id for each page, so we can track the pages.
    BentoTree* mBentoTree;
    QVector<QColor> mColor;
    QVector<QVector<QVector<QVector<int> > > > histogram; //X,Y,rgb,bin
    uint mPageID; //Used as IslandID.
    QHash<QString, QVector<QString> > ComputedStyleList;
    QHash<QString, QVector<QString> > ComputedStyleListBackup;
    QSet<QString> updatedKeys;
    QList<double> distance;
    QList<double> distanceMulti;
    QWebPage* webpageP;
    QList<QWebElement> mDOMNodes;


    //public functions
    ~Page() { delete mBentoTree; }
    QString getHtml(){return webpageP->mainFrame()->toHtml();}
    Page(QWebPage& webPage=*new QWebPage, int pageID=0, QString url="", int ind=-1, QString dateT="na");


    void setUpdatedKeys(QString key);
    QSet<QString> getUpdatedKeys();

    inline QList<QString> getKeys(){
        return ComputedStyleList.keys();
    }
    inline void updateStylesANDList(){
        //updateStyles(mBentoTree->mRootBlock);
        ComputedStyleListBackup=ComputedStyleList; //Pretty ugly
        if(pRand()){ //Todo: add parameter for this
            ComputedStyleList.clear();
        }

        updateStylesANDList(mBentoTree->mRootBlock);
    }

    inline void updateStyleList(bool init=false){
        if(!updatedKeys.isEmpty() || init){
             //Todo: add parameter for this
            foreach(QString key, updatedKeys)
                if(pRand()) ComputedStyleList.remove(key);

            //need to be called twice due to things change
            //during the first call.. Can probably be done in a nicer way.

            updateStyleList2(mBentoTree->mRootBlock,init);
            updateStyleList2(mBentoTree->mRootBlock,init);
        }
    }

    inline void updateStyles(){
        if(!getUpdatedKeys().isEmpty()){
            updateStyles(mBentoTree->mRootBlock);
        }
    }

    void updatePage(Page &newPage);
    void updatePage(Page &newPage, QWebElement newpart, int location);//for the copy of pages!

    bool saveImage();
    void createHistogram();
    void buildColorList();
    void updateColor();
    void mutateElement(){
        //mBentoTree->mRootBlock.
    }

private:

    inline bool pRand(double probability=0.5){ return (double)rand()/RAND_MAX < probability;}//Move to a common function file


    inline QList<QString> getKeys(QList<QString> keys){
        if(keys.empty()) return ComputedStyleList.keys();
        QList<QString> keyOut;
        foreach (QString key, keys) {
            if(ComputedStyleList.keys().contains(key)){
                keyOut.append(key);
            }
        }
        return keyOut;
    }

    //Temp
    void printList(const QWebElement& domNode,int g_x);




    int addStyles(QString styleValue, QString key);
    //Private variablen
    double fitness;
    double diffFitness;  /*Not used yet, but the idea is to also use
                           the diff on how the fitness changes for each page*/
    QImage image; //screenshot of the page
    QString dateTime;
    //
    void updateStylesANDList(BentoBlock* bentoBlock);
    void updateStyleList2(BentoBlock* bentoBlock, bool init=false);
    void updateStyles(BentoBlock* bentoBlock);


    QString mURL;



    //private Functions

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



