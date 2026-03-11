#include "ScenesPage.h"
#include "ui_ScenesPage.h"

#include "services/SceneService.h"

#include <QMessageBox>

ScenesPage::ScenesPage(AppContext& ctx, QWidget* parent)
    : QWidget(parent), ui(new Ui::ScenesPage), m_ctx(ctx) {
    ui->setupUi(this);

    connect(ui->btnHomeMode, &QPushButton::clicked, this, [this]{
        QString err;
        if (!m_ctx.sceneService->activateScene("admin", QString::fromUtf8("回家模式"), &err)) {
            QMessageBox::warning(this, QString::fromUtf8("场景失败"), err);
        }
    });

    connect(ui->btnSleepMode, &QPushButton::clicked, this, [this]{
        QString err;
        if (!m_ctx.sceneService->activateScene("admin", QString::fromUtf8("睡眠模式"), &err)) {
            QMessageBox::warning(this, QString::fromUtf8("场景失败"), err);
        }
    });

    connect(ui->btnAwayMode, &QPushButton::clicked, this, [this]{
        QString err;
        if (!m_ctx.sceneService->activateScene("admin", QString::fromUtf8("离家模式"), &err)) {
            QMessageBox::warning(this, QString::fromUtf8("场景失败"), err);
        }
    });
}

ScenesPage::~ScenesPage() { delete ui; }
