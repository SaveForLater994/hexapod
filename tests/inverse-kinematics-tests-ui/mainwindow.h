#pragma once

#include <QMainWindow>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include "ik-tests.h"
#include "canvas2d.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSlidersChanged();

private:
    QSlider *m_sliderX;
    QSlider *m_sliderY;
    QSlider *m_sliderZ;
    QLabel *m_labelXVal;
    QLabel *m_labelYVal;
    QLabel *m_labelZVal;

    QLineEdit *m_inputCoxa;
    QLineEdit *m_inputFemur;
    QLineEdit *m_inputTibia;
    QLabel *m_statusLabel;

    LegCanvas2D *m_canvasTop;
    LegCanvas2D *m_canvasSide;

    servo_data m_ikData;
};
