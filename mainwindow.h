#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "recordinglightstatemachine.h"

namespace Ui {
class MainWindow;
}

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    RecordingLightStateMachine* m_rlSM;

private slots:
    void recordingStatusChanged();
    void connectionStatusChanged();

    void pushButtonConnectClicked();
    void colorLabelClicked();
    void pushButtonGetColorsClicked();
    void pushButtonSetColorsClicked();

    void onColorReceived(RecordingLightStateMachine::colorHolder holder, QColor col);
};

#endif // MAINWINDOW_H
