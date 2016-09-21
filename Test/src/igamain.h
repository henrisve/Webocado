#ifndef IGAMAIN_H
#define IGAMAIN_H


#include <QList>
#include <QDoubleSpinBox>
#include <random>
#include <QDebug>

#include "Page.h"

//#include "window.h" //Only for test as i cant seem to fix the linkage for changecolor()


namespace webocado{
    class igacado{
    public:
        void mutateColor();
        void mutateColor(int i);
        void crossover();
        void nextIndividual(double fitness);//bool like,int time);
        //void userVote(bool like,int time);
        igacado(/*float mR, float cR, int popSize, float tSelectPara, int tSize, int numGen, int eCopies, QList<bricolage::Page> *pop*/);
        void initIgacado(QHash<QString,QDoubleSpinBox*> *set,QHash<QString,QList<double> > *numericLimits, int popSize, QList<bricolage::Page> *pop);
        //void igaMainf(QList<bricolage::Page>, igaSettings settings);
        int currIndividial;
        inline bool pRand(double probability=0.5){ return (double)rand()/RAND_MAX < probability;}
        void testtest(QWebElement domNode, QWebElement newNode);
        int popSize(){
            return populationSize;
        }



    private:
        static const QStringList swapList;
        static const QStringList sizeList;

        //private function
        void crossOverBasic(bricolage::Page* mPage1, bricolage::Page* mPage2);
        void mutateSize(bricolage::Page *mPage, QString key, bool negativeOk, int unicorn);
        void nextGeneration();
        void changeColor(int popIndex, int noOfColors, int r, int g, int b);
        void rotateColor(bricolage::Page *mPage,int index,int dh,int ds, int dl);
        void replace(bricolage::Page *oldPage, bricolage::Page *newPage);
        void newColor(QVector<QColor>* currColors,int colorSize);
        void updateSettings();
        inline void newColor(QVector<QColor>* currColors) {
            newColor(currColors,currColors->size()+1); //if colorsize is empty, add one.
        }

        void mutateElementColor(bricolage::Page *mpage,bricolage::BentoBlock *bentoBlock, int ColorSize);
        inline void mutateElementColor(bricolage::Page *mpage){
            mutateElementColor(mpage, mpage->mBentoTree->mRootBlock, mpage->mColor.size());
        }
        int tournamentSelection(QList<QPair<double,int> > array, double tournamentSelectionParameter);
        // int tournamentSelection(QList<int> indexlist,QList<double> fitnesslist);

        uint fullSize(const bricolage::BentoBlock* bentoBlock);
        //privade variables

        int currGeneration;
        //private Parameters for iga settings
        double mRate;
        double cRate;
        int populationSize; //due to issues in qt4 (I think), We cant put the pages in a vector, so need to hardcode this.
        double tournamentSelectionParameter;
        int tournamentSize;
        int numberOfGenerations; //max generation until we stop
        int eliteCopies;
        double mRateCE_t; //change to an existing
        double mRateCE_n; //change to a new (i.e add new color)

        double ColorRand;
        double ColorRot;
        double Colorhue_F;
        double Colorhue_R;
        double Colorhue_M;
        double Colorsat_F;
        double Colorsat_M;
        double Colorsat_R;
        double Colorlig_F;
        double Colorlig_M;
        double Colorlig_R;
        double timeFitness;
        double swapPropa;
        double sizeChangeProba;
        double sizeChangefitness;
        double BasicCrossProba;
        double BasicCrossProbaKey;
        double BasicCrossProbaValue;
        double BasicCrossKeepLengthProb;
        struct QPairFirstComparer{
            template<typename T1, typename T2>
            bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const{
                return a.first < b.first;
            }
        };


        QHash<QString,QDoubleSpinBox*> *settings; //if this work remove the other above!!
        QHash<QString,QList<double> > *numLim;

        //fitness etc:
        QList<double> fitnessList;
        QList<bricolage::Page> *population;

        //            //For randomness:

        typedef std::normal_distribution<double> distribution;
        distribution SizeNormRand;
        //add more rand here
        //distribution NormRand;
        //move to .cpp, here because was inline before
        QList<QPair<double,QString> > getNumberFromQString(const QString &str){
            QList<QPair<double,QString> > list;
            int pos = 0;
            QRegExp rx("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?[a-zA-Z%]*)");

            while ((pos = rx.indexIn(str, pos)) != -1) {
                QRegExp rxnum("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");
                QRegExp rxletters("([a-zA-Z%]+)");
                QString s= rx.cap(1);

                rxnum.indexIn(s);
                QString number = rxnum.cap(1);
                rxletters.indexIn(s);
                QString text = rxletters.cap(1);

                QPair<double,QString> newqpair;
                newqpair.first=number.toDouble();
                //                    if(stringout.size()<2){
                //                        newqpair.second="";
                //                    }else{
                newqpair.second = text;
                //  }
                list.append(newqpair);
                //list << rxnum.cap(1).toDouble();
                pos += rx.matchedLength();
            }
            return list;
        }

        static bool letssThan( const QString & e1, const QString & e2 ){

            QList<double> list1=getNumberFromQString2(e1);
            QList<double> list2=getNumberFromQString2(e2);

            if(!list1.isEmpty()&&!list2.isEmpty()){
                qDebug() << endl << e1 << list1[0] << e2 << list2[0] << "is a number" << (list1[0] < list2[0]?"<":">") << endl;
                return list1[0] < list2[0];
            }
            qDebug() << endl << e1 << e2 << "is non number"<< (e1 < e2?"<":">")<< endl;
            return e1<e2;
        }
        static QList<double> getNumberFromQString2(const QString &str){ //Move to a common file..
            QList<double> list;
            int pos = 0;
            QRegExp rx("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");
            while ((pos = rx.indexIn(str, pos)) != -1) {
                list << rx.cap(1).toDouble();
                pos += rx.matchedLength();
            }
            return list;
        }
        bool isEven(int n){ //move to a separate file
            return floor(n/2)*2 == n;
        }
        double limMinMax(double value,double min,double max);
        double limMinMax(double value,QString key);
    };
}
#endif // IGAMAIN_H
