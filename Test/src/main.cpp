//#####################################################################
// Copyright 2016, Henrik K. Svensson.
//
// Parts of this software is using Bento, Copyright 2012, Ranjitha Kumar.
// This software is governed by the license contained in LICENSE.
//#####################################################################
#include <QApplication>
#include <QDir>
#include "igamain.h"
#include "window.h"
#include <QDebug>

void customHandler(QtMsgType type, const char* msg);


int main(int argc, char *argv[]){

    QTime now = QTime::currentTime();
    qsrand(now.msec());
    qInstallMsgHandler(customHandler);

    QApplication app(argc, argv);

    bricolage::Window window;
    window.init();
    window.readini();
    window.show();

    return app.exec();
}

void customHandler(QtMsgType type, const char* msg) {
    fprintf(stderr, msg);
    fflush(stderr);
}
