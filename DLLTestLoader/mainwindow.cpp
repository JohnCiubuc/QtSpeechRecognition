#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  CSR = new QtSpeechRecognition(0);
  connect(CSR, &QtSpeechRecognition::debugMicLevel, this, &MainWindow::debugMicLevel);
  QStringList a;
  a << "open menu";
  a << "reload my gun";
  a << "show inventory";
  a << "quick save";
  CSR->loadKeywords(a);
  CSR->startListening();
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_pushButton_clicked()
{
//  CSR->startDebug();
  QStringList a;
  a << "say hello";
  a << "open quests";
  CSR->loadKeywords(a);
}

void MainWindow::on_pushButton_2_clicked()
{
//  CSR->stopDebug();
}

void MainWindow::debugMicLevel(float avg)
{
  ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText().append("\n").append(QString::number(avg)));
  ui->plainTextEdit->ensureCursorVisible();
  QScrollBar * sb = ui->plainTextEdit->verticalScrollBar();
  sb->setValue(sb->maximum());
}
