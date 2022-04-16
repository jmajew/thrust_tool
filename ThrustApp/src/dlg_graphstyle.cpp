#include <QDebug>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>

#include "dlg_graphstyle.h"
#include "widget_graph.h"
#include "document.h"
#include "dlg_linestyle.h"


NumberFormatDelegate::NumberFormatDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QString NumberFormatDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
	QString formattedNum = locale.toString(value.toDouble(), 'f', 2);
	return formattedNum;
}


DlgGraphStyle::DlgGraphStyle( GraphData* pdata, GraphWidget* parent) 
	: QDialog( parent), mpData(pdata)
{
	ui.setupUi(this);

	ui.tableWidget->setColumnCount( 5);
	ui.tableWidget->setRowCount( 0);

	ui.tableWidget->setItemDelegateForColumn(3, new NumberFormatDelegate(this));
	ui.tableWidget->setItemDelegateForColumn(4, new NumberFormatDelegate(this));


	QStringList labels;
	labels << tr("Name") << tr("On") << tr("Color") << tr("Min") << tr("Max");
	ui.tableWidget->setHorizontalHeaderLabels(labels);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode( 2, QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode( 3, QHeaderView::Stretch);
	//ui.tableWidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch);

	ui.tableWidget->setColumnWidth(1, 40);

	QHeaderView *verticalHeader = ui.tableWidget->verticalHeader();
	verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(20);

	//ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	//int row = filesTable->rowCount();
	//filesTable->insertRow(row);
	//filesTable->setItem(row, 0, fileNameItem);
	//filesTable->setItem(row, 1, sizeItem);

	//for (int irow=0; irow!=ui.tableWidget->rowCount(); ++irow)
	//{
	//	QTableWidgetItem * const item = new QTableWidgetItem;
	//	item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
	//	item->setCheckState( Qt::Checked );
	//	
	//	ui.tableWidget->setItem( irow, 1, item);
	//}

	mtabPen.resize( mpData->sizeChannel() );

	for ( int i=0; i<(int)mpData->sizeChannel(); ++i)
	{
		const GraphChannel& ch = mpData->cChannel(i);

		ui.tableWidget->insertRow( i);

//		QTableWidgetItem *item = new QTableWidgetItem( ch.cName() );
		QTableWidgetItem *item = new QTableWidgetItem( mpData->getData()->getChannel(i).getName() );
		ui.tableWidget->setItem( i, 0, item );

		item = new QTableWidgetItem;
		item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
//		item->setTextAlignment(Qt::AlignHCenter);
		if ( ch.isEnabled() )
			item->setCheckState( Qt::Checked );
		else
			item->setCheckState( Qt::Unchecked );
		ui.tableWidget->setItem( i, 1, item );

		//QPushButton* button = new QPushButton( "...");
		//ui.tableWidget->setCellWidget( i, 1, button);

		////QWidget *pwidget = ( ui.tableWidget->cellWidget(i,1));
		////pwidget->setStyleSheet("margin-left:50%; margin-right:50%;");
		//item = new QTableWidgetItem;
		//ui.tableWidget->setItem( i, 1, item );
		//QCheckBox *checkBox = new QCheckBox();
		////checkBox->setStyleSheet("margin-left:50%; margin-right:50%;");
		//ui.tableWidget->setCellWidget(i,1, checkBox);
		//ui.tableWidget->cellWidget(i,1)->setStyleSheet("margin-left:50%; margin-right:50%;");

		mtabPen[i] = ch.getPen();

		ui.tableWidget->setItem(i, 2, new QTableWidgetItem);
		ui.tableWidget->item(i, 2)->setBackground( mtabPen[i].color() );

		const ChannelRange& chinfo = mpData->getData()->getDataRange(i);

		item = new QTableWidgetItem( QString::number( chinfo.cMin() ) );
		ui.tableWidget->setItem( i, 3, item );

		item = new QTableWidgetItem( QString::number( chinfo.cMax() ) );
		ui.tableWidget->setItem( i, 4, item );

		//QTableWidgetItem *itm = ui.tableWidget->item(i,1);
		//if ( itm->checkState() == Qt::Checked )
		//	qDebug() << "wdqwd";
	}

	ui.buttonOK->setFocus(Qt::OtherFocusReason);

	connect( ui.tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &DlgGraphStyle::onSectionClicked );

	connect( ui.buttonApply, &QPushButton::clicked, this, &DlgGraphStyle::updateData);
	connect( ui.buttonOK, &QPushButton::clicked, this, &DlgGraphStyle::updateData);
	connect( ui.buttonSaveDef, &QPushButton::clicked, this, &DlgGraphStyle::saveDefaults);
	connect( ui.buttonLoadDef, &QPushButton::clicked, this, &DlgGraphStyle::loadDefaults);
	connect( ui.tableWidget, &QTableWidget::itemChanged, this, &DlgGraphStyle::onItemChanged);

	connect(ui.checkBox_EnCooked, &QCheckBox::stateChanged, this, &DlgGraphStyle::onEnableCooked);
	connect(ui.checkBox_EnRaw, &QCheckBox::stateChanged, this, &DlgGraphStyle::onEnableRaw);

}

void DlgGraphStyle::onEnableCooked(int state)
{
	bool bstate = true;

	if ( state == Qt::Unchecked)
		bstate = false;
	else if ( state == Qt::Checked)
		bstate = true;

	for ( int i=CHANNEL_RAW_COUNT; i<CHANNEL_COUNT; ++i)
	{
		GraphChannel& ch = mpData->rChannel(i);
		ch.setEnabled( bstate);

		ui.tableWidget->item(i, 1)->setCheckState( (Qt::CheckState)state );
	}

	((GraphWidget*)parent())->updateFull();		// FIXME ::
}

void DlgGraphStyle::onEnableRaw(int state)
{
	bool bstate = true;

	if ( state == Qt::Unchecked)
		bstate = false;
	else if ( state == Qt::Checked)
		bstate = true;

	for ( int i=0; i<CHANNEL_RAW_COUNT; ++i)
	{
		GraphChannel& ch = mpData->rChannel(i);
		ch.setEnabled( bstate);

		ui.tableWidget->item(i, 1)->setCheckState( (Qt::CheckState)state );
	}

	((GraphWidget*)parent())->updateFull();		// FIXME ::
}


void DlgGraphStyle::onSectionClicked( int index)
{
	qDebug() << "Header id =" << index << "clicked";

	if ( index != 2 )
		return;

	QModelIndexList selection = ui.tableWidget->selectionModel()->selectedRows();

	if ( selection.count() == 0 )
	{
		QMessageBox::warning( this,	tr("Warning"), tr("No rows are selected") );
		return;
	}

	int first_row = selection.at(0).row();
	DlgLineStyle dialog( mtabPen[first_row], this);

	if ( dialog.exec() == QDialog::Accepted )
	{
		for( int i=0; i< selection.count(); ++i)
		{
			int id = selection.at(i).row();

			mtabPen[id] = dialog.getPen();

			ui.tableWidget->item(id, 2)->setBackground( mtabPen[id].color() );
		}
	}


	//for( int i=0; i< selection.count(); ++i)
	//{
	//	QModelIndex index = selection.at(i);
	//	qDebug() << " row id =" << index.row();
	//}
	
}

void DlgGraphStyle::onItemChanged(QTableWidgetItem *item)
{
	int col = item->column();
	int row = item->row();

	if ( col < 3 )
		return;

	double dmin, dmax;

	if ( col == 3 )			// range minimum
	{
		QTableWidgetItem *itemMax = ui.tableWidget->item( row, 4);
		dmin = item->data( Qt::DisplayRole ).toReal();
		dmax = itemMax->data( Qt::DisplayRole ).toReal();
	}
	else if ( col == 4 )	// range maximum
	{
		QTableWidgetItem *itemMin = ui.tableWidget->item( row, 3);
		dmin = itemMin->data( Qt::DisplayRole ).toReal();
		dmax = item->data( Qt::DisplayRole ).toReal();
	}

	
	ChannelRange range;
	range.setRange( dmin, dmax);

	GraphChannel& ch = mpData->rChannel(row);
	ch.updaTeTransform( range);
}


void DlgGraphStyle::showEvent( QShowEvent *event)
{
	QSettings settings;

	restoreGeometry(settings.value("DlgGraphStyle/WindowGeometry").toByteArray());
} 

void DlgGraphStyle::hideEvent( QHideEvent *event)
{
	emit this->sigHidden();

	QSettings settings;
	settings.setValue("DlgGraphStyle/WindowGeometry", saveGeometry());

	//qDebug() << saveGeometry();
} 

void DlgGraphStyle::saveDefaults()
{
	mpData->saveSettings();
}

void DlgGraphStyle::loadDefaults()
{
	mpData->loadSettings();

	for ( int i=0; i<(int)mpData->sizeChannel(); ++i)
	{
		const GraphChannel& ch = mpData->cChannel(i);

		int id = i;

		if ( ch.isEnabled() )
			ui.tableWidget->item(id, 1)->setCheckState( Qt::Checked );
		else
			ui.tableWidget->item(id, 1)->setCheckState( Qt::Unchecked );

		QRgb color = ch.getPen().color().rgb();

		mtabPen[id] = ch.getPen();

		ui.tableWidget->item(id, 2)->setBackground( mtabPen[id].color() );
	}


	((GraphWidget*)parent())->updateFull();		// FIXME ::
}


void DlgGraphStyle::updateData()
{
	for ( int i=0; i<(int)mpData->sizeChannel(); ++i)
	{
		GraphChannel& ch = mpData->rChannel(i);

		QTableWidgetItem *item = ui.tableWidget->item(i,1);
		if ( item->checkState() == Qt::Checked )
			ch.setEnabled(true);
		else
			ch.setEnabled(false);

		item = ui.tableWidget->item(i,0);
		//ch.rName() = item->text();

		ch.setPen( mtabPen[i] );

		int32_t ch_min = ui.tableWidget->item(i,3)->text().toULongLong();
		int32_t ch_max = ui.tableWidget->item(i,4)->text().toULongLong();


		((GraphWidget*)parent())->updateFull();		// FIXME ::

		//const ChannelInfo& chinfo = mpData->getMsData()->getDataRange(i);

		//item = new QTableWidgetItem( QString::number( chinfo.cMin() ) );
		//ui.tableWidget->setItem( i, 2, item );

		//item = new QTableWidgetItem( QString::number( chinfo.cMax() ) );
		//ui.tableWidget->setItem( i, 3, item );
	}
}

