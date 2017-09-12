#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStringListModel(this);

    // Make data
    //QStringList List;
    //List << "Clair de Lune" << "Reverie" << "Prelude";

    // Populate our model
    //model->setStringList(List);

    // Glue model and view together
    ui->listView->setModel(model);
    //ui->comboBox->setModel(model);

    // Add additional feature so that
    // we can manually modify the data in ListView
    // It may be triggered by hitting any key or double-click etc.
    //ui->listView->
    //        setEditTriggers(QAbstractItemView::AnyKeyPressed |
    //                        QAbstractItemView::DoubleClicked);

    scene = new QGraphicsScene(this);
    pixItem = new QGraphicsPixmapItem();

    connect(ui->listView, &QListView::activated, this, &MainWindow::showInfo);
    connect(ui->listView, &QListView::clicked, this, &MainWindow::showInfo);
    connect(ui->listView, &QListView::doubleClicked, this, &MainWindow::showInfo);
    connect(ui->listView, &QListView::entered, this, &MainWindow::showInfo);
    connect(ui->listView, &QListView::pressed, this, &MainWindow::showInfo);
}

MainWindow::~MainWindow()
{
    delete pixItem;
    delete scene;
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

static QStringList getSupportedFormatsFilters()
{
  QStringList filters;
  filters.append("JSON file (*.json)");
  filters.append("Any files (*)");
  return filters;
}

void MainWindow::showInfo(const QModelIndex& index)
{
    int id = index.row();
    if (id != -1) {
        ui->textBrowser->setText(lkp[id].formatedInfo());
        if (imgDir.exists()) {
            QString fileName = lkp[id].id() + ".jpg";
            QImage img(imgDir.filePath(fileName));

            scene->removeItem(pixItem);
            delete scene;
            scene = new QGraphicsScene(this);
            pixItem->setPixmap(QPixmap::fromImage(img));
            scene->addItem(pixItem);
            //item.setPos(0, 0);

            lkp[id].drawHumans(scene);
            //scene->addLine(0, 0, 42, 42, QPen(QBrush(Qt::black),1));

            scene->setSceneRect(pixItem->boundingRect());
            ui->graphicsView->setScene(scene);
            //ui->graphicsView->setFixedSize(img.width(), img.height());
            ui->graphicsView->setFixedSize(scene->sceneRect().width(), scene->sceneRect().height());
            //ui->graphicsView->update();
            ui->graphicsView->show();
        }
    }
}

void MainWindow::on_openButton_clicked()
{
    QFileDialog openDialog(this);
    //openDialog.setDirectory(settings.value("lastFilePath").toString());
    openDialog.setFileMode(QFileDialog::ExistingFiles);
    openDialog.setNameFilters(getSupportedFormatsFilters());

    QStringList fileNames;
    if (openDialog.exec())
      fileNames = openDialog.selectedFiles();

    QString filePath;
    if (fileNames.count() > 0)
    {
        filePath = fileNames.at(0);

        jp.Parse(filePath);

        QStringList list;
        for (int i = 0; i < jp.size(); ++i) {
            KeyPointJson kp(jp.read(i));
            lkp.push_back(kp);
            list.append(kp.id());
        }
        model->setStringList(list);
    }
}

void MainWindow::on_dirButton_clicked()
{
    QFileDialog openDialog(this);
    //openDialog.setDirectory(settings.value("lastFilePath").toString());
    openDialog.setFileMode(QFileDialog::DirectoryOnly);

    QStringList dirNames;
    if (openDialog.exec())
      dirNames = openDialog.selectedFiles();

    if (!dirNames.empty()) {
        QString dirName = dirNames[0];

        imgDir.setPath(dirName);
    }
}
