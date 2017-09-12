#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "jsonparser.h"
#include "keypointjson.h"

class QImage;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();

    void on_openButton_clicked();

    void on_dirButton_clicked();

private:
    Ui::MainWindow *ui;

    JsonParser jp;
    QList<KeyPointJson> lkp;
    QStringListModel * model;

    QDir imgDir;
    void showInfo(const QModelIndex &index);

    QGraphicsScene * scene;
    QGraphicsPixmapItem * pixItem;
};

#endif // MAINWINDOW_H
