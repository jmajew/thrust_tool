#ifndef __DLG_LINESTYLE_H__
#define __DLG_LINESTYLE_H__

#include <QDialog>
#include <QPen>


#include "ui_dlg_linestyle.h" 

class GraphChannel;

//////////////////////////////////////////////////////////////////////
// class DlgLineStyle
//////////////////////////////////////////////////////////////////////
class DlgLineStyle : public QDialog
{
	Q_OBJECT

public:
	Ui::DlgLineStyle ui;

	DlgLineStyle( const QPen& pen, QWidget* parent);

	QPen	getPen();

protected:

public slots:
	void	onColor();

private:
	QPen	mPen;
}; 


#endif // __DLG_LINESTYLE_H__
