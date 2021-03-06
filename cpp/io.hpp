/*
QeoDart Copyright (C) 2012 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#ifndef IO_HPP
#define IO_HPP

#include <QWidget>
#include <QDomDocument>
#include <QFile>
#include <QLocale>
#include <QSettings>

struct place;
struct qcfFile;

class dart;

class io : public QWidget {
	Q_OBJECT
	
	dart *myDart;
	
	public:
		QSettings *settings;	
		
		bool bPortable;
		
		io(dart*);
		~io();
		int iReadQcf(QString mapname);
		int iReadOsm(QString filename);
		int iWriteQcf(QList<place> &places, qcfFile &f);
		int iFindQcf();
		int iCheckQcf(QFile &file, QDomDocument &doc);
		void vFillCurrentTypePlaces();
		void vLoadSettings();
		void vLoadHighScores(QString mapName);
		void vSaveHighScores(QString id);
		
 	private:
		QString qsGetQcfFilePath(QString mapname, int &index);
		QString qsGetIdFromMapName(QString mapName);
		
		QStringList qslGetPreferedQcfLanguage(), qslPlaceTypesNamesRegExps;
		
		bool bDeterminedPortable;
		
		void vInsertQcfxFile(qcfFile &f);
		void vGetMetaData(QDomDocument &doc, qcfFile &file);
};
#endif //IO_HPP 
