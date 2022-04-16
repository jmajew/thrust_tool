#ifndef __DLG_AUTOCREATE_H__
#define v

#include <QDialog>

#include "ui_dlg_autocreate.h" 


class GraphData;
class MeasurementData;

//////////////////////////////////////////////////////////////////////
// class DlgAutoCreate
//////////////////////////////////////////////////////////////////////
class DlgAutoCreate : public QDialog
{
	Q_OBJECT

public:
	Ui::DlgAutoCreate ui;

	DlgAutoCreate( QWidget* parent);

	// void	initData( const GraphData* pdat);

	// void	showEvent( QShowEvent *event);
	// void	hideEvent( QHideEvent *event);

	// int		getChannelId() const	{ return ui.comboChannel->currentIndex(); }
	// void	setValues( double x, double y, double ch_y);
	// void	setValues( double x, double y, const std::vector<double>& tab);

signals:
	// void	sigHidden();

public slots:

private:
	//const MeasurementData*	mpData;
	// const GraphData*	mpData;
}; 


#endif // __DLG_AUTOCREATE_H__

