#include <QDebug>
#include <QDir>
#include <QTimer>
//#include "CutyCapt.cpp"
#include "igamain.h"
//#include "igamain.cpp"

#include "window.h"
#include "BentoBlock.h"
#include "Bento.h"
#include <QTabWidget>
#include <QTime>
#include <QMap>
#include <QWebSettings>
//#include <QDialog>
#include <QMessageBox>

using namespace bricolage;
int g_x=0;
QTime myTimer;

//#####################################################################
// Function on_pushButton_clicked()
//#####################################################################
void Window::on_pushButton_clicked(){
    //Make it possible to get event from click on webwidget

    for(int i=0;i<10000000;i++){
        qrand();//some rand seem to need "warmup" dont know if that is the case here, but do it anyway
    }
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    setEnabled(false);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    QString webURLall=webURLs->toPlainText();
    QStringList webURLlist = webURLall.split("\n", QString::SkipEmptyParts);
    if(webURLlist.size()*2 > S_PopSize->value()){
        QMessageBox msgBox;
        msgBox.setText("Population should be atleast 2 times number of websites");
        msgBox.setInformativeText("Do you want to abort or ignore??");
        msgBox.setStandardButtons(QMessageBox::Ignore | QMessageBox::Abort);
        if(msgBox.exec()==QMessageBox::Abort){
            qApp->restoreOverrideCursor();
            setEnabled(true);
            return;
        }
    }
    if(tab_nine->isVisible() && webURLlist.size()<9){
        QMessageBox msgBox;
        msgBox.setText("Population should be atleast 9 for \"3x3-mode\"");
        msgBox.setInformativeText("Do you want to abort or continue with normal mode?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
        if(msgBox.exec()==QMessageBox::Abort){
            qApp->restoreOverrideCursor();
            setEnabled(true);
            return;
        }
    }

    treeWidget->clear();
    treeWidget2->clear();
    managerVector.clear();
    webPageVector.clear();    //clear so if used before we start fresh.
    pageId.clear();
    isButtonClicked=true;



    for(int i=0; i<webURLlist.size();i++){
        QString temp=webURLlist.at(i);
        if(temp.contains("http",Qt::CaseInsensitive)) continue;
        if(temp.contains("www",Qt::CaseInsensitive)){
            webURLlist[i]=("http://" + temp);
        }else if(temp.endsWith(".html",Qt::CaseInsensitive)){
            webURLlist[i]= ("/home/lox/Dropbox/Skola/Master-Thesis/Websites/" + temp);
        }else if(!temp.contains(".")){
            webURLlist[i]= ("/home/lox/Dropbox/Skola/Master-Thesis/Websites/" + temp + ".html");
        }else{
            webURLlist[i]=("http://www." + temp);
        }

    }





    managerVector.append(new QNetworkAccessManager(this));
    connect(managerVector.last(), SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onRequestCompleted(QNetworkReply*)));

    QWebSettings *settings = QWebSettings::globalSettings();
    settings->setAttribute(QWebSettings::AutoLoadImages, false); //later it would be cool to modify color etg in images
    settings->setAttribute(QWebSettings::JavaEnabled, false);
    settings->setAttribute(QWebSettings::JavascriptEnabled, false); //migth need this`??

    int urlIndex=0;
    int noPlusOne=(int)S_PopSize->value()%webURLlist.size();
    int j=1;

    //now we load the pages populationsize times, instead this should be only once per page and
    //then dublicatied the same way as in igamain.
    for(int i=0;i< S_PopSize->value();i++){
        int individualPerPage = floor(S_PopSize->value()/webURLlist.size());
        if(urlIndex<noPlusOne){  //make same amount of each page,
            individualPerPage++;
        }
        if(j>individualPerPage){
            urlIndex++;
            j=1;
        }
        j++;
        //        int urlIndex;
        //        if(pRand(0.7)){
        //             urlIndex=i%webURLlist.size();       //if we later want random order
        //        }else{                                   //in that case also edit iga main at 2 different places
        //            urlIndex= qrand()%webURLlist.size(); //as it will order too.
        //        }
        qDebug() << i <<": " << urlIndex << "iperpage: " <<individualPerPage << endl;
        webPageVector.append(new QWebPage);
        webPageVector.last()->setNetworkAccessManager(managerVector[0]);
        webPageVector.last()->mainFrame()->setUrl(QUrl(webURLlist[urlIndex]));
        pageId.append(urlIndex);
    }


}

void Window::onRequestCompleted(QNetworkReply * reply){
    //So this worked different than planned, each picture etc is different reqeast, i.e we get more than the vector size
    //Todo, not really needde as we wait 5 seconds after anyway, but should be a check to see how many it should be
    //so we can finish when all is down.
    //bc=(bc|(1<<i));
    if(reply->errorString()=="Unknown error") bc++;
    qDebug() << "NEW REQUEST DONE!" << bc  << "errorstr=" <<reply->errorString()<<  "url:" <<reply->url() << endl;
    if(bc>=S_PopSize->value()){
        allreqdone();
    }
}
void Window::allreqdone(){
    //todo, need to do anyting else here? otherwise can remove and go directly instead.
    on_mainWeb_1_loadFinished();
}

//#####################################################################
// Function on_webView_initialLayoutCompleted
//#####################################################################
void Window::on_webView_initialLayoutCompleted() {
    qDebug().nospace() << "Initial Layout Completed";
}
//#####################################################################
// Function :on_webView_loadFinished
//#####################################################################
void Window::on_mainWeb_1_loadFinished()
{
    qDebug() << "timer set" << endl;
    if(isButtonClicked) QTimer::singleShot(5000, this, SLOT(on_timer_finished())); //as we dont know exact amount of request, this make sure we catch them all
    isButtonClicked=false; //only allow once and when button is pressed
}
//#####################################################################
// Function on_timer_finished
//#####################################################################
void Window::on_timer_finished() {

    //Clear again?? should be possible to have at same place....
    bentoBlockHash.clear();
    treeWidget->clear();
    treeWidget2->clear();
    ColorTable->clear();
    population.clear();
    mainWeb_1->setPage(webPageVector[0]);
    nineList.clear();
    nineFitness.clear();
    nineCounter=0;
    nineNumberPage.clear();

    qDebug() <<endl << "all sites loaded, now lets turn them into bentopage: these are done:";
    QUrl url;

    for(int i=0;i<S_PopSize->value();i++){
        qDebug() << i;
        //QUrl oldurl= url;
        url=webPageVector[i]->mainFrame()->baseUrl();
        //if(oldurl!=url) pageid++;
        population.append(*new Page(*webPageVector[i],pageId[i],url.toString()));
    }

    qDebug() << endl << "Done!, now create the copyareas: ";
    for(int i=0;i<S_PopSize->value();i++){
        qDebug() << i;
        population.append(*new Page());
    }

    //for testing the new merge, migth be usefule later.....
    //    population[0]. (webPageVector[0]->mainFrame()->documentElement(),0);
    //    QString htmll1 = webPageVector[0]->mainFrame()->documentElement().lastChild().firstChild().lastChild().toInnerXml();
    //    QString htmll2 = webPageVector[0]->mainFrame()->documentElement().lastChild().firstChild().lastChild().toOuterXml();
    //    qDebug()<< endl << "-----------------------------------------------------------------------------------" << endl
    //            << htmll1
    //            << endl << "-----------------------------------------------------------------------------------" << endl
    //            << htmll2 << endl;
    //    webPageVector[0]->mainFrame()->documentElement().lastChild().firstChild().lastChild().setOuterXml(htmll2);
    //    qDebug()<< endl << "-----------------------------------------------------------------------------------" <<endl;
    //     population[0].printList(webPageVector[0]->mainFrame()->documentElement(),0);
    //     qDebug()<< endl << "-----------------------------------------------------------------------------------" <<endl;
    //    webPageVector[0]->mainFrame()->documentElement().lastChild().firstChild().lastChild().setInnerXml(htmll1);
    //    qDebug()<< endl << "-----------------------------------------------------------------------------------" <<endl;
    //    population[0].printList(webPageVector[0]->mainFrame()->documentElement(),0);
    //    qDebug()<< endl << "-----------------------------------------------------------------------------------" <<endl;


    for(int i=0;i<population.size();i++){
        testtest(&population[i],population[i].mBentoTree->mRootBlock); //important function that deserve a real name
    }

    /*
     *
     * These two(3) lines can copy parts of the text and stucture, but not the stylesheet
     */
    //QWebElement testt= population[6].mBentoTree->mRootBlock->mDOMNode.clone();//.takeFromDocument();
    //population[0].mBentoTree->mRootBlock->mChildren[1]->mChildren[1]->mDOMNode.replace(testt);
    //population[1].mBentoTree->mRootBlock->mDOMNode.encloseContentsWith(testt);
    /*
     *
     * These two lines change the link color of an item.
     */
    //QString thisName=  population[0].mBentoTree->mRootBlock->mChildren[1]->mChildren[2]->mChildren[2]->mDOMNode.attribute("class");
    //population[0].mBentoTree->mRootBlock->mChildren[1]->mChildren[2]->mChildren[2]->mDOMNode.appendInside("<style type=\"text/css\">#" + thisName + " a:link { color: #00ff00; }</style>");
    //QDoubleSpinBox *test2 = S_ColorNew;




    //mainWeb_1->setZoomFactor(0.5);
    qDebug() << endl << "Done, now create the iga object " << endl;
    popBuildColorList();
    igaObject->initIgacado(&settings,&numericLimits,S_PopSize->value(),&population);
    qDebug() << "Done, mutate first generation" << endl;
    popUpdateColor();
    igaObject->mutateColor();
    qDebug() << "Done, create style lists" << endl;

    //in some cases we need to do this?
    //popBuildColorList();
    popUpdateColor();
    popBuildColorList();
    updateColorTable();

    //Following is for web9
    if(tab_nine->isVisible() && webPageVector.size()>=9){
        qDebug() << "Done" << endl << "create web9" << endl;
        tab_single->setEnabled(false);
        tab_nine->setEnabled(true);
        nineGenerateNew();
        Web9_1->installEventFilter(this);
        Web9_2->installEventFilter(this);
        Web9_3->installEventFilter(this);
        Web9_4->installEventFilter(this);
        Web9_5->installEventFilter(this);
        Web9_6->installEventFilter(this);
        Web9_7->installEventFilter(this);
        Web9_8->installEventFilter(this);
        Web9_9->installEventFilter(this);

//        Web9_1->setZoomFactor(0.5);
//        Web9_2->setZoomFactor(0.5);
//        Web9_3->setZoomFactor(0.5);
//        Web9_4->setZoomFactor(0.5);
//        Web9_5->setZoomFactor(0.5);
//        Web9_6->setZoomFactor(0.5);
//        Web9_7->setZoomFactor(0.5);
//        Web9_8->setZoomFactor(0.5);
//        Web9_9->setZoomFactor(0.5);

        for(int i=0;i<9;i++){
            nineNumberPage.append(new QWebPage);
            nineNumberPage[i]->mainFrame()->setHtml("<p style=\"font-size:50px\">"+QString::number(i)+"</p>");
        }

    }else{
        tab_nine->setEnabled(false);
        tab_single->setEnabled(true);
        fitnessSlider->installEventFilter(this);
        nineNumberPage.append(new QWebPage);//didnt plan to have it here, but actually pretty nice..
        nineNumberPage[0]->mainFrame()->setHtml("<p style=\"font-size:50px\"> Please wait... </p>");
    }

    button_dislike->setEnabled(true);
    button_like->setEnabled(true);
    myTimer.start();
    qDebug() << "Everything done! now it's time to start playing!" << endl;
    qApp->restoreOverrideCursor();
    setEnabled(true);
}


//#####################################################################
// Function examineChildElements
//#####################################################################
void Window::examineChildElements(const BentoBlock* bentoBlock, QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    bentoBlockHash[item] = bentoBlock;
    if (!bentoBlock->mDOMNode.isNull())
        item->setText(0, QString::number(bentoBlock->mBentoID) + " " + bentoBlock->mDOMNode.tagName() + " " + bentoBlock->mDOMNode.attribute("id") + " " + bentoBlock->mDOMNode.attribute("class"));
    else
        item->setText(0, QString::number(bentoBlock->mBentoID) + " MERGE BLOCK");

    parentItem->addChild(item);

    for(uint i=0; i<(uint)bentoBlock->mChildren.size(); i++) examineChildElements(bentoBlock->mChildren[i], item);
}

//#####################################################################
// Function BuildColorlist
// For each kid, put its color in the higest mcolor, and 
// then link to this with a pointer, i.e if changed in the
// main mcolor all the ones under change too
//#####################################################################
void Window::popBuildColorList() {
    for(int i=0;i<population.size()/2;i++){
        population[i].updateStyleList();
        buildColorList(&population[i]);

    }
}
void Window::buildColorList(Page *mPage) {
    //mPage->ComputedStyleList.clear();
    mPage->mColor.clear();
    buildColorList(mPage->mBentoTree->mRootBlock,&mPage->mColor);
}



void Window::buildColorList(BentoBlock *bentoBlock,QVector<QColor>* colorList){
    /* These could be inserted later, but not really important?
    "border-right-color"
    "border-top-color"
    "text-line-through-color"
    "text-overline-color"
    "text-underline-color"
    "border-left-color"
    "border-bottom-color"
    */

    //under this is the old, keep until it works

    QColor bg= DOMUtils::parseColorFeature(bentoBlock->mDOMNode.styleProperty("background-color", QWebElement::ComputedStyle));
    QColor co= DOMUtils::parseColorFeature(bentoBlock->mDOMNode.styleProperty("color", QWebElement::ComputedStyle));
    QColor link= DOMUtils::parseColorFeature(bentoBlock->mDOMNode.findFirst("a").styleProperty("color", QWebElement::ComputedStyle));    //Skip other types of links, the users will probably not check that anyway, and will be too much
    QColor outline= DOMUtils::parseColorFeature(bentoBlock->mDOMNode.styleProperty("outline-color", QWebElement::ComputedStyle));
    QColor border= DOMUtils::parseColorFeature(bentoBlock->mDOMNode.styleProperty("border-color", QWebElement::ComputedStyle));


    addColor(bentoBlock->mbgColor,colorList, bg);
    addColor(bentoBlock->mtColor, colorList, co);
    addColor(bentoBlock->mborderColor, colorList, border);
    addColor(bentoBlock->moutlineColor, colorList, outline);
    addColor(bentoBlock->mlinkColor, colorList, link);

    for(int i=0; i<bentoBlock->mChildren.size(); i++) {
        buildColorList(bentoBlock->mChildren[i],colorList);
    }
}

//This should be in the bentoblock file! haha. FML
void Window::addColor(int &colorBlock,QVector<QColor>* colorList, QColor newColor){
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
int Window::colorIndex(QVector<QColor>* colorList, QColor color){
    int index=-1;

    for(uint i=0;i<colorList->size();i++){
        if(colorList->at(i).name()==color.name()){
            index=i;
            break;
        }
    }
    return index;
}

void Window::popUpdateColor() {
    for(int i=0;i<population.size()/2;i++){
        updateColor(population[i].mBentoTree->mRootBlock,&population[i].mColor);
        population[i].updateStyles();
    }
}
void Window::updateColor(BentoBlock *bentoBlock,QVector<QColor>* colorList) {
    //    bentoBlock->mDOMNode.classes();

    //TODOCOLOR

    //We better handle this somewhere else!
    if(bentoBlock->mtColor >= colorList->size())
        bentoBlock->mtColor=0; //TODO Add new color??? or how to do here??
    if(bentoBlock->mbgColor >= colorList->size())
        bentoBlock->mbgColor=0; //TODO Add new color??? or how to do here??
    if(bentoBlock->mlinkColor >= colorList->size())
        bentoBlock->mlinkColor=0;
    if(bentoBlock->mborderColor >= colorList->size())
        bentoBlock->mborderColor=0;
    if(bentoBlock->moutlineColor >= colorList->size())
        bentoBlock->moutlineColor=0;


    //Variables holding the colors
    QColor textColor = colorList->at(bentoBlock->mtColor);
    QColor bgColor = colorList->at(bentoBlock->mbgColor);
    QColor border = colorList->at(bentoBlock->mborderColor);
    QColor outline = colorList->at(bentoBlock->moutlineColor);
    QColor link = colorList->at(bentoBlock->mlinkColor);



    //Sets the colors
    bentoBlock->mDOMNode.setStyleProperty("color",DOMUtils::qColor2RGBStr(textColor));
    bentoBlock->mDOMNode.setStyleProperty("background-color",DOMUtils::qColor2RGBStr(bgColor));
    bentoBlock->mDOMNode.setStyleProperty("outline-color",DOMUtils::qColor2RGBStr(outline));
    bentoBlock->mDOMNode.setStyleProperty("border-color",DOMUtils::qColor2RGBStr(border));
    foreach(QWebElement element,bentoBlock->mDOMNode.findAll("a")){
        element.setStyleProperty("color",DOMUtils::qColor2RGBStr(link));
    }
    //    bentoBlock->mDOMNode.setStyleProperty("a:link color",DOMUtils::qColor2RGBStr(linkColor));

    for (int i=0; i<bentoBlock->mChildren.size(); i++) {
        updateColor(bentoBlock->mChildren[i],colorList);
    }
}




//void bricolage::Window::on_fitnessSlider_sliderReleased(){
//    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
//    setEnabled(false);

//    qDebug()<<"new Vote! Slider" << fitnessSlider->value() << endl;
//    igaObject->nextIndividual(double(fitnessSlider->value())/100);
//    if(igaObject->currIndividial==0){
//        qDebug() << "new gen update colors:" << endl;
//        popUpdateColor();
//        popBuildColorList();
//        updateColorTable();
//    }
//    mainWeb_1->setPage(population[igaObject->currIndividial].webpageP);
//    qDebug() << "Done, your turn!" << endl;
//    myTimer.start();

//    htmlView->document()->setPlainText(population[igaObject->currIndividial].getHtml());
//    treeWidget->clear();
//    examineChildElements(population[igaObject->currIndividial].mBentoTree->mRootBlock, treeWidget->invisibleRootItem());
//    qApp->restoreOverrideCursor();
//    setEnabled(true);
//}
void Window::on_button_dislike_clicked(){


    button_vote(false);
}

void bricolage::Window::on_button_like_clicked(){
    button_vote(true);
}

void Window::button_vote(bool liked){
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    mainWeb_1->setPage(nineNumberPage[0]);
    setEnabled(false);

    qDebug()<<"new Vote! Time" << myTimer.elapsed() << endl;
    double fitness = (1+exp(-double(myTimer.elapsed())/S_TimeFitness->value()));
    fitness = liked ? fitness/2 : (1/fitness)-0.5;
    igaObject->nextIndividual(fitness);
    //mainTabWidget->setCurrentIndex(igaObject->currIndividial);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

    if(igaObject->currIndividial==0){
        qDebug() << "new gen update colors:" << endl;
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        popUpdateColor();
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        popBuildColorList();
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        updateColorTable();
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    }
    StyleListTextboxOld->clear();
    StyleListTextboxOld->insertPlainText(StyleListTextbox->toPlainText());
    StyleListTextbox->clear();
    QList<QString> keys =population[igaObject->currIndividial].ComputedStyleList.keys();
    foreach (QString key, keys){
        StyleListTextbox->appendPlainText("\n"+ key + ":");
        foreach (QString val, population[igaObject->currIndividial].ComputedStyleList[key]){
            StyleListTextbox->appendPlainText(val);
        }
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    mainWeb_1->setPage(population[igaObject->currIndividial].webpageP);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);


    qDebug() << "Done, your turn!" << endl;

    myTimer.start();
    //
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    htmlView->document()->setPlainText(population[igaObject->currIndividial].getHtml());
    treeWidget->clear();
    examineChildElements(population[igaObject->currIndividial].mBentoTree->mRootBlock, treeWidget->invisibleRootItem());



    //Start timer for next round

    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    qApp->restoreOverrideCursor();
    setEnabled(true);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

void Window::updateColorTable(){
    int pSize=igaObject->popSize();
    ColorTable->clear();
    ColorTable->setRowCount(pSize);
    for(int i=0;i<pSize;i++){
        int cSize=population[i].mColor.size();
        if(cSize > ColorTable->columnCount())
            cSize = ColorTable->columnCount();
        for(int j=0;j<cSize;j++){
            ColorTable->setItem(i,j,new QTableWidgetItem);
            if(population[i].mColor[j].alpha()<255){
                ColorTable->item(i,j)->setText(QString::number((int)(population[i].mColor[j].alpha()/25.5)));
            }
            ColorTable->item(i,j)->setBackgroundColor(population[i].mColor[j].rgb());
        }
    }

}

////Remove!1
void Window::testtest(Page *mpage, BentoBlock* bentoBlock){
    bentoBlock->mDomNodeID=-1; //TODO, migth be a problem?

    /*For the domenodes, probably not needed, and we should take from org page*/
    for(int i=0;i<mpage->mDOMNodes.size();i++){
        if(bentoBlock->mDOMNode == mpage->mDOMNodes[i]){
            bentoBlock->mDomNodeID=i;
        }
    }
    if(bentoBlock->mDomNodeID==-1){
        //qDebug() << endl << "---- no match ----" << endl << bentoBlock->mDOMNode.toOuterXml() << endl << endl << "----end ----" << endl;
    }
    for(int i=0;i<bentoBlock->mChildren.size();i++){
        testtest(mpage,bentoBlock->mChildren[i]);
    }
}

bool Window::eventFilter(QObject *object, QEvent *event){
    if (object == Web9_1 && event->type() == QEvent::MouseButtonPress) {

        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

        qDebug()<<"woop1";
        Web9_1->setZoomFactor(1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        Web9_1->setPage(nineNumberPage[nineCounter]);
        nineNewClick(nineList[0]);
    }
    if (object == Web9_2 && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug()<<"woop2";
        Web9_2->setZoomFactor(1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        Web9_2->setPage(nineNumberPage[nineCounter]);
        nineNewClick(nineList[1]);
    }
    if (object == Web9_3 && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug()<<"woop3";
        Web9_3->setZoomFactor(1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        Web9_3->setPage(nineNumberPage[nineCounter]);
        nineNewClick(nineList[2]);
    }
    if (object == Web9_4 && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug()<<"woop4";
        Web9_4->setZoomFactor(1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        Web9_4->setPage(nineNumberPage[nineCounter]);       
        nineNewClick(nineList[3]);
    }
    if (object == Web9_5 && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug()<<"woop5";
        Web9_5->setZoomFactor(1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        Web9_5->setPage(nineNumberPage[nineCounter]);

        nineNewClick(nineList[4]);
    }
    if (object == Web9_6 && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug()<<"woop6";
        Web9_6->setZoomFactor(1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        Web9_6->setPage(nineNumberPage[nineCounter]);
        nineNewClick(nineList[5]);
    }
    if (object == Web9_7 && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug()<<"woop7";
        Web9_7->setZoomFactor(1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        Web9_7->setPage(nineNumberPage[nineCounter]);
        nineNewClick(nineList[6]);
    }
    if (object == Web9_8 && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug()<<"woop8";
        Web9_8->setZoomFactor(1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        Web9_8->setPage(nineNumberPage[nineCounter]);
        nineNewClick(nineList[7]);
    }
    if (object == Web9_9 && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug()<<"woop9";
        Web9_9->setZoomFactor(1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        Web9_9->setPage(nineNumberPage[nineCounter]);
        nineNewClick(nineList[8]);
    }
    if (object == fitnessSlider && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        double newval =fitnessSlider->maximum()*((double)mouseEvent->x()/fitnessSlider->width());
        fitnessSlider->setValue(newval);
        qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
        setEnabled(false);
        igaObject->nextIndividual(newval);
        if(igaObject->currIndividial==0){
            qDebug() << "new gen update colors:" << endl;
            popUpdateColor();
            popBuildColorList();
            updateColorTable();
        }
        mainWeb_1->setPage(population[igaObject->currIndividial].webpageP);
        qDebug() << "Done, your turn!" << endl;
        myTimer.start();
        htmlView->document()->setPlainText(population[igaObject->currIndividial].getHtml());
        treeWidget->clear();
        examineChildElements(population[igaObject->currIndividial].mBentoTree->mRootBlock, treeWidget->invisibleRootItem());
        qApp->restoreOverrideCursor();
        setEnabled(true);

        event->ignore();
    }
    // The event will be correctly sent to the widget
    return false;
    // If you want to stop the event propagation now:
    // return true
}
void Window::nineNewClick(int id){
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

    nineCounter++;
    if(nineFitness.contains(id)){
        nineFitness[id]=(nineFitness[id]+1-(double(nineCounter)/10))/2;
    }else{
        nineFitness[id]=1-(double(nineCounter)/10);
    }
    qDebug()<<"this fitness was" << nineCounter << ". new score for" << id <<"is"<<nineFitness[id]<<endl;
    if(nineCounter>=9){
        nineCounter=0;
        if(nineFitness.size()>=igaObject->popSize()){
            qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
            setEnabled(false);
            //send all to next generation. for now loop next individual
            for(int i=0;i<nineFitness.size();i++){
                igaObject->nextIndividual(nineFitness[i]);
            }
            qDebug()<<"new generation" << endl;
            popUpdateColor();
            popBuildColorList();
            updateColorTable();
            nineFitness.clear();
            qApp->restoreOverrideCursor();
            setEnabled(true);
        }
        nineGenerateNew();
    }
    qDebug()<< "ninecounter" << nineCounter << "fitsize" << nineFitness.size();

}
void Window::nineGenerateNew(){
    nineList.clear();
    for(int i = 0; i < 9; ++i){
        int r;
        int e=0;
        do{
            e++;
            r = rand()%(int)igaObject->popSize();
            //qDebug()<<"no"<<i<<"try:"<<e<<"value"<<r << endl;
            if(nineFitness.contains(r) && pRand()) continue;
        }while(nineList.contains(r));

        nineList.append(r);
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    Web9_1->setZoomFactor(21);
    Web9_2->setZoomFactor(21);
    Web9_3->setZoomFactor(21);
    Web9_4->setZoomFactor(21);
    Web9_5->setZoomFactor(21);
    Web9_6->setZoomFactor(21);
    Web9_7->setZoomFactor(21);
    Web9_8->setZoomFactor(21);
    Web9_9->setZoomFactor(21);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    Web9_1->setPage(webPageVector[nineList[0]]);
    Web9_2->setPage(webPageVector[nineList[1]]);
    Web9_3->setPage(webPageVector[nineList[2]]);
    Web9_4->setPage(webPageVector[nineList[3]]);
    Web9_5->setPage(webPageVector[nineList[4]]);
    Web9_6->setPage(webPageVector[nineList[5]]);
    Web9_7->setPage(webPageVector[nineList[6]]);
    Web9_8->setPage(webPageVector[nineList[7]]);
    Web9_9->setPage(webPageVector[nineList[8]]);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    Web9_1->setZoomFactor(0.5);
    Web9_2->setZoomFactor(0.5);
    Web9_3->setZoomFactor(0.5);
    Web9_4->setZoomFactor(0.5);
    Web9_5->setZoomFactor(0.5);
    Web9_6->setZoomFactor(0.5);
    Web9_7->setZoomFactor(0.5);
    Web9_8->setZoomFactor(0.5);
    Web9_9->setZoomFactor(0.5);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}
void Window::init(){
    fitnessSlider->installEventFilter(this);
    settings.insert("S_ColorCross",S_ColorCross);
    settings.insert("S_ColorFlip",S_ColorFlip);
    settings.insert("S_ColorNew",S_ColorNew);
    settings.insert("S_ColorRand",S_ColorRand);
    settings.insert("S_ColorRot",S_ColorRot);
    settings.insert("S_Colorhue_F",S_Colorhue_F);
    settings.insert("S_Colorhue_R",S_Colorhue_R);
    settings.insert("S_Colorhue_M",S_Colorhue_M);
    settings.insert("S_Colorsat_F",S_Colorsat_F);
    settings.insert("S_Colorsat_M",S_Colorsat_M);
    settings.insert("S_Colorsat_R",S_Colorsat_R);
    settings.insert("S_Colorlig_F",S_Colorlig_F);
    settings.insert("S_Colorlig_M",S_Colorlig_M);
    settings.insert("S_Colorlig_R",S_Colorlig_R);
    settings.insert("S_TimeFitness",S_TimeFitness);
    settings.insert("S_evolTs",S_evolTs);
    settings.insert("S_evolTsize",S_evolTsize);
    settings.insert("S_Size_mean",S_Size_mean);
    settings.insert("S_Size_std",S_Size_std);
    settings.insert("S_Size_fitness",S_Size_fitness);
    settings.insert("S_sizeChangeProba",S_sizeChangeProba);
    settings.insert("S_swapPropa",S_swapPropa);
    settings.insert("S_BasicCrossProba",S_BasicCrossProba);
    settings.insert("S_BasicCrossProbaKey",S_BasicCrossProbaKey);
    settings.insert("S_BasicCrossProbaValue",S_BasicCrossProbaValue);
    settings.insert("S_BasicCrossKeepLengthProb",S_BasicCrossKeepLengthProb);
}
bool Window::saveToIni(){
    QFile file("webocado.ini");
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream.flush();
        stream << "#:widget:name:decimals():minimum():maximum():singleStep():value()" << endl;
        foreach (QString key, settings.keys()) {
            stream << "widget\t:\t" << key << "\t:\t"
                                << settings[key]->decimals()<< "\t:\t"
                                << settings[key]->minimum()<< "\t:\t"
                                << settings[key]->maximum()<< "\t:\t"
                                << settings[key]->singleStep()<< "\t:\t"
                                << settings[key]->value() << endl;
        }
        stream << "#:numeric_Limits::name:minimum():maximum()" << endl;
        foreach(QString key, numericLimits.keys()){
            stream << "numeric_Limits\t:\t"  <<key << ":" << numericLimits[key][0] << ":" << numericLimits[key][1]<<endl;
        }

        //stream << "something" << endl;
    }
}

bool Window::readini(){
    QFile file("webocado.ini");

    if(!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"webocado.ini Not Found.";
        return false;
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line2 = in.readLine();
        QString line=line2.simplified();//Remove all whitespace, ie \t
        QStringList fields = line.split(":");
        for(int i=0;i<fields.size();i++){
            fields[i]=fields[i].trimmed();
        }

        if(fields[0]=="#") continue;

        if(fields[0]=="widget"){ //if widget don't exist, create a new?
            if(!settings.contains(fields[1])) continue;
            settings[fields[1]]->setDecimals(fields[2].toInt());
            settings[fields[1]]->setMinimum(fields[3].toDouble());
            settings[fields[1]]->setMaximum(fields[4].toDouble());
            settings[fields[1]]->setSingleStep(fields[5].toDouble());
            settings[fields[1]]->setValue(fields[6].toDouble());
        }else if(fields[0]=="numeric_Limits"){
            if(numericLimits.contains(fields[1])){
                numericLimits[fields[1]][0] = fields[2].toDouble();
                numericLimits[fields[1]][1] = fields[3].toDouble();
            }else{
                numericLimits[fields[1]].append(fields[2].toDouble());
                numericLimits[fields[1]].append(fields[3].toDouble());
            }
        }else{
            qDebug() << "unvalid line" << fields[0] << "pos:" << in.pos()<< endl;

        }
    }

    file.close();

    return true;
}
void Window::closeEvent(QCloseEvent *event){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit?", "Do you want to save settings before exit?",
                                  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    if (reply == QMessageBox::Yes) {
      qDebug() << "Yes was clicked";
      saveToIni();
      event->accept();
    } else if(reply == QMessageBox::No) {
      qDebug() << "no was clicked";
      event->accept();
    } else {
        qDebug() << "cancel was clicked";
        event->ignore();
    }
}


//class Window::fitnessSlider : public QSlider
//{
//    Q_OBJECT
//protected:
//    void mousePressEvent(QMouseEvent *event);// Q_DECL_OVERRIDE;

//};
//void fitnessSlider::mousePressEvent ( QMouseEvent * event ){
//  QStyleOptionSlider opt;
//  initStyleOption(&opt);
//  QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

//  if (event->button() == Qt::LeftButton &&
//      sr.contains(event->pos()) == false)
//  {
//    int newVal;
//    double halfHandleWidth = (0.5 * sr.width()) + 0.5; // Correct rounding
//    int adaptedPosX = event->x();
//    if ( adaptedPosX < halfHandleWidth )
//            adaptedPosX = halfHandleWidth;
//    if ( adaptedPosX > width() - halfHandleWidth )
//            adaptedPosX = width() - halfHandleWidth;
//    // get new dimensions accounting for slider handle width
//    double newWidth = (width() - halfHandleWidth) - halfHandleWidth;
//    double normalizedPosition = (adaptedPosX - halfHandleWidth)  / newWidth ;

//    newVal = minimum() + ((maximum()-minimum()) * normalizedPosition);

//    if (invertedAppearance() == true)
//        setValue( maximum() - newVal );
//    else
//        setValue(newVal);

//    qDebug() << newVal;
//    event->accept();
//  }
//  QSlider::mousePressEvent(event);
//}
