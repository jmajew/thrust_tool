#ifndef __FORM_AUTOMEASUREMENT__
#define __FORM_AUTOMEASUREMENT__

#include <array>

#include <QAbstractTableModel>

#include "ui_form_automeasurement.h"




enum EMsPoint
{
    EMsPoint_Level = 0,
    EMsPoint_Measure = 1,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class AutoMeasureList
////////////////////////////////////////////////////////////////////////////////////////////////////
class AutoMeasureList : public QAbstractTableModel
{
    Q_OBJECT

public:
    using MsPoint = std::array<int,2>;

    //------------------------------------------------------------------------------------------------//
    // QAbstractTableModel interface

	int			rowCount(const QModelIndex &parent = QModelIndex()) const override			
	{ 
		//if ( ! mbIsReady )
		//	return 0;
		//else
			return (int)mtabPnts.size(); 
	}

	int			columnCount(const QModelIndex &parent = QModelIndex()) const override		
	{ 
		return std::tuple_size<MsPoint>::value; 
	}

    QVariant	data(const QModelIndex &index, int role = Qt::DisplayRole) const override
	{
		if ( index.column() >= std::tuple_size<MsPoint>::value )
			return QVariant();

		if ( index.row() >= mtabPnts.size() )
			return QVariant();

		//if (role == Qt::DisplayRole)
        if (role == Qt::DisplayRole || role == Qt::EditRole)
			return mtabPnts[index.row()][index.column()];

		return QVariant();
	}

	QVariant	    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags   flags(const QModelIndex &index) const;
    bool            setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool            insertRows(int position, int rows, const QModelIndex &index = QModelIndex());  
    bool            removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

    //------------------------------------------------------------------------------------------------//

public:
    void    append( const MsPoint& data)
    {
        mtabPnts.emplace_back( data);
		emit this->layoutChanged();
    }

    size_t  size() const        { return mtabPnts.size(); }
    void    clear()             { return mtabPnts.clear(); }

    const MsPoint&  operator[](size_t i) const  { return mtabPnts[i]; }
    MsPoint&        operator[](size_t i)        { return mtabPnts[i]; }

    //const MsPoint&  operator[](EMsPoint e) const  { return mtabPnts[(int)e]; }
    //MsPoint&        operator[](EMsPoint e)        { return mtabPnts[(int)e]; }

    int     getPntBegining(int i) const
    {
        if ( i >= mtabPnts.size())
        {
            //ASSERT(0);
            return -1;
        }
        else
        {
            int n = 0;
            for ( int k=0; k<i; ++k)
                n += mtabPnts[k][EMsPoint_Measure];
            return n;
        }
    }

private:
    std::vector<MsPoint>    mtabPnts;
};



////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormAutoMeasurement
////////////////////////////////////////////////////////////////////////////////////////////////////
class FormAutoMeasurement : public QWidget, public Ui::FormAutoMeasurement
{
    Q_OBJECT

public:
    explicit FormAutoMeasurement(QWidget *parent = nullptr);

    const AutoMeasureList&   cList() const      { return mList; }

public slots:
    //void    onAddPoint();
    void    onAutoCreate();
    void    deleteRow();
    void    insertRow();
    void    onContextMenu( const QPoint& );

private:
    AutoMeasureList mList;

    // dlg data
    int mLevelMin;
    int mLevelMax;
    int mDLevel;
    int mMeasure;
    int mSeparation;

};



#endif // __FORM_AUTOMEASUREMENT__
