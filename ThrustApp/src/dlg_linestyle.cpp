#include <QDebug>
#include <QCheckBox>
#include <QToolButton>
#include <QPalette>
#include <QColorDialog>

#include "dlg_linestyle.h"
#include "widget_graph.h"

DlgLineStyle::DlgLineStyle( const QPen& pen, QWidget* parent) 
	: QDialog( parent), mPen(pen)
{
	ui.setupUi(this);

//	mColor = mPen.color();
	ui.spinWidth->setValue( mPen.width() );

	QPalette palette;
	palette.setColor(QPalette::Window, mPen.color() );
	palette.setColor(QPalette::WindowText, Qt::black);

	ui.lblColor->setAutoFillBackground(true);
	ui.lblColor->setPalette(palette);
	//ui.lblColor->setText("What ever text");


	ui.comboStyle->addItem( QStringLiteral("Solid Line"), (int)Qt::SolidLine);
	ui.comboStyle->addItem( QStringLiteral("Dash Line"), (int)Qt::DashLine);
	ui.comboStyle->addItem( QStringLiteral("Dot Line"), (int)Qt::DotLine);
	ui.comboStyle->addItem( QStringLiteral("Dash Dot Line"), (int)Qt::DashDotLine);
	ui.comboStyle->addItem( QStringLiteral("Dash Dot Dot Line"), (int)Qt::DashDotDotLine);


	connect( this->ui.btnColor, &QToolButton::clicked, this, &DlgLineStyle::onColor);
};

QPen DlgLineStyle::getPen()
{
	mPen.setWidthF( ui.spinWidth->value() );
	mPen.setStyle( (Qt::PenStyle) ui.comboStyle->currentData().toInt() );
	//QPen pen( mPen.color(), ui.spinWidth->value(), Qt::SolidLine );
	return mPen;
}

void DlgLineStyle::onColor()
{
	QColor color = QColorDialog::getColor( mPen.color(), this );

	QPalette palette;
	palette.setColor(QPalette::Window, color);
	palette.setColor(QPalette::WindowText, Qt::black);

	ui.lblColor->setAutoFillBackground(true);
	ui.lblColor->setPalette(palette);

	mPen.setColor( color);
}
