//#####################################################################
// Copyright 2016, Henrik K. Svensson.
// Parts of this software is Copyright 2012, Ranjitha Kumar.
// This software is governed by the license contained in LICENSE.
//#####################################################################
#ifndef _WINDOW_H_
#define _WINDOW_H_
#include "igamain.h"
#include <QMainWindow>
#include <QtWebKit>
#include "ui_window.h"
#include "Bento.h"

namespace bricolage {
	
class Window : public QMainWindow, private Ui::Window{
    Q_OBJECT

private:
    //Private Variables
    QString dateTime;
    QHash<QString,QDoubleSpinBox*> settings;
    QHash<QString,QList<double> > numericLimits;
    QVector<QNetworkAccessManager*> managerVector;
    QVector<QWebPage*> webPageVector;
    QVector<QNetworkRequest*> webRequestVector;
    QVector<int> pageId;
    int bc=0; //keeptrack if all networreplies is done
	BentoTree* bentoTree;
    QHash<QTreeWidgetItem*, const BentoBlock*> bentoBlockHash;
    bool isButtonClicked;
    QWebPage AncetryTree;
    QWebPage htmlDistance;

    //ninething
    QVector<int> nineList;
    QHash<int,double> nineFitness;
    int nineCounter=0;
    QVector<QWebPage*> nineNumberPage;
    bool runFlag=false;

    struct QPairFirstComparer{
        template<typename T1, typename T2>
        bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const{
            if(a.first == b.first) return a.second < b.second;
            return a.first < b.first;
        }
    };

    //Private Functions
    void testtest(bricolage::Page *mpage, bricolage::BentoBlock* bentoBlock); //remove!!!
    void allreqdone();
    void examineChildElements(const BentoBlock* bentoBlock, QTreeWidgetItem* parentItem);
    void updateColorTable();
    void initAncestryTree();
    void addToAncestryTree(double f=0);
    void addNewGenAncestryTree();
    void initHtmlDistance();
    void addToHtmlDistance();


    //For 9 tiles thing
    void nineGenerateNew();
    QList<Page> population; //private or public?
    webocado::igacado* igaObject = new webocado::igacado(/*0.2,0.2,5,0.7,2,10,1,&population*/);
public:
    void init();
    bool readini();
    bool saveToIni();
    bool eventFilter(QObject *object, QEvent *event);
    inline bool pRand(double probability=0.5){ return (double)rand()/RAND_MAX < probability;} //<0 always false, >1 always true
    Window(QWidget *parent = 0):QMainWindow(parent){
        setupUi(this);
        QObject::connect(mainWeb_1->page()->currentFrame(), SIGNAL(initialLayoutCompleted()),
                         this, SLOT(on_webView_initialLayoutCompleted())); //TODO, only when ALL have loaded. how too?
        button_dislike->setEnabled(false);
        button_like->setEnabled(false);
    }
    ~Window(){
    }

protected:
    void closeEvent(QCloseEvent *event);// Q_DECL_OVERRIDE;


public slots:
private slots:
    void nineNewClick(int id, bool isRight);
    void onRequestCompleted(QNetworkReply *reply);
	void on_timer_finished();
    void on_mainWeb_1_loadFinished(); //not a slot anymore?
	void on_webView_initialLayoutCompleted();
	void on_pushButton_clicked();
    void button_vote(bool liked);
    void on_button_dislike_clicked();
    void on_button_like_clicked();

//#####################################################################
};

//#####################################################################
} // namespace bricolage

#endif // _WINDOW_H_
