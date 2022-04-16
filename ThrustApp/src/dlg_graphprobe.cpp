#include <QDebug>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>

#include "dlg_graphprobe.h"
#include "document.h"
#include "widget_graph.h"


DlgGraphProbe::DlgGraphProbe( const GraphData* pdat, QWidget* parent) 
	: QDialog( parent)
{
	ui.setupUi(this);

	initData( pdat);

//	for ( int i=0; i<mpData->sizeChannel(); ++i)
//		ui.comboChannel->addItem( QString::number(i) + QString(" - ") + mpData->getChannel(i).getName(), i );
//
//	ui.comboChannel->setCurrentIndex( 0 );
//
//	QHeaderView *verticalHeader = ui.tableChannel->verticalHeader();
//	verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
//	verticalHeader->setDefaultSectionSize(20);
//	
//	ui.tableChannel->setEditTriggers(QAbstractItemView::NoEditTriggers);
//
//	ui.tableChannel->setRowCount( (int)mpData->sizeChannel() );
//    ui.tableChannel->setColumnCount(2);
//	for ( int ir=0; ir<mpData->sizeChannel(); ++ir)
//	{
//		ui.tableChannel->setItem(ir, 0, new QTableWidgetItem( mpData->getChannel(ir).getName() ) );
//		ui.tableChannel->setItem(ir, 1, new QTableWidgetItem( QString::number(0) ) );
//	}
//
////	connect( ui.tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &DlgGraphStyle::onSectionClicked );
}

void DlgGraphProbe::initData(const GraphData* pdat)
{
	assert( pdat != nullptr);

	mpData = pdat;

	if ( ui.comboChannel->count() != 0)
		ui.comboChannel->clear();

	for ( int i=0; i<mpData->getData()->sizeChannel(); ++i)
		ui.comboChannel->addItem( QString::number(i) + QString(" - ") + mpData->getDataChannel(i)->getName(), i );

	ui.comboChannel->setCurrentIndex( 0 );

	QHeaderView *verticalHeader = ui.tableChannel->verticalHeader();
	verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(20);
	
	ui.tableChannel->setEditTriggers(QAbstractItemView::NoEditTriggers);

	if ( ui.tableChannel->rowCount() != 0 )
		ui.tableChannel->clear();

	ui.tableChannel->setRowCount( (int)mpData->getData()->sizeChannel() );
    ui.tableChannel->setColumnCount(2);
	for ( int ir=0; ir<mpData->getData()->sizeChannel(); ++ir)
	{
		ui.tableChannel->setItem(ir, 0, new QTableWidgetItem( mpData->getDataChannel(ir)->getName() ) );
		ui.tableChannel->setItem(ir, 1, new QTableWidgetItem( QString::number(0) ) );
	}

	QStringList labels;
	labels << tr("Name") << tr("Value");
	ui.tableChannel->setHorizontalHeaderLabels(labels);

	ui.tableChannel->setColumnWidth(1, 90);
	ui.tableChannel->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch);
	//int w = ui.tableChannel->getContentsMargins();
	//int k = w;

}



void DlgGraphProbe::showEvent( QShowEvent *event)
{
	QSettings settings;

	restoreGeometry(settings.value("DlgGraphProbe/WindowGeometry").toByteArray());
} 

void DlgGraphProbe::hideEvent( QHideEvent *event)
{
	emit this->sigHidden();

	QSettings settings;
	settings.setValue("DlgGraphProbe/WindowGeometry", saveGeometry());
} 

void DlgGraphProbe::setValues( double x, double y, double ch_y)
{
	QString str_x = QString::number( x, 'f', 0 );
	QString str_y = QString::number( y, 'f', 0 );
	QString str_chy = QString::number( ch_y, 'f', 3 );

	ui.labelValueX->setText( str_x );
	ui.labelValueY->setText( str_y );
	ui.labelChannelY->setText( str_chy );
}

void DlgGraphProbe::setValues(double x, double y, const std::vector<double>& tab)
{
    for (int ch = 0; ch < mpData->sizeChannel(); ++ch)
    {
		QString str_ch = QString::number( tab[ch] );
		QTableWidgetItem *item = ui.tableChannel->item(ch,1);
		item->setText(str_ch);
	}

}
