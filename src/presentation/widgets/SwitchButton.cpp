#include "SwitchButton.h"
#include <QPainter>
#include <QPaintEvent>

SwitchButton::SwitchButton(QWidget* parent) : QAbstractButton(parent) {
    setCheckable(true);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QSize SwitchButton::sizeHint() const {
    return QSize(54, 32);
}

void SwitchButton::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);
    const int w = width();
    const int h = height();
    const int radius = h / 2;
    const int margin = 2;
    const int knob = h - margin * 2;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor bg = isChecked() ? QColor(76, 175, 80) : QColor(180, 180, 180);
    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    QRectF groove(margin, margin, w - margin * 2, h - margin * 2);
    p.drawRoundedRect(groove, radius, radius);

    QColor knobColor = Qt::white;
    p.setBrush(knobColor);
    int x = isChecked() ? (w - margin - knob) : margin;
    QRectF handle(x, margin, knob, knob);
    p.drawEllipse(handle);
}
