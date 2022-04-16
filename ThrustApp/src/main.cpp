#include "mainwindow.h"

#include <QDebug>
#include <QApplication>
#include <QStyleFactory>
#include <QSettings> 

#include "stat_value.hpp"

//enum AA
//{
//	a,
//	b,
//	c = b,
//	d
//};


template <class T>
constexpr void swap_ints( T& x, T& y)
{
	x ^= y;
	y ^= x;
	x ^= y;
}


//#define QT_NO_DEBUG_OUTPUT

int main(int argc, char *argv[])
{
	//qDebug() << a << " " << b << " " << c << " " << d << " " << endl;
	 
	//{
	//	int16_t x = 0xffff;
	//	int16_t y = 0x7fff;
	//	qDebug() << x << y;
	//	swap_ints(x,y);
	//	//x ^= y;
	//	//y ^= x;
	//	//x ^= y;
	//	qDebug() << x << y;
	//}
	//{
	//	float x = 0xffff;
	//	float y = 0x7fff;
	//	qDebug() << x << y;
	//	std::swap(x,y);
	//	qDebug() << x << y;
	//}

    qDebug() << "Available styles:";
	QStringList styles = QStyleFactory::keys();
	for (int i = 0; i < styles.size(); ++i)
		 qDebug() << styles.at(i).toLocal8Bit().constData();
	
	QApplication::setStyle(QStyleFactory::create("Fusion")); 

    QApplication app(argc, argv);

	app.setOrganizationName("Qontrol-eXperiment-4");
	app.setApplicationName("ThrustApp");

    MainWindow w(nullptr);
    w.show();
    return app.exec();
}
