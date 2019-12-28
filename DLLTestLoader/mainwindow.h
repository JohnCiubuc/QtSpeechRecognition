#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QScrollBar>

#include "qtspeechrecognition.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget * parent = nullptr);
  ~MainWindow();

  void populateDefaultList();
private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_lineEdit_returnPressed();

  void updateKeywords();

private:
  Ui::MainWindow * ui;
  QtSpeechRecognition * QtSR;
  bool bListening = false;
};
#endif // MAINWINDOW_H
