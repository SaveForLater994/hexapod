#include "canvas2d.h"
#include <cmath>

LegCanvas2D::LegCanvas2D(ViewMode mode, QWidget *parent)
    : QWidget(parent), m_mode(mode) {
    setMinimumSize(250, 250);
    m_pos = {0, 0, 0};
    m_data = { {0,0,0}, 40.0f, 140.0f, 180.0f, 0.0f, 0.0f };
    m_angles = {0, 0, 0};
}

void LegCanvas2D::updateState(const vec3_float &pos, const leg_data &data, const servo_angles &angles, bool isValid, ik_status_t status) {
    m_pos = pos;
    m_data = data;
    m_angles = angles;
    m_isValid = isValid;
    m_status = status;
    update();
}

void LegCanvas2D::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Темный фон canvas
    painter.fillRect(rect(), QColor(30, 30, 35));

    int w = width();
    int h = height();
    QPointF center(w / 2.0, h / 2.0);

    // 1. Отрисовка осей координат (перекрестье 0,0 ровно в центре)
    painter.setPen(QPen(QColor(70, 70, 80), 1, Qt::DashLine));
    painter.drawLine(0, center.y(), w, center.y()); // Горизонтальная ось
    painter.drawLine(center.x(), 0, center.x(), h); // Вертикальная ось

    // Засечки и подписи осей
    painter.setPen(QColor(120, 120, 140));
    painter.setFont(QFont("Courier New", 10));
    if (m_mode == TOP_VIEW) {
        painter.drawText(w - 25, center.y() - 5, "+X");
        painter.drawText(center.x() + 5, 15, "+Y");
        painter.drawText(center.x() + 5, h - 5, "-Y");
        painter.drawText(5, center.y() - 5, "-X");
    } else {
        painter.drawText(w - 25, center.y() - 5, "+R");
        painter.drawText(center.x() + 5, 15, "+Z");
        painter.drawText(center.x() + 5, h - 5, "-Z");
    }

    // Заголовок
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    if (m_mode == TOP_VIEW) {
        painter.drawText(10, 20, "Top View (XY Plane)");
    } else {
        painter.drawText(10, 20, "Side View (R-Z Plane)");
    }

    // Если решение IK не найдено (точка вне зоны)
    if (!m_isValid) {
        painter.setPen(QPen(QColor(255, 82, 82), 2));
        painter.setFont(QFont("Arial", 11, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, QString("Out of Reach!\n%1").arg(ik_status_to_string(m_status)));
        return;
    }

    float scale = 1.0f; // 1 мм = 1 пиксель

    if (m_mode == TOP_VIEW) {
        // --- TOP VIEW (ПЛОСКОСТЬ XY) ---
        QPointF origin = center;
        // В Qt ось Y экрана идет ВНИЗ, поэтому минус перед Y
        QPointF target(origin.x() + m_pos.X * scale, origin.y() - m_pos.Y * scale);

        // Линия от центра до цели
        painter.setPen(QPen(QColor(0, 200, 255), 3));
        painter.drawLine(origin, target);

        // Начальная точка (Coxa joint)
        painter.setBrush(Qt::green);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(origin, 6, 6);

        // Целевая точка
        painter.setBrush(Qt::red);
        painter.drawEllipse(target, 5, 5);

    } else {
        // --- SIDE VIEW (ПЛОСКОСТЬ R-Z) ---
        float r = std::sqrt(m_pos.X * m_pos.X + m_pos.Y * m_pos.Y);
        float z = m_pos.Z;

        // Нулевой сустав ноги (p0) строго в ЦЕНТРЕ СК (0,0)
        QPointF p0 = center;
        QPointF target(p0.x() + r * scale, p0.y() - z * scale);

        float l1 = m_data.leg_femur * scale;
        float beta = m_angles.femur_servo_angle;

        // Точка колена (Femur -> Tibia joint) через угол бедра beta
        QPointF p1(p0.x() + l1 * std::cos(beta), 
                    p0.y() - l1 * std::sin(beta));

        // Кончик ноги (Tibia tip) - соединяет колено с целевой позицией стопы
        QPointF p2 = target;

        // 1. Звено Femur (оранжевый цвет)
        painter.setPen(QPen(QColor(255, 170, 0), 4));
        painter.drawLine(p0, p1);

        // 2. Звено Tibia (голубой цвет)
        painter.setPen(QPen(QColor(0, 200, 255), 4));
        painter.drawLine(p1, p2);

        // 3. Маркер цели (красное штриховое кольцо)
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(target, 8, 8);

        // 4. Суставы p0 и p1 (белые кружки)
        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(p0, 5, 5);
        painter.drawEllipse(p1, 5, 5);

        // 5. Кончик ноги p2 (зеленый кружок - накладывается точно в маркер)
        painter.setBrush(Qt::green);
        painter.drawEllipse(p2, 4, 4);
    }
}
