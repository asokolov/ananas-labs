/****************************************************************************
** $Id: main.cpp,v 1.9 2007/09/18 11:49:34 app Exp $
**
** Main file of Ananas Administrator application
**
** Created : 20040823
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
** Copyright (C) 2003-2005 Grigory Panov <gr1313 at mail dot ru>, Yoshkar-Ola.
**
** This file is part of the Engine application of the Ananas
** automation accounting system.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.leaderit.ru/page=ananas or email sales@leaderit.ru
** See http://www.leaderit.ru/gpl/ for GPL licensing information.
**
** Contact org@leaderit.ru if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qapplication.h>
#include <qmessagebox.h>
#include <qtranslator.h>
#include <qsplashscreen.h>
#include <qtextcodec.h>
#include "mainform.h"
#include "ananas.h"
#include "alog.h"
#include "binreloc.h"

//QApplication *application = 0;
//MainForm *mainform = 0;
//QTranslator *translator = 0, tr_app(0), tr_lib(0), tr_plugins(0);
QString lang="en", 
	rcfile="", 
	username="", 
	userpassword="";

int setTranslator(QString lang)
{
        QString langdir;
#ifdef _Windows
	langdir = qApp->applicationDirPath()+"/";
#else
	BrInitError error; 
	if (br_init_lib(&error) == 0 && error != BR_INIT_ERROR_DISABLED) { 
		aLog::print(aLog::MT_INFO, QObject::tr("Warning: BinReloc failed to initialize (error code %1)\n").arg(error));
		aLog::print(aLog::MT_INFO, QObject::tr("Will fallback to hardcoded default path.\n"));
	} 
	aLog::print(aLog::MT_DEBUG, QObject::tr("setTranslator - BinReloc path to data dir is %1.\n").arg( br_find_data_dir("/usr/share") ));
	
	langdir = QString(br_find_data_dir("/usr/share")) + QString("/ananas/translations/");
#endif
//	tr_app.load( langdir+"ananas-administrator-"+lang+".qm",".");
//	tr_lib.load( langdir+"ananas-lib-"+lang+".qm",".");
//	tr_plugins.load( langdir+"ananas-plugins-"+lang+".qm",".");
	return 0;
}

int 
parseCommandLine( int argc, char **argv )
{
	QString param, name, value;
	int i;
	char *s, locale[50]="en";

	strncpy( locale, QTextCodec::locale(), sizeof( locale ) );
	s = strchr( locale, '_' );
	if ( s ) {
	    *s = 0;
	}
        lang = locale;
        setTranslator( lang );
//	printf("locale=%s\n", locale );	
	QString str_ru=QString::null, str_en=QString::null;
	bool lang_setted = false;
	bool help_setted = false;
	for ( i=1; i<argc; i++)
	{
	    param = argv[i];
	    name = param.section("=",0,0).lower();
	    value = param.section("=",1);
	    if (param == "--help")
	    {
		    str_ru = "Использование: ananas-administrator [--help] [--lang=<LANG>] [--rc=<RC_PATH>]\n";
		    str_ru+= "LANG=ru|en\n";
		    str_ru+= "RC_PATH=путь к *.rc файлу описания бизнес схемы\n";
			    
	    	    str_en = "Usage: ananas-administrator [--help] [--lang=<LANG>] [--rc=<RC_PATH>]\n";
		    str_en+= "LANG=ru|en\n";
		    str_en+= "RC_PATH=path to *.rc file of paticular business scheme\n";
		    help_setted = true;
			    
	    }
	    if (name == "--lang") {
		lang = value;
		lang_setted = true;
	        setTranslator( lang );
	    }
	    if (name == "--rc") rcfile = value;
	}
	if(help_setted)
	{
		if(lang == "ru")
		{
			printf("%s",(const char*)str_ru.local8Bit());
		}
		else
		{
			printf("%s",str_en.ascii());
		}
		return 1;
	}
	return 0;
}

int main( int argc, char ** argv )
{

	QApplication a( argc, argv );
	//dSelectDB dselectdb;
	int rc = 1;
	bool ok;
	QPixmap pixmap;
	BrInitError error; 
	

	QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF8") );
	qApp->addLibraryPath( qApp->applicationDirPath() );
	if ( parseCommandLine( argc, argv ) ) return 1;
//	qApp->installTranslator( &tr_app );
//	qApp->installTranslator( &tr_lib );
//	qApp->installTranslator( &tr_plugins );
	pixmap = QPixmap::fromMimeSource( "administrator-splash-"+lang+".png" );
	if ( pixmap.isNull() )
#ifdef _Windows
	pixmap = QPixmap::fromMimeSource( qApp->applicationDirPath()+"/administrator-splash-"+lang+".png" );
#else
	if (br_init_lib(&error) == 0 && error != BR_INIT_ERROR_DISABLED) { 
		aLog::print(aLog::MT_INFO, QObject::tr("Warning: BinReloc failed to initialize (error code %1)\n").arg(error));
		aLog::print(aLog::MT_INFO, QObject::tr("Will fallback to hardcoded default path.\n"));
	} 
	aLog::print(aLog::MT_DEBUG, QObject::tr("main - BinReloc path to data dir is %1.\n").arg( br_find_data_dir("/usr/share") ));
	
	pixmap = QPixmap::fromMimeSource( QString(br_find_data_dir("/usr/share") ) + "/ananas/administrator/locale/administrator-splash-"+lang+".png" );
	qApp->addLibraryPath( QString( br_find_lib_dir("/usr/lib")) + "/ananas/qt3plugins" );
	
    QStringList list = a.libraryPaths();
    QString libPath = "";
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
        libPath += *it+":";
        ++it;
    }	
	aLog::print(aLog::MT_DEBUG, QString("main - qt library path is '%1'\n").arg( libPath));
#endif
	if ( pixmap.isNull() )
		pixmap = QPixmap::fromMimeSource( "administrator-splash-en.png" );
	QSplashScreen *splash = new QSplashScreen( pixmap );
	if ( ananas_login( rcfile, username, userpassword ) ){
//	if ( rcfile.isEmpty() ) {
//    	    if (dselectdb.exec()==QDialog::Accepted) rcfile = dselectdb.rcfile;
//	}
//	if ( !rcfile.isEmpty() ) {
//		if (dselectdb.rcfile.isEmpty()) return 0;
	       	splash->show();
		splash->message( QObject::tr("Init application"), Qt::AlignBottom, Qt::white );
		MainForm *w = new MainForm( 0, "MainForm");
		//mainform = w;
//		mainformws = mainform->ws;
//		mainformwl = mainform->wl;
		qApp->setMainWidget( w );
		w->rcfile = rcfile;
//		printf( "rcfile = %s\n", rcfile.ascii() );
		w->show();
		ok = w->init();
		splash->clear();
       		splash->finish( w );
       		delete splash;
		if ( ok ) {
			a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
			rc = a.exec();
		} else {
			QMessageBox::critical(0, "error","Error init Ananas engine");
		}
		return rc;
	} else return 0;
}
