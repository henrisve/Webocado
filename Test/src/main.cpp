//#####################################################################
// Copyright 2012, Ranjitha Kumar.
// This software is governed by the license contained in LICENSE.
//#####################################################################
#include <QApplication>
#include <QDir>
#include "igamain.h"
#include "window.h"
#include <QDebug>
//#include "myrand.h"

//std::normal_distribution<double> distribution;
//std::mt19937 engine;
//std::random_device rd;



void customHandler(QtMsgType type, const char* msg);
//#test# int g_test = 1;
using namespace bricolage;
int main(int argc, char *argv[]){

//    int po=10;
//    int is=1;

//    for(int i=0;i<(po-is);i+=2){
//        qDebug() << i << i+1<<endl;
//    }
//    for(int i=1;i<=is;i++){
//        qDebug() << "u" <<po-i;
//    }











    //Temp above:
    QTime now = QTime::currentTime();
    qsrand(now.msec());
    qInstallMsgHandler(customHandler);

    QApplication app(argc, argv);


    Window window;
    window.init();
    window.readini();
    window.show();

    return app.exec();
}

void customHandler(QtMsgType type, const char* msg) {
    fprintf(stderr, msg);
    fflush(stderr);
}



