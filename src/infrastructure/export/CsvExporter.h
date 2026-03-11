#pragma once
#include <QString>
#include <QStringList>
#include <QVector>

class CsvExporter {
public:
  static bool exportToFile(const QString& filePath, const QStringList& header, const QVector<QStringList>& rows, QString* err = nullptr);
};