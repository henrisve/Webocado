/********************************************************************************
** Form generated from reading UI file 'windowt.ui'
**
** Created: Fri Sep 16 22:58:51 2016
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOWT_H
#define UI_WINDOWT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtWebKit/QWebView>

QT_BEGIN_NAMESPACE

class Ui_Window
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_2;
    QWebView *webView;
    QWebView *webView2;
    QStatusBar *statusbar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QTreeWidget *treeWidget;
    QLineEdit *lineEdit;
    QTreeWidget *treeWidget2;
    QLineEdit *lineEdit2;
    QPushButton *pushButton;
    QMenuBar *menubar;

    void setupUi(QMainWindow *Window)
    {
        if (Window->objectName().isEmpty())
            Window->setObjectName(QString::fromUtf8("Window"));
        Window->resize(1569, 744);
        centralwidget = new QWidget(Window);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(centralwidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        webView = new QWebView(centralwidget);
        webView->setObjectName(QString::fromUtf8("webView"));
        sizePolicy.setHeightForWidth(webView->sizePolicy().hasHeightForWidth());
        webView->setSizePolicy(sizePolicy);
        webView->setProperty("url", QVariant(QUrl("about:blank")));

        horizontalLayout_2->addWidget(webView);

        webView2 = new QWebView(centralwidget);
        webView2->setObjectName(QString::fromUtf8("webView2"));
        sizePolicy.setHeightForWidth(webView2->sizePolicy().hasHeightForWidth());
        webView2->setSizePolicy(sizePolicy);
        webView2->setProperty("url", QVariant(QUrl("about:blank")));

        horizontalLayout_2->addWidget(webView2);

        Window->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(Window);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        Window->setStatusBar(statusbar);
        dockWidget = new QDockWidget(Window);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(4, 4, 4, 4);
        treeWidget = new QTreeWidget(dockWidgetContents);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        sizePolicy.setHeightForWidth(treeWidget->sizePolicy().hasHeightForWidth());
        treeWidget->setSizePolicy(sizePolicy);
        treeWidget->header()->setVisible(false);

        verticalLayout->addWidget(treeWidget);

        lineEdit = new QLineEdit(dockWidgetContents);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        verticalLayout->addWidget(lineEdit);

        treeWidget2 = new QTreeWidget(dockWidgetContents);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QString::fromUtf8("1"));
        treeWidget2->setHeaderItem(__qtreewidgetitem1);
        treeWidget2->setObjectName(QString::fromUtf8("treeWidget2"));
        sizePolicy.setHeightForWidth(treeWidget2->sizePolicy().hasHeightForWidth());
        treeWidget2->setSizePolicy(sizePolicy);
        treeWidget2->header()->setVisible(false);

        verticalLayout->addWidget(treeWidget2);

        lineEdit2 = new QLineEdit(dockWidgetContents);
        lineEdit2->setObjectName(QString::fromUtf8("lineEdit2"));

        verticalLayout->addWidget(lineEdit2);

        pushButton = new QPushButton(dockWidgetContents);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);

        dockWidget->setWidget(dockWidgetContents);
        Window->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);
        menubar = new QMenuBar(Window);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1569, 24));
        Window->setMenuBar(menubar);

        retranslateUi(Window);
        QObject::connect(pushButton, SIGNAL(clicked()), Window, SLOT(update()));

        QMetaObject::connectSlotsByName(Window);
    } // setupUi

    void retranslateUi(QMainWindow *Window)
    {
        Window->setWindowTitle(QApplication::translate("Window", "Web Element DOM Traversal", 0, QApplication::UnicodeUTF8));
        dockWidget->setWindowTitle(QApplication::translate("Window", "Document Structure", 0, QApplication::UnicodeUTF8));
        lineEdit->setText(QApplication::translate("Window", "/home/lox/Desktop/b.html", 0, QApplication::UnicodeUTF8));
        lineEdit2->setText(QApplication::translate("Window", "/home/lox/Desktop/a.html", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("Window", "Segment Page", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Window: public Ui_Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOWT_H
