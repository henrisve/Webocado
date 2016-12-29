#include "igamain.h"
#include <QDebug>
#include "qmath.h"
#include "window.h"
#include "BentoBlock.h"
#include <QList>
#include <QHash>
#include <QDoubleSpinBox>
#include  <random>
#include <QColor>
#include "Page.h"
#include <unordered_set>


//For test?

//Now
/* TODO!
 * Important todo
 * 1.
 *
 * now we have all things saved, time to play
 * 1. go to the csssm2 and copy over all the styles
 *    These are in sub-atributes that bento remove, so probably
 *    need to go through all the atributes inside that block that
 *    dont belong to anoter block, maybe just nodes without child is enaugh?
 *    and then check all the styles in those.. maybe it's enaugh to only check
 *    styles thats not in parent? it should inherit that anyway. oh, and seems
 *    like body is not checked?? remember that gave an error? hmmm or maybe
 *    something before body is not there?
 *
 * 3. in the mutation
 *  3.1 randomly change to another                                                            //DONE, hmm sure?
 *  3.2 add other, for example add new font from list of font.                                //maybe, so far we can get fonts from other sites for example
 * 6.add new elements(from other during crossover?)
 * 7.option to skip sorted stylelist, maybe 2, one for number, one for rest                     //for now it works good enough
 * 9.create a re-bento function, that fixes the bentotree after crossover/mutation
 *      like when a element is moved to another place, it should change branch in the tree      //should be easy but time-consuming
 * 10. enable the treelist thing to the left, when an element is selected the user should
 *       be able to maybe set the parameters for this element only. or maybe "freeze" one
 *       element from changing,                                                                 //not time for this, but could be cool.
 * 11. Could it be a benefit to allow different parameters for different populations?           //did i mean on islands if so maybe, but not enough to spend time on.
 * 14. Machine learning to remove bad copies, it will probably be too hard to identify "good" copies?
 *         if we should have the auto like for good copies, we probably need computer vision.   //probably it will be hard because not enough data
 * 16. because we now have orderered, the swap should be removed!
 * 17. add a "selective mutation"                                                                  //done on higher level, can benefit to do it more specific?
 * 19. add "back in time", so if the fitness has dropped alot, it's a chance to turn it back
 *            to prev. version.                                                                    //never seen any other do this, could benefit, but ill skip it.
 * 20. make all positions absolute? this will make it easier for things to change position         // would definutly be good thing, for this the mgeometry can be
 *                                                                                                       used thats in each block, and just set the position with
 *                                                                                                        set style in each block... easy and should work... try it
 *                                                                                                         if bored some day....
 * 21. dynamic population based on the diverity.. for example, if a indisid is simalar to x other, it can
 *          die with a probability (c,x,y,f), where c is a contstant, x is the amount of similar, and y is       // will not do, but other papers shows that it wont
 *          the avrage "similarity value", and f is the inverted fitness.. or something, need more planning          benefit so much
 * 22. posibility to "kill" an island, should be after x generation and if none is seleted(i.e dont always
 *          add elite, probably wait minimum of x generation before this
 * 24. what if we insted of setting a new fitness, update in respect to the old, maybe (0.3*old+0.7*new).
 *          This would make the fitness more stable.
 * 25. for 3x3, the selection should be done winh a discrete_distribution where the weights decreeses more
 *           the closer to 0,or 1. thus, those previuly not selected have highest chance, and those around
 *           0.5 have higher probability than big like or dislike. thus the ones uncertian have bigger chance
 *           to get a more distinct fitness near 0 or 1.
 * 26. Now the tree structure is not used for anything.... :(
 */

std::random_device rd;
std::mt19937 rng(rd());

using namespace webocado;
class bricolage::Window;
class bricolage::BentoBlock;
igacado::igacado(){

    //some problem whit the constructor due to
    //calling items before it was made etc..
    //So init below:
}
void igacado::initIgacado(QHash<QString,QDoubleSpinBox*> *set,QHash<QString,QList<double> >*numericLimits, int popSize, QList<bricolage::Page> *pop){
    settings=set;

    fitnessList.clear();
    evolveType.clear();
    parents.clear();
    evolveType.append(qMakePair(settings->value("S_Selective_Mut_Color")->value(),colorKeyList));
    evolveType.append(qMakePair(settings->value("S_Selective_Mut_Size")->value(),sizeKeyList));
    evolveType.append(qMakePair(settings->value("S_Selective_Mut_Pos")->value(),positionKeyList));
    evolveType.append(qMakePair(settings->value("S_Selective_Mut_Text")->value(),textKeyList));
    evolveType.append(qMakePair(settings->value("S_Selective_Mut_Border")->value(),borderKeyList));
    evolveType.append(qMakePair(settings->value("S_Selective_Mut_Other")->value(),otherKeyList));
    updateSettings();

    numLim=numericLimits;
    populationSize=popSize;

    //to make this  correct, use &pop above instead of *pop(and change [0][i] everywhere...)
    population=pop; //turn This into an 2-dimensional??
    qDebug()<< "prepare lists etc, these are done";
    for(int i=0;i<populationSize;i++){

        fitnessList.append(0);//init the list of fitness to all 0
        parents.append(qMakePair(0,0));
        for(int j=0;j<popSize;j++){
            population[0][i].distance.append(1);
            population[0][i].distanceMulti.append(1);
        }
        population[0][i].updateStyleList();
        population[0][i].buildColorList();
        population[0][i].updateColor();
        population[0][i].updateStyles();
        qDebug()<<i;
    }
    currGeneration=0;
    currIndividial=0;
    qDebug()<<"done!" << endl << "mutate";
    mutate();

    for(int i=0;i<populationSize;i++){
        population[0][i].updateColor();
        population[0][i].updateStyles();
        population[0][i].updateStyleList();
        population[0][i].buildColorList();
        population[0][i].updateColor();
        population[0][i].updateStyles();
        population[0][i].saveImage();
        population[0][i].createHistogram();

    }
    qDebug()<<"done!" << endl << "calc distances";
    calcHistDist();
    qDebug()<<"done!" << endl;
}
void igacado::updateSettings(){
    evolveType[0].first=settings->value("S_Selective_Mut_Color")->value(); //bah, maybe should have used hash...
    evolveType[1].first=settings->value("S_Selective_Mut_Size")->value();
    evolveType[2].first=settings->value("S_Selective_Mut_Pos")->value();
    evolveType[3].first=settings->value("S_Selective_Mut_Text")->value();
    evolveType[4].first=settings->value("S_Selective_Mut_Border")->value();
    evolveType[5].first=settings->value("S_Selective_Mut_Other")->value();

    cRate     = settings->value("S_ColorCross")->value(); //Not exist yet
    mRateCE_t = settings->value("S_ColorFlip")->value(); //0.002
    mRateCE_n = settings->value("S_ColorNew")->value(); //0.005
    ColorRand = settings->value("S_ColorRand")->value(); //1/40
    ColorRot = settings->value("S_ColorRot")->value(); //1
    Colorhue_F = settings->value("S_Colorhue_F")->value(); //done
    Colorhue_R = settings->value("S_Colorhue_R")->value();
    Colorhue_M = settings->value("S_Colorhue_M")->value();
    Colorsat_F = settings->value("S_Colorsat_F")->value();
    Colorsat_M = settings->value("S_Colorsat_M")->value();
    Colorsat_R = settings->value("S_Colorsat_R")->value();
    Colorlig_F = settings->value("S_Colorlig_F")->value(); //5
    Colorlig_M = settings->value("S_Colorlig_M")->value();
    Colorlig_R = settings->value("S_Colorlig_R")->value(); //done
    timeFitness = settings->value("S_TimeFitness")->value(); //800
    tournamentSelectionParameter = settings->value("S_evolTs")->value();
    tournamentSize = (int)settings->value("S_evolTsize")->value();
    BasicCrossProba = settings->value("S_BasicCrossProba")->value();
    BasicCrossProbaKey = settings->value("S_BasicCrossProbaKey")->value();
    //BasicCrossProbaValue = settings->value("S_BasicCrossProbaValue")->value();

    BasicCrossKeepLengthProb = settings->value("S_BasicCrossKeepLengthProb")->value();


 swapPropa = settings->value("S_swapPropa")->value();
    sizeChangeProba = (int)settings->value("S_sizeChangeProba")->value();
    sizeChangefitness = (int)settings->value("S_Size_fitness")->value();
    double mean = settings->value("S_Size_mean")->value();
    double stddev  = settings->value("S_Size_std")->value();
    SizeNormRand=distribution(mean,stddev);
    gaussLevel=settings->value("S_gaussLevel")->value();
    gaussSigma=settings->value("S_gaussSigma")->value();

    //todo: move to window: //ehm, why did I write this:/
}
void igacado::updateSettingWidgets(){
    settings->value("S_Selective_Mut_Color")->setValue(evolveType[0].first);
    settings->value("S_Selective_Mut_Size")->setValue(evolveType[1].first);
    settings->value("S_Selective_Mut_Pos")->setValue(evolveType[2].first);
    settings->value("S_Selective_Mut_Text")->setValue(evolveType[3].first);
    settings->value("S_Selective_Mut_Border")->setValue(evolveType[4].first);
    settings->value("S_Selective_Mut_Other")->setValue(evolveType[5].first);
}

/***************************************************/
/***************************************************/
/******************* Main functions ****************/
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/

//#####################################################################
// Function nextindividiual
// show next individual and set
//#####################################################################
void igacado::nextIndividual(double fitness){//bool like,int time){
    updateSettings();
    fitnessList[currIndividial]=fitness;//like ? (1+exp(-double(time)/timeFitness))/2 : (1/(1+exp(-double(time)/timeFitness)))-0.5;
    qDebug()<< "this fitness" << fitnessList[currIndividial] << endl;
    currIndividial++;

    if(currIndividial<populationSize){
        //Just continue.. probably nothing to do here...
    }else{//new generation
        qDebug() << "oooh, this generation is over, time to kill the old guys and hatch those new eggs!" << endl;
        currGeneration++; //for now we wont stop when we get to number of generation
        currIndividial=0;
        nextGeneration();
        qDebug() << "calc Distance ";
        calcHistDist();
        qDebug() << "Done " << endl;
    }
    updateSettingWidgets();

}

//#####################################################################
// Function nextGeneration
// handles everything for the next generation
//#####################################################################
void igacado::nextGeneration(){

    bool atleastOne = false;
    for(int i=0; i<evolveType.size();i++){
        if(evolveType[i].first > 0){
            evolveType[i].first--;// -=50;
            atleastOne=true;
        }else{
            evolveType[i].first =0; //probably not needed...
        }
    }
    if(!atleastOne){
        evolveType[qrand()% evolveType.size()].first=5;

    }


    //QVector< QVector<QColor> > colorCopy;
    QList<double> fitnessCopy = fitnessList;
    qDebug() << "copy the old";
    for(int i=0;i<populationSize;i++){
        population[0][i+populationSize].updatePage(population[0][i]);
    }
    //qDebug() << "ge3" << population[0][1].mBentoTree->mRootBlock->mChildren.size()<< " : " <<population[0][1].mBentoTree->mRootBlock->mChildren.size() << endl;

    qDebug() << "Done, create new population, the following is done:" << endl;
    //QVector<int> newindList;
    //QVector<int> indexsort;
    QList<QPair<int,int> > parentList;
    QVector<int> islandPop;
    QVector<int> islandPopSize;
    QVector<double> islandFitness;
    //OKay, soo, I need to change this.. because of the adding of elite, this wont work. 2 alternatives
    //1: Add list of population to each island
    //2: change how elite works, this should be easiest. then check this again. //I think I did this, so should be done?
    //probably need to redo anyway.. seems strange with the avraging  thing: (islandFitness[island-1]/=(int)islandPopSize[island-1])))
    bool islandMode=true;//move out as a setting
    if(islandMode){
        //QVector<int> populationIsland;
        //calc island pop//calc island fitness
        for(int i=0;i<populationSize;i++){
            int island=population[0][i].mPageID;
            if(island>=islandPop.size()){
                islandPop.append(i);
                islandPopSize.append(1);
                islandFitness.append(fitnessCopy[i]);
            }else{
                islandPopSize[island]++;
                islandFitness[island]+=fitnessCopy[i];
            }
        }
        for(int i=0;i<islandFitness.size();i++){
            islandFitness[i]/=islandPopSize[i];
        }
    }

    //qDebug() << "ge4" << population[0][1].mBentoTree->mRootBlock->mChildren.size()<< " : " <<population[0][1].mBentoTree->mRootBlock->mChildren.size() << endl;

    int previousIsland;
    int prevIndivid=-1;
    double crossIslandProb=0.2; ///////////////////////////  MOVE THIS OUTSIDE!!
    //for(int i=0;i<populationSize-islandFitness.size()+1;i++){
    for(int i=0;i<floor(populationSize/2)*2;i++){
    // for(int i=0;i<populationSize;i++){
        //QList<int> tsIndexlist;
        //QList<double> tsFitnesslist;
        QList<QPair<double,int> > tsIslandlist;
        int randIsland;

        if(islandMode){
            /*if(i<islandFitness.size()*2){
                randIsland=floor(i/2);
                previousIsland=randIsland;
            }else */

            if(isEven(i) || pRand(crossIslandProb)){

                std::vector<int> weights;
                for(int k=0; k<islandFitness.size(); k++) { //This is not needed to be done every time?? move out!
                    weights.push_back(20+islandFitness[k]*10); //100+ makes the weigt "more equal"
                    //qDebug() << weights.back() << " ; ";
                }
                //qDebug() << "woooooend " << endl;

                std::discrete_distribution<> d(weights.begin(), weights.end()); // maybe tournament was better after all
                randIsland=d(rng);
                //                for(int j=0;j<2;j++){ // different tournamentSize;??
                //                    randIsland=qrand()%islandPop.size();
                //                    tsIslandlist.append(qMakePair(islandFitness[randIsland],randIsland));
                //                }
                //                qSort(tsIslandlist.begin(), tsIslandlist.end(), QPairFirstComparer());
                //                randIsland=tournamentSelection(tsIslandlist, tournamentSelectionParameter); // different tournamentSelectionParameter??

                //here we should have tournament
                previousIsland=randIsland;
            }else{
                randIsland=previousIsland;
            }
            //Now we have selected the island we want,
        }
        //qDebug() << "island " << randIsland;
        //This part prepare the list for tournament
        QList<QPair<double,int> > tslist;
        for(int j=0;j<tournamentSize;j++){
            int newind;
            do{
                if(islandMode){
                    newind=((qrand()+j+i)%islandPopSize[randIsland]) + islandPop[randIsland];
                }else{
                    newind=qrand()%populationSize;
                }
            }while(newind == prevIndivid && pRand(0.99));
            //tsIndexlist.append(newind); //todo, better to use qpair.
            //tsFitnesslist.append(fitnessCopy[newind]);
            tslist.append(qMakePair(fitnessCopy[newind],newind));

        }
        qSort(tslist.begin(), tslist.end(), QPairFirstComparer()); //Sort for the tournament
        int newind=tournamentSelection(tslist, tournamentSelectionParameter);
        qDebug() << "newind " << newind << endl;
        prevIndivid=newind;
        if(isEven(i)){
            parentList.append(qMakePair(newind,99));
            parentList.append(qMakePair(99,newind));
        }else{
            int s=parentList.size();
            parentList[s-2].second=newind;
            parentList[s-1].first=newind;
        }


        //
        /*

        //same but sort
        int ji=0;
        for(int j=0;j<newindsort.size();j++){
            if(newind<=newindsort[j]){
                //newindsort.insert(j,newind);
                //indexsort.insert(j,i);
                break;
            }
            ji++;
        }
        newindsort.insert(ji,newind);
        indexsort.insert(ji,i);
        */

    }
    //qDebug() << endl << parentList;
    QList<int> sortList = sortedIndex(parentList);
    //qDebug() << endl << sortList << endl;


    for(int i=0;i<floor(populationSize/2)*2;i+=2){ //rounds down to even number
    //for(int i=0;i<populationSize-islandFitness.size();i+=2){ //for readability, should be other than islandfitness, is just the number of islands
        do{
            //population[0][i].updatePage(population[0][newindsort[i]+populationSize]);
            //population[0][i+1].updatePage(population[0][newindsort[i+1]+populationSize]);
            //fitnessList[i]=fitnessCopy[newindsort[i]]; //the fitness variable should have been in each page object...

            int i1 = sortList[i];//indexsort[i];
            int i2 = sortList[i+1];//indexsort[i+1];
            //int is1= newindsort[i];
            //int is2= newindsort[i+1];
            int p1=parentList[i].first;
            int p2=parentList[i].second;

            population[0][i1].updatePage(population[0][p1+populationSize]);
            population[0][i2].updatePage(population[0][p2+populationSize]);
            qDebug() << "new individs" << i1 <<" and " <<i2 << " from " <<p1<< " and " << p2 << endl;
            fitnessList[i1]=fitnessCopy[p1]; //the fitness variable should have been in each page object...
            fitnessList[i2]=fitnessCopy[p2];

            int s1 = population[0][i1].mColor.size();
            int s2 = population[0][i2].mColor.size();
            int p1start = s1>2 ? (qrand()%(s1-2))+1 : 0;
            int p2start = s2>2 ? (qrand()%(s2-2))+1 : 0;
            int maxlen = s1-p1start < s2-p2start ? s1-p1start : s2-p2start;
            int len = qrand()%maxlen;

            if(pRand(BasicCrossProba)) crossOverBasic(&population[0][i1],&population[0][i2]);
            if(pRand(cRate) && evolveType[0].first>0){
                QVector<QColor> starttemp= population[0][i1].mColor.mid(0,p1start);
                QVector<QColor> starttemp2=population[0][i2].mColor.mid(0,p2start);
                QVector<QColor> midtemp= population[0][i1].mColor.mid(p1start,len);
                QVector<QColor> midtemp2=population[0][i2].mColor.mid(p2start,len);
                QVector<QColor> endtemp= population[0][i1].mColor.mid(p1start+len);
                QVector<QColor> endtemp2=population[0][i2].mColor.mid(p2start+len);
                population[0][i1].mColor.clear();
                population[0][i1].mColor=starttemp + midtemp2 + endtemp;
                population[0][i2].mColor.clear();
                population[0][i2].mColor=starttemp2 + midtemp + endtemp2;
            }

            mutate(i1); //should it really be i1/i2?? should be just i/i+1
            mutate(i2);

           // qDebug() << endl << "pop " << i << " is now " << newindsort[i] << ". we update " << i1 << endl;
            //() << endl << "pop " << i+1 << " is now " << newindsort[i+1] << ". we update " << i2 << endl;
            int parent1=p1;
            int parent2=p2;
            if(parent2>parent1){ //just to make the graph look nicer without crossings
                int tmp=parent2;
                parent2=parent1;
                parent1=tmp;
            }


            //qDebug() << "new individs" << i1 <<" and " <<i2 << " from " <<newindsort[i1]<< " and " << newindsort[i2] << endl;
            population[0][i1].pID=(QString::number(currGeneration) + "_" + QString::number(i1) + "_" + QString::number(p1) + "+" + QString::number(p2));
            population[0][i2].pID=(QString::number(currGeneration) + "_" + QString::number(i2) + "_" + QString::number(p2) + "+" + QString::number(p1));
            parents[i1].first=parent1;
            parents[i1].second=parent2;
            parents[i2].first=parent1;
            parents[i2].second=parent2;

            /*
                 * feed the ml with the data here
                 * create a ml-object?
                 *
                 *
                 */

        }while(false); //insert the Machine learning here? so if the ml think the new page is bad it will create a new page instead.
    }
    //qDebug() << "ge6" << population[0][1].mBentoTree->mRootBlock->mChildren.size()<< " : " <<population[0][1].mBentoTree->mRootBlock->mChildren.size() << endl;

    /* Find elites
     */
    QList<double> elites;
    QList<int> elitesindex;
    for(int i=0;i<islandFitness.size();i++){ //I should have used some variable for number of islands....
        elites.append(0);
        elitesindex.append(0); //is there a better way to fill??
    }
    for(int i=0;i<popSize();i++){
        int island=population[0][i+popSize()].mPageID;
        if(fitnessCopy[i]>elites[island]){
            elites[island]=fitnessCopy[i];
            elitesindex[island]=i;
        }
    }

    //todo, dont work because islandpop is before selection, thus we need to recalculate it
   if(islandMode){
       islandPop.clear();
       islandPopSize.clear();
       //QVector<int> populationIsland;
       //calc island pop//calc island fitness
       //test a better way
       for(int i=0;i<populationSize;i++){
           int island=population[0][i].mPageID;
           if(island>=islandPop.size()){
               islandPop.append(i);
               islandPopSize.append(1);
           }else{
               islandPopSize[island]++;
           }
       }
       if(islandPop.size()<elitesindex.size()){
           islandPop.append(populationSize-1);
           islandPopSize.append(0);
       }
 //      int tmpi=0;
 //      for(int i=0;i<populationSize;i++){
 //          int island=population[0][i].mPageID;
 //         if(tmpi<island){ //new island found!
 //
 //          }
 //      }
   }
    for(int i=0;i<elitesindex.size();i++){
        int index=islandPop[i]+islandPopSize[i]-1;
        //qDebug() << "pop" << islandPop[i] << "+" << islandPopSize[i]<< "=" << index << endl;
        //population[0][elitesindex[0]+popSize()].mColor[1].setRgb(255,0,255,200);
        population[0][index].updatePage(population[0][elitesindex[i]+popSize()]);
        population[0][index].pID = QString::number(currGeneration) + "_" + QString::number(index) + "_E" + QString::number(i) + "_" + QString::number(elitesindex[i]);
        parents[index].first=elitesindex[i];
        parents[index].second=-1;

    }
    qDebug() << "Done " << endl;
    qDebug()<<"update styles,save image and create histogram,";
    for(int i=0;i<populationSize;i++){

        population[0][i].updateColor();
        population[0][i].updateStyles();
        population[0][i].updateStyleList();
        population[0][i].buildColorList();
        population[0][i].updateColor();
        population[0][i].updateStyles();

        population[0][i].saveImage();
        population[0][i].createHistogram();
    }
//    int i=1;
//        int index=popSize()-elitesindex.size()+i;
//        population[0][index].updatePage(population[0][elitesindex[i]+popSize()]);
//    //}

    qDebug() << "Done " << endl;
}


//#####################################################################
// Function tournamentSelection
// Takes a list of index and return the winner, list should be ordered
// todo: could probably be done better, and, the fist one does not
//       need array[i].first anywhere, so should only keep the int? or
//       am I wrong again?
//#####################################################################
int igacado::tournamentSelection(QList<QPair<double,int> > array, double tournamentSelectionParameter){
    for(int i=(array.size()-1);i>=0;i--){
        if(pRand(tournamentSelectionParameter)) return array[i].second;
    }
    return array[0].second;
}

//#####################################################################
// Function CrossoverBasic
// Do basic crossover for allitems
//#####################################################################
void igacado::crossOverBasic(bricolage::Page* mPage1,bricolage::Page* mPage2/*,bool keep_length*/){
    QVector<QString> keys;//Only keeps keys that are in both.
    QStringList keysEvol = getEvolKeys();
    QStringList keysPage1 = mPage1->getKeys();
    QStringList keysPage2 = mPage2->getKeys();

//    foreach(QString key3, keysEvol)
//        foreach (QString key, keys) {//This doesnt work....
//            if(pRand(swapPropa)){
//                int size= mpage->ComputedStyleList[key].size();
//                if(bentoBlock->mComputedStyles.contains(key)){
//                    //qDebug() << "index before" <<bentoBlock->mComputedStyles[key].first << "size acc to old " << bentoBlock->mComputedStyles[key].second<< "real size" << size << endl;
//                    bentoBlock->mComputedStyles[key].first= qrand()%size;
//                }
//            }

//    foreach(QString key3, keysEvol)
//        //todo, replace with .contians
//        foreach(QString key1, mPage1->getKeys()){

//            if(key3!=key1){
//                continue;
//            }
//            qDebug() << key1 << "match in 1" << key3 <<endl;

//            foreach(QString key2, mPage2->getKeys()){

//                if(key1==key2){
//                    qDebug() << key1 << "match both!!" <<endl;
//                    keys << key1;
//                    break;
//                }
//            }
//            qDebug() << key3 << "Done!!" <<endl;
//            break;
//        }


 //   foreach(QString key,keys){
    //qDebug() << endl << currGeneration << endl;
    foreach(QString key, keysEvol){


        if(pRand(BasicCrossProbaKey) && (keysPage1.contains(key) || keysPage2.contains(key))){
            //qDebug() << key << " - basic cross thing" ;
            if (keysPage1.contains(key) && !keysPage2.contains(key)){ //If key only exist in one, copy to the other.
                 mPage2->ComputedStyleList[key]=mPage1->ComputedStyleList[key];
            }else if(!keysPage1.contains(key) && keysPage2.contains(key)){
                mPage1->ComputedStyleList[key]=mPage2->ComputedStyleList[key];
            }else{
                QVector<QString> list1=mPage1->ComputedStyleList[key];
                QVector<QString> list2=mPage2->ComputedStyleList[key];
                int s1 = list1.size();
                int s2 = list2.size();
                if((s1<2 && s2<2) || pRand(0.5)){ //if short, just swap whole list. "==1"  //Todo: Add parameter for keylevelCrossover
                   // qDebug() << "- swap whole:" << s1 << "-" << s2 << "-";
                    if(!pRand(BasicCrossKeepLengthProb) || s1==s2){
                        //qDebug() << "yes" << endl;
                        mPage1->ComputedStyleList[key]=list2;
                        mPage2->ComputedStyleList[key]=list1;
                    }
                }else{
                    int p1start = qrand()%(s1);
                    int p2start = qrand()%(s2);
                    int len1;
                    int len2;
                    if(pRand(BasicCrossKeepLengthProb)){
                        int maxlen = s1-p1start < s2-p2start ? s1-p1start : s2-p2start;
                        len1 = qrand()%maxlen;
                        len2 = len1;
                    }else{
                        len1= qrand()%(s1-p1start);
                        len2= qrand()%(s2-p2start);
                    }

                    QVector<QString> list1n=list1.mid(0,p1start) + list2.mid(p2start,len2) + list1.mid(p1start+len1);
                    QVector<QString> list2n=list2.mid(0,p2start) + list1.mid(p1start,len1) + list2.mid(p2start+len2);
                    qSort(list1n.begin(),list1n.end(),letssThan);
                    qSort(list2n.begin(),list2n.end(),letssThan);
                    //Maybe the insert double wont work planned
                    //ex. [1,2,3,4] => [1,1,2,2,3,4] for size 6.
                    //
                    for(int j=0; 0 < (s1-list1n.size()); j++){
                        int pos=(j*2)%list1n.size();
                        list1n.insert(pos,list1n[pos]);
                    }
                    for(int j=0; 0 < (s2-list2n.size()); j++){
                        int pos=(j*2)%list2n.size();
                        list2n.insert(pos,list2n[pos]);
                    }
                    mPage1->ComputedStyleList[key]=list1n;
                    mPage2->ComputedStyleList[key]=list2n;
                }
            }
        }
    }
}
//#####################################################################
// Function Mutate
// calls mutate color and mutate the other stuff
//#####################################################################
void igacado::mutate(int i){
    mutateElementColor(&population[0][i]);  //does it matter if this is here or end of this function?

    if(evolveType[0].first>0) mutateColor(i);
    QStringList keysEvol = getEvolKeys();

    foreach(QString key,keysEvol){
        //todo replace with .contains
        foreach(QString key2, population[0][i].getKeys()){
            if(key==key2){
                if(pRand(sizeChangeProba)){
                    mutateNumeric(&population[0][i],key,i);
                }
                break;
            }
        }
    }

}
//#####################################################################
// Function MutateNumeric
// Mutates styles that dont have a special function   ##Maybe should be numerical mutate, and use all numerical things in in??
//#####################################################################
void igacado::mutateNumeric(bricolage::Page* mPage, QString key, int unicorn){
    double cMDiff = (1-fitnessList[unicorn]);
    int s=mPage->ComputedStyleList[key].size();
    for(int i=0;i<s;i++){

        QList<QPair<double,QString> > newintlist= getNumberFromQString(mPage->ComputedStyleList[key][i]);//
        if(!newintlist.isEmpty()){ //only numerics.
            QString newstr;
            for(int j=0;j<newintlist.size();j++){
                QPair<double,QString> newint =newintlist[j];
                int newNumber = newint.first + SizeNormRand(rng) * (1+cMDiff*sizeChangefitness);

                //   Uncomment these lines to add new limits from the websites...
                //                if(!numLim->contains(key)){
                //                    QList<double> templist;
                //                    templist.append(newNumber);
                //                    templist.append(newNumber);
                //                    numLim->insert(key,templist);
                //                }else{
                //                if(newNumber < numLim->value(key)[0]){
                //                        QList<double> templist;
                //                        templist.append(newNumber);
                //                        templist.append(numLim->value(key)[1]);
                //                        numLim->remove(key);
                //                        numLim->insert(key,templist);
                //                    }
                //                    if(newNumber > numLim->value(key)[1]){
                //                        QList<double> templist;
                //                        templist.append(numLim->value(key)[0]);
                //                        templist.append(newNumber);
                //                        numLim->remove(key);
                //                        numLim->insert(key,templist);
                //                    }
                //                }
                newstr += QString::number(limMinMax(newNumber,key)) + newint.second+ " ";

            }
            mPage->ComputedStyleList[key].replace(i, newstr);
        }
    }
}

/***************************************************/
/***************************************************/
/****************** Color functions ****************/
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/

//#####################################################################
// Function MutateColor
// Mutates the color pallet
//#####################################################################
void igacado::mutateColor(int i){
    if(pRand(ColorRot)){
        double cMDiff = (1-fitnessList[i]);
        //Todo: change all these to gaussian!!
        int huep=Colorhue_R + (cMDiff*Colorhue_F);
        int hue = Colorhue_M;
        if(huep!=0) hue += ((qrand()%(huep*2))-huep);

        int satp=Colorsat_R + (cMDiff*Colorsat_F);
        int sat= Colorsat_M;
        if(satp!=0) sat += ((qrand()%(satp*2))-satp);
        int ligp=Colorlig_R + (cMDiff*Colorlig_F);
        int lig= Colorlig_M;
        if(ligp!=0) lig += ((qrand()%(ligp*2))-ligp); //Should this depend on current values??
        rotateColor(&population[0][i], -1 , hue, sat, lig);
    }

    if(pRand((1-fitnessList[i])*ColorRand)) { //todo parameter for how much the fitness affect is needed
        rotateColor(&population[0][i],-1,qrand() % 360,(qrand() % 200)-100,(qrand() % 200)-100);
    }
}



//#####################################################################
// Function rotatecolor
// Rotate color in the hsl, index -1 rotates all.
// Inputs is the change, e.g dh=180, rotates color 180 degre from
// current color.
//#####################################################################
void igacado::rotateColor(bricolage::Page *mPage,int index,int dh,int ds, int dl){

    if(index < 0){
        for(int i = 1; i < mPage->mColor.size();i++){ //skip first, thats the tranparant.
            int h=(dh + mPage->mColor[i].hslHue() )%359; //360 including 0
            h= ((h < 0) ? h+359 : h);  //modulus in c++ return negative number
            int s=(ds + mPage->mColor[i].hslSaturation() ); //should these be like "wheel, or -5 be threted as 0?
            dl *= pRand(0.7) ? 1 : -1; //30% chance  to change the sign, some elements darker and some ligther.
            int l=(dl + mPage->mColor[i].lightness() ); // todo, try without %, migth be added in the qcolor already
            s = (s>255 ? 255 : (s<0 ? 0 : s));
            l = (l>255 ? 255 : (l<0 ? 0 : l));//max-min, to keep s in limit 0-255
            mPage->mColor[i].setHsl(h,s,l,mPage->mColor[i].alpha());
        }
    }else{
        if(index > mPage->mColor.size()) index = 0;
        int h=(dh + mPage->mColor[index].hslHue() %359); //360 including 0
        int s=(ds + mPage->mColor[index].hslSaturation() )% 255; //should these be like "wheel, or -5 be threted as 0?
        int l=(dl + mPage->mColor[index].lightness() )% 255; // todo, try without %, migth be added in the qcolor already
        s = (s>255 ? 255 : (s<0 ? 0 : s));
        l = (l>255 ? 255 : (l<0 ? 0 : l));//max-min, to keep s in limit 0-255
        mPage->mColor[index].setHsl(h,s,l,mPage->mColor[index].alpha());
    }
}

//#####################################################################
// Function newColor
// Add colors so the pallet has <colorSize> number of colors.
// Now it just takes a random existing and change the ligthness
// Other methods could be, adjent colors, complementary colors etc.
//#####################################################################
void igacado::newColor(QVector<QColor>* currColors,int colorSize){
    //int nNew = colorSize - currColors->size();
    int nOld =currColors->size();
    for(int i=nOld;i<colorSize;i++){
        if(pRand()){ //Todo, add parameter to gui
            currColors->append(currColors[0][qrand()%nOld].lighter(qrand()%200+20)); //do we want this as parameter?? should also be able to add color?
        }else{
            currColors->append(QColor(qrand()%255,qrand()%255,qrand()%255,qrand()%255));//totally new color

        }
    }

}

//#####################################################################
// Function mutateElementColor (mutate all, not just color anymore
// For each , change its link to the color list randomly
// Todo, move to Bentoblock
//#####################################################################
void igacado::mutateElementColor(bricolage::Page *mpage, bricolage::BentoBlock* bentoBlock, int ColorSize){

    //Todo, add for other than color
    //I thougt this is done before.. but cant find it????
    QStringList keys=getEvolKeys();
    double gaussFactor=gaussian(bentoBlock->mLevel);
    foreach (QString key, keys) {//This doesnt work....
        if(bentoBlock->mComputedStyles.contains(key)){
            if(pRand(swapPropa*gaussFactor)){ // todo, change the name to mutateelement or something
                int size= mpage->ComputedStyleList[key].size();
                //qDebug() << "index before" <<bentoBlock->mComputedStyles[key].first << "size acc to old " << bentoBlock->mComputedStyles[key].second<< "real size" << size << endl;
                bentoBlock->mComputedStyles[key].first= qrand()%size;
            }
        }
    }
    //above other

    //This part does something.. I really need to track what Im doing..
//    if(bentoBlock->mDomNodeID!=-1){
//        //Why is this here??
//        if(bentoBlock->mDomNodeID < mpage->mDOMNodes.size()){
//            QWebElement temp = mpage->mDOMNodes[bentoBlock->mDomNodeID];
//            bentoBlock->mDOMNode = temp;
//            //test
//        }
//    }

    //below, color
    if(evolveType[0].first>0){
        if(pRand(mRateCE_n/*/ColorSize*/)){//Todo: is it possible to make it depend on size smarter way
            if(pRand()) bentoBlock->mbgColor=ColorSize;
            else bentoBlock->mtColor=ColorSize;
            newColor(&mpage->mColor);
            ColorSize++;
        }else{

            if(pRand(mRateCE_t*gaussFactor)) bentoBlock->moutlineColor=(qrand() % (ColorSize-1))+1; //-1))+1, we dont want text to be tranparant
            if(pRand(mRateCE_t*gaussFactor)) bentoBlock->mlinkColor=((qrand()+1) % (ColorSize-1))+1;  //gives error if colorsize 1, but should be impossible to be?
            if(pRand(mRateCE_t*gaussFactor)) bentoBlock->mborderColor=((qrand()+2) % (ColorSize-1))+1;
            if(pRand(mRateCE_t*gaussFactor)) bentoBlock->mbgColor=((qrand()+3) % (ColorSize)); //becauce for some reason qrand likes to give the same value multiple time..
            if(pRand(mRateCE_t*gaussFactor)){
                int trueBgColor=bentoBlock->mbgColor;
                bricolage::BentoBlock* parentTempBlock=bentoBlock;
                while(trueBgColor == 0/* && bentoBlock->mParent != 0*/){
                    if(parentTempBlock->mParent == 0) break;
                    parentTempBlock = parentTempBlock->mParent;
                    trueBgColor=parentTempBlock->mbgColor;
                }
                //if bentoBlock->mbgColor==
                do{
                    bentoBlock->mtColor=(qrand()+4) % (ColorSize);
                }while(bentoBlock->mtColor==trueBgColor && ColorSize > 1); //don't allow same text as background
            }
        }
    }
    for (int i=0; i<bentoBlock->mChildren.size(); i++) {
        mutateElementColor(mpage,bentoBlock->mChildren[i],ColorSize);
    }


}
/***************************************************/
/***************************************************/
/****************** Element functions **************/
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/


//How to do this?????
//void igacado::replace(bricolage::Page* oldPage,bricolage::Page* newPage){
//    oldPage->mBentoTree->mHeight=newPage->mBentoTree->mHeight;
//    oldPage->mBentoTree->mNumNodes=newPage->mBentoTree->mNumNodes;
//    oldPage->mBentoTree->mPostOrderList=newPage->mBentoTree->mPostOrderList;
//    oldPage->mBentoTree->mRootBlock=newPage->mBentoTree->mRootBlock;
//    oldPage->mColor=newPage->mColor;
//   // oldPage->mDOMNodes

//    //oldPage->mDOMNodes=newPage->mDOMNodes;
//    //oldPage->mPageID=newPage->mPageID;
//    //oldPage->mURL=newPage->mURL;

//}

//#####################################################################
// Function fullSize
// return the number of nodes in the tree. (is there a better way to do this?)
//#####################################################################
//void igacado::moveElements(){

//}




/***************************************************/
/***************************************************/
/************* Misc. help functions ****************/
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/
double igacado::limMinMax(double value,double min,double max){
    return(value>max ? max : (value<min ? min : value));
}
double igacado::limMinMax(double value,QString key){
    if(numLim->contains(key)){
        return limMinMax(value,numLim->value(key)[0],numLim->value(key)[1]);
    }
    return value;
}
QList<int> igacado::sortedIndex(QList<QPair<int,int> > parentList){
    QList<int> listout;
    int len=parentList.size();
    for(int i=0;i<len;i++){
        listout.append(0);
    }


    for(int i=0;i<len-1;i++){
        for(int j=i+1;j<len;j++){
            if(parentList[i].first > parentList[j].first){
                listout[i]++;
            }else if(parentList[i].first < parentList[j].first){
                listout[j]++;
            }else{
                if(parentList[i].second > parentList[j].second){
                    listout[i]++;
                }else{
                    listout[j]++;
                }
            }
        }
    }
    return listout;
}

void igacado::calcHistDist(){
    QVector<QVector<QVector<QVector<int> > > > h1;
    QVector<QVector<QVector<QVector<int> > > > h2;
    int windows = population[0][0].histogram.size();
    int bins = population[0][0].histogram[0][0][0].size();
    int binPenalty=5;

    for(int i=0;i<popSize()-1;i++){
        bricolage::Page* p1 = &population[0][i];
        //qDebug() << i ;
        h1=population[0][i].histogram;
        for(int j=i+1;j<popSize();j++){
            bricolage::Page* p2 = &population[0][j];
            //for each populaion with eachother, remember dij=dji
            h2=population[0][j].histogram;
            double dist=0;
            for(int wx=0;wx<windows;wx++){
                for(int wy=0;wy<windows;wy++){
                    double sumdist=0;
                    for(int irgb=0;irgb<3;irgb++){
                        //uint maxdist=0;
                        //uint d=1;
                        double distarea=0;
                        for(int bin=0;bin<bins;bin++){
                            int d=1;
                            int d1=0;
                            int d1min=10000;
                            int d2=0;
                            int d2min=10000;
                            int ic=0;

                            for(int ic=0;ic<d;(ic>0?ic=-ic:ic=-ic+1)){
                                if(bin+ic<bins && bin+ic >=0){
                                    d1=abs(h1[wx][wy][irgb][bin]-h2[wx][wy][irgb][bin+ic])+(abs(ic)*binPenalty);
                                    d2=abs(h1[wx][wy][irgb][bin+ic]-h2[wx][wy][irgb][bin])+(abs(ic)*binPenalty);
                                    if(d1<d1min) d1min=d1;
                                    if(d2<d2min) d2min=d2;
                                    if(d1min<d2min){
                                        d=d2min;
                                    }else if(d1min>=d2min){
                                        d=d1min;
                                    }
                                }
                            }
                            /*d=abs(h1[wx][wy][irgb][bin]-h2[wx][wy][irgb][bin]);
                            if(d>0){

                            }
                            distarea+=d;
                            if(d>maxdist){
                                maxdist=d;
                            }*/
                            distarea+=d;
                        }
                        sumdist+=(distarea/2500)*(distarea/2500);//maxdist;
                    }
                    dist+=sqrt(sumdist);
                }

            }

            dist=1-((dist*2)/(windows*windows)); //1=same, < 0 too far away
            double styleDist=0;
            double styleDist2=0;
            int elementProduct=20;
            //dist now include the histogram distance, now try the other features
            QStringList keys1=p1->ComputedStyleList.keys();
            QStringList keys2=p2->ComputedStyleList.keys();
            QStringList keys=keys1;
            foreach(QString key,keys2){
                if(!keys.contains(key)) keys.append(key);
            }
            foreach(QString key,keys){
                if(!keys1.contains(key)) //ie, only in keys2
                    styleDist += population[0][j].ComputedStyleList[key].size()*elementProduct;
                else if(!keys2.contains(key)) //ie, only in keys2
                    styleDist += population[0][i].ComputedStyleList[key].size()*elementProduct;
                else{//both have the key
                    QSet<QString> textInKeyp1;
                    QList<double> numInKeyp1;
                    QSet<QString> textInKeyp2;
                    QList<double> numInKeyp2;
                    for(int iStyle=0;iStyle<p1->ComputedStyleList[key].size();iStyle++){
                        QList<double> newintlist1= getNumberFromQString2(p1->ComputedStyleList[key][iStyle]);//add to string...
                        if(newintlist1.empty()){
                            textInKeyp1.insert(p1->ComputedStyleList[key][iStyle]);
                        }else{
                            numInKeyp1.append(newintlist1[0]); //not common with many, ignore for now
                        }
                    }
                    for(int iStyle=0;iStyle<p2->ComputedStyleList[key].size();iStyle++){
                        QList<double> newintlist2= getNumberFromQString2(p2->ComputedStyleList[key][iStyle]);
                        if(newintlist2.empty()){
                            textInKeyp2.insert(p2->ComputedStyleList[key][iStyle]);
                        }else{
                            numInKeyp2.append(newintlist2[0]);
                        }

                    }
                    styleDist+=(textInKeyp1.size()+textInKeyp2.size()-2*textInKeyp1.intersect(textInKeyp2).size())*elementProduct;//probably worst way,

                    int s1=numInKeyp1.size();
                    int s2=numInKeyp2.size();
                    double s=s1<s2?s1:s2;
                    double sm=s1>s2?s1:s2;
                    double sum=0;
                    for(int il=0;il<s;il++){

                        if(s1<s2)
                            sum+=abs(numInKeyp1[il]-numInKeyp2[int(il*(sm/s)+(sm)/(2*s))]);
                        else
                            sum+=abs(numInKeyp2[il]-numInKeyp1[int(il*(sm/s)+(sm)/(2*s))]);
                    }

                    if(s>0){
                        styleDist2+=sum/s;
                    }


                }
            }

            population[0][i].distance[j]=dist;
            population[0][j].distance[i]=dist;
            double tmpSD1=1-(styleDist/20);
            double tmpSD2=1-(styleDist2/500);
            population[0][i].distanceMulti[j]=(dist+dist+tmpSD1+tmpSD2)/4; //Check what solution is the best
            population[0][j].distanceMulti[i]=(dist+dist+tmpSD1+tmpSD2)/4;
            qDebug() << endl << i << " to " << j << " has a distance of (only img)" << dist << " (all): " << (dist+dist+tmpSD1+tmpSD2)/4 << " sd1:"<< tmpSD1 << " sd2:" <<tmpSD2 ;
        }
    }
}

//#####################################################################
// Function change Color
// Change to a totaly new color, only used for test purpose
//#####################################################################
void igacado::changeColor(int popIndex,int noOfColors,int r, int g, int b){ //Only test purpose, too big changes?
    for(int i=0;i<populationSize;i++){
        for(int j=0;j<population[0][i].mColor.size();j++){
            population[0][i].mColor[j].setRgb(qrand() % 255,qrand() % 255,qrand() % 255);
        }

    }
}
QStringList igacado::getEvolKeys(){
    QStringList list;
    for(int i=0;i<evolveType.size();i++){
        if(evolveType[i].first>0){ //todo, make the value work as probability, i.e a chance to choose more than 1.
            list << evolveType[i].second;
        }
    }
    return list;
}



//const QStringList igacado::swapList = QStringList()<< "direction" << "display" << "font" << "font-family" << "font-size" << "font-style" << "font-variant" <<
//                                                      "font-weight"  <<"text-rendering"<< "background" <<  "background-attachment" << "background-clip" <<
//                                                      "background-image" << "background-origin" << "background-position" << "background-position-x" << "background-position-y" <<
//                                                      "background-repeat" << "background-repeat-x" << "background-repeat-y" << "background-size" << "border" <<
//                                                      "border-bottom" <<  "border-bottom-left-radius" << "border-bottom-right-radius" <<
//                                                      "border-bottom-style" << "border-bottom-width" << "border-collapse" <<  "border-image" <<
//                                                      "border-image-outset" << "border-image-repeat" << "border-image-slice" << "border-image-source" << "border-image-width" <<
//                                                      "border-left" <<  "border-left-style" << "border-left-width" << "border-radius" << "border-right" <<
//                                                      "border-right-style" << "border-right-width" << "border-spacing" << "border-style" <<
//                                                      "border-top" <<  "border-top-left-radius" << "border-top-right-radius" << "border-top-style" <<
//                                                      "border-top-width" << "border-width" << "bottom" << "box-shadow" << "box-sizing" << "clear" << "clip" << "content" <<
//                                                      "counter-increment" << "counter-reset" << "cursor" << "empty-cells" << "float" << "font-stretch" << "image-rendering" <<
//                                                      "letter-spacing" << "list-style" << "list-style-image" << "list-style-position" << "list-style-type" << "margin" <<
//                                                      "margin-bottom" << "margin-left" << "margin-right" << "margin-top" << "max-height" << "max-width" << "min-height" <<
//                                                      "min-width" << "opacity" << "orphans" << "outline"  << "outline-offset" << "outline-style" <<
//                                                      "outline-width" << "overflow" << "overflow-x" << "overflow-y" << "padding" << "padding-bottom" << "padding-left" <<
//                                                      "padding-right" << "padding-top" << "page" << "page-break-after" << "page-break-before" << "page-break-inside" <<
//                                                      "pointer-events" << "position" << "quotes" << "resize" << "size" << "src" << "speak" << "table-layout" << "tab-size" <<
//                                                      "text-align" << "text-decoration" << "text-indent" << "text-line-through" <<
//                                                      "text-line-through-mode" << "text-line-through-style" << "text-line-through-width" << "text-overflow" << "text-overline" <<
//                                                      "text-overline-mode" << "text-overline-style" << "text-overline-width" << "text-shadow" <<
//                                                      "text-transform" << "text-underline"  << "text-underline-mode" << "text-underline-style" <<
//                                                      "text-underline-width" << "unicode-bidi" << "unicode-range" << "vertical-align" << "visibility" << "white-space" <<
//                                                      "widows" << "word-break" << "word-spacing" << "word-wrap" << "z-index" << "left" << "height"<<"line-height"<<"right"<<"top" << "voice-family";

const QStringList igacado::sizeKeyList = QStringList() << "font-size" << "background-size" <<  "max-height" << "max-width" << "min-height" <<
                                                          "min-width" << "size"  << "height"<<"line-height";

const QStringList igacado::positionKeyList = QStringList() << "bottom"<< "float"<< "background-position" << "background-position-x" << "background-position-y" << "list-style-position"<< "margin" <<
                                                              "margin-bottom" << "margin-left" << "margin-right" << "margin-top" << "padding" << "padding-bottom" << "padding-left" <<
                                                              "padding-right" << "padding-top" <<  "position"<< "left"<<"right"<<"top";

const QStringList igacado::textKeyList = QStringList() << "direction" << "font" << "font-family"<< "font-style" << "font-variant" <<"font-weight" <<"text-rendering"<<
                                                          "font-stretch"<<"letter-spacing"<< "text-align" << "text-decoration" << "text-indent" << "text-line-through" <<
                                                          "text-line-through-mode" << "text-line-through-style" << "text-line-through-width" << "text-overflow" << "text-overline" <<
                                                          "text-overline-mode" << "text-overline-style" << "text-overline-width" << "text-shadow" <<
                                                          "text-transform" << "text-underline"  << "text-underline-mode" << "text-underline-style" <<
                                                          "text-underline-width" << "white-space" << "word-spacing" << "word-wrap";

const QStringList igacado::otherKeyList = QStringList() <<  "display" << "background" <<"table-layout" <<
                                                            "box-shadow" << "box-sizing"<<"vertical-align" << "visibility"
                                                                                                              "cursor"  << "list-style-type" <<"opacity"<<"overflow"<< "overflow-x" << "overflow-y";

const QStringList igacado::borderKeyList = QStringList() <<"border" << "border-bottom"<<  "border-bottom-left-radius" << "border-bottom-right-radius" <<
                                                           "border-bottom-style" << "border-bottom-width" << "border-collapse" <<  "border-image" <<
                                                           "border-image-outset" << "border-image-repeat" << "border-image-slice" << "border-image-source" << "border-image-width" <<
                                                           "border-left" <<  "border-left-style" << "border-left-width" << "border-radius" << "border-right" <<
                                                           "border-right-style" << "border-right-width" << "border-spacing" << "border-style" <<
                                                           "border-top" <<  "border-top-left-radius" << "border-top-right-radius" << "border-top-style" <<
                                                           "border-top-width" << "border-width" << "outline"  << "outline-offset" << "outline-style" <<"outline-width";
//const QStringList igacado::sizeKeyList = QStringList() << "size";
//const QStringList igacado::positionKeyList = QStringList() << "top";
//const QStringList igacado::textKeyList = QStringList() << "font";
//const QStringList igacado::otherKeyList = QStringList() << "display";
//const QStringList igacado::borderKeyList = QStringList() << "border";


const QStringList igacado::colorKeyList = QStringList() <<"color-dummy";

//const QStringList igacado::nouseList = QStringList() <<   <<  "tab-size" << "empty-cells" << "image-rendering" <<"clear" << "clip" << "content" <<"counter-increment" << "counter-reset" << "orphans"<<"background-attachment" << "background-clip" <<"background-image" <<
//                                                         "resize""background-origin" << "quotes" <<  << "background-repeat" << "background-repeat-x" << "background-repeat-y" << "list-style" << "list-style-image"  "pointer-events" <

