#ifndef __DLG_GRAPHSTYLE_H__
#define __DLG_GRAPHSTYLE_H__

#include <QDialog>
#include <QStyledItemDelegate>

#include "ui_dlg_graphstyle.h" 


class GraphData;
class GraphWidget;


class NumberFormatDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit NumberFormatDelegate(QObject *parent = 0);
	virtual QString displayText(const QVariant &value, const QLocale &locale) const;

signals:

public slots:
};

//////////////////////////////////////////////////////////////////////
// class DlgGraphStyle
//////////////////////////////////////////////////////////////////////
class DlgGraphStyle : public QDialog
{
	Q_OBJECT

public:
	Ui::DlgGraphStyle ui;

	DlgGraphStyle( GraphData* pdata, GraphWidget* parent);

	void	updateData();

	void	showEvent( QShowEvent *event);
	void	hideEvent( QHideEvent *event);

signals:
	void	sigHidden();

protected:
    //void	hideEvent(QHideEvent *event);

	void	saveDefaults();
	void	loadDefaults();


public slots:
	void	onSectionClicked( int index);
	void	onItemChanged(QTableWidgetItem *item);
	void	onEnableCooked(int state);
	void	onEnableRaw(int state);
	// void	togglePerspective( int state);
	// void	setViewAngle( double value);
	// void	toggleRotCenter();
	// void	setManualRotCenter();
	// void	updateRotCenter( const GLdGVect& data);

private:
	GraphData*			mpData;
	std::vector<QPen>	mtabPen;
}; 


#endif // __DLG_GRAPHSTYLE_H__
