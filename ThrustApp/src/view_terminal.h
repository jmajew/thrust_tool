#ifndef __VIEW_TERMINAL_H__
#define __VIEW_TERMINAL_H__

#include <QWidget>
#include <QPushButton>
#include <QtSerialPort/QSerialPort>

#include "console.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// class TerminalView
////////////////////////////////////////////////////////////////////////////////////////////////////
class TerminalView : public QWidget
{
	Q_OBJECT

	friend class MainWindow;
	friend class TabWidget;

public:
	explicit TerminalView( QWidget* parent);
	
	void	setPortInfo( QString name, int rate)	{ mPortName = name; mPortRate = rate; }

signals:
		void	restoreSerial();

private slots:
    void	writeData(const QByteArray &data);
    void	readData();
	void	handleError(QSerialPort::SerialPortError error);

public:
	void	enableTerminal();
	void	disableTerminal();

private:
 //   QAction*        mptestAct;
	//QToolBar*		mpToolBar;

	QPushButton*	mpbtnOpenTerm;
	QPushButton*	mpbtnCloseTerm;

    QSerialPort		mSerial;
    QString			mPortName;
    int				mPortRate;

	Console*		mpConsole;
};

#endif // __VIEW_TERMINAL_H__
