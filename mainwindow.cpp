#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "recordinglightstatemachine.h"
#include "bluetoothserialport.h"

#include <QColorDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_rlSM = new RecordingLightStateMachine();

    connect(m_rlSM, SIGNAL(recordingStatusChanged()), this, SLOT(recordingStatusChanged()));
    connect(m_rlSM, SIGNAL(connectionStatusChanged()), this, SLOT(connectionStatusChanged()));

    connect(ui->pushButtonConnect, SIGNAL(clicked()), this, SLOT(pushButtonConnectClicked()));

    connect(ui->labelIdleColor, SIGNAL(clicked()), this, SLOT(colorLabelClicked()));
    connect(ui->labelReadyColor, SIGNAL(clicked()), this, SLOT(colorLabelClicked()));
    connect(ui->labelRecColor, SIGNAL(clicked()), this, SLOT(colorLabelClicked()));
    connect(ui->labelCueColor, SIGNAL(clicked()), this, SLOT(colorLabelClicked()));

    connect(ui->pushButtonSendTestMessage, SIGNAL(clicked()), m_rlSM, SLOT(sendTestMessage()));

    recordingStatusChanged();
    connectionStatusChanged();

    QColor col = Qt::white;
    ui->labelIdleColor->setColor(col);
    ui->labelReadyColor->setColor(col);
    ui->labelRecColor->setColor(col);
    ui->labelCueColor->setColor(col);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_rlSM;
}

void MainWindow::recordingStatusChanged(){
    switch (m_rlSM->getRecordingStatus()){
    case MidiHandler::Idle:
        ui->labelRecordingStatus->setText("Idle");
        ui->labelRecordingStatus->setStyleSheet("QLabel { color: black }");
        break;
    case MidiHandler::Ready:
        ui->labelRecordingStatus->setText("Ready");
        ui->labelRecordingStatus->setStyleSheet("QLabel { color: orange }");
        break;
    case MidiHandler::Recording:
        ui->labelRecordingStatus->setText("Recording");
        ui->labelRecordingStatus->setStyleSheet("QLabel { color: red }");
        break;
    case MidiHandler::Cue:
        ui->labelRecordingStatus->setText("Cue");
        ui->labelRecordingStatus->setStyleSheet("QLabel { color: blue }");
        break;
    default:
        qDebug() << "recording status unknown";
        break;
    }
}

void MainWindow::connectionStatusChanged(){
    switch (m_rlSM->getConnectionStatus()){
    case RLCmdMaster::NOTCONNECTED:
        ui->labelConnectionStatus->setText("Not Connected");
        break;
    case RLCmdMaster::OK:
        ui->labelConnectionStatus->setText("OK");
        break;
    case RLCmdMaster::ERROR:
        ui->labelConnectionStatus->setText("Error");
        break;
    default:
        qDebug() << "connection status unknown";
        break;
    }
}

void MainWindow::pushButtonConnectClicked(){
    m_rlSM->connectToDevice();
}

void MainWindow::colorLabelClicked(){
    ColorLabel* lbl = qobject_cast<ColorLabel*>(sender());
    QColorDialog dialog;
    dialog.setCurrentColor(lbl->color());
    if (dialog.exec() == QColorDialog::Accepted){
        QColor col = dialog.currentColor();
        lbl->setColor(col);
    }

}

void MainWindow::pushButtonGetColorsClicked(){
    m_rlSM->requestColor(RecordingLightStateMachine::idle);
    m_rlSM->requestColor(RecordingLightStateMachine::ready);
    m_rlSM->requestColor(RecordingLightStateMachine::rec);
    m_rlSM->requestColor(RecordingLightStateMachine::cue);
}

void MainWindow::pushButtonSetColorsClicked(){
    m_rlSM->setColor(RecordingLightStateMachine::idle, ui->labelIdleColor->color());
    m_rlSM->setColor(RecordingLightStateMachine::ready, ui->labelReadyColor->color());
    m_rlSM->setColor(RecordingLightStateMachine::rec, ui->labelRecColor->color());
    m_rlSM->setColor(RecordingLightStateMachine::cue, ui->labelCueColor->color());
}

void MainWindow::onColorReceived(RecordingLightStateMachine::colorHolder holder, QColor col){
    switch(holder){
    case RecordingLightStateMachine::idle:
        ui->labelIdleColor->setColor(col);
        break;
    case RecordingLightStateMachine::ready:
        ui->labelReadyColor->setColor(col);
        break;
    case RecordingLightStateMachine::rec:
        ui->labelRecColor->setColor(col);
        break;
    case RecordingLightStateMachine::cue:
        ui->labelCueColor->setColor(col);
        break;
    }
}
