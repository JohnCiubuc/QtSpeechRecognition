#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  QtSR = new QtSpeechRecognition(0, QDir::currentPath().append("/model"));
  connect(QtSR, &QtSpeechRecognition::firstHypothesis, this,
          [ = ]( const QString hypothesis )
  {
    ui->label_hypo->setText(hypothesis);
  });
  populateDefaultList();
  updateKeywords();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::populateDefaultList()
{
  QStringList a;
  a << "open menu";
  a << "reload my gun";
  a << "show inventory";
  a << "quick save";

  for(auto string : a)
  {
    QListWidgetItem * nItem = new QListWidgetItem;
    nItem->setText(string);
    ui->listWidget->insertItem(0, nItem);
  }
}


void MainWindow::on_pushButton_clicked()
{
  bListening = !bListening;

  if(bListening)
  {
    ui->pushButton->setText("Stop Listening");
    QtSR->startListening();
  }
  else
  {
    ui->pushButton->setText("Start Listening");
    QtSR->stopListening();
  }
}

void MainWindow::on_pushButton_2_clicked()
{
  for(auto item : ui->listWidget->selectedItems())
    ui->listWidget->removeItemWidget(item);

  updateKeywords();
}

void MainWindow::on_lineEdit_returnPressed()
{
  QListWidgetItem * nItem = new QListWidgetItem;
  nItem->setText(ui->lineEdit->text());
  ui->listWidget->insertItem(0, nItem);
  ui->lineEdit->clear();
  updateKeywords();
}

void MainWindow::updateKeywords()
{
  QStringList keywords;

  for(int i = 0; i < ui->listWidget->count(); ++i)
    keywords << ui->listWidget->item(i)->text();

  QtSR->loadKeywords(keywords);
}
