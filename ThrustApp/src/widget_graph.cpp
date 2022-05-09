#include <QDebug>
#include <QLayout>
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QWheelEvent>
#include <QSettings>

#include "widget_graph.h"
#include "document.h"
#include "dlg_graphstyle.h"
#include "dlg_graphprobe.h"


const qreal PLOT_MARGX  = 0.;
const qreal PLOT_MARGY  = 0.;
const qreal PLOT_HEIGHT = 400. + 2.*PLOT_MARGX;
const qreal PLOT_WIDTH  = 800. + 2.*PLOT_MARGY;


////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphChannel
////////////////////////////////////////////////////////////////////////////////////////////////////

GraphChannel::GraphChannel(size_t id) 
    : mId(id), mbEnabled(true)
{
    mPen = QPen( Qt::red, 2.0, Qt::SolidLine );
    mmtx.reset();
}

//void GraphChannel::reset()
//{
//    mboundRect = QRectF(0., 0., 10., 10.);
//    mtabPnts.clear();
//}

void GraphChannel::updaTeTransform( const ChannelRange& chinfo)
{
    // scale to (0,0,PLOT_WIDTH,PLOT_HEIGHT) and flip by X-axis
    qreal range = (qreal)(chinfo.cMax() - chinfo.cMin());
    qreal sx = PLOT_WIDTH / 100.;
    qreal sy = PLOT_HEIGHT / range;
    qreal tx = 0;
    qreal ty = PLOT_HEIGHT + sy * (qreal)chinfo.cMin();

    mmtx = QTransform( sx, 0, 0, -sy, tx, ty);

    //// check
    //size_t id = mId;
    //QPointF p0( 0., (qreal)chinfo.cMin() );
    //QPointF p1( 100., (qreal)chinfo.cMax() );
    //
    //QPointF pp0 = mmtx.map(p0);
    //QPointF pp1 = mmtx.map(p1);
    //
    //qDebug() << mId << "GraphChennel data_rec" << p0 << p1 << " maps to " << mmtx.map(p0) << mmtx.map(p1);
}


void GraphChannel::saveSettings()
{
	QSettings settings;
    QString key_root = "GraphChannel/" + QString::number(mId) + "/";

	settings.setValue( key_root + "Enabled", mbEnabled );
    QRgb c = mPen.color().rgb();
	settings.setValue( key_root + "Color", mPen.color().rgb() );
	settings.setValue( key_root + "Width", mPen.widthF() );
	settings.setValue( key_root + "Style", (int)mPen.style() );
}

void GraphChannel::loadSettings()
{
	QSettings settings;
    QString key_root = "GraphChannel/" + QString::number(mId) + "/";

    mbEnabled = settings.value(key_root + "Enabled").toBool();

    QRgb color = settings.value(key_root + "Color").toUInt();
   // mPen.color().setRgb( color);

    qreal width = settings.value(key_root + "Width").toReal();
    //mPen.setWidthF( width);

    qreal style = settings.value(key_root + "Style").toInt();
    //mPen.setStyle( (Qt::PenStyle)style );

    mPen = QPen( QColor(color), width, (Qt::PenStyle)style);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphData
////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphData::initData( const MeasurementData* ptr)
{
    if ( ptr == nullptr )
        return;

    mpData = ptr;

    mtabChannel.clear();
    mtabChannel.resize( getData()->sizeChannel() );
    for ( size_t i=0; i<mtabChannel.size(); ++i )
    {
        mtabChannel[i].setId( getData()->getChannel(i).getId() );
        if ( getData()->getChannel(i).getType() == EDataType::Cooked )
        {
            mtabChannel[i].setEnabled( false);
            mtabChannel[i].setPen( QPen( Qt::blue, 2.0, Qt::SolidLine ) );
        }

        //mtabChannel[i].rName() = getData()->getChannel(i).getName();
    }

//    updateTransform();
}

void GraphData::reinitData( const MeasurementData* ptr )
{
    initData( ptr);

    updateTransform();
}


void GraphData::updateTransform()
{
    for ( auto& ch : mtabChannel )
        ch.updaTeTransform( getData()->getDataRange( ch.getId() ) );
}

void GraphData::paint( QPainter* painter, QTransform& mtx_view )
{
    for ( auto& ch : mtabChannel )
    {
        if ( ch.isEnabled() )
        {
            QTransform mtx = ch.getTransform() * mtx_view;
            painter->setPen( ch.getPen() );

            //QPointF p0(0., 0.), p1(1., 0.);
            //QPointF dp = mtx.map(p1) - mtx.map(p0);

            //qreal xdi = 1.;
            //if ( dp.x() != 0)
            //    xdi = 1./dp.x();
            //int di = std::max( (int)xdi, 1);


            //for ( size_t i=1; i<getData()->size(); i+=di )
            //{
            //    QPointF pnt0( (double)(i-1), getData()->getData( ch.getId(), i-1) );
            //    QPointF pnt1( (double)(i), getData()->getData( ch.getId(), i) );

            //    painter->drawLine( mtx.map(pnt0), mtx.map(pnt1) );
            //}

            if ( getData()->size() > 0 )
            {
                QPointF pnt0 = mtx.map( QPointF( (double)(0), getData()->getData( ch.getId(), 0) ) );
                for ( size_t i=1; i<getData()->size(); ++i )
                {
                    QPointF pnt1 = mtx.map( QPointF( (double)(i), getData()->getData( ch.getId(), i) ) );

                    QPointF dp = pnt1 - pnt0;
                    if ( dp.x()*dp.x() + dp.y()*dp.y() > 1. && ( pnt0.x() > 0. || pnt1.x() > 0. ) )
                    {
                        painter->drawLine( pnt0, pnt1 );
                        pnt0 = pnt1;
                    }
                }
            }

            //for ( size_t i=0; i<getData()->size(); ++i )
            //{
            //    QPointF pnt( (double)(i), getData()->getData( ch.getId(), i) );
            //    painter->drawPoint( mtx.map(pnt) );
            //}

            //if ( getData()->size() > 0 )
            //{
            //    QPainterPath path;
            //    QPointF pnt0( (double)(0), getData()->getData( ch.getId(), 0) );
            //    path.moveTo( mtx.map(pnt0));
            //    for ( size_t i=1; i<getData()->size(); ++i )
            //    {
            //        QPointF pnt( (double)(i), getData()->getData( ch.getId(), i) );
            //        path.lineTo( mtx.map(pnt));
            //    }
            //    painter->drawPath(path);
            //}
        }
    }
}

QRectF GraphData::channelRectRaw( size_t ich )
{
    double h = (double)(getData()->getDataRange(ich).cMax() - getData()->getDataRange(ich).cMin());
    double w = (double)(getData()->size() - 1);
    QRectF rec( 0, (double)getData()->getDataRange(ich).cMin(), w, h);

    return rec;
}

QRectF GraphData::channelRect( size_t ich )
{
    QRectF rec = this->channelRectRaw(ich);
    return mtabChannel[ich].getTransform().mapRect( rec );
}

QRectF GraphData::dataRect()
{
    if ( mpData == nullptr)
        return QRectF(0,0,10,10);

    if ( getData()->size() == 0 )
        return QRectF(0,0,10,10);

    QRectF rec = this->channelRect(0);    //mtabChannel[0].boundingRect();
    for ( size_t i=0; i<getData()->sizeChannel(); ++i )
        rec |= this->channelRect(i);

    return rec;
}

void GraphData::saveSettings()
{
    for ( auto& ch : mtabChannel )
    {
        ch.saveSettings();
    }
}

void GraphData::loadSettings()
{
    for ( auto& ch : mtabChannel )
    {
        ch.loadSettings();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphWidget
////////////////////////////////////////////////////////////////////////////////////////////////////

GraphWidget::GraphWidget( const Document* ptr, QWidget *parent) : QWidget(parent)
{
    assert( ptr!=nullptr );

    bpaintFull = true;

    //mpDoc = ptr;
    mData.initData( &ptr->cData() );

    QSize wsize = this->size();

    mmouseMode = EMouseMode::None;
    mmtxView = QTransform();


    antialiased = false;
    transformed = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    this->setPen( QPen(Qt::blue, 3) );

    //if (style == Qt::LinearGradientPattern)
        
        //QLinearGradient linearGradient(0, 0, 100, 100);
        //linearGradient.setColorAt(0.0, Qt::white);
        //linearGradient.setColorAt(0.2, Qt::green);
        //linearGradient.setColorAt(1.0, Qt::black);
        //renderArea->setBrush(linearGradient);

    this->setBrush( QBrush(Qt::green) );

    setCursor(Qt::CrossCursor);

    setMouseTracking(true);

    mbProbe = false;
    mpDlgProbe = new DlgGraphProbe( &mData, this);
    mpDlgProbe->hide();

    mpDlgStyle = new DlgGraphStyle( &mData, this);
    mpDlgStyle->hide();

    mtabvalProbe.resize( mData.sizeChannel(), 0);

}

QSize GraphWidget::minimumSizeHint() const
{
    return QSize(100, 80);
}

QSize GraphWidget::sizeHint() const
{
    return QSize(800, 400);
}

void GraphWidget::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void GraphWidget::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void GraphWidget::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    updateFull();
}

void GraphWidget::zoomAll()
{
    QSizeF szw = this->size();
    QRectF recd = mData.dataRect();

    //mmtxView.reset();
    //mmtxView.scale( szw.width() / recd.width(), szw.height() / recd.height() );

    mmtxView.reset();
    mmtxView.translate( PLOT_MARGX, PLOT_MARGY );
    mmtxView.scale( (szw.width() - 2.*PLOT_MARGX) / recd.width(), 
                    (szw.height() -2.*PLOT_MARGY) / recd.height() );
    this->updateFull();
}

void GraphWidget::setStyle()
{
    DlgGraphStyle dialog( &this->mData, this);
    //dialog.setWordCount(document().wordCount());
    int res = dialog.exec();
    if ( res == QDialog::Accepted )
    {
        dialog.updateData();
    }
}

void GraphWidget::showProbe( bool b)
{
    if ( b )
    {
        mbProbe = true;
        mpDlgProbe->show();
    }
    else
    {
        mbProbe = false;
        mpDlgProbe->hide();
        this->update();
    }
}

void GraphWidget::showStyle( bool b)
{
    if ( b )
    {
        mbStyle = true;
        mpDlgStyle->show();
    }
    else
    {
        mbStyle = false;
        mpDlgStyle->hide();
        this->updateFull();
    }
}

void GraphWidget::applyStyle()
{
}


void GraphWidget::resetData()
{
    //this->clearChannels();
    //this->createChannels(DATA_SIZE);
    this->updateFull();
}

void GraphWidget::paintEvent( QPaintEvent * event )
{
    QSize wsz = this->size();
    QRectF drec = mData.dataRect();

    QPainter painter(this);

    if ( bpaintFull )
    {
        pixmap = QPixmap( wsz);
        pixmap.fill( Qt::yellow);

        QPainter pixpainter( &pixmap);

        pixpainter.setPen(QPen(Qt::black, 0));
        pixpainter.drawRect( QRectF( 0., 0., wsz.width()-1, wsz.height()-1 ) );

        // grid
        pixpainter.setFont( QFont("Helvetica [Cronyx]", 10) );
        pixpainter.setPen(QPen(Qt::black, 0));
    
        int ich = 0;

        const GraphChannel& ch = mData.cChannel(ich);
        QTransform mtx_ch = ch.getTransform();
        QTransform mtx = mmtxView*mtx_ch;
        QTransform mtxI = mtx.inverted();

        QRectF rec_vd = mtxI.mapRect( QRectF(QPointF(0,0), wsz ) );

        QRectF drrec = mData.channelRectRaw( ich);

        if ( mData.getData()->size() > 0 )
        {
            QRectF wrec = mmtxView.mapRect( drec);
            pixpainter.drawRect( wrec );

            pixpainter.setPen(QPen(Qt::black, 0, Qt::DashLine));

            int idist = 10;
            while ( true )
            {
                double h = mtx.map(QPointF(idist,0)).x() - mtx.map(QPointF(0,0)).x();
                if ( h > 30 )
                    break;
                else
                    idist *= 2;
            }

            int n = drrec.width() / idist;
            for ( int i=1; i<=n; ++i)
            {
                QPointF px(idist*i,0);
                double x = mtx_ch.map(px).x();
                QPointF p0( x, drec.top() ); 
                QPointF p1( x, drec.bottom() ); 

                pixpainter.drawLine( mmtxView.map(p0), mmtxView.map(p1) );
                //pixpainter.drawText( QPointF(mmtxView.map(p1).x(), drec.bottom()) + QPointF(1,-1), QString::number(i*idist/10) );
                pixpainter.drawText( QPointF(mmtxView.map(p1).x(), wsz.height()) + QPointF(1,-4), QString::number(i*idist/10) );
                //pixpainter.drawText( mmtxView.map(p1) + QPointF(1,-1), QString::number(i*idist/10) );
            }
        }


        //if (antialiased)
        pixpainter.setRenderHint(QPainter::Antialiasing, true);


        // draw channels
        mData.paint( &pixpainter, mmtxView );

        bpaintFull = false;

    }

    painter.drawPixmap( 0, 0, pixmap);

    //painter.setPen(QPen(Qt::black, 0));
    //painter.drawRect( QRectF( 0., 0., wsz.width()-1, wsz.height()-1 ) );

    //// grid
    //painter.setFont( QFont("Helvetica [Cronyx]", 10) );
    //painter.setPen(QPen(Qt::black, 0));
    //
    //int ich = 0;

    //const GraphChannel& ch = mData.cChannel(ich);
    //QTransform mtx_ch = ch.getTransform();
    //QTransform mtx = mmtxView*mtx_ch;
    //QTransform mtxI = mtx.inverted();

    //QRectF rec_vd = mtxI.mapRect( QRectF(QPointF(0,0), wsz ) );

    //QRectF drrec = mData.channelRectRaw( ich);

    //if ( mData.getData()->size() > 0 )
    //{
    //    QRectF wrec = mmtxView.mapRect( drec);
    //    painter.drawRect( wrec );

    //    painter.setPen(QPen(Qt::black, 0, Qt::DashLine));

    //    int idist = 10;
    //    while ( true )
    //    {
    //        double h = mtx.map(QPointF(idist,0)).x() - mtx.map(QPointF(0,0)).x();
    //        if ( h > 30 )
    //            break;
    //        else
    //            idist *= 2;
    //    }

    //    int n = drrec.width() / idist;
    //    for ( int i=1; i<=n; ++i)
    //    {
    //        QPointF px(idist*i,0);
    //        double x = mtx_ch.map(px).x();
    //        QPointF p0( x, drec.top() ); 
    //        QPointF p1( x, drec.bottom() ); 

    //        painter.drawLine( mmtxView.map(p0), mmtxView.map(p1) );
    //        painter.drawText( QPointF(mmtxView.map(p1).x(), drec.bottom()) + QPointF(1,-1), QString::number(i*idist/10) );
    //    }
    //}


    ////if (antialiased)
    //painter.setRenderHint(QPainter::Antialiasing, true);


    //// draw channels
    //mData.paint( &painter, mmtxView );

    // zoom window
    if ( mmouseMode == EMouseMode::ZoomWindow )
    {
        painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        painter.setPen(QColor(0xff, 0xff, 0xff));
        //painter.setBrush( Qt::BrushStyle::NoBrush );
        painter.setBrush( QColor(0,0,0xff,170) );

        painter.drawRect( mrecZoomWin );
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
    else if ( mmouseMode == EMouseMode::Select )
    {
        painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        painter.setPen(QColor(0xff, 0xff, 0xff));
        //painter.setBrush( Qt::BrushStyle::NoBrush );
        painter.setBrush( QColor(0,0x80,0xff,170) );

        QRect selSize = this->contentsRect();
        selSize.setLeft( mrecSelection.left() );
        selSize.setRight( mrecSelection.right() );

        painter.drawRect( selSize );
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }


    // probe
    if ( mbProbe )
    {
        int ichp = mpDlgProbe->getChannelId();
        if ( ichp >= 0 && ichp < mData.sizeChannel() )
        {
            QTransform mtxp = mData.cChannel(ichp).getTransform()*mmtxView;

            QPointF pnt = mtxp.map( mpntProbe );
            QPointF pntVal = mtxp.map( QPointF( mpntProbe.x(), mvalProbe ) );

            painter.setPen( QPen( Qt::black, 0) );
            painter.drawLine( 0, pnt.y(), wsz.width()-1, pnt.y() );
            painter.drawLine( pnt.x(), 0, pnt.x(), wsz.height()-1 );

            painter.setPen( QPen( Qt::blue, 0) );
            painter.drawLine( 0, pntVal.y(), wsz.width()-1, pntVal.y() );
        }
    }
}

void GraphWidget::resizeEvent(QResizeEvent *event)
{
    QSizeF szm( 2.*PLOT_MARGX, 2.*PLOT_MARGY);
    QSizeF szo = event->oldSize();
    QSizeF szn = event->size();

    bpaintFull = true;

    if ( szo.width() > 0 && szn.height() > 0 )
    {
        QTransform mtx;
        mtx.scale( (qreal)szn.width() / (qreal)szo.width(), (qreal)szn.height() / (qreal)szo.height() );

        //mmtxView.translate( -PLOT_MARGX, -PLOT_MARGY );
        mmtxView *= mtx;
        //mmtxView.translate( PLOT_MARGX, PLOT_MARGY );
    }
    else
    {
        mmtxView.translate( PLOT_MARGX, PLOT_MARGY );
        mmtxView.scale( (qreal)szn.width() / PLOT_WIDTH, (qreal)szn.height() / PLOT_HEIGHT );
    }

    //qreal dx = mmtxView.m31();
    //qreal dy = mmtxView.m32();
}

void GraphWidget::mousePressEvent( QMouseEvent *event)
{
    //this->setCursor( Qt::OpenHandCursor);

	if ( event->buttons() & Qt::LeftButton && event->modifiers() & Qt::ControlModifier )
	{
        mmouseMode = EMouseMode::ZoomWindow;

        mrecZoomWin.setTopLeft( event->pos() );
        mrecZoomWin.setBottomRight( event->pos() );
    }
    else if ( event->buttons() & Qt::RightButton && event->modifiers() == Qt::NoModifier )
    {
        mmouseMode = EMouseMode::Translate;
		this->setCursor( Qt::OpenHandCursor);

        mpntPanStart = event->pos();
    }
    else if ( event->buttons() & Qt::RightButton && event->modifiers() == Qt::ControlModifier )
    {
        mmouseMode = EMouseMode::Select;
        
        mrecSelection.setTopLeft( event->pos() );
        mrecSelection.setBottomRight( event->pos() );
        
		//this->setCursor( Qt::OpenHandCursor);

        mpntPanStart = event->pos();
    }
}

void GraphWidget::mouseReleaseEvent( QMouseEvent *event)
{
	if ( mmouseMode == EMouseMode::ZoomWindow )
    {
        mrecZoomWin.setBottomRight( event->pos() );
        mrecZoomWin = mrecZoomWin.normalized();

        QSizeF winsz = this->size();
       
        QTransform mtxz;
        mtxz.scale( winsz.width() / mrecZoomWin.width(), winsz.height() / mrecZoomWin.height() );
        mtxz.translate( -mrecZoomWin.topLeft().x(), -mrecZoomWin.topLeft().y() );

        mmtxView *= mtxz;
  
        mmouseMode = EMouseMode::None;
        this->updateFull();
    }
    else if ( mmouseMode == EMouseMode::Translate )
    {
        this->setCursor(Qt::CrossCursor);

        mmouseMode = EMouseMode::None;
        //this->updateFull();
    }
    else if ( mmouseMode == EMouseMode::Select )
    {
        mrecSelection.setBottomRight( event->pos() );
        mrecSelection = mrecSelection.normalized();

        QSizeF winsz = this->size();
        
        mmouseMode = EMouseMode::None;
        this->updateFull();
    }

}

void GraphWidget::mouseMoveEvent( QMouseEvent *event)
{
	if ( mmouseMode == EMouseMode::ZoomWindow )
    {
        this->update();
        mrecZoomWin.setBottomRight( event->pos() );
        this->update();
    }
    else if ( mmouseMode == EMouseMode::Translate )
    {
        QPointF delta = event->pos() - mpntPanStart ;

        QTransform mtxt;
        mtxt.translate( delta.x(), delta.y() );
        mmtxView *= mtxt;
        mpntPanStart = event->pos();

        this->updateFull();
    }
	else if ( mmouseMode == EMouseMode::Select )
    {
        this->update();
        mrecSelection.setBottomRight( event->pos() );
        this->update();
    }


    if ( mbProbe )
    {
        int ich = mpDlgProbe->getChannelId();
        if ( ich >= 0 && ich < mData.sizeChannel() )
        {
            QTransform mtx_ch = mData.cChannel(ich).getTransform();
            QTransform mtxI = (mtx_ch*mmtxView).inverted();

            QPointF pnt_scr = event->pos();
            mpntProbe = mtxI.map( pnt_scr );


            int ind = (int)(mpntProbe.x() + 0.5);
            if ( ind >= 0 && ind < mData.getData()->size() )
                mvalProbe = mData.getData()->getData( ich, ind );
            else
                mvalProbe = 0.;

            mpDlgProbe->setValues( mpntProbe.x(), mpntProbe.y(), mvalProbe );
        }

        for (int ch = 0; ch < mData.sizeChannel(); ++ch)
        {
            int ind = (int)(mpntProbe.x() + 0.5);
            
            double val = 0.0;
            if ( ind >= 0 && ind < mData.getData()->size() )
                val = mData.getData()->getData( ch, ind );

            mtabvalProbe[ch] = val;
        }

        mpDlgProbe->setValues( mpntProbe.x(), mpntProbe.y(), mtabvalProbe );

        this->update();
    }
}

void GraphWidget::wheelEvent( QWheelEvent *event)
{
    if ( event->modifiers() & Qt::ControlModifier) 
    {
        QPoint pnt0 = event->position().toPoint();

		qreal d = (qreal)event->angleDelta().y() / 2000.0;
        qreal scale = 1. + d;

        QTransform mtxe;
        mtxe.translate( pnt0.x(), pnt0.y());
        mtxe.scale(scale, scale); 
        mtxe.translate( -pnt0.x(), -pnt0.y());

        mmtxView *= mtxe;

        this->updateFull();

        //event->accept();
    } 
}


//void GraphWidget::clearChannels()
//{       
//    //mtabChannel.clear();  // FIXME ::
//}
//
//void GraphWidget::createChannels( size_t n, const Document& dat)
//{
//    //assert( mtabChannel.size() == 0 );
//
//    mData.initData( &dat);
//
//    //mtabChannel.resize( n);
//    //for ( int ich=0; ich<mtabChannel.size(); ++ich )
//    //{
//    //    mtabChannel[ich].setId( ich);
//    //    mtabChannel[ich].setChInfo( dat.GetDataInfo(ich));
//    //}
//
//    ////for ( int ich=0; ich<mtabChannel.size(); ++ich )
//    ////{
//    ////    mtabChannel[ich] = new ChannelItem( ich );
//
//    ////    this->addItem( mtabChannel[ich] );
//    ////}
//}

void GraphWidget::updateData( const MeasurementData* pdat, int count)
{
    if ( pdat == nullptr )
        return; 

    const QRectF  paintArea( 0., 0., PLOT_WIDTH, PLOT_HEIGHT );

    const double w = paintArea.width();
    const double h = paintArea.height();

    if ( pdat != mData.getData() )
    {
        qDebug() << "GraphWidget::updateData :: pdat != mData.getData()";
        // FIXME :: ERROR
        // 
        //mData.initData( pdat->getParentDoc() );
    }

    //if ( mData.getData() != &pdoc->cData() )   // compare id
    //{
    //    //clearChannels();
    //    mData.initData( pdoc);
    //}

    mData.updateTransform();

    QSize sz = this->size();
    QRectF drec = mData.dataRect();
    QRectF vrec = mmtxView.mapRect( drec);

    if ( vrec.right() > sz.width() )
    {
        qreal dx = vrec.right() - sz.width();
        QTransform mt;
        mt.translate( -dx, 0);
        mmtxView *= mt;
    }

    this->updateFull();
}



void GraphWidget::reinitData( MeasurementData* pdat)
{
    const QRectF  paintArea( 0., 0., PLOT_WIDTH, PLOT_HEIGHT );

    const double w = paintArea.width();
    const double h = paintArea.height();

    mData.reinitData( pdat );
    mData.loadSettings();
    
    mpDlgProbe->initData( &mData);

    QSize sz = this->size();
    QRectF drec = mData.dataRect();
    QRectF vrec = mmtxView.mapRect( drec);

    if ( vrec.right() > sz.width() )
    {
        qreal dx = vrec.right() - sz.width();
        QTransform mt;
        mt.translate( -dx, 0);
        mmtxView *= mt;
    }

    this->updateFull();
}
