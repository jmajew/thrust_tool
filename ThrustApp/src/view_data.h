#ifndef __VIEW_DATA_H__
#define __VIEW_DATA_H__

#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QTableView>

#include "measure_data.h"

class Document;
//class MeasurementData : public QAbstractTableModel;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class DataView
////////////////////////////////////////////////////////////////////////////////////////////////////
class DataView : public QWidget
{
public:
    explicit DataView( Document* ptr, QWidget* parent=nullptr);

    void    hideColumns();
    void    showAllColumns();

 //   void    reinitData();

    void    setModel( MeasurementData* pdat)     { mpTable->setModel(pdat); }


public slots:
	void	onSectionClicked( int index);
    void    onContextMenu( const QPoint& );

public:
    QTableView* mpTable;
};



#endif // __VIEW_DATA_H__
