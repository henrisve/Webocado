#include "igamain.h"
#include <QDebug>
#include "qmath.h"
#include "window.h"
#include "BentoBlock.h"
#include <QList>
#include <QHash>
#include <QDoubleSpinBox>
#include  <random>




//For test?
#include <QColor>
//Now
/* TODO!
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
 *  3.1 randomly change to another                                       //DONE
 *  3.2 add other, for example add new font from list of font.
 * 6.add new elements(from other during crossover?)
 * 7.option to skip sorted stylelist, maybe 2, one for number, one for rest
 * 8.add "QSlider mouse direct jump"
 * 9.create a re-bento function, that fixes the bentotree after crossover/mutation
 *      like when a element is moved to another place, it should change branch in the tree
 * 10. enable the treelist thing to the left, when an element is selected the user should be able to maybe
 *       set the parameters for this element only. or maybe "freeze" one element from changing,
 * 11. Could it be a benefit to allow different parameters for different populations?
 * 12. Add some kind of "island mode", where the tournament have bigger chance to just
 *        choose in the same page, or island. that would keep the different websites for a longer time
 *        and just update within it.. but sometimes it should be able to go between island.
 *        it would also make it more diverce. (crossover should of course be on same island) //DONE?
 *
 * 13.For the one above, also make it when the user have the same website several time in the list,
 *         it should be on different islands. or, a parameter so that each website also is x islands? /done
 * 14. Machine learning to remove bad copies, it will probably be too hard to identify "good" copies?
 *         if we should have the auto like for good copies, we probably need computer vision.
 * 15. settings for each parameter, like. max-min for the size stuff(like font at 1px is useless.) //Done
 * 16. because we now have orderered, the swap should be removed!
 * 17. add a "selective mutation"
 * 18. add elite,
 * 19. add "back in time", so if the fitness has dropped alot, it's a chance to turn it back to prev. version.
 * 20. make all positions absolute? this will make it easier for things to change position
 * 21. dynamic population based on the diverity.. for example, if a indisid is simalar to x other, it can
 *          die with a probability (c,x,y,f), where c is a contstant, x is the amount of similar, and y is
 *          the avrage "similarity value", and f is the inverted fitness.. or something, need more planning
 * 22. posibility to "kill" an island, should be after x generation and if none is seleted(i.e dont create
 *          the always keep some thing
 * 23. mix of 22,23, and always keep one, in the end, add one elit of each island instead of 2 in the beggining
 *           and ofcourse a chance not to if fitness is low
 * 24. what if we insted of setting a new fitness, update in respect to the old, maybe (0.3*old+0.7*new).
 * 25. for 3x3, the selection should be done winh a discrete_distribution where the weights decreeses more
 *           the closer to 0,or 1. thus, those previuly not selected have highest chance, and those around
 *           0.5 have higher probability than big like or dislike. thus the ones uncertian have bigger chance
 *           to get a more distinct fitness near 0 or 1.
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
    numLim=numericLimits;
    //eliteCopies=eCopies;
    //numberOfGenerations=numGen;
    populationSize=popSize;

    fitnessList.clear();
    for(int i=0;i<popSize;i++)
        fitnessList.append(0);//init the list of fitness to all 0


    //to make this  correct, use &pop above instead of *pop(and change [0][i] everywhere...)
    population=pop; //turn This into an 2-dimensional??

    currGeneration=0;
    currIndividial=0;
    //changeColor(0,0,0,0,0);
    //mutateColor();

    updateSettings();

    //        for(int i=0;i<population[0][0].mDOMNodes.size();i++){
    //            population[0][0].mDOMNodes[i].
    //        }

    /*        population[0][0].mBentoTree->mRootBlock->mDOMNode=population[0][5].mBentoTree->mRootBlock->mDOMNode.document();
        population[0][1].mBentoTree->mRootBlock->mDOMNode.replace(population[0][6].mBentoTree->mRootBlock->mDOMNode.document());


        population[0][7].mBentoTree->mRootBlock->mDOMNode=population[0][2].mBentoTree->mRootBlock->mDOMNode.document();
        population[0][8].mBentoTree->mRootBlock->mDOMNode.replace(population[0][3].mBentoTree->mRootBlock->mDOMNode.document());*/


}
void igacado::updateSettings(){

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
    BasicCrossProbaValue = settings->value("S_BasicCrossProbaValue")->value();

    BasicCrossKeepLengthProb = settings->value("S_BasicCrossKeepLengthProb")->value();


    swapPropa = settings->value("S_swapPropa")->value();
    sizeChangeProba = (int)settings->value("S_sizeChangeProba")->value();
    sizeChangefitness = (int)settings->value("S_Size_fitness")->value();
    double mean = settings->value("S_Size_mean")->value();
    double stddev  = settings->value("S_Size_std")->value();
    SizeNormRand=distribution(mean,stddev);



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
    }
}

//#####################################################################
// Function nextGeneration
// handles everything for the next generation
//#####################################################################
void igacado::nextGeneration(){
    //QVector< QVector<QColor> > colorCopy;
    QList<double> fitnessCopy = fitnessList;
    qDebug() << "copy the old";
    for(int i=0;i<populationSize;i++){
        population[0][i+populationSize].updatePage(population[0][i]);
    }
    qDebug() << "Done, create new population, the following is done:" << endl;
    QVector<int> newindsort;
    QVector<int> indexsort;
    QVector<int> islandPop;
    QVector<int> islandPopSize;
    QVector<double> islandFitness;
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
                if(island>0){
                    islandFitness[island-1]/=(int)islandPopSize[island-1];//islandPop[island]-islandPop[island-1];
                }
            }else{
                islandPopSize[island]++;
                islandFitness[island]+=fitnessCopy[i];
            }
        }
        //for(int i=0;i<floor(populationSize/2)*2;i+=2){
        //make an list of which population is what island, i.e in pairs..
    }
    int previousIsland;
    double crossIslandProb=0.1; ///////////////////////////  MOVE THIS OUTSIDE!!
    for(int i=0;i<populationSize;i++){
        //QList<int> tsIndexlist;
        //QList<double> tsFitnesslist;
        QList<QPair<double,int> > tsIslandlist;
        int randIsland;

        if(islandMode){
            if(i<islandFitness.size()*2){
                randIsland=floor(i/2);
                previousIsland=randIsland;
            }else if(isEven(i) || pRand(crossIslandProb)){
                std::vector<int> weights;
                for(int i=0; i<islandFitness.size(); i++) {
                    weights.push_back(100+islandFitness[i]*100); //100+ makes the weigt "more equal"
                }

                std::discrete_distribution<> d(weights.begin(), weights.end());
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
        }

        QList<QPair<double,int> > tslist;
        for(int j=0;j<tournamentSize;j++){
            int newind;
            if(islandMode){
                newind=(qrand()%islandPopSize[randIsland]) + islandPop[randIsland];
            }else{
                newind=qrand()%populationSize;
            }
            //tsIndexlist.append(newind); //todo, better to use qpair.
            //tsFitnesslist.append(fitnessCopy[newind]);
            tslist.append(qMakePair(fitnessCopy[newind],newind));

        }
        qDebug() << "this island:" << randIsland << endl;
//        for(int eo=0;eo<tslist.size();eo++){
//            qDebug() << i << "=" << tslist[eo].second << endl;
//        }
        qSort(tslist.begin(), tslist.end(), QPairFirstComparer());
        int newind=tournamentSelection(tslist, tournamentSelectionParameter);
        //int newind=tournamentSelection(tsIndexlist,tsFitnesslist, double tournamentSelectionParameter);
        //newIndexes[newind]=i;
        //qDebug() << "should be same?" <<population[0][newind].mPageID << endl;
        //same but sort
        int ji=0;
        for(int j=0;j<newindsort.size();j++){
            if(newind<=newindsort[j]) break;
            ji++;
        }
        newindsort.insert(ji,newind);
        indexsort.insert(ji,i);

    }

    foreach (int ao, newindsort) {
        qDebug() << ao << population[0][ao+populationSize].mPageID << endl;
    }

    //        for(int i=0;i<populationSize;i++){
    //            population[0][i].updatePage(population[0][newindsort[i]+populationSize]);
    //            fitnessList[i]=fitnessCopy[newindsort[i]]; //the fitness variable should have been in each page object...
    //        }

    for(int i=0;i<floor(populationSize/2)*2;i+=2){ //rounds down to even number
        do{
            population[0][i].updatePage(population[0][newindsort[i]+populationSize]);
            population[0][i+1].updatePage(population[0][newindsort[i+1]+populationSize]);
            fitnessList[i]=fitnessCopy[newindsort[i]]; //the fitness variable should have been in each page object...

            int i1 = indexsort[i];
            int i2 = indexsort[i+1];
            int s1 = population[0][i1].mColor.size();
            int s2 = population[0][i2].mColor.size();
            int p1start = qrand()%(s1-1);
            int p2start = qrand()%(s2-1);
            int maxlen = s1-p1start < s2-p2start ? s1-p1start : s2-p2start;
            int len = qrand()%maxlen;
            if(pRand(BasicCrossProba)) crossOverBasic(&population[0][i1],&population[0][i2]);
            if(pRand(cRate)){
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
            mutateColor(i1);
            mutateColor(i2);
            /*
                 * feed the ml with the data here
                 * create a ml-object?
                 *
                 *
                 */

        }while(false); //insert the Machine learning here? so if the ml think the new page is bad it will create a new pace insead.
    }

    if(!isEven(populationSize)){//floor(populationSize/2)*2 != populationSize){ //if uneven, just copy the last as is, probably better way to check it
        int i=populationSize-1;
        population[0][i].updatePage(population[0][newindsort[i]+populationSize]);
        fitnessList[i]=fitnessCopy[newindsort[i]];
        mutateColor(i);
    }


    //mutateColor();
    qDebug() << "Done " << endl;
    //testtest(population[0][0].mBentoTree->mRootBlock->mDOMNode,population[0][5].mBentoTree->mRootBlock->mDOMNode);
    //testtest(population[0][6].mBentoTree->mRootBlock->mDOMNode,population[0][1].mBentoTree->mRootBlock->mDOMNode);

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
//rename to sort or something
//int igacado::tournamentSelection(QList<int> indexlist,QList<double> fitnesslist, double tournamentSelectionParameter){
//    QList<QPair<double,int> > array;
//    for (int i = 0; i < fitnesslist.size(); i++){
//        array.append(qMakePair(fitnesslist[i],indexlist[i]));
//    }
//    qSort(array.begin(), array.end(), QPairFirstComparer());
//    return tournamentSelection(array, tournamentSelectionParameter);
//}


//#####################################################################
// Function CrossoverBasic
// Do basic crossover for allitems
//#####################################################################
void igacado::crossOverBasic(bricolage::Page* mPage1,bricolage::Page* mPage2/*,bool keep_length*/){
    //if(!pRand(BasicCrossProba)) return;    <-----  Put this outside before calling instead.
    QVector<QString> keys;//Only keeps keys that are in both.
    foreach(QString key1, mPage1->getKeys()){
        foreach(QString key2, mPage2->getKeys()){
            if(key1==key2){
                keys << key1;
                break;
            }
        }
    }

    foreach(QString key,keys){
        if(!pRand(BasicCrossProbaKey)) continue;
        QVector<QString> list1=mPage1->ComputedStyleList[key];
        QVector<QString> list2=mPage2->ComputedStyleList[key];
        //        bool isNumber=!getNumberFromQString(list1[0]).isEmpty(); //true if a number
        //        if(isNumber){
        //            int s1=list1.size();
        //            int s2=list2.size();
        //            int sized=s1/s2;
        //            int j=sized/2;
        //            int dist=
        //        }else{ //standard crossover

        int s1 = list1.size();
        int s2 = list2.size();
        if(s1<2 && s2<2){ //if short, just swap whole list. "==1"
            mPage1->ComputedStyleList[key]=list2;
            mPage2->ComputedStyleList[key]=list1;
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

        //        } //For now only do standard crossover for all, numbers should be done different???
        // QList<int> = for(int i=0;i<10;i++) i;


        //        foreach (QString value1, ) {
        //            foreach (QString value2,mPage2->ComputedStyleList[key]) {
        //                if(pRand(BasicCrossProbaValue)){

        //                    QList<double> newintlist= getNumberFromQString(mPage->ComputedStyleList[key][i]);//
        //                    if(!newintlist.isEmpty()){

        //                    }
        //                }
        //            }
        //        }
    }
}
//#####################################################################
// Function MutateSize
// Mutates styles that dont have a special function   ##Maybe should be numerical mutate, and use all numerical things in in??
//#####################################################################
void igacado::mutateSize(bricolage::Page* mPage,QString key,bool negativeOk,int unicorn){
    double cMDiff = (1-fitnessList[unicorn]);
    //SizeNormRand=distribution(SizeNormRand.mean(),SizeNormRand.stddev()) //(mean,stddev);
    int s=mPage->ComputedStyleList[key].size();
    for(int i=0;i<s;i++){
        qDebug() << key << "= "<< mPage->ComputedStyleList[key][i];
        QList<QPair<double,QString> > newintlist= getNumberFromQString(mPage->ComputedStyleList[key][i]);//
        if(!newintlist.isEmpty()){
            QString newstr;
            for(int j=0;j<newintlist.size();j++){

                //foreach (QPair<int,QString> newint, newintlist) {
                QPair<double,QString> newint =newintlist[j];
                if(newint.first<0) negativeOk=true; //should be from a list later!! TODO
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
                negativeOk=false; //REMOVE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }
            mPage->ComputedStyleList[key].replace(i, newstr);
        }
        qDebug() <<" new" << key << "= "<< mPage->ComputedStyleList[key][i]<<endl;
        int a=5;
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
void igacado::mutateColor(){
    for(int i=0; i<populationSize; i++){
        mutateColor(i);
    }
}
void igacado::mutateColor(int i){ //not only color, use for everything!
    foreach(QString key, population[0][i].getKeys()){ //using different solution... not good, but which is best??
        //if(swapList.contains(key) && pRand(swapPropa)) population[0][i].swapStyle(key);
        if(sizeList.contains(key) && pRand(sizeChangeProba)) mutateSize(&population[0][i],key,false,i);
    }
    mutateElementColor(&population[0][i]); //Color was implemented first, the other is using a better method
    //and thus color is by itself

    //        //the closer to any limits the less we will change the ligthness
    //        double ligthp=(((255-avrageLigth) < avrageLigth ? (255-avrageLigth): avrageLigth)/10);
    double rant2 = (double)rand()/RAND_MAX;
    if(rant2 < ColorRot){
        double cMDiff = (1-fitnessList[i]);
        int huep=Colorhue_R + (cMDiff*Colorhue_F);
        int hue= Colorhue_M + huep==0 ? 0 : (qrand()%(huep*2))-huep;
        int satp=Colorsat_R + (cMDiff*Colorsat_F);
        int sat= Colorsat_M + satp==0 ? 0 : (qrand()%(satp*2))-satp;
        int ligp=Colorlig_R + (cMDiff*Colorlig_F);
        int lig= Colorlig_M + ligp==0 ? 0 : (qrand()%(ligp*2))-ligp; //Should this depend on current values??
        rotateColor(&population[0][i], -1 , hue, sat, lig);
    }
    double rant = (double)rand()/RAND_MAX;
    if(rant<(1-fitnessList[i])*ColorRand) { //todo set parameter
        rotateColor(&population[0][i],-1,qrand() % 360,(qrand() % 200)-100,(qrand() % 200)-100);
    }

    // changeColor(0,0,0,0,0);
}



//#####################################################################
// Function rotatecolor
// Rotate color in the hsl, index -1 rotates all.
// Inputs is the change, e.g dh=180, rotates color 180 degre from
// current color.
//#####################################################################
void igacado::rotateColor(bricolage::Page *mPage,int index,int dh,int ds, int dl){

    if(index < 0){
        for(int i = 0; i < mPage->mColor.size();i++){
            int h=(dh + mPage->mColor[i].hslHue() )%359; //360 including 0
            h= ((h < 0) ? h+359 : h);  //modulus in c++ return negative number
            int s=(ds + mPage->mColor[i].hslSaturation() ); //should these be like "wheel, or -5 be threted as 0?
            dl *= pRand(0.7) ? 1 : -1; //30% chance  to change the sign, some elements darker and some ligther.
            int l=(dl + mPage->mColor[i].lightness() ); // todo, try without %, migth be added in the qcolor already
            s = (s>255 ? 255 : (s<0 ? 0 : s));
            l = (l>255 ? 255 : (l<0 ? 0 : l));//max-min, to keep s in limit 0-255
            //qDebug() << " h:" <<h << " s:"<< s << " l:"<< l << endl;

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
        currColors->append(currColors[0][qrand()%nOld].lighter(qrand()%200+20)); //do we want this as parameter?? should also be able to add color?
    }

}

//#####################################################################
// Function mutateElementColor
// For each , change its link to the color list randomly
//#####################################################################
void igacado::mutateElementColor(bricolage::Page *mpage, bricolage::BentoBlock* bentoBlock, int ColorSize){
    if(bentoBlock->mDomNodeID!=-1){
        if(bentoBlock->mDomNodeID < mpage->mDOMNodes.size()){
            QWebElement temp = mpage->mDOMNodes[bentoBlock->mDomNodeID];
            bentoBlock->mDOMNode = temp;
            //test
        }else{
            qDebug()<< endl << "To few elements, reqested" << bentoBlock->mDomNodeID << "of" << mpage->mDOMNodes.size()
                    << "| bentoid:" << bentoBlock->mBentoID;
        }
    }
    if(pRand(mRateCE_n/ColorSize)){
        if(pRand()) bentoBlock->mbgColor=ColorSize;
        else bentoBlock->mtColor=ColorSize;
        newColor(&mpage->mColor);
        //rotateColor(mpage,ColorSize,qrand() % 50,qrand() % 50,qrand() % 50);
        ColorSize++;
    }else{
        if(pRand(mRateCE_t)) bentoBlock->mbgColor=qrand() % (ColorSize);
        if(pRand(mRateCE_t)) bentoBlock->mtColor=qrand() % (ColorSize);
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
//#####################################################################
// Function fullSize
// return the number of nodes in the tree. (is there a better way to do this?)
//#####################################################################
//uint igacado::fullSize(const bricolage::BentoBlock* bentoBlock)
//{
////    uint fs=1;
////    for(uint i=0; i<(uint)bentoBlock->mChildren.size(); i++){
////         fs+=fullSize(bentoBlock->mChildren[i]);   // This is maybe a better way to index than .at
////    }
////    return fs;
//}

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
    //population->at(0).mColor.at(index).setRgb(r,g,b);
    //    //changeColora(&population[0],index,r, g, b);
    //    int colorIndex=qrand()%population[0][popIndex].mColor.size();
    //    population[0][popIndex].mColor[colorIndex].setRgb(r,g,b);

}

//void igacado::testtest(QWebElement oldNode,QWebElement newNode){
//   // if(first){
//        //oldNode.removeAllChildren();
//        oldNode.replace(newNode.clone());
//        //oldNode.appendOutside(newNode.clone());
//        //oldNode.removeAllChildren();
//  //  }


////    QWebElement domChild = newNode.firstChild();

////    while (!domChild.isNull()) {
////        oldNode.appendInside(newNode);
////         QWebElement olddomChild = olddomChild.firstChild();
////        testtest(,domChild);
////        domChild = domChild.nextSibling();
////    }
////    oldNode.replace(newNode);



//}


const QStringList igacado::swapList = QStringList()<< "direction" << "display" << "font" << "font-family" << "font-size" << "font-style" << "font-variant" <<
                                                      "font-weight"  <<"text-rendering"<< "background" <<  "background-attachment" << "background-clip" <<
                                                      "background-image" << "background-origin" << "background-position" << "background-position-x" << "background-position-y" <<
                                                      "background-repeat" << "background-repeat-x" << "background-repeat-y" << "background-size" << "border" <<
                                                      "border-bottom" <<  "border-bottom-left-radius" << "border-bottom-right-radius" <<
                                                      "border-bottom-style" << "border-bottom-width" << "border-collapse" <<  "border-image" <<
                                                      "border-image-outset" << "border-image-repeat" << "border-image-slice" << "border-image-source" << "border-image-width" <<
                                                      "border-left" <<  "border-left-style" << "border-left-width" << "border-radius" << "border-right" <<
                                                      "border-right-style" << "border-right-width" << "border-spacing" << "border-style" <<
                                                      "border-top" <<  "border-top-left-radius" << "border-top-right-radius" << "border-top-style" <<
                                                      "border-top-width" << "border-width" << "bottom" << "box-shadow" << "box-sizing" << "clear" << "clip" << "content" <<
                                                      "counter-increment" << "counter-reset" << "cursor" << "empty-cells" << "float" << "font-stretch" << "image-rendering" <<
                                                      "letter-spacing" << "list-style" << "list-style-image" << "list-style-position" << "list-style-type" << "margin" <<
                                                      "margin-bottom" << "margin-left" << "margin-right" << "margin-top" << "max-height" << "max-width" << "min-height" <<
                                                      "min-width" << "opacity" << "orphans" << "outline"  << "outline-offset" << "outline-style" <<
                                                      "outline-width" << "overflow" << "overflow-x" << "overflow-y" << "padding" << "padding-bottom" << "padding-left" <<
                                                      "padding-right" << "padding-top" << "page" << "page-break-after" << "page-break-before" << "page-break-inside" <<
                                                      "pointer-events" << "position" << "quotes" << "resize" << "size" << "src" << "speak" << "table-layout" << "tab-size" <<
                                                      "text-align" << "text-decoration" << "text-indent" << "text-line-through" <<
                                                      "text-line-through-mode" << "text-line-through-style" << "text-line-through-width" << "text-overflow" << "text-overline" <<
                                                      "text-overline-mode" << "text-overline-style" << "text-overline-width" << "text-shadow" <<
                                                      "text-transform" << "text-underline"  << "text-underline-mode" << "text-underline-style" <<
                                                      "text-underline-width" << "unicode-bidi" << "unicode-range" << "vertical-align" << "visibility" << "white-space" <<
                                                      "widows" << "word-break" << "word-spacing" << "word-wrap" << "z-index" << "left" << "height"<<"line-height"<<"right"<<"top" << "voice-family";

const QStringList igacado::sizeList = QStringList() << "background-position-y" << "font-size" <<  "background-size" << "border-bottom-width" <<  "border-image-width" <<
                                                       "border-left-width"  <<"border-right-width" << "border-spacing" << "border-top" <<
                                                       "border-top-width" << "border-width" << "bottom" << "box-sizing" <<"letter-spacing" << "margin" <<
                                                       "margin-bottom" << "margin-left" << "margin-right" << "margin-top" << "max-height" << "max-width" << "min-height" <<
                                                       "min-width"  << "outline-width" << "padding" << "padding-bottom" << "padding-left" <<"padding-right" << "padding-top" << "resize" <<
                                                       "size" << "tab-size" << "text-line-through-width" << "text-overline-width" << "text-underline-width" <<  "word-spacing" << "left" << "height"<<"line-height"<<"right"<<"top" ;

