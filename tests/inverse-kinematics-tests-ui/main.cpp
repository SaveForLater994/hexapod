#include <QApplication>
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSlider>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(new QSlider());

    MainWindow w;
    w.show();

    return a.exec();
}
