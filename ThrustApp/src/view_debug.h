#ifndef __VIEW_DEBUG_H__
#define __VIEW_DEBUG_H__

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class DebugView : public QWidget
{
    Q_OBJECT

public:
    DebugView( QWidget *parent );

private:
    //QTextEdit*      txtView;
    //QPushButton*    btnSend;
    //QLineEdit*      edtCommand;
    //QLineEdit*      edtData;
    //QLabel*         labCommand;
    //QLabel*         labData;

};

#endif // __VIEW_DEBUG_H__
