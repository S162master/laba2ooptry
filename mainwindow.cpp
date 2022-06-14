#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fasade.h"
#include <QFile>
#include <QFileDialog>
#include <string>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::print_json(){ // просто метод для отрисовки, ничего такого
    QString text;
    for(size_t i = 0; i < dataVector.size(); i++){
        text.append(QString::number(i + 1) + '\t' + QString::fromStdString(dataVector[i]) + '\n');
    }
    ui->json_text->setText(text);
}

void MainWindow::on_pushButton_clicked() // слот для выбора файла
{
    QFile f (QFileDialog::getOpenFileName(this,tr("Open File"),"",tr("Data Files (*.json)")));
   if(f.fileName().isNull())
   {
       ui->textBrowser_name_file->setText("No file celected!");
   }else
   {
       QFileInfo fileInfo(f.fileName());
       ui->textBrowser_name_file->setText(fileInfo.fileName());
       std::string name = (f.fileName()).toStdString();
       dataVector.clear();
       if (!fasade::interfaceBundle(fasade::OPEN_FILE, name, dataVector))
       {
           ui->textBrowser_name_file->setText(f.errorString());
           qDebug() << f.errorString();
       }
   }
   print_json();
}


void MainWindow::on_pushButton_2_clicked() // слот для проверки на грамотность файла JSON
{
    std::string error;
    if (fasade::interfaceBundle(fasade::CHECK, error, dataVector)){
        ui->textBrowser_error->setText("OK");
    } else {
         ui->textBrowser_error->setText("Error check: " + QString::fromStdString(error));
    }
}

