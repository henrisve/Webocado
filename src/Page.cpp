//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#include "Page.h"
#include "Bento.h"
#include "BentoBlock.h"
#include <QDir>
#include "qmath.h"
#include <qpainter.h>
//#include <QCoreApplication>
//class Window;
//#include <QDebug>
using namespace bricolage;
#include <QTime>
QTime myTimer3;

//#####################################################################
// Function Page
//#####################################################################
Page::Page(QWebPage& webPage, int pageID, QString url,int ind,QString dateT)
:mPageID(pageID),mURL(url),mBentoTree(new BentoTree()),dateTime(dateT)
{
    //qDebug() << url;
    pID = ("0_" + QString::number(ind));
    webpageP = &webPage;
    setDOMNodes(webPage.mainFrame()->documentElement());
    Bento bento;
    image = QImage(100,100,QImage::Format_ARGB32);

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

    webpageP->mainFrame()->setHtml(newPage.getHtml()); // This takes 50 ms!
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
    //if(orgBlock->mLevel == 0) qDebug() << "c0" << "this:" << orgBlock->mChildren.size() << endl;
   // if(orgBlock->mLevel == 0) qDebug() << "c0.5" << "this:" << copyBlock->mChildren.size() << endl;
   // if(orgBlock->mLevel == 0) qDebug() << "c1" << "this:" << mBentoTree->mRootBlock->mChildren.size() << endl;
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
    //if(orgBlock->mLevel == 0) qDebug() << "c2" << "this:" << mBentoTree->mRootBlock->mChildren.size() << endl;
    if(copyBlock->mDomNodeID>0){ // why
        if(copyBlock->mDomNodeID >= mDOMNodes.size()){
            copyBlock->mDOMNode = mDOMNodes.last();
            qDebug() << "Warning: mDOMNode too small, use last element. size:" << mDOMNodes.size() << " requested:" << copyBlock->mDomNodeID<< endl;
        }else{
            copyBlock->mDOMNode = mDOMNodes[copyBlock->mDomNodeID];
        }
    //    if(orgBlock->mLevel == 0) qDebug() << ".";
    }else{
    //    qDebug() << "*";
    }
    //if(orgBlock->mLevel == 0 && copyBlock == orgBlock)qDebug() << "ncitfu"<< endl;
    copyBlock->mChildren.clear(); //should already be clear, except for maybe first time!
    for (int i=0; i<orgBlock->mChildren.size(); i++) {
        copyBlock->mChildren.append(new BentoBlock);
        copyBlock->mChildren[i]->mParent=copyBlock;
        copyPage(copyBlock->mChildren[i],orgBlock->mChildren[i]);
    }
}

/*Note to self. it seemed that updating list and style at the same location did not
 * increase the speed. but instead added new problems.
 *  It migth be possible to solve somehow, but not now. so keeping it for now.
 *
 */
void Page::updateStylesANDList(BentoBlock* bentoBlock){
    myTimer3.start();
        //somtings wronh
    //bentoBlock->updateStyles(&ComputedStyleList);

    QHash<QString, QString> ComputedStyles = bentoBlock->getStyles();
    QList<QString> keys2 = ComputedStyles.keys();
    QList<QString> keys = bentoBlock->mComputedStyles.keys();
    foreach(QString key,keys2){
        if(!keys.contains(key)){
            keys.append(key);
            qDebug() << "keys: " << key << endl;
        }
    }
    //Can we maybe just take the keys that have been in the mutation part? rest shouldnt change right?
    //then we also need to find how to handle where we empty it corrertly
    qDebug() << "a" << myTimer3.elapsed();
    foreach (QString key, keys) {


        int kindex=bentoBlock->mComputedStyles.value(key).first;
        if(kindex > ComputedStyleListBackup.value(key).size()){
            qDebug() << endl << "stylelist too small, select last available instead";
            kindex = ComputedStyleListBackup.value(key).size()-1; //I dont know why this happens sometimes.. but very rare, so ignore it
        }
        int styleSize=ComputedStyleListBackup.value(key).size();
        if(kindex>=styleSize){
            kindex=(qrand()%styleSize);//this is not correct way, but I dont care
            qDebug() << "Warning: stylelist too small, new index:" << kindex << endl;
        }

        bentoBlock->mDOMNode.setStyleProperty(key,ComputedStyleListBackup.value(key).at(kindex) + " !important");



        QString styleValue = ComputedStyles.value(key);
        int index=addStyles(styleValue,key);

        int size=ComputedStyleList.value(key).size();
        bentoBlock->setStyles(index,key,size);

    }


    qDebug() << "b" << myTimer3.elapsed();

    for(int i=0; i<bentoBlock->mChildren.size(); i++) {
        updateStylesANDList(bentoBlock->mChildren[i]);
    }
}


void Page::updateStyleList2(BentoBlock* bentoBlock,bool init){
    QSet<QString> keys;

    QHash<QString, QString> ComputedStyles = bentoBlock->getStyles();

    if(init){
        QList<QString> tmp=ComputedStyles.keys();
        keys = QSet<QString>::fromList(tmp);
        //keys.fromList() /fromList(ComputedStyles.keys());
    }else{
        keys = getUpdatedKeys();
    }

    foreach (QString key, keys) {

        QString styleValue = ComputedStyles.value(key);
        int index=addStyles(styleValue,key);

        int size=ComputedStyleList.value(key).size();
        bentoBlock->setStyles(index,key,size);
    }
    for(int i=0; i<bentoBlock->mChildren.size(); i++) {
        updateStyleList2(bentoBlock->mChildren[i],init);
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
            QList<double> al=getNumberFromQString(styleValue); //todo:can be moved outside the loop?
            QList<double> bl=getNumberFromQString(ComputedStyleList.value(key)[i]);

            if(!al.isEmpty() && !bl.isEmpty()){
                int a=getNumberFromQString(styleValue)[0]; //todo:same as al[0] ???
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
}

void Page::setUpdatedKeys(QString key){
    updatedKeys.insert(key);
}
QSet<QString> Page::getUpdatedKeys(){
    return updatedKeys;
}


//This is a bit "wrong", see color, think that's better solution.
void Page::updateStyles(BentoBlock* bentoBlock){

    bentoBlock->updateStyles(&ComputedStyleList,getUpdatedKeys());

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
bool Page::saveImage(){
    QImage tmpimg(1000,1000,QImage::Format_ARGB32);
    QPainter p(&tmpimg);
    webpageP->setViewportSize(QSize(1000,1000));
    webpageP->mainFrame()->render(&p);
    p.end();
    //todo, dont know if smoothed is better or worse. try both
    image = tmpimg.scaled(1000,1000,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    QString path=QDir::currentPath() + "/img" + dateTime + "/" + pID + ".png";
    if(!image.save(path)){
        qDebug() << "could not save file new way" << path;
    }
    image = tmpimg.scaled(100,100,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    return true; //set this to image.save(path);
}
void Page::createHistogram(){
    int windows=5;
    int wsize=50;
    int dist=(image.width()-wsize)/(windows-1);
    int bins=17;
    int binLen=255/bins;

    //histogram = QList<QList<QList<QList<int> > > >; //X,Y,rgb,bin
    QVector<QVector<QVector<QVector<int> > > > tmphist(windows,
                                               QVector <QVector < QVector <int > > > (windows,
                                               QVector <QVector < int > >(4, //number of colors (rgb + lightness
                                               QVector <int> (bins+1, 0))));
    for(int i=0;i<image.width();i++){
        for(int j=0;j<image.height();j++){
            QColor color = image.pixel(i,j);
            int r=color.red()/binLen;
            int g=color.green()/binLen;
            int b=color.blue()/binLen;
            int gray=color.lightness()/binLen;
            for(int wi=0;wi<windows;wi++){ //is there a better way than all these loops?
                if(i>=dist*wi && i<dist*wi+wsize){
                    for(int wj=0;wj<windows;wj++){
                        if(j>=dist*wj && j<dist*wj+wsize){
                            tmphist[wi][wj][0][r]++;
                            tmphist[wi][wj][1][g]++;
                            tmphist[wi][wj][2][b]++;
                            tmphist[wi][wj][3][gray]++;
                        }
                    }
                }
            }
        }
    }
    /*qDebug() << endl << "histogramr:";
    for(int wi=0;wi<windows;wi++){ //is there a better way than all these loops?
        for(int wj=0;wj<windows;wj++){
            qDebug() << endl << "wi:" << wi << "wj" << wj << endl;
            for(int i=0;i<=17;i++){
                qDebug() << tmphist[wi][wj][0][i];
             }
        }
    }
    qDebug() << endl << "histogramg:";
    for(int wi=0;wi<windows;wi++){ //is there a better way than all these loops?
        for(int wj=0;wj<windows;wj++){
            qDebug() << endl << "wi:" << wi << "wj" << wj << endl;
            for(int i=0;i<=17;i++){
                qDebug() << tmphist[wi][wj][1][i];
             }
        }
    }
    qDebug() << endl << "histogramb:";
    for(int wi=0;wi<windows;wi++){ //is there a better way than all these loops?
        for(int wj=0;wj<windows;wj++){
            qDebug() << endl << "wi:" << wi << "wj" << wj << endl;
            for(int i=0;i<=17;i++){
                qDebug() << tmphist[wi][wj][2][i];
             }
        }
    }*/
    histogram = tmphist;
}

void Page::buildColorList() {
    //mPage->ComputedStyleList.clear();
    if(pRand()){ //Todo:add parameter for this on the gui
        mColor.clear();
    }
    if(mColor.empty()){
        QColor tranp;
        tranp.setRgb(0,0,0,0);
        mColor.append(tranp);
    }
    mBentoTree->mRootBlock->buildBlockColorList(&mColor);

}
void Page::updateColor(){
    mBentoTree->mRootBlock->updateBlockColor(&mColor);
}

