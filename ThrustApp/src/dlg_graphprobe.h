#ifndef __DLG_GRAPHPROBE_H__
#define __DLG_GRAPHPROBE_H__

#include <QDialog>

#include "ui_dlg_graphprobe.h" 


class GraphData;
class MeasurementData;

//////////////////////////////////////////////////////////////////////
// class DlgGraphProbe
//////////////////////////////////////////////////////////////////////
class DlgGraphProbe : public QDialog
{
	Q_OBJECT

public:
	Ui::DlgGraphProbe ui;

	DlgGraphProbe( const GraphData* pdat, QWidget* parent);

	void	initData( const GraphData* pdat);

	void	showEvent( QShowEvent *event);
	void	hideEvent( QHideEvent *event);

	int		getChannelId() const	{ return ui.comboChannel->currentIndex(); }
	void	setValues( double x, double y, double ch_y);
	void	setValues( double x, double y, const std::vector<double>& tab);

signals:
	void	sigHidden();

public slots:

private:
	//const MeasurementData*	mpData;
	const GraphData*	mpData;
}; 


#endif // __DLG_GRAPHPROBE_H__

