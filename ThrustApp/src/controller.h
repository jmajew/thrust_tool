#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <QDebug>
#include <QtGlobal>
#include <QObject>

#include "widget_control.h"


class MainWindow;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class Controlller
////////////////////////////////////////////////////////////////////////////////////////////////////
class Controller : public QObject
{
    Q_OBJECT

public:
	using MsPoint = AutoMeasureList::MsPoint;

	Controller( MainWindow* parent = nullptr);

	void	Execute( const AutoMeasureList& list);

signals:
	void	started();
	void	stopped();

public slots:
	void	onStart( const AutoMeasureList* plist);
	void	onStop();
	void	onDataReceived();

private:
	void	ExecutePoint( const MsPoint& pnt);

private:
	MainWindow*	mpMain;

	bool	mbIsOn;
	const AutoMeasureList* mpList;

	int		mIdBeg;

	int			mIdCurPoint;
	int			mIdCur;
};

#endif // __CONTROLLER_H__
