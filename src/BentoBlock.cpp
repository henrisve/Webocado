
#include "BentoBlock.h"



using namespace bricolage;
QHash<QString, QString> BentoBlock::getStyles(){
    return DOMUtils::getStyles(mDOMNode);
}
void BentoBlock::setStyles(int keyIndex, QString key,int size){
    mComputedStyles[key].first=keyIndex;
    mComputedStyles[key].second=size;
}

void BentoBlock::updateStyles(const QHash<QString, QVector<QString> >* styleList, QSet<QString> UpdatedKeys){
    if(styleList->isEmpty()){
        qDebug() << endl << "Warning: Stylelist is empty, ignore"<< endl;
        return;
    }
    //QList<QString> keys = mComputedStyles.keys()

    /*Note to self.
     * mComputedStyles is a list and thus takes longer time to search
     * can we make it a set or other way?
     */
    foreach (QString key, UpdatedKeys){
        if(mComputedStyles.keys().contains(key)){
            int index=mComputedStyles.value(key).first;
    //            if(index >= styleList->value(key).size()){
    //                index=styleList->value(key).size()-1;
    //            }
            //qDebug() << "this" <<index <<" of "<<mComputedStyles.value(key).second << "keys size "<<keys.size()<<"stylist size"<<styleList->size()<<"mComputedStyles.size"<<mComputedStyles.size()<<endl;
            if(index > styleList->value(key).size()){
                qDebug() << endl << "stylelist too small, select last available instead";
                index = styleList->value(key).size()-1; //I dont know why this happens sometimes.. but very rare, so ignore it
           }
           int styleSize=styleList->value(key).size();
           if(index>=styleSize){
               index=(qrand()%styleSize);//this is not correct way, but I dont care
               qDebug() << "Warning: stylelist too small, new index:" << index << endl;
           }
           mDOMNode.setStyleProperty(key,styleList->value(key).at(index) + " !important");
        }
    }
}


void BentoBlock::buildBlockColorList(QVector<QColor>* colorList){
    QColor bg= DOMUtils::parseColorFeature(mDOMNode.styleProperty("background-color", QWebElement::ComputedStyle));
    QColor co= DOMUtils::parseColorFeature(mDOMNode.styleProperty("color", QWebElement::ComputedStyle));
    QColor link= DOMUtils::parseColorFeature(mDOMNode.findFirst("a").styleProperty("color", QWebElement::ComputedStyle));    //Skip other types of links, the users will probably not check that anyway, and will be too much
    QColor outline= DOMUtils::parseColorFeature(mDOMNode.styleProperty("outline-color", QWebElement::ComputedStyle));
    QColor border= DOMUtils::parseColorFeature(mDOMNode.styleProperty("border-color", QWebElement::ComputedStyle));
    addColor(mbgColor,colorList, bg);
    addColor(mtColor, colorList, co);
    addColor(mborderColor, colorList, border);
    addColor(moutlineColor, colorList, outline);
    addColor(mlinkColor, colorList, link);
    //qDebug()<< endl << "localname:" << mDOMNode.localName()<< "tagname:" <<mDOMNode.tagName()<< "attlist:";
    //QStringList sl=mDOMNode.attributeNames();
    //foreach(QString att,sl){
    //    qDebug() <<  att << ":" << mDOMNode.attribute(att)<< "|";
    //}
    //qDebug()<<  "end" << endl;
    for(int i=0; i<mChildren.size(); i++) {
        mChildren[i]->buildBlockColorList(colorList);
    }
}
//#####################################################################
// Function BentoBlock::addColor
// Add color to the list.
//#####################################################################
void BentoBlock::addColor(int &colorBlock,QVector<QColor>* colorList, QColor newColor){ //could probably be merge it function above...
    if(newColor.alpha()==0){
        colorBlock=0; //All transparant is always represented by the first.
    }
    int index=colorIndex(colorList, newColor);
    if(index<0){
        colorList->append(newColor);
        colorBlock=colorList->size()-1;//&colorList->back();
    }else{
        colorBlock=index;//&temp->back();
    }
}
//#####################################################################
// Function colorIndex
// Return the index of the color of a list.
// returns -1 if not found
//#####################################################################
int BentoBlock::colorIndex(QVector<QColor>* colorList, QColor color){
    int index=-1;
    for(uint i=0;i<colorList->size();i++){
        if(colorList->at(i).name()==color.name()){
            index=i;
            break;
        }
    }
    return index;
}
void BentoBlock::updateBlockColor(QVector<QColor>* colorList) {
    //    bentoBlock->mDOMNode.classes();

    //TODOCOLOR

    //We better handle this somewhere else!
    if(mtColor >= colorList->size())
        mtColor=0; //TODO Add new color??? or how to do here??
    if(mbgColor >= colorList->size())
        mbgColor=0; //TODO Add new color??? or how to do here??
    if(mlinkColor >= colorList->size())
        mlinkColor=0;
    if(mborderColor >= colorList->size())
        mborderColor=0;
    if(moutlineColor >= colorList->size())
        moutlineColor=0;


    //Variables holding the colors
    QColor textColor = colorList->at(mtColor);
    QColor bgColor = colorList->at(mbgColor);
    QColor border = colorList->at(mborderColor);
    QColor outline = colorList->at(moutlineColor);
    QColor link = colorList->at(mlinkColor);



    //Sets the colors
    mDOMNode.setStyleProperty("color",DOMUtils::qColor2RGBStr(textColor));
    mDOMNode.setStyleProperty("background-color",DOMUtils::qColor2RGBStr(bgColor));
    mDOMNode.setStyleProperty("outline-color",DOMUtils::qColor2RGBStr(outline));
    mDOMNode.setStyleProperty("border-color",DOMUtils::qColor2RGBStr(border));
    foreach(QWebElement element,mDOMNode.findAll("a")){
        element.setStyleProperty("color",DOMUtils::qColor2RGBStr(link));
    }
    //    bentoBlock->mDOMNode.setStyleProperty("a:link color",DOMUtils::qColor2RGBStr(linkColor));

    for (int i=0; i<mChildren.size(); i++) {
        mChildren[i]->updateBlockColor(colorList);
    }
}

