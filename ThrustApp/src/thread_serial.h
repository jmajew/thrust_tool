#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QQueue>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>
#include <QtSerialPort/QSerialPort>

#include "thsp_packet.hpp"
#include "thsp_handler.hpp"



 enum class ESerialRequest
 {
     UNKNOWN = 0,
     IDLE,
     OPEN,
     CLOSE,
     TRANSMIT,
     RECEIVE,

     START_DATA_STREAM,
     STOP_DATA_STREAM
 };

 enum class ESerialMode
 {
     UNKNOWN = 0,
     TRANSMIT,
     DATA_STREAM,
     TERMINAL
 };

 class ThspRequest
 {
 public:
     ThspRequest( ESerialRequest r = ESerialRequest::UNKNOWN) : mReqType(r)      {}
     ThspRequest( ESerialRequest r, const ThspMessage& msg) 
         : mReqType(r), mMsg(msg)      {}


     ESerialRequest mReqType;
     ThspMessage    mMsg;
 };


class SerialThread : public QThread
{
    Q_OBJECT

public:
    explicit SerialThread( QObject *parent );
    ~SerialThread();

    void	run() Q_DECL_OVERRIDE;

signals:
    void    connected( bool flag);
    void    response( bool b);
    void    error( const QString &s);
    //void  timeout( const QString &s);
    void    timeout( bool btype, int ccode);
    void    start_terminal();


public:
    void    startConnection( const QString& portName, const int& portRate, const int& waitTimeout);
    void    closeConnection();
    void    stopThread();

    void    request( ESerialRequest, const ThspMessage& msg);
    void    getReply( QQueue<ThspMessage>& tabmsg);
    //void    clearReply();

    void    terminalStopped();

private:

    QMutex			mMutex;
    QWaitCondition	mWakeCond;
    bool 			mbQuit;

    QString			mPortName;
    int				mPortRate;
    int				mWaitTimeout;

    QQueue<ThspRequest> mQue;

    QMutex              mreplyMux;
    QQueue<ThspMessage> mQueReply;

    QMutex          mMutex_Term;
    QWaitCondition	mWakeCond_Term;

private:
    ESerialMode     mSerialMode;

    ThspHandler     mHandler;

    //uint64_t									mIdFrame;
    //std::queue< std::shared_ptr<SerialFrameMsp> >	mInBuffer;
};

#endif // SERIALTHREAD_H
