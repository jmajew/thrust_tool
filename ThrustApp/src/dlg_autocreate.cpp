#include <QDebug>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>

#include "dlg_autocreate.h"
#include "document.h"
#include "widget_graph.h"


DlgAutoCreate::DlgAutoCreate( QWidget* parent) 
	: QDialog( parent)
{
	ui.setupUi(this);
}

