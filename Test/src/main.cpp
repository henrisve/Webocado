//#####################################################################
// Copyright 2012, Ranjitha Kumar.
// This software is governed by the license contained in LICENSE.
//#####################################################################
#include <QApplication>
#include <QDir>
#include "igamain.h"
#include "window.h"
#include <QDebug>

void customHandler(QtMsgType type, const char* msg);

using namespace bricolage;
int main(int argc, char *argv[]){

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
