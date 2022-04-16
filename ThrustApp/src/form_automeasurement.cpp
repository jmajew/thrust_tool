#include "form_automeasurement.h"

//#include <iostream>
//#include <sstream>
//#include <iomanip>

#include <QMenu>

#include "dlg_autocreate.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class AutoMeasureList
////////////////////////////////////////////////////////////////////////////////////////////////////

QVariant AutoMeasureList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) 
	{
        switch (section)
        {
        case 0:
            return QString("Level");
        case 1:
            return QString("Measure");
        };
    }
	else if (role == Qt::DisplayRole && orientation == Qt::Vertical)
	{
		return QString::number(section);
	}

    return QVariant();
}

Qt::ItemFlags AutoMeasureList::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool AutoMeasureList::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) 
    {
        //stringList.replace(index.row(), value.toString());
        mtabPnts[index.row()][index.column() ] = value.toInt();
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool AutoMeasureList::insertRows(int position, int rows, const QModelIndex &index )
{
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) 
        mtabPnts.insert( mtabPnts.begin() + position, MsPoint{0,0} );

    endInsertRows();
    return true;
}

bool AutoMeasureList::removeRows(int position, int rows, const QModelIndex &index )
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) 
    {
        mtabPnts.erase( mtabPnts.begin() + position, mtabPnts.begin() + position + rows);
    }

    endRemoveRows();
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormAutoMeasurement
////////////////////////////////////////////////////////////////////////////////////////////////////

FormAutoMeasurement::FormAutoMeasurement(QWidget *parent)
    : QWidget(parent)
{
    mLevelMin   = 20;
    mLevelMax   = 100;
    mDLevel     = 10;
    mMeasure    = 30;
    mSeparation = 20;

    setupUi(this);

    tableView->setModel( &mList);

    //QFont fontH = tableView->horizontalHeader()->font();
    //fontH.setPointSize( 10 );
    //tableView->horizontalHeader()->setFont( fontH );

    //QFont font = tableView->verticalHeader()->font();
    //font.setPointSize( 10 );
    //tableView->verticalHeader()->setFont( font );

    QFont font = tableView->horizontalHeader()->font();

	QHeaderView *verticalHeader = tableView->verticalHeader();
    verticalHeader->setFont( font );
	verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(20);

	tableView->setColumnWidth(0, 50);
	tableView->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Stretch);

    //connect( spinNSamples, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &FormCalibration::updateTime);

    //connect( this->pushBtn_AddPnt,  &QPushButton::clicked, this, &FormAutoMeasurement::onAddPoint );
    connect( this->pushBtn_AutoCreate, &QPushButton::clicked, this, &FormAutoMeasurement::onAutoCreate );

    tableView->setContextMenuPolicy(Qt::CustomContextMenu);     //set contextmenu
    verticalHeader->setContextMenuPolicy(Qt::CustomContextMenu);     //set contextmenu

    connect(this->tableView, &QTableView::customContextMenuRequested, this, &FormAutoMeasurement::onContextMenu );
    connect(verticalHeader, &QHeaderView::customContextMenuRequested, this, &FormAutoMeasurement::onContextMenu );

}

//void FormAutoMeasurement::onAddPoint()
//{
//    int level = spinBox_Motor->value();
//    int measure = spinBox_Measure->value();
//
//    if ( measure >= 5 )
//    {
//        AutoMeasureList::MsPoint data;
//        data[0] = level;
//        data[1] = measure;
//
//        this->mList.append( data);
//    }
//}

void FormAutoMeasurement::onAutoCreate()
{
    using MsPoint =  AutoMeasureList::MsPoint;

    DlgAutoCreate dialog( this);

    //int mLevelMin;
    //int mLevelMax;
    //int mDLevel;
    //int mMeasure;
    //int mSeparation;

    dialog.ui.spinBox_Measure->setValue( mMeasure);
    dialog.ui.spinBox_Separation->setValue( mSeparation);
    dialog.ui.spinBox_MinLevel->setValue( mLevelMin);
    dialog.ui.spinBox_MaxLevel->setValue( mLevelMax);
    dialog.ui.spinBox_DLevel->setValue( abs(mDLevel) );


    int res = dialog.exec();
    if ( res == QDialog::Accepted )
    {
        mLevelMin   = dialog.ui.spinBox_MinLevel->value();
        mLevelMax   = dialog.ui.spinBox_MaxLevel->value();
        mDLevel     = dialog.ui.spinBox_DLevel->value();
        mMeasure    = dialog.ui.spinBox_Measure->value();
        mSeparation = dialog.ui.spinBox_Separation->value();

        // create mList
        mList.clear();

        bool bsig =  mLevelMax - mLevelMin > 0;

        if ( !bsig )
            mDLevel = -mDLevel;


        this->mList.append( MsPoint{ 0, mMeasure } );
     
        int lev = mLevelMin;
        for ( ; ( bsig && lev <= mLevelMax ) || ( !bsig && lev >= mLevelMax ); lev += mDLevel )
        {
            if ( mSeparation > 0 && lev != mLevelMin )
                this->mList.append( MsPoint{ 0, mSeparation } );

            this->mList.append( MsPoint{ lev, mMeasure } );
        }

        if ( abs(lev - dialog.ui.spinBox_MaxLevel->value()) < abs(mDLevel) )
        {
            if ( mSeparation > 0 )
                this->mList.append( MsPoint{ 0, mSeparation } );

            this->mList.append( MsPoint{ mLevelMax, mMeasure } );
        }

        //if ( mDLevel > 0 )
        //{
        //    int lev = mLevelMin;
        //    for ( ; lev <= mLevelMax; lev += mDLevel )
        //    {
        //        if ( mSeparation > 0 && lev > mLevelMin )
        //            this->mList.append( MsPoint{ 0, mSeparation } );
        //
        //        // TODO ::do some checkings
        //        this->mList.append( MsPoint{ lev, mMeasure } );
        //    }
        //
        //    if ( lev - dialog.ui.spinBox_MaxLevel->value() < mDLevel )
        //    {
        //        if ( mSeparation > 0 )
        //            this->mList.append( MsPoint{ 0, mSeparation } );
        //
        //        // TODO ::do some checkings
        //        this->mList.append( MsPoint{ mLevelMax, mMeasure } );
        //    }
        //}
        //else
        //{
        //    int lev = mLevelMin;
        //    for ( ; lev >= mLevelMax; lev += mDLevel )
        //    {
        //        if ( mSeparation > 0 && lev < mLevelMin )
        //            this->mList.append( MsPoint{ 0, mSeparation } );
        //
        //        // TODO ::do some checkings
        //        this->mList.append( MsPoint{ lev, mMeasure } );
        //    }
        //
        //    if ( abs(lev - dialog.ui.spinBox_MaxLevel->value()) < abs(mDLevel) )
        //    {
        //        if ( mSeparation > 0 )
        //            this->mList.append( MsPoint{ 0, mSeparation } );
        //
        //        // TODO ::do some checkings
        //        this->mList.append( MsPoint{ mLevelMax, mMeasure } );
        //    }
        //}

        this->mList.append( MsPoint{ 0, mMeasure } );
    }
}

void FormAutoMeasurement::onContextMenu( const QPoint& pos )
{
    QPoint off = this->tableView->geometry().topLeft();
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Insert Row", this);
    QAction action2("Delete Row", this);

    connect( &action1, &QAction::triggered, this, &FormAutoMeasurement::insertRow );
    connect( &action2, &QAction::triggered, this, &FormAutoMeasurement::deleteRow );
   
    contextMenu.addAction(&action1);
    contextMenu.addAction(&action2);

    contextMenu.exec( mapToGlobal(pos+off));
}

void FormAutoMeasurement::deleteRow()
{
	//QModelIndexList selection = tableView->selectionModel()->selectedRows();
	QModelIndexList selection = tableView->selectionModel()->selectedIndexes();

    if ( selection.count() > 0 )
        mList.removeRows( selection.at(0).row(), 1);

    tableView->clearSelection();
}

void FormAutoMeasurement::insertRow()
{
	//QModelIndexList selection = tableView->selectionModel()->selectedRows();
	QModelIndexList selection = tableView->selectionModel()->selectedIndexes();

    if ( selection.count() == 0 )
    {
        int pos = mList.rowCount();
        mList.insertRows( pos, 1);
    }
    else if ( selection.count() > 0 )
    {
        mList.insertRows( selection.at(0).row(), 1);
    }

    tableView->clearSelection();
}
