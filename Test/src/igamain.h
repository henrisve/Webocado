#ifndef IGAMAIN_H
#define IGAMAIN_H


#include <QList>
#include <QDoubleSpinBox>
#include <random>
#include "Page.h"
//#include "BentoBlock.h"
//#include "window.h" //Only for test as i cant seem to fix the linkage for changecolor()


namespace webocado{
    class igacado{
    public:
        //public Variable
        int currIndividial;

        //Public function
        igacado();
        void initIgacado(QHash<QString,QDoubleSpinBox*> *set, QHash<QString,QList<double> > *numericLimits, int popSize, QList<bricolage::Page> *pop);

        void mutateFirst();
        inline void mutate(){
            for(int i=0; i<populationSize; i++) mutate(i);
        }
        void mutate(int i);
        void mutateColor(int i);
        void crossover();
        void nextIndividual(double fitness);
        inline bool pRand(double probability=0.5){ return (double)rand()/RAND_MAX < probability;}
        void testtest(QWebElement domNode, QWebElement newNode);
        int popSize(){
            return populationSize;
        }
        QString getCurrIName(int ind){
            return population[0][ind].pID;
        }
        QString getCurrIName(){
            return population[0][currIndividial].pID;
        }
        int getCurrGen(){
            return currGeneration;
        }
        int getParent(int targetID,bool first){
            if(first){
                return parents[targetID].first;
            }else{
                return parents[targetID].second;
            }
        }
        double getFitness(int index){
            return fitnessList[index];
        }

    //just for test, remove later
        bricolage::BentoBlock *rblock(bricolage::BentoBlock* block){
            return rblock(block, block);
        }

        bricolage::BentoBlock *rblock(bricolage::BentoBlock* block, bricolage::BentoBlock *parent);

    private:

        //private function
        QStringList getEvolKeys();
        void crossOverBasic(bricolage::Page* mPage1, bricolage::Page* mPage2);
        void mutateNumeric(bricolage::Page *mPage, QString key, int unicorn);
        void nextGeneration();
        void changeColor(int popIndex, int noOfColors, int r, int g, int b);
        void rotateColor(bricolage::Page *mPage,int index,int dh,int ds, int dl);
        void replace(bricolage::Page *oldPage, bricolage::Page *newPage);
        void newColor(QVector<QColor>* currColors,int colorSize);
        void updateSettings();
        void updateSettingWidgets();
        inline void newColor(QVector<QColor>* currColors) {
            newColor(currColors,currColors->size()+1); //if colorsize is empty, add one.
        }
        int gaussLevel;
        int gaussSigma;
        inline double gaussian(int level){
            int r=(level-gaussLevel);
            return (exp(-(r*r)/gaussSigma));
        }
        void calcHistDist();
        void mutateElement(bricolage::Page *mpage,bricolage::BentoBlock *bentoBlock, int ColorSize);
        inline void mutateElement(bricolage::Page *mpage){
            mutateElement(mpage, mpage->mBentoTree->mRootBlock, mpage->mColor.size());
        }
        int tournamentSelection(QList<QPair<double,int> > array, double tournamentSelectionParameter);
        QList<QPair<double,QString> > getNumberFromQString(const QString &str);
        static QList<double> getNumberFromQString2(const QString &str);
        static bool letssThan( const QString & e1, const QString & e2 );
        bool isEven(int n);
        double limMinMax(double value,double min,double max);
        double limMinMax(double value,QString key);
        QList<int> sortedIndex(QList<QPair<int,int> > parentList);
        double calcemd(QVector<int> Q,QVector<int> P);

        //Private Variables

        static const QStringList positionKeyList;
        static const QStringList sizeKeyList;
        static const QStringList textKeyList;
        static const QStringList otherKeyList;
        static const QStringList borderKeyList;
        static const QStringList colorKeyList;
        //and here shold be color if we did that same way...
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
                if(a.first == b.first) return a.second < b.second;
                return a.first < b.first;
            }
        };


        QHash<QString,QDoubleSpinBox*> *settings; //if this work remove the other above!!
        QHash<QString,QList<double> > *numLim;

        //fitness etc:
        QList<double> fitnessList;
        QList<bricolage::Page> *population;
        QList<QPair<double, QStringList> > evolveType;
        QList<QPair<int, int> > parents; //contians int of each parents

        //For randomness:
        typedef std::normal_distribution<double> distribution;
        distribution SizeNormRand;
        //add more rand here
        //distribution NormRand;

    };
}
#endif // IGAMAIN_H
