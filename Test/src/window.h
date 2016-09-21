//#####################################################################
// Copyright 2012, Ranjitha Kumar.
// This software is governed by the license contained in LICENSE.
//#####################################################################
#ifndef _WINDOW_H_
#define _WINDOW_H_
#include "igamain.h"
class webocado::igacado;


#include <QMainWindow>
#include <QtWebKit>
#include "ui_window.h"
#include "Page.h"
#include "BentoTree.h"
#include "Bento.h"

#include <QTabWidget>

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace bricolage {
	
class Window : public QMainWindow, private Ui::Window
{
    Q_OBJECT

private:

	QWebElement mDocument;
    //Page* mPage;
    //QWebElement mDocument2;
    //Page* mPage2;
    //TODO Hardcoded, needs to be changed in future
    Page* mPage1;
    Page* mPage2;
    Page* mPage3;
    Page* mPage4;
    Page* mPage5; //Move this to igamain??
    Page* cPage1;
    Page* cPage2;
    Page* cPage3;
    Page* cPage4;
    Page* cPage5;

    QHash<QString,QDoubleSpinBox*> settings;
    QHash<QString,QList<double> > numericLimits;
    QVector<QNetworkAccessManager*> managerVector;
    QVector<QWebPage*> webPageVector;
    QVector<QNetworkRequest*> webRequestVector;
    QVector<int> pageId;

    int bc=0; //keeptrack if all networreplies is done


	BentoTree* bentoTree;
    QHash<QTreeWidgetItem*, const BentoBlock*> bentoBlockHash;
    QWebElement selectedElement;

    bool isButtonClicked;

    //ninething
    QVector<int> nineList;
    QHash<int,double> nineFitness;
    int nineCounter=0;
    QVector<QWebPage*> nineNumberPage;
	
public:
    void init();
    bool readini();
    bool saveToIni();
    bool eventFilter(QObject *object, QEvent *event);



    QList<Page> population; //private or public?
    webocado::igacado* igaObject = new webocado::igacado(/*0.2,0.2,5,0.7,2,10,1,&population*/);
    inline bool pRand(double probability=0.5){ return (double)rand()/RAND_MAX < probability;}

    Window(QWidget *parent = 0):QMainWindow(parent){ 
		setupUi(this); 
        QObject::connect(mainWeb_1->page()->currentFrame(), SIGNAL(initialLayoutCompleted()),
                         this, SLOT(on_webView_initialLayoutCompleted())); //TODO, only when ALL wem have loaded. how too?
        // QObject::connect(webView2->page()->currentFrame(), SIGNAL(initialLayoutCompleted()),
        //                  this, SLOT(on_webView_initialLayoutCompleted()));
        button_dislike->setEnabled(false);
        button_like->setEnabled(false);
	}
	
    ~Window(){
        //delete mPage;
   //     delete mPage2;
    }
	
//    void setUrl(const QUrl &url){
//        mainWeb_1->setUrl(url);
//   //     webView2->setUrl(url);
//    }
protected:
    void closeEvent(QCloseEvent *event);// Q_DECL_OVERRIDE;


public slots:
    void nineNewClick(int id);

    void onRequestCompleted(QNetworkReply *reply);

	void on_timer_finished();
    //void on_webView_loadFinished();
    void on_mainWeb_1_loadFinished();
	void on_webView_initialLayoutCompleted();
   // void on_treeWidget_itemSelectionChanged();
   // void on_treeWidget2_itemSelectionChanged();
	void on_pushButton_clicked();
    void button_vote(bool liked);
//    void changeColor(Page *mPage,int index,int r, int g, int b); //remove?



private slots:
    //void on_button_like_pressed();

    //void on_treeWidget_expanded(const QModelIndex &index);

        void on_button_dislike_clicked();

        void on_button_like_clicked();

        void on_fitnessSlider_sliderReleased();

private:
    void testtest(bricolage::Page *mpage, bricolage::BentoBlock* bentoBlock); //remove!!!
    void allreqdone();
    void examineChildElements(const BentoBlock* bentoBlock, QTreeWidgetItem* parentItem);
    void printList(const BentoBlock* bentoBlock);
	void drawSeparators(Bento& bento);

    uint fullSize(const BentoBlock* bentoBlock);
    void crossOver(const BentoBlock* bentoBlock1,const BentoBlock* bentoBlock2);
    const BentoBlock* selectNode(const BentoBlock* bentoBlock,uint ind, uint* currInd);
    const BentoBlock* selectNode(const BentoBlock* bentoBlock,uint ind);
    void insertSibling(BentoBlock* bentoBlock,BentoBlock* newBlock);
    void setChildID(BentoBlock* bentoBlock);
    void popBuildColorList();
    void buildColorList(Page *mPage);
    void buildColorList(BentoBlock *bentoBlock, QVector<QColor>* colorList);
    int colorIndex(QVector<QColor>* colorList, QColor color);
    //void changeColor(Page *mPage/*QVector<QColor>* colorList*/, int r, int g, int b); //For test only??
    //void changeColor(Page *mPage,int index,int r, int g, int b); moved to public
    void rotateColor(Page *mPage,int index,int dh,int ds, int dl);

    bool isUniqueColor(QVector<QColor> colorList, QColor color); //remove later

    void popUpdateColor();
    void updateColor(BentoBlock *bentoBlock,QVector<QColor>* colorList);
    void updateColorTable();
    void addColor(int &colorBlock,QVector<QColor>* colorList, QColor newColor);

    //For 9 tiles thing
    //void nineNewClick(int id);
    void nineGenerateNew();


//#####################################################################
};

//#####################################################################
} // namespace bricolage

#endif // _WINDOW_H_
