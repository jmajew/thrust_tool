#include <random>

#include <QDebug>
#include <QLayout>
#include <QHeaderView>
#include <QMenu>

#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QFont>
#include <QWidget>

#include "view_data.h"
#include "document.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////
//// class ViewChannel
//////////////////////////////////////////////////////////////////////////////////////////////////////

DataView::DataView( Document* ptr, QWidget* parent)
	: QWidget(parent)
{
	mpTable = new QTableView( this);
    this->setModel( &ptr->rData() );

    
    QFont font = mpTable->horizontalHeader()->font();

	QHeaderView *verticalHeader = mpTable->verticalHeader();
    verticalHeader->setFont( font );
	verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(30);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(mpTable);
    setLayout(mainLayout);

	connect( mpTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &DataView::onSectionClicked );

    //connect horizontal header QTableView with contextmenu
    QHeaderView* horizHeader = mpTable->horizontalHeader();
    horizHeader->setContextMenuPolicy(Qt::CustomContextMenu);     //set contextmenu

    connect(horizHeader, &QHeaderView::customContextMenuRequested, this, &DataView::onContextMenu );

}

void DataView::onContextMenu( const QPoint& pos )
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Show All", this);
    QAction action2("Hide Column", this);

    connect( &action1, &QAction::triggered, this, &DataView::showAllColumns );
    connect( &action2, &QAction::triggered, this, &DataView::hideColumns );
   
    contextMenu.addAction(&action1);
    contextMenu.addAction(&action2);

    contextMenu.exec(mapToGlobal(pos));
}

void DataView::hideColumns()
{
	QModelIndexList selection = mpTable->selectionModel()->selectedColumns();

	for( int i=0; i< selection.count(); ++i)
	{
		int id = selection.at(i).column();
        mpTable->hideColumn( id);
    }

    mpTable->clearSelection();
}

void DataView::showAllColumns()
{

    QAbstractItemModel* tableModel =  mpTable->model();
    for ( int i = 0; i< tableModel->columnCount(); ++i)
    {
        if ( mpTable->isColumnHidden(i) )
            mpTable->showColumn(i);
    }
}


void DataView::onSectionClicked( int index)
{
    //QMenu contextMenu(tr("Context menu"), this);

    //QAction action1("Show All", this);
    //QAction action2("Hide Column", this);

    //connect( &action1, SIGNAL(triggered()), this, SLOT(showAll()));
    //connect( &action2, SIGNAL(triggered()), this, SLOT(hideColumn(index)));
   
    //contextMenu.addAction(&action1);
    //contextMenu.addAction(&action2);

    //contextMenu.exec(mapToGlobal(pos));
}
