#include "CsvExporter.h"
#include <QFile>
#include <QTextStream>

static QString esc(const QString& s) {
  QString t = s;
  t.replace("\"", "\"\"");
  if (t.contains(',') || t.contains('"') || t.contains('\n')) {
    t = "\"" + t + "\"";
  }
  return t;
}

bool CsvExporter::exportToFile(const QString& filePath, const QStringList& header, const QVector<QStringList>& rows, QString* err) {
  QFile f(filePath);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    if (err) *err = "无法写入文件";
    return false;
  }
  QTextStream out(&f);
  out.setEncoding(QStringConverter::Utf8);

  auto writeRow = [&](const QStringList& r) {
    QStringList e;
    for (const auto& c : r) e << esc(c);
    out << e.join(",") << "\n";
  };

  writeRow(header);
  for (const auto& r : rows) writeRow(r);
  return true;
}