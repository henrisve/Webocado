//#####################################################################
// Copyright 2016, Henrik K. Svensson.
// Parts of this software is using Bento, Copyright 2012, Ranjitha Kumar.
// This software is governed by the license contained in LICENSE.
//#####################################################################
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QDateTime>
#include "igamain.h"
#include "window.h"
#include "BentoBlock.h"
#include "Bento.h"
#include <QTabWidget>
#include <QTime>
#include <QMap>
#include <QWebSettings>
#include <QMessageBox>

using namespace bricolage;
int g_x=0;
QTime myTimer;

//#####################################################################
// Function on_pushButton_clicked()
//#####################################################################
void Window::on_pushButton_clicked(){

    //Make it possible to get event from click on webwidget


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
    if(tab_nine->isVisible() && S_PopSize->value()<9){
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
    if(!managerVector.isEmpty()){
        QMessageBox msgBox;
        msgBox.setText("cancel current run and start again?");
        msgBox.setInformativeText("Do you want to abort or continue?");
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

    //For ancestry tree
    dateTime = QDateTime::currentDateTime().toString("MMddhhmms");
    QString path=QDir::currentPath() + "/img" + dateTime;
    QDir dir = QDir::root();
    dir.mkpath(path);
    initAncestryTree();
    initHtmlDistance();


    QString pathw=QDir::currentPath() + "/webpages/";
    dir = QDir::root();
    dir.mkpath(pathw);
    for(int i=0; i<webURLlist.size();i++){
        QString temp=webURLlist.at(i);
        if(temp.contains("http",Qt::CaseInsensitive)) continue;
        if(temp.contains("www",Qt::CaseInsensitive)){
            webURLlist[i]=("http://" + temp);
        }else if(temp.endsWith(".html",Qt::CaseInsensitive)){
            webURLlist[i]= pathw + temp;
        }else if(!temp.contains(".")){
            webURLlist[i]= (pathw + temp + ".html");
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
        //view->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
        qDebug() << i <<": " << urlIndex << "iperpage: " <<individualPerPage << endl;
        webPageVector.append(new QWebPage);
        webPageVector.last()->setNetworkAccessManager(managerVector[0]);
        webPageVector.last()->mainFrame()->setUrl(QUrl(webURLlist[urlIndex]));
        webPageVector.last()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
        webPageVector.last()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
        //webPageVector.last()->setViewportSize(QSize(1000,1000));//Todo, 1000 thing should not be hardcoded here!

        webPageVector.last()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
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
    //bentoBlockHash.clear();
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
        webPageVector[i]->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
        webPageVector[i]->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
        webPageVector[i]->setViewportSize(QSize(1000,1000));
        //if(oldurl!=url) pageid++;
        population.append(*new Page(*webPageVector[i],pageId[i],url.toString(),i,dateTime));
    }

    qDebug() << endl << "Done!, now create the copyareas: ";
    for(int i=0;i<S_PopSize->value();i++){
        qDebug() << i;
        population.append(*new Page());
    }

    for(int i=0;i<population.size();i++){ //this also take the copy part:/
        testtest(&population[i],population[i].mBentoTree->mRootBlock); //important function that deserve a real name
    }
    qDebug() << endl << "Done, now create the iga object " << endl;
    igaObject->initIgacado(&settings,&numericLimits,S_PopSize->value(),&population);
    qDebug() << "Done, update color table" << endl;
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
        for(int i=0;i<igaObject->popSize();i++){
            population[i].mBentoTree->mRootBlock->mDOMNode.webFrame()->setZoomFactor(0.5);
        }
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
// Remove this?
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
//#####################################################################
void Window::on_button_dislike_clicked(){
    button_vote(false);
}
void bricolage::Window::on_button_like_clicked(){
    button_vote(true);
}
//#####################################################################
// Function buttonvote
// runs when one of the two vote buttons is clicked
//#####################################################################
void Window::button_vote(bool liked){
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    if(runFlag) return;
    runFlag =true;
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    mainWeb_1->setPage(nineNumberPage[0]);
    setEnabled(false);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    qDebug()<<"new Vote! Time" << myTimer.elapsed() << endl;
    double fitness = (1+exp(-double(myTimer.elapsed())/S_TimeFitness->value()));
    fitness = liked ? fitness/2 : (1/fitness)-0.5;
    if(igaObject->currIndividial==igaObject->popSize()-1){ //both this and next is always true at the same time.
        addToAncestryTree(fitness);
    }
    igaObject->nextIndividual(fitness);
    if(igaObject->currIndividial==0){
        addNewGenAncestryTree();
        addToHtmlDistance();
        updateColorTable();
    }
    //adds visible text of the stylelist to gui
    StyleListTextboxOld->clear(); //todo, first plan was old should be prev. generation.. probably not that important..
    StyleListTextboxOld->insertPlainText(StyleListTextbox->toPlainText());
    StyleListTextbox->clear();
    QList<QString> keys =population[igaObject->currIndividial].ComputedStyleList.keys();
    foreach (QString key, keys){
        StyleListTextbox->appendPlainText("\n"+ key + ":");
        foreach (QString val, population[igaObject->currIndividial].ComputedStyleList[key]){
            StyleListTextbox->appendPlainText(val);
        }
    }

    mainWeb_1->setPage(population[igaObject->currIndividial].webpageP);



    qDebug() << "Done, your turn!" << endl;

    myTimer.start();//Start timer for next round

    htmlView->document()->setPlainText(population[igaObject->currIndividial].getHtml());
    treeWidget->clear();
    examineChildElements(population[igaObject->currIndividial].mBentoTree->mRootBlock, treeWidget->invisibleRootItem());


    qApp->restoreOverrideCursor();
    setEnabled(true);
    //qDebug() << "V3" << "p0:" << population[0].ComputedStyleList["height"].size() << "p1:" << population[1].ComputedStyleList["height"].size()<<endl;
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    runFlag=false;
}
 //Todo, very slow!!
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

//#####################################################################
// Function ????
//now this is an important thing, change name! also, move to page.cpp
//Sorts up the domnodes or something...
//#####################################################################
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

//#####################################################################
// Function eventfilter
// Handles all events and choose what to do with them.
//#####################################################################
bool Window::eventFilter(QObject *object, QEvent *event){
    if(event->type() == QEvent::ContextMenu){
        qDebug("testt");
        QContextMenuEvent* mevent = static_cast<QContextMenuEvent *>(event);
        if(mevent->reason() == QContextMenuEvent::Mouse) {
            qDebug("I have blocked the context menu.");
            return true;
        } else return QObject::eventFilter(object, event);
    }


    if(runFlag) return false; //true=ignore it, i.e
    runFlag =true;
    if(event->type() == QEvent::ContextMenu){
        QContextMenuEvent* mevent = static_cast<QContextMenuEvent *>(event);
        if(mevent->reason() == QContextMenuEvent::Mouse) {
            qDebug("I have blocked the context menu.");
            return true;
        } else return QObject::eventFilter(object, event);
    }
    if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        bool isRight = mouseEvent->button()==Qt::RightButton;//2; //

        if (object == Web9_1) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            Web9_1->setPage(nineNumberPage[nineCounter]);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            nineNewClick(nineList[0],isRight);
        }
        if (object == Web9_2) {
            //QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            Web9_2->setPage(nineNumberPage[nineCounter]);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            nineNewClick(nineList[1],isRight);
        }
        if (object == Web9_3) {
            //QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            Web9_3->setPage(nineNumberPage[nineCounter]);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            nineNewClick(nineList[2],isRight);
        }
        if (object == Web9_4 && event->type() == QEvent::MouseButtonPress) {
            //QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            Web9_4->setPage(nineNumberPage[nineCounter]);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            nineNewClick(nineList[3],isRight);
        }
        if (object == Web9_5 && event->type() == QEvent::MouseButtonPress) {
            //QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            Web9_5->setPage(nineNumberPage[nineCounter]);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            nineNewClick(nineList[4],isRight);
        }
        if (object == Web9_6 && event->type() == QEvent::MouseButtonPress) {
            //QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            Web9_6->setPage(nineNumberPage[nineCounter]);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            nineNewClick(nineList[5],isRight);
        }
        if (object == Web9_7 && event->type() == QEvent::MouseButtonPress) {
            //QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            Web9_7->setPage(nineNumberPage[nineCounter]);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            nineNewClick(nineList[6],isRight);
        }
        if (object == Web9_8 && event->type() == QEvent::MouseButtonPress) {
            //QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            Web9_8->setPage(nineNumberPage[nineCounter]);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            nineNewClick(nineList[7],isRight);
        }
        if (object == Web9_9 && event->type() == QEvent::MouseButtonPress) {
            //QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            Web9_9->setPage(nineNumberPage[nineCounter]);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            nineNewClick(nineList[8],isRight);
        }
    }
    if (object == fitnessSlider && event->type() == QEvent::MouseButtonRelease) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        double newval =fitnessSlider->maximum()*((double)mouseEvent->x()/fitnessSlider->width());
        fitnessSlider->setValue(newval);
        qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
        mainWeb_1->setPage(nineNumberPage[0]);
        setEnabled(false);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        if(igaObject->currIndividial==igaObject->popSize()-1){ //both this and next is always true at the same time.
            addToAncestryTree(newval);
        }
        //save time to compute fitness
        QFile file("slidertest.csv");

        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream stream(&file);
            stream.flush();
            stream << myTimer.elapsed() << "," << newval << "," << igaObject->getCurrGen() << "," << igaObject->currIndividial << ","<< dateTime << "," <<
                      QDateTime::currentDateTime().toString("MMddhhmms") << endl;

        }
        file.close();

        igaObject->nextIndividual(newval);
        if(igaObject->currIndividial==0){
            qDebug() << "new gen update colors:" << endl;
            addNewGenAncestryTree();
            addToHtmlDistance();
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
        myTimer.start();
        event->ignore();
    }

    // The event will be correctly sent to the widget
    runFlag =false;

    return false;

    // If you want to stop the event propagation now:
    // return true

}
//#####################################################################
// Function nineNewClick
// This function runs after user clicked one of the 9 pages.
//#####################################################################
void Window::nineNewClick(int id,bool isRight){ //id=popid, todo: change varname
    qDebug()<<"you choose " << id << ", the following got new score: ";
    qDebug() << endl;
    for(int i=0;i<igaObject->popSize();i++){
        qDebug() << i << "\t|  ";

    }
    qDebug() << endl;
    for(int i=0;i<igaObject->popSize();i++){
        if(nineFitness.contains(i)){
            int tmpp=nineFitness[i]*100;

            qDebug() << tmpp << "\t|";
        }else{
            qDebug() << "NaN" << "\t|";
        }
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    nineCounter++;
    int noOfNN=0;
    QVector<int> testNN;
    QVector<int> testdist;

    for(int i=0;i<igaObject->popSize();i++){
        double dist=population[id].distanceMulti[i]; //todo, try change so "0" is 0.5, and add right click?
        double fitness=0.5 + (isRight?-1:1)*dist/2;
        testdist.append(dist > -5 ? dist*100 : -600);
        //if(dist!=1) fitness-=0.2; //-0.2, even if very similar, we dont want unselected to get approx the same value as those selected
        if(pRand(dist)){//fitness>qrand()){
            testNN.append(i);
            noOfNN++;

            if(nineFitness.contains(i)){
                nineFitness[i]=(nineFitness[i]+fitness)/2;//(nineFitness[i]*(2-fitness)+fitness)/2;
            }else{
                nineFitness[i]=fitness;
            }
            //qDebug() << i << "("<<dist<<"): ";//" now have " << nineFitness[i] <<" new:(" << fitness << ")";
        }
    }
    qDebug() << endl;//" (" << noOfNN << " nn)";
    if(nineCounter>=1){//before I used to select all 9 in order, now just one, the rest is considered to be bad.
        //qDebug() << endl << "the individs not selected get 0.5 with probability p0";
        for(int i=0;i<9;i++){
            if(pRand(0.9) && nineList[i] != id){ //why doing this for the one choosen? because fuck you choosen one... ?
                if(!nineFitness.contains(nineList[i])){
                    nineFitness[nineList[i]]=0.5;
                }else{
                    nineFitness[nineList[i]]=(nineFitness[nineList[i]]+0.5)/2;
                }
            }
            //qDebug() << " : "<< nineList[i] << "now have" << nineFitness[nineList[i]] ;
        }

        qDebug() << endl;
        for(int i=0;i<igaObject->popSize();i++){
            if(nineFitness.contains(i)){
                int tmpp=nineFitness[i]*100;

                qDebug() << tmpp << "\t|";
            }else{
                qDebug() << "NaN" << "\t|";
            }
        }
        qDebug() << endl;
        for(int i=0;i<igaObject->popSize();i++){
            if(i==id){
                qDebug() << (isRight?"R":"L") << "\t| ";
            }else if(nineList.contains(i) && testNN.contains(i)){
                qDebug() << "N9N" << "\t| ";
            }else if(nineList.contains(i)){
                qDebug() << "9" << "\t| ";
            }else if(testNN.contains(i)){
                qDebug() << "NN" << "\t| ";
            }else{
                qDebug() << "  " << "\t| ";
            }
        }
        qDebug() << endl;
        for(int i=0;i<igaObject->popSize();i++){
            qDebug() << testdist[i] << "\t|";
        }
        qDebug() << endl << nineFitness.size() << "out of" << igaObject->popSize() << "is selected" << endl;
        nineCounter=0;
        if(nineFitness.size()>=igaObject->popSize()){
            qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
            setEnabled(false);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            for(int i=0;i<igaObject->popSize();i++){
                population[i].mBentoTree->mRootBlock->mDOMNode.webFrame()->setZoomFactor(1);
                QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            }
            //send all to next generation. for now loop next individual
            addToAncestryTree();
            for(int i=0;i<nineFitness.size();i++){
                igaObject->nextIndividual(nineFitness[i]);
                QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            }
            addNewGenAncestryTree();
            addToHtmlDistance();
            qDebug()<<"new generation" << endl;
            updateColorTable();
            nineFitness.clear();
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            for(int i=0;i<igaObject->popSize();i++){
                population[i].mBentoTree->mRootBlock->mDOMNode.webFrame()->setZoomFactor(0.5);
                QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
            }
            qApp->restoreOverrideCursor();
            setEnabled(true);
        }
        nineGenerateNew();
    }
}
//#####################################################################
// Function nineGenerateNew
// takes 9 individual for the 3x3 selection
//#####################################################################
void Window::nineGenerateNew(){
    nineList.clear();
    for(int i = 0; i < 9; i++){
        int r;
        double maxfit;
        bool noSame;
        bool preferNear;
        bool preferNew;
        do{//while()
            do{r = rand()%(int)igaObject->popSize();}while(nineList.contains(r)); //get unique r
            maxfit=0;
            for(int j=0;j<i;j++){
                double fitness=population[r].distanceMulti[nineList[j]];

                if(fitness>maxfit){
                    maxfit=fitness;
                }
            }
            noSame = maxfit==1 && pRand(0.95);
            preferNear = pRand(1-maxfit) && pRand(0.95);
            preferNew = nineFitness.contains(r) && pRand();
            /*if(maxfit==1){
                qDebug() << endl << r << " exact copy exist in the 9";
                if(noSame) qDebug() << "but ok this time";
            }
            if(preferNear)
                qDebug() <<endl << "with a value of" << maxfit << "," << r << "was too close this time";
            if(preferNew)
                qDebug() << endl << r << " was selected before, 50% chance of be picked, so no luck this time";*/
        }while(noSame || preferNear || preferNew);//(maxfit==1 && pRand(0.95)) || pRand(0.55-(maxfit/2) || nineFitness.contains(r) && pRand()));
        //If maxfit is 1(i.e exact copy of one we already have, only have a small chance to pick it(this is due to
        //in some cases when most of the population is the same, we migth have no other candidates.(should instead
        //be replaced by a blank page when voting?
        //if(maxfit==1) r=-1; <-- something like this maybe? but need to do more as this value is directly used
        //as index below, maybe a getwebpage function that return an empty page if -1?
        //2nd part will prefer individs that's similar to to the ones already selected, and the 3rd will prefer those not yet selected.


        //This function did had a prefference of choosing more distant individuals.. BUT, what we really want is too choose between closely related that's not too close? rigth?
        /*do{//while()
            do{r = rand()%(int)igaObject->popSize();}while(nineList.contains(r)); //get unique r
            maxfit=0;
            for(int j=0;j<i;j++){
                double fitness=population[r].distanceMulti[nineList[j]];
                if(fitness>maxfit){
                    maxfit=fitness;
                }
            }
        }while(pRand(0.55+(maxfit/2)));*///if maxfit==1(same as the selected), dont choose same, if maxfit<0, always select
        //formula used is 0.05+(maxfit/2)+0.5, 0.05 is if using small population, and the only available individuals is equal to one choosed, this would end in
        //infinite loop. not the best solution as it may waste time if many similar, but good enough. "/2)+0.5" makes the lines less steep while still start at same point.
            //if(nineFitness.contains(r) && pRand()) continue; //if done before, only 0.5 chance to keep

        //double fitness=population[id].distanceMulti[i];

        qDebug() << r << "selected";
        nineList.append(r);
    }
    qDebug() << "9 new selected!" << nineList;
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
}
//#####################################################################
// Function init
// init some settings and stuff
//#####################################################################
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
    //settings.insert("S_BasicCrossProbaValue",S_BasicCrossProbaValue);
    settings.insert("S_BasicCrossKeepLengthProb",S_BasicCrossKeepLengthProb);
    settings.insert("S_gaussLevel",S_gaussLevel);
    settings.insert("S_gaussSigma",S_gaussSigma);
    settings.insert("S_Selective_Mut_Color",S_Selective_Mut_Color);
    settings.insert("S_Selective_Mut_Size",S_Selective_Mut_Size);
    settings.insert("S_Selective_Mut_Pos",S_Selective_Mut_Pos);
    settings.insert("S_Selective_Mut_Text",S_Selective_Mut_Text);
    settings.insert("S_Selective_Mut_Border",S_Selective_Mut_Border);
    settings.insert("S_Selective_Mut_Other",S_Selective_Mut_Other);

}
//#####################################################################
// Function saveToIni
//This function will save the settings to ini
//#####################################################################
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
    }
}
//#####################################################################
// Function readini
// Reads settings
//#####################################################################
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

//#####################################################################
// Function closeEvent
// What to do when user try to close the application
//#####################################################################
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

//#####################################################################
// Functions for Distance visualizer
//#####################################################################
void Window::initHtmlDistance(){
    QString header = "<!doctype html>\n<html lang=\"en\">"
             "<head>\n<meta charset=\"utf-8\">\n<title>The HTML5 Herald</title>\n"
             "<meta name=\"description\" content=\"The HTML5 Herald\">\n"
             "<meta name=\"author\" content=\"SitePoint\">\n"
             "</head>\n<body>\n</body>\n</html>";
    htmlDistance.mainFrame()->setHtml(header);
    QFile file(QDir::currentPath() + "/img" + dateTime + "/distance.html");
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream.flush();
        stream << htmlDistance.mainFrame()->toHtml() << endl;
    }
    file.close();
}
void Window::addToHtmlDistance(){
    int imgWidth = 2000/igaObject->popSize();
    int barHigth = imgWidth>50?imgWidth:50;
    QString barHigthstr = QString::number(barHigth);

    QString newhtml="<div id=\"wrapper\">";
    for(int i=0;i<igaObject->popSize();i++){
        newhtml+="<div id=\"wrapper\">";
        newhtml+="<img src=\"" + igaObject->getCurrIName(i) + ".png\" style=\"width:"+QString::number(imgWidth)+"px;\"> : ";
        QList<QPair<double,int> > dList;
        //QList<QPair<double,int> > dListemd;
        for(int j=0;j<igaObject->popSize();j++){
            if(j==i) continue;
            double dist=population[i].distanceMulti[j];
            //if(dist>0){
                dList.append(qMakePair(dist,j));
            //}
        }
        qSort(dList.begin(), dList.end(), QPairFirstComparer());

        for(int id=dList.size()-1;id>=0;id--){

            QString cname="myCanvas"+ QString::number(igaObject->getCurrGen()) + "a" +QString::number(id)+"a" +QString::number(i);
            double fitn=dList[id].first;
            newhtml += QString::number(fitn,'g',3)+"<img src=\"" + igaObject->getCurrIName(dList[id].second) + ".png\" style=\"width:"+QString::number(imgWidth-9)+"px;\" title=\"" + QString::number(fitn) + "\">"
                       "<canvas id=\""+ cname +"\" width=\"5\" height=\""+barHigthstr+"\"></canvas><script>var ca = document.getElementById('"+ cname +"');"
                       "var ct = ca.getContext('2d');ct.beginPath();ct.rect(0, "+barHigthstr+",5, " + QString::number(-fitn*barHigth)+ ")"
                       ";ct.fillStyle = 'red';ct.fill();ct.stroke();</script>\n";
        }
         newhtml+= "</div><br />";
         //For OMD TEST
         //Temp for emd test
         /*
         for(int j=0;j<igaObject->popSize();j++){
             if(j==i) continue;
             double dist=population[i].distance[j];
             //if(dist>0){
                 dListemd.append(qMakePair(dist,j));
             //}
         }

         qSort(dListemd.begin(), dListemd.end(), QPairFirstComparer());

         newhtml+="<div id=\"wrapper\">";
         newhtml+="emd<img src=\"" + igaObject->getCurrIName(i) + ".png\" style=\"width:"+QString::number(imgWidth)+"px;\"> : ";
         for(int id=dListemd.size()-1;id>=0;id--){

             QString cname="myCanvas"+ QString::number(igaObject->getCurrGen()) + "b" +QString::number(id)+"b" +QString::number(i);
             double fitn=dListemd[id].first;
             newhtml += QString::number(fitn,'g',3)+"<img src=\"" + igaObject->getCurrIName(dListemd[id].second) + ".png\" style=\"width:"+QString::number(imgWidth-9)+"px;\" title=\"" + QString::number(fitn) + "\">"
                        "<canvas id=\""+ cname +"\" width=\"5\" height=\""+barHigthstr+"\"></canvas><script>var ca = document.getElementById('"+ cname +"');"
                        "var ct = ca.getContext('2d');ct.beginPath();ct.rect(0, "+barHigthstr+",5, " + QString::number(-fitn*barHigth)+ ")"
                        ";ct.fillStyle = 'red';ct.fill();ct.stroke();</script>\n";
         }
         */
         //To here
    }
    newhtml+= "</div><br />---------------<br />";

    htmlDistance.mainFrame()->documentElement().lastChild().appendInside(newhtml);
    QFile file(QDir::currentPath() + "/img" + dateTime + "/distance.html");
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream.flush();
        stream << htmlDistance.mainFrame()->toHtml() << endl;
    }
    file.close();
}
//#####################################################################
// Functios for the tree html file
//#####################################################################
void Window::initAncestryTree(){
    QString header = "<!doctype html>\n<html lang=\"en\">"
             "<head>\n<meta charset=\"utf-8\">\n<title>The HTML5 Herald</title>\n"
             "<meta name=\"description\" content=\"The HTML5 Herald\">\n"
             "<meta name=\"author\" content=\"SitePoint\">\n"
             "</head>\n<body>\n</body>\n</html>";
    AncetryTree.mainFrame()->setHtml(header);



    QFile file(QDir::currentPath() + "/img" + dateTime + "/webocado.html");
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream.flush();
        stream << AncetryTree.mainFrame()->toHtml() << endl;
    }
    file.close();
}

void Window::addToAncestryTree(double f){

    int imgWidth = 1000/igaObject->popSize();
    int barHigth = imgWidth>50?imgWidth:50;

    QString barHigthstr = QString::number(barHigth);

    QString newhtml="<div id=\"wrapper\">";
    //" + QString::number(igaObject->getFitness(i))+ "
    for(int i=0;i<igaObject->popSize();i++){
        double fitn;
        if(!nineFitness.isEmpty()){
            fitn=nineFitness[i];
            //qDebug() << "fitn " << fitn;
        }else{
            fitn=(i==igaObject->popSize()-1) ? f :igaObject->getFitness(i);
            //qDebug() << "boo " << fitn;
        }
        QString cname="myCanvas"+ QString::number(igaObject->getCurrGen()) + "a" +QString::number(i);
        newhtml += "<div style=\"float:left;border:2px solid black\"><img src=\"" + igaObject->getCurrIName(i) + ".png\" style=\"width:"+QString::number(imgWidth-9)+"px;\">"
                   "<canvas id=\""+ cname +"\" width=\"5\" height=\""+barHigthstr+"\"></canvas><script>var ca = document.getElementById('"+ cname +"');"
                   "var ct = ca.getContext('2d');ct.beginPath();ct.rect(0, "+barHigthstr+",5, " + QString::number(-fitn*barHigth)+ ")"
                   ";ct.fillStyle = 'red';ct.fill();ct.stroke();</script></div>\n";
    }
    newhtml+= "</div>";
    //QString newhtml = "<img src=\"" + igaObject->getCurrIName() + ".png\" style=\"width:"+QString::number(imgWidth-4)+"px;\">\n";
    AncetryTree.mainFrame()->documentElement().lastChild().appendInside(newhtml);

}

void Window::addNewGenAncestryTree(){
    int imgWidth = 1000/igaObject->popSize();

    QString newhtml = "\n<br />\n<canvas id=\"myCanvas"+ QString::number(igaObject->getCurrGen()) +"\" height=\"100\" width=\"1000\">\n"
                    "Your browser does not support the HTML5 canvas tag.</canvas>\n"
                    "<script> \n"
                    "var c = document.getElementById(\"myCanvas"+ QString::number(igaObject->getCurrGen()) +"\");\n"
                    "var ctx = c.getContext(\"2d\");\n"
                    "ctx.beginPath();\n";
    for(int i=0;i<igaObject->popSize();i++){

        if(igaObject->getParent(i,false)>=0){ //not elite
            /*if(igaObject->getParent(i,true) == igaObject->getParent(i,false)){ //is same
                newhtml += "ctx.moveTo(" + QString::number(((igaObject->getParent(i,true)+1) *imgWidth)-(imgWidth/2))+",0);\n"
                            "ctx.lineTo(" + QString::number(((i+1)*imgWidth)-(imgWidth/2))+",100);\n"
                            //"ctx.lineWidth=10;"
                            "ctx.stroke();\n";
            }else{*/
                newhtml += "ctx.moveTo(" + QString::number(((igaObject->getParent(i,true)+1) *imgWidth)-(imgWidth*.2))+",0);\n"
                            "ctx.lineTo(" + QString::number(((i+1)*imgWidth)-(imgWidth*.2))+",100);\n"
                            "ctx.stroke();\n";
                newhtml += "ctx.moveTo(" + QString::number(((igaObject->getParent(i,false)+1)*imgWidth)-(imgWidth*.8))+",0);\n"
                            "ctx.lineTo(" + QString::number(((i+1)*imgWidth)-(imgWidth*.8))+",100);\n"
                            "ctx.stroke();\n";
            //}
        }else{ //elite
            newhtml += "ctx.moveTo(" + QString::number(((igaObject->getParent(i,true)+1) *imgWidth)-(imgWidth/2))+",0);\n"
                        "ctx.lineTo(" + QString::number(((i+1)*imgWidth)-(imgWidth/2))+",100);\n"
                        "ctx.stroke();\n";

        }
    }
    newhtml+="</script>\n<br />\n";


    AncetryTree.mainFrame()->documentElement().lastChild().appendInside(newhtml);



    QFile file(QDir::currentPath() + "/img" + dateTime + "/webocado.html");
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream.flush();
        stream << AncetryTree.mainFrame()->toHtml() << endl;
    }
    file.close();
}
