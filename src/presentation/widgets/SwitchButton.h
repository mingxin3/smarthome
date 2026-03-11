#pragma once
#include <QAbstractButton>
#include <QColor>

class SwitchButton : public QAbstractButton {
    Q_OBJECT
public:
    explicit SwitchButton(QWidget* parent = nullptr);
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent* e) override;
};
