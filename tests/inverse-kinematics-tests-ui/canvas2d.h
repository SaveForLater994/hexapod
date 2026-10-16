#pragma once

#include <QWidget>
#include <QPainter>
#include "ik-tests.h"

enum ViewMode {
    TOP_VIEW,   // XY plane
    SIDE_VIEW   // Radial distance vs Z plane
};

class LegCanvas2D : public QWidget {
    Q_OBJECT
public:
    explicit LegCanvas2D(ViewMode mode, QWidget *parent = nullptr);

    void updateState(const vec3_float &pos, const leg_data &data, const servo_angles &angles, bool isValid, ik_status_t status);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    ViewMode m_mode;
    vec3_float m_pos;
    leg_data m_data;
    servo_angles m_angles;
    bool m_isValid = false;
    ik_status_t m_status = IK_OK;
};

