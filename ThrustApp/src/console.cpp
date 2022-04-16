/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>
#include <QScrollBar>

#include "console.h"
#include "view_terminal.h"


const char	sseqBspc[] = "\b \b";
const char	sseqEscSeq1[] = "\x1b[70D";		// CSI D Cursor Backward Ps Times
const char	sseqEscSeq2[] = "\x1b[4C";		// CSI C Cursor Forward Ps Times
const char	sseqEscSeq3[] = "\x1b[K";		// CSI K Erase in Line (from the cursor to the eol)



Console::Console(QWidget *parent) : QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);

	bExit = false;
	bAppend = true;
}

void Console::putData(const QByteArray &data)
{
	bool escape = false;
	bool bracket = false;

	QByteArray buf;

	for ( int i = 0; i < data.size(); ++i) 
	{
		const char&  c = data[i];

		if ( bAppend )	// standard mode		
		{
			if ( data[i] == 8 || data[i] == 27 )
			{
				sseq.clear();
				sseq.append( data[i] );
				bAppend = false;
			}
			else
			{
				// terminal buffer update
				buf.append( data[i]);

				// current command line update (search for exit)
				if ( c == '\r' )
				{
					if ( line == "ch> exit" )
					{
						qDebug() << "Console ::  'exit' command issued !!!";
						bExit = true;
					}

					line.clear();
				}
				else
				{
					if ( c != '\n' && c != '\r' )
						line.push_back( c);
				}

			}
		}
		else	// special sequence mode
		{
			sseq.append( data[i] );

			if ( sseq == sseqBspc )
			{
				QTextCursor tmpCursor = this->textCursor();
				tmpCursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
				tmpCursor.removeSelectedText();
				tmpCursor.clearSelection();
				this->setTextCursor( tmpCursor);

				line.pop_back();

				bAppend = true;
			}
			else if ( sseq == sseqEscSeq1 )		// CSI D
			{
				// ignore
				bAppend = true;
			}
			else if ( sseq == sseqEscSeq2 )		// CSI C
			{
				// ignore
				bAppend = true;
			}
			else if ( sseq == sseqEscSeq3 )		// CSI K
			{
				// delete cur line (leaving prompt)
				QTextCursor tmpCursor = this->textCursor();

				tmpCursor.movePosition( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
				tmpCursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, 4 );

				tmpCursor.movePosition( QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
				tmpCursor.removeSelectedText();
				tmpCursor.clearSelection();
				this->setTextCursor( tmpCursor);

				bAppend = true;
			}

		}


		//		case 'D': // move left
		//			{
		//				QTextCursor tmpCursor = this->textCursor();
		//				tmpCursor.movePosition( QTextCursor::Left, QTextCursor::MoveAnchor, std::min( d, tmpCursor.columnNumber() ));
		//				this->setTextCursor(tmpCursor);
		//				break;
		//			}

		//		case 'C':	// move right
		//			{
		//				QTextCursor tmpCursor = this->textCursor();
		//				tmpCursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, d);
		//				this->setTextCursor(tmpCursor);
		//				break;
		//			}
	}

    insertPlainText( buf);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    m_localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch ( e->key()) 
	{
    //case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
		break;

    case Qt::Key_Up:
        emit getData( "\033[A");
		break;

    case Qt::Key_Down:
        emit getData( "\033[B");
		break;

    default:
        if (m_localEchoEnabled)
            QPlainTextEdit::keyPressEvent( e);

        emit getData( e->text().toLocal8Bit() );
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
