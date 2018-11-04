#ifndef SCANTEST_H
#define SCANTEST_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class ScanTest;
}

class ScanTest : public QDialog
{
    Q_OBJECT

public:
    explicit ScanTest(QWidget *parent = 0);
    ~ScanTest();

private:
    Ui::ScanTest *ui;

private slots:
    void showValue(QByteArray value);
};

#endif // SCANTEST_H
