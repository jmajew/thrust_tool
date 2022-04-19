#include <fstream>
#include <iomanip>

#include <QTime>
#include <QDebug>

#include "thread_serial.h"


const uint8_t CMD_START_DATASTREAM  = THSP_DATASTREAM_START;
const uint8_t CMD_STOP_DATASTREAM   = THSP_DATASTREAM_STOP;

//
//signals:
//	void QSerialPort::errorOccurred(QSerialPort::SerialPortError error)


SerialThread::SerialThread( QObject *parent) 
	:   QThread( parent ), mWaitTimeout( 0 ), mbQuit( false ), 
        mSerialMode(ESerialMode::TRANSMIT)
{
}

SerialThread::~SerialThread()
{
    if (isRunning())
    {
        mMutex.lock();
        mbQuit = true;
        mWakeCond.wakeOne();
        mMutex.unlock();

        wait();
    }
}


void    SerialThread::request( ESerialRequest req, const ThspMessage& msg)
{
    mMutex.lock();
    mQue.enqueue( ThspRequest( req, msg) );
    mMutex.unlock();

    if ( !isRunning() )
        start();
    else
        mWakeCond.wakeOne();
}


void    SerialThread::startConnection( const QString& portName, const int& portRate, const int& waitTimeout)
 {
    mMutex.lock();

    this->mPortName = portName;
	this->mPortRate = portRate;
	this->mWaitTimeout = waitTimeout;

    mMutex.unlock();

    this->request( ESerialRequest::OPEN, ThspMessage() );
}

void    SerialThread::closeConnection()
{
    this->request( ESerialRequest::CLOSE, ThspMessage() );
}

void    SerialThread::stopThread()
{
    //const QMutexLocker locker(&mMutex);

    if ( isRunning() )
    {
        mMutex.lock();
        mbQuit = true;
        mWakeCond.wakeOne();
        mMutex.unlock();

        //wait();
    }

}

void    SerialThread::getReply( QQueue<ThspMessage>& quemsg)
{
    mreplyMux.lock();
    quemsg = mQueReply;
    mQueReply.clear();
    mreplyMux.unlock();
}

//void    SerialThread::clearReply()
//{
//    mreplyMux.lock();
//    mtabReply.clear();
//    mreplyMux.unlock();
//}

void    SerialThread::terminalStopped()
{
    mMutex_Term.lock();
    mWakeCond_Term.wakeOne();
    mMutex_Term.unlock();
}


void dumLog(std::ofstream& flog, const std::string& name, const QByteArray& data)
{
    flog << name <<"\n";
    for ( int i=0; i<data.size(); ++i)
        flog << data[i];
    flog << "\n";

    for ( int i=0; i<data.size(); ++i)
        flog << (uint16_t)data[i] << ":";
    flog << "\n" << std::endl;
}

void	SerialThread::run()
{
    std::ofstream flog("_serial.log", std::ios_base::app);
	if ( ! flog)
	{
		qDebug() <<"can not open serial log";
		//QMessageBox::warning( nullptr,	("Warning"), ("LatexGenerator: Can't open the output file:") + QString(mOutName.c_str()) );
		//return;
	}

    QSerialPort mSerial;

    QByteArray dataLeft;

    int currentWaitTimeout;

    //if (currentPortName.isEmpty()) 
    //{
    //    emit error(tr("No port name specified"));
    //    return;
    //}


    while (! mbQuit) 
    {
        ThspRequest req;

        if ( ! mQue.empty() )
        {
            mMutex.lock();
            req = mQue.dequeue();
            currentWaitTimeout = mWaitTimeout;
            mMutex.unlock();
        }
        else if ( mSerialMode == ESerialMode::DATA_STREAM )
        {
            mMutex.lock();
            req.mReqType = ESerialRequest::RECEIVE;
            currentWaitTimeout = mWaitTimeout;
            mMutex.unlock();
        }
        else
        {
            mMutex.lock();
            mWakeCond.wait(&mMutex);
            mMutex.unlock();    // <-- ??????
            continue;
        }


        if ( ! mSerial.isOpen() ) // serial is CLOSED
        {
//--------------------------------------------------------------------------------------------------
// OPEN
            if ( req.mReqType == ESerialRequest::OPEN )
            {
                mMutex.lock();
                mSerial.setPortName( mPortName);
                mSerial.setBaudRate( mPortRate);
    
                mSerial.setDataBits(QSerialPort::Data8);
                mSerial.setParity(QSerialPort::NoParity);
                mSerial.setStopBits(QSerialPort::OneStop);
                mMutex.unlock();

                if ( mSerial.open(QIODevice::ReadWrite) ) 
                {
                    //emty serial buffer
                    QByteArray responseData = mSerial.readAll();
                    while ( mSerial.waitForReadyRead(6) )
                    {
                        QByteArray str = mSerial.readAll();
                        responseData += str;
                    }

                    // send signal serial_ready
                    emit connected( true);
                }
                else
                {
                    emit connected( false);
                    emit error(tr("Can't open %1, error code %2")
                               .arg(mPortName).arg(mSerial.error()));

                }
            }
        }
        else // serial is OPEN
        {
//--------------------------------------------------------------------------------------------------
// CLOSE
            if ( req.mReqType == ESerialRequest::CLOSE )
            {
                mHandler.Reset();
                QByteArray responseData = mSerial.readAll();
                mSerial.close();
                
                // send signal serial_stoped
                emit connected( false);
            }
        
//--------------------------------------------------------------------------------------------------
// TRANSMIT
            if ( req.mReqType == ESerialRequest::TRANSMIT )
            {
                mHandler.PackPackage( &req.mMsg);
                    
                QByteArray dataArray( (char*)mHandler.rSBuf().GetPBeg(), (int)mHandler.rSBuf().Length() );

                // TODO :: log ALL outcoming traffic
                //dumLog(flog, "TRANSMIT", dataArray);

                mSerial.write(dataArray);
                if ( mSerial.waitForBytesWritten(currentWaitTimeout) ) 
                {
                    //switch mode to read wait
                    req.mReqType = ESerialRequest::RECEIVE;

                }
                else
                {
                    //emit timeout(tr("Wait write request timeout %1; msg %2")
                    //                .arg(QTime::currentTime().toString())
                    //                .arg(QString::number(req.mMsg.command)) );

                    emit timeout( false, (int)req.mMsg.command );
                }
            }
//--------------------------------------------------------------------------------------------------
// READ
            if ( req.mReqType == ESerialRequest::RECEIVE/* || mSerialMode == ESerialMode::DATA_STREAM*/ )
            {
                if ( mSerial.waitForReadyRead( currentWaitTimeout) || dataLeft.size() > 0 )
                {
                    QByteArray responseData = mSerial.readAll();

                    while ( mSerial.waitForReadyRead(6) )
                    {
                        QByteArray str = mSerial.readAll();
                        responseData += str;
                    }

                    // TODO :: log ALL incoming traffic
                    //dumLog(flog, "RECEIVE", responseData);


                    QQueue<ThspMessage> queReply;
                    bool bterm = false;

                    dataLeft = responseData;
                    do
                    {
                        ThspMessage cur_reply;

                        for ( int i=0; i < dataLeft.size(); ++i)
                        {
                            //QString strHex = "0x" + QString::number( responseData.at(i), 16 ) + " : ";
                            //qDebug() << " -- th_serial : reveived " << strHex;

                            mHandler.ProcessReceivedData( &cur_reply, dataLeft.at(i) );
                            
                            // FIXME :: handle return false
                            if ( mHandler.GetState() == THSP_COMMAND_RECEIVED )
                            {
                                dataLeft.remove( 0, i+1);

                                if ( cur_reply.command == THSP_SHELL_START )
                                {
                                    dataLeft.clear();

                                    mSerial.close();
                                    this->msleep(500);
                                    emit start_terminal();

                                    // sleep
                                    mMutex_Term.lock();
                                    mWakeCond_Term.wait(&mMutex_Term);
                                    mMutex_Term.unlock();    // <-- ??????

                                    // wakeup
                                    mMutex.lock();
                                    mSerial.setPortName( mPortName);
                                    mSerial.setBaudRate( mPortRate);
    
                                    mSerial.setDataBits(QSerialPort::Data8);
                                    mSerial.setParity(QSerialPort::NoParity);
                                    mSerial.setStopBits(QSerialPort::OneStop);
                                    mMutex.unlock();

                                    if ( ! mSerial.open(QIODevice::ReadWrite) ) 
                                    {
                                        emit error(tr("Can't reopen %1, error code %2")
                                                   .arg(mPortName).arg(mSerial.error()));
                                    }

                                    bterm = true;
                                }
                                else
                                {
                                    queReply.enqueue( cur_reply);
                                }

                                break;
                            }
                        }
                    }
                    while ( dataLeft.size() > 0 );

                    qDebug() << "tab_reply.size = " << queReply.size();


                    if ( bterm )
                    {
                        bterm = false;
                    }
                    else if ( queReply.size() > 0 )
                    {
                        mreplyMux.lock();
                        mQueReply.append( queReply);
                        mreplyMux.unlock();

                        emit this->response(true);

                        while ( queReply.size() > 0 )
                        {
                            ThspMessage reply = queReply.dequeue();

                            if ( reply.command == CMD_START_DATASTREAM )
                                mSerialMode = ESerialMode::DATA_STREAM;
                            else if ( reply.command == CMD_STOP_DATASTREAM )
                                mSerialMode = ESerialMode::TRANSMIT;

 
                            //// TERMINAL
                            //if ( req.mMsg.command == THSP_SHELL_START )
                            //{
                            //    mSerial.close();
                            //    this->msleep(500);
                            //    emit start_terminal();
                            //
                            //    // sleep
                            //    mMutex_Term.lock();
                            //    mWakeCond_Term.wait(&mMutex_Term);
                            //    mMutex_Term.unlock();    // <-- ??????
                            //
                            //    // wakeup
                            //    mMutex.lock();
                            //    mSerial.setPortName( mPortName);
                            //    mSerial.setBaudRate( mPortRate);
                            //
                            //    mSerial.setDataBits(QSerialPort::Data8);
                            //    mSerial.setParity(QSerialPort::NoParity);
                            //    mSerial.setStopBits(QSerialPort::OneStop);
                            //    mMutex.unlock();
                            //
                            //    if ( ! mSerial.open(QIODevice::ReadWrite) ) 
                            //    {
                            //        emit error(tr("Can't reopen %1, error code %2")
                            //                   .arg(mPortName).arg(mSerial.error()));
                            //    }
                            //
                            //}
                        }
                    }
                    else
                    {
                        qDebug() << "SerialThread::run() : not a valid message";
                        qDebug() << "mHandler.GetState() = " << mHandler.GetState();

                        emit error( tr("not a valid message") );
                        mHandler.Reset();
                    }

                } 
                else 
                {
                    qDebug() << "Wait read-response timeout " << currentWaitTimeout << " ms : "<< req.mMsg.command;

                    //emit timeout(tr("Wait read response timeout %1; msg %2")
                    //                .arg(QTime::currentTime().toString())
                    //                .arg(QString::number(req.mMsg.command))  );

                    // HACK ::
                    if ( req.mMsg.command == 0 )
                        mSerialMode = ESerialMode::TRANSMIT;
                    else                                          
                        emit timeout( true, (int)req.mMsg.command );
                }
            }
//--------------------------------------------------------------------------------------------------
// END
        
        }

        req.mReqType = ESerialRequest::IDLE;

        //mMutex.unlock();

        //mMutex.lock();
        //mWakeCond.wait(&mMutex);

    }

    mSerial.close();
    quit();
}

