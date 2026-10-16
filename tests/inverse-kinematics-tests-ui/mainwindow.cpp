#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <cmath>

const float PI = 3.14159265358979323846f;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Hexapod IK Test Visualizer");
    resize(950, 550);

    m_ikData.leg_femur = 40.0f;
    m_ikData.leg_tarsus = 130.0f;

    m_ikData.servo_coxa_max_angle_rad = 3.14159f;
    m_ikData.servo_femur_max_angle_rad = 3.14159f;
    m_ikData.servo_tibia_max_angle_rad = 3.14159f;
    
    m_ikData.delta_coxa_femur_angle = 0.0f;
    m_ikData.delta_tibia_leg_angle = 0.0f;

    m_ikData.coxa_angle_offset = 0.0f;
    m_ikData.femur_angle_offset = PI_6; // 30 degrees offset for femur
    m_ikData.tibia_angle_offset = PI/2.0f; // 60 degrees offset for tibia

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // LEFT PANEL
    QVBoxLayout *leftPanel = new QVBoxLayout();

    QGroupBox *sliderGroup = new QGroupBox("Target Position (X, Y, Z in mm)", this);
    QGridLayout *sliderLayout = new QGridLayout(sliderGroup);

    sliderLayout->addWidget(new QLabel("X:"), 0, 0);
    m_sliderX = new QSlider(Qt::Horizontal, this);
    m_sliderX->setRange(-200, 200);
    m_sliderX->setValue(100);
    m_labelXVal = new QLabel("100 mm", this);
    sliderLayout->addWidget(m_sliderX, 0, 1);
    sliderLayout->addWidget(m_labelXVal, 0, 2);

    sliderLayout->addWidget(new QLabel("Y:"), 1, 0);
    m_sliderY = new QSlider(Qt::Horizontal, this);
    m_sliderY->setRange(-100, 100);
    m_sliderY->setValue(100);
    m_labelYVal = new QLabel("100 mm", this);
    sliderLayout->addWidget(m_sliderY, 1, 1);
    sliderLayout->addWidget(m_labelYVal, 1, 2);

    sliderLayout->addWidget(new QLabel("Z:"), 2, 0);
    m_sliderZ = new QSlider(Qt::Horizontal, this);
    m_sliderZ->setRange(-150, 150);
    m_sliderZ->setValue(-50);
    m_labelZVal = new QLabel("-50 mm", this);
    sliderLayout->addWidget(m_sliderZ, 2, 1);
    sliderLayout->addWidget(m_labelZVal, 2, 2);

    leftPanel->addWidget(sliderGroup);

    QGroupBox *outputGroup = new QGroupBox("Calculated Servo Angles (Deg)", this);
    QFormLayout *outputLayout = new QFormLayout(outputGroup);

    m_inputCoxa = new QLineEdit(this);
    m_inputCoxa->setReadOnly(true);
    m_inputCoxa->setStyleSheet("font-weight: bold; font-size: 14px;");

    m_inputFemur = new QLineEdit(this);
    m_inputFemur->setReadOnly(true);
    m_inputFemur->setStyleSheet("font-weight: bold; font-size: 14px;");

    m_inputTibia = new QLineEdit(this);
    m_inputTibia->setReadOnly(true);
    m_inputTibia->setStyleSheet("font-weight: bold; font-size: 14px;");

    outputLayout->addRow("Coxa Angle:", m_inputCoxa);
    outputLayout->addRow("Femur Angle:", m_inputFemur);
    outputLayout->addRow("Tibia Angle:", m_inputTibia);

    leftPanel->addWidget(outputGroup);

    m_statusLabel = new QLabel("Status: OK", this);
    m_statusLabel->setStyleSheet("font-weight: bold; color: green; font-size: 13px;");
    leftPanel->addWidget(m_statusLabel);

    leftPanel->addStretch();
    mainLayout->addLayout(leftPanel, 1);

    // RIGHT PANEL
    QHBoxLayout *canvasLayout = new QHBoxLayout();
    m_canvasTop = new LegCanvas2D(TOP_VIEW, this);
    m_canvasSide = new LegCanvas2D(SIDE_VIEW, this);

    canvasLayout->addWidget(m_canvasTop);
    canvasLayout->addWidget(m_canvasSide);

    mainLayout->addLayout(canvasLayout, 2);

    connect(m_sliderX, &QSlider::valueChanged, this, &MainWindow::onSlidersChanged);
    connect(m_sliderY, &QSlider::valueChanged, this, &MainWindow::onSlidersChanged);
    connect(m_sliderZ, &QSlider::valueChanged, this, &MainWindow::onSlidersChanged);

    onSlidersChanged();
}

MainWindow::~MainWindow()
{
}

void MainWindow::onSlidersChanged()
{
    float x = m_sliderX->value();
    float y = m_sliderY->value();
    float z = m_sliderZ->value();

    m_labelXVal->setText(QString("%1 mm").arg(x));
    m_labelYVal->setText(QString("%1 mm").arg(y));
    m_labelZVal->setText(QString("%1 mm").arg(z));

    m_ikData.desired_pos = { x, y, z };

    servo_angles angles;
    int res = conversion_pipeline(&angles, &m_ikData);
    qDebug() << "IK Result: " << res << ", Angles: coxa=" << angles.coxa_servo_angle << ", femur=" << angles.femur_servo_angle << ", tibia=" << angles.tibia_servo_angle;
    const float RAD_TO_DEG = 180.0f / 3.1415926535f;

    if (res == IK_OK) {
        float coxa_deg = angles.coxa_servo_angle * RAD_TO_DEG;
        float femur_deg = angles.femur_servo_angle * RAD_TO_DEG;
        float tibia_deg = angles.tibia_servo_angle * RAD_TO_DEG;

        m_inputCoxa->setText(QString::number(coxa_deg, 'f', 2) + "°");
        m_inputFemur->setText(QString::number(femur_deg, 'f', 2) + "°");
        m_inputTibia->setText(QString::number(tibia_deg, 'f', 2) + "°");

        m_statusLabel->setText("Status: OK");
        m_statusLabel->setStyleSheet("font-weight: bold; color: #00E676; font-size: 13px;");

        m_canvasTop->updateState(m_ikData.desired_pos, m_ikData, angles, true, IK_OK);
        m_canvasSide->updateState(m_ikData.desired_pos, m_ikData, angles, true, IK_OK);
    } else {
        m_inputCoxa->setText("NaN");
        m_inputFemur->setText("NaN");
        m_inputTibia->setText("NaN");

        m_statusLabel->setText(QString("Status: %1").arg(ik_status_to_string((ik_status_t)res)));
        m_statusLabel->setStyleSheet("font-weight: bold; color: #FF5252; font-size: 13px;");

        m_canvasTop->updateState(m_ikData.desired_pos, m_ikData, angles, false, (ik_status_t)res);
        m_canvasSide->updateState(m_ikData.desired_pos, m_ikData, angles, false, (ik_status_t)res);
    }
}
