#include <QLayout>
#include <QMessageBox>

#include "view_terminal.h"


TerminalView::TerminalView(  QWidget* parent) : QWidget(parent)
{
    mPortName = "";
    mPortRate = 9600;

	mpConsole = new Console(this);

	QFont font( "Courier New", 11);
	mpConsole->setFont( font);
    mpConsole->setEnabled(false);

    mpbtnOpenTerm = new QPushButton( "Start Terminal");
    //mpbtnCloseTerm = new QPushButton( "Stop Terminal");
    //mpbtnCloseTerm->setDisabled(true);

    QHBoxLayout *bttnLayout = new QHBoxLayout();
    bttnLayout->addWidget( mpbtnOpenTerm);
    //bttnLayout->addWidget( mpbtnCloseTerm);
    bttnLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(bttnLayout);
    mainLayout->addWidget(mpConsole);
    setLayout(mainLayout);

    //connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    //connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    //connect( mpConsole, &Console::getData, this, &MainWindow::writeData);

}

void TerminalView::enableTerminal()
{
    //connect( mSerial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);

    mSerial.setPortName( mPortName);
    mSerial.setBaudRate( mPortRate);
    
    mSerial.setDataBits(QSerialPort::Data8);
    mSerial.setParity(QSerialPort::NoParity);
    mSerial.setStopBits(QSerialPort::OneStop);

    bool b = mSerial.open(QIODevice::ReadWrite);

    bool bopen = mSerial.isOpen();
    bool bw = mSerial.isWritable();
    bool br = mSerial.isReadable();

    mpConsole->setEnabled(true);
    mpConsole->setLocalEchoEnabled(false);

    connect( &mSerial, &QSerialPort::errorOccurred, this, &TerminalView::handleError);
    connect( &mSerial, &QSerialPort::readyRead, this, &TerminalView::readData);
    connect( mpConsole, &Console::getData, this, &TerminalView::writeData);

    mpbtnOpenTerm->setDisabled(true);
    mpConsole->setFocus();

    mSerial.write("\r\n");
    const QByteArray data = mSerial.readAll();
}

void TerminalView::disableTerminal()
{
    disconnect( &mSerial, &QSerialPort::readyRead, this, &TerminalView::readData);
    disconnect( mpConsole, &Console::getData, this, &TerminalView::writeData);

    mSerial.close();

    mpConsole->setEnabled(false);
    mpbtnOpenTerm->setEnabled(true);

    emit restoreSerial();
}




void TerminalView::writeData(const QByteArray &data)
{
    mSerial.write(data);
}

void TerminalView::readData()
{
    const QByteArray data = mSerial.readAll();
    mpConsole->putData(data);

    if ( mpConsole->Exit() )
    {
        disableTerminal();
        mpConsole->ResetExit();
    }
}

void TerminalView::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), mSerial.errorString());
        //closeSerialPort();
    }
}
