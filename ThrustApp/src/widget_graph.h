#ifndef __WIDGET_GRAPH_H__
#define __WIDGET_GRAPH_H__

#include <limits>

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QToolBar>
#include <QRectF>

#include "document.h"


enum class EMouseMode
{
	None = 0,
	ZoomWindow,
	Translate,
    Select
};


class ChannelRange;
class DlgGraphProbe;
class DlgGraphStyle;


////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphChannel
////////////////////////////////////////////////////////////////////////////////////////////////////
class GraphChannel
{
public:
    explicit GraphChannel( size_t id = 0 );

    //void    setLinePen( const QPen& pen);
    //void    setSymbolPen( const QPen& pen);
    //void    setSymbolBrush( const QBrush& brush);

    //void    reset();

    size_t      getId() const                   { return mId; }
    void        setId( size_t id)               { mId = id; }

    bool        isEnabled() const               { return mbEnabled; }
    void        setEnabled( bool b)             { mbEnabled = b; }

    const QPen& getPen() const                  { return mPen; }
    void        setPen( const QPen& pen)        { mPen = pen; }

    void                updaTeTransform( const ChannelRange& chinfo);
    const QTransform&   getTransform() const    { return mmtx; }

    void    saveSettings();
    void    loadSettings();


private:
    size_t      mId;
    bool        mbEnabled;

    QPen        mPen;
    QTransform  mmtx;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphData
////////////////////////////////////////////////////////////////////////////////////////////////////
class GraphData
{
public:
    GraphData() : mpData(nullptr)       {}

    void    initData( const MeasurementData* ptr); 
    void    reinitData( const MeasurementData* ptr);

    void    updateTransform();
    void    paint( QPainter* painter, QTransform& mtx );

    QRectF  channelRectRaw( size_t ich );
    QRectF  channelRect( size_t ich );
    QRectF  dataRect();

    const MeasurementData*  getData() const                     { return mpData; }
    const DataChannel*      getDataChannel( size_t i) const     { return &mpData->getChannel(i); }

	size_t	sizeChannel() const					        
    { 
        assert( mtabChannel.size() == mpData->sizeChannel() );
        return  mpData->sizeChannel(); 
    }

    const GraphChannel& cChannel( size_t ich) const     {return mtabChannel[ich]; }
    GraphChannel&       rChannel( size_t ich)           {return mtabChannel[ich]; }

    void    saveSettings();
    void    loadSettings();


private:
    const MeasurementData*      mpData;
    std::vector<GraphChannel>   mtabChannel;

};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphWidget
////////////////////////////////////////////////////////////////////////////////////////////////////
class GraphWidget : public QWidget
{
	Q_OBJECT

    friend class GraphView;

public:
    explicit GraphWidget( const Document* ptr, QWidget *parent = nullptr );

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);

    void resetData();
    void zoomAll();
    void setStyle();

    void showProbe( bool b);
    void showStyle( bool b);

    void applyStyle();

    void updateFull()
    {
        bpaintFull = true;
        this->update();
    }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent( QMouseEvent *event) override;
	void mouseReleaseEvent ( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event) override;
	void wheelEvent( QWheelEvent *event) override;

public:
    void updateData( const MeasurementData* pdoc, int count);

    void reinitData( MeasurementData* pdat);


private:
    QPen pen;
    QBrush brush;
    bool antialiased;
    bool transformed;

    QPixmap pixmap;
    bool    bpaintFull;

    bool                mbProbe;
    QPointF             mpntProbe;
    double              mvalProbe;
    std::vector<double> mtabvalProbe;
    DlgGraphProbe*      mpDlgProbe; 

    bool                mbStyle;
    DlgGraphStyle*      mpDlgStyle;

    QTransform          mmtxView;
    EMouseMode          mmouseMode;
    QRect               mrecZoomWin;
    QPoint              mpntPanStart;
    QRect               mrecSelection;

    GraphData           mData;
};


#endif // __WIDGET_GRAPH_H__


