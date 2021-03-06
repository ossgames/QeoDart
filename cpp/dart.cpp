/*
QeoDart Copyright (C) 2012 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"

using namespace std;

dart::dart(QMainWindow *parent) : QMainWindow(parent) {
	
	setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint);
	
	qlLayersNames << "background" << "elevations"<< "borders" << "rivers" ;
	
	qlComments << tr("Very Good!") << tr("Super!") << tr("Very Fine!")
	           << tr("Well Done!") << tr("Good!") << tr("That was good!")
	           << tr("Well.") << tr("That was OK.") << tr("That was reasonable.")
	           << tr("Oh boy!") << tr("Not really…") << tr("Not quite…")
	           << tr("Completely Wrong!") << tr("That wasn't much of a hit…") << tr("Missed completely!")
	           << tr("Read wrongly?") << tr("Clicked wrongly?") << tr("D'oh!"); // TODO typed wrongly?
	
	qlPlaceTypesNames << tr("States") << tr("Capitals of States") << tr("Countries") << tr("Capitals of Countries") << tr("Counties") << tr("Cities") << tr("Towns");
	
	iPlaceCount=0;
	iCurrentPlayer=0;
	bAcceptingClickEvent=true;
	bAcceptingResizeEvent=false;
	dPxToKm=1;
	iCurrentQcf=0;
	iScoreAreaMode=1;
	pTrainingPlaceNumber=NULL;
	iPaddingTop=0;
	iMarginTop=0;
	bGaveHint=false;
	bPlacesSubsetActive=false;
	
	setupUi(this);
	
	if(!(QCoreApplication::arguments().contains("--banner"))) {
		lblAd->setVisible(false);
		lblAd_2->setVisible(false);
	}
	
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(vTimeout()));
	
	resizeTimer = new QTimer(this);
	resizeTimer->setSingleShot(true);
	connect(resizeTimer, SIGNAL(timeout()), this, SLOT(vToolbarOverflow()));
	
// "error: unresolved external symbol time" when compiling for WinCE
#ifdef Q_OS_WINCE
	srand(GetTickCount());
#else
	srand(time(NULL));
#endif

	toolBar->setMovable(false);
	
	for(int i=0; i<4; i++) {
		QLabel *lblMap = new QLabel(this);
		lblMap->setAlignment(Qt::AlignTop);
		lblMap->setParent(centralwidget);
		lblMap->show();
		lblMap->setGeometry(0, 0, this->width(), this->width());
		qlMapLayers.append(lblMap);
	}
	
	lblMouseClickOverlay = new QMouseReleaseLabel(this);
	lblMouseClickOverlay->setParent(centralwidget); //we want the label to be placed under the toolbar
	lblMouseClickOverlay->setAlignment(Qt::AlignTop);
	lblMouseClickOverlay->show();
	lblMouseClickOverlay->setGeometry(0, 0, iGetWindowSize(), iGetWindowSize());
	
	agGameMode = new QActionGroup(this);
	agGameMode->addAction(actionFind_Place);
	agGameMode->addAction(actionTraining);
	agGameMode->addAction(actionLocal);
	agGameMode->addAction(actionNetwork);
	actionLocal->setChecked(true);
	
	agAskForMode = new QActionGroup(this);
	agAskForMode->addAction(actionPosition_of_Place);
	agAskForMode->addAction(actionName_of_Place);
	actionName_of_Place->setChecked(true);
	
	agLayers = new QActionGroup(this);
	agLayers->setExclusive(false);
	agLayers->addAction(actionElevations);
	agLayers->addAction(actionBorders);
	agLayers->addAction(actionRivers);
	
	agPlaceTypes = new QActionGroup(this);
	agPlaceTypes->setExclusive(false);
	agPlaceTypes->addAction(actionStates);
	agPlaceTypes->addAction(actionCapitals_of_States);
	agPlaceTypes->addAction(actionCountries);
	agPlaceTypes->addAction(actionCapitals_of_Countries);
	agPlaceTypes->addAction(actionCounties);
	agPlaceTypes->addAction(actionCities);
	agPlaceTypes->addAction(actionTowns);
	
	connect(actionHigh_Score_List, SIGNAL(triggered()), this, SLOT(vShowHighScores()));
	actionHigh_Score_List->setIcon(QIcon::fromTheme("games-highscores", QIcon(":/icons/oxygen/games-highscores.png")));
	connect(actionConfigure, SIGNAL(triggered()), this, SLOT(vShowPreferences()));
	actionConfigure->setIcon(QIcon::fromTheme("configure", QIcon(":/icons/oxygen/configure.png")));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(vClose()));
	actionQuit->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/oxygen/application-exit.png")));
	connect(actionNew_Game, SIGNAL(triggered()), this, SLOT(vNewGame()));
	actionNew_Game->setIcon(QIcon::fromTheme("document-new", QIcon(":/icons/oxygen/document-new.png")));
	connect(action100, SIGNAL(triggered()), this, SLOT(vResize()));
	action100->setIcon(QIcon::fromTheme("zoom-original", QIcon(":/icons/oxygen/zoom-original.png")));
	connect(actionFind_Place, SIGNAL(triggered()), this, SLOT(vSetGameMode()));
	actionFind_Place->setIcon(QIcon::fromTheme("edit-find", QIcon(":/icons/oxygen/edit-find.png")));
	connect(actionTraining, SIGNAL(triggered()), this, SLOT(vSetGameMode()));
	actionTraining->setIcon(QIcon::fromTheme("user-identity", QIcon::fromTheme("emblem-personal", QIcon(":/icons/oxygen/user-identity.png"))));
	connect(actionLocal, SIGNAL(triggered()), this, SLOT(vSetGameMode()));
	actionLocal->setIcon(QIcon::fromTheme("system-users", QIcon(":/icons/oxygen/system-users.png")));
//	connect(actionNetwork, SIGNAL(triggered()), this, SLOT(vSetGameMode()));
	actionNetwork->setIcon(QIcon::fromTheme("network-workgroup", QIcon(":/icons/oxygen/network-workgroup.png")));
	connect(actionNumber_of_Players, SIGNAL(triggered()), this, SLOT(vSetNumberOfPlayers()));
	connect(actionPlayers, SIGNAL(triggered()), this, SLOT(vSetNumberOfPlayers()));
	connect(actionAgainst_Time, SIGNAL(triggered()), this, SLOT(vSetAgainstTime()));
	actionAgainst_Time->setIcon(QIcon::fromTheme("time", QIcon::fromTheme("player-time", QIcon(":/icons/oxygen/player-time.png"))));
	connect(actionName_of_Place, SIGNAL(triggered()), this, SLOT(vSetAskForMode()));
	connect(actionPosition_of_Place, SIGNAL(triggered()), this, SLOT(vSetAskForMode()));
	connect(actionCountries, SIGNAL(triggered()), this, SLOT(vSetPlaceType()));
	connect(actionCapitals_of_Countries, SIGNAL(triggered()), this, SLOT(vSetPlaceType()));
	connect(actionStates, SIGNAL(triggered()), this, SLOT(vSetPlaceType()));
	connect(actionCapitals_of_States, SIGNAL(triggered()), this, SLOT(vSetPlaceType()));
	connect(actionCounties, SIGNAL(triggered()), this, SLOT(vSetPlaceType()));
	connect(actionCities, SIGNAL(triggered()), this, SLOT(vSetPlaceType()));
	connect(actionTowns, SIGNAL(triggered()), this, SLOT(vSetPlaceType()));
	connect(actionAdd_Map, SIGNAL(triggered()), this, SLOT(vAddMap()));
	actionAdd_Map->setIcon(QIcon::fromTheme("list-add", QIcon(":/icons/oxygen/list-add.png")));
	connect(actionHelp, SIGNAL(triggered()), this, SLOT(vShowHelp()));
	actionHelp->setIcon(QIcon::fromTheme("help-contents", QIcon(":/icons/oxygen/help-contents.png")));
	connect(action_Whats_this, SIGNAL(triggered()), this, SLOT(vActivateContextHelp()));
	action_Whats_this->setIcon(QIcon::fromTheme("help-contextual", QIcon(":/icons/oxygen/help-contextual.png")));
	connect(actionHint, SIGNAL(triggered()), this, SLOT(vGiveHint()));
	actionHint->setIcon(QIcon::fromTheme("games-hint", QIcon::fromTheme("help-hint", QIcon(":/icons/oxygen/games-hint.png"))));
	connect(actionReport_Bug, SIGNAL(triggered()), this, SLOT(vReportBug()));
	actionReport_Bug->setIcon(QIcon::fromTheme("tools-report-bug", QIcon::fromTheme("lpi-bug", QIcon(":/icons/oxygen/tools-report-bug.png"))));
	connect(actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(actionAbout_QeoDart, SIGNAL(triggered()), this, SLOT(vShowAboutWindow()));
	actionAbout_QeoDart->setIcon(QIcon::fromTheme("help-about", QIcon(":/icons/oxygen/help-about.png")));
	connect(actionMenu_Bar, SIGNAL(triggered()), this, SLOT(vSetToolMenuBarState()));
	actionMenu_Bar->setIcon(QIcon::fromTheme("show-menu", QIcon(":/icons/oxygen/show-menu.png")));
	connect(actionToolbar, SIGNAL(triggered()), this, SLOT(vSetToolMenuBarState()));
	connect(actionBorders, SIGNAL(triggered()), this, SLOT(vToggleMapLayer()));
	connect(actionRivers, SIGNAL(triggered()), this, SLOT(vToggleMapLayer()));
	connect(actionElevations, SIGNAL(triggered()), this, SLOT(vToggleMapLayer()));
	
	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(vReturnPressedEvent()));
	connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(vTextEditedEvent()));
	connect(cbMatchBehaviour, SIGNAL(currentIndexChanged(int)), this, SLOT(vTextEditedEvent()));
	
	// these menus are needed for QToolButtons only and shouldn't be displayed in the main menus
	menubar->removeAction(menuApplication->menuAction());
	menuSettings->removeAction(menuAskForMode->menuAction());
	menuSettings->removeAction(menuPlaceType->menuAction());
	
	btApplication = new QToolButton(this);
	btApplication->setMenu(menuApplication);
	btApplication->setPopupMode(QToolButton::InstantPopup);
	btApplication->setText(tr("Game"));
	actionBtApplication=toolBar->addWidget(btApplication);
	
	menuApplication->removeAction(actionQuit);
	menuApplication->addMenu(menuHelp);
	menuApplication->addSeparator();
	menuApplication->addAction(actionQuit);
	
	toolBar->addAction(actionNew_Game);
	
	btGameMode = new QToolButton(toolBar);
	btGameMode->setMenu(menuGameMode);
	btGameMode->setPopupMode(QToolButton::InstantPopup);
	btGameMode->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	toolBar->addWidget(btGameMode);
	
	toolBar->addAction(actionPlayers);
	
	toolBar->addAction(actionAgainst_Time);
	
	btAskForMode = new QToolButton(toolBar);
	btAskForMode->setMenu(menuAskForMode);
	btAskForMode->setPopupMode(QToolButton::InstantPopup);
	btAskForMode->setToolButtonStyle(Qt::ToolButtonTextOnly);
	actionBtAskForMode=toolBar->addWidget(btAskForMode);
	
	menuSubset = new QMenu(tr("Subset"),this);
	menuPlaceType->addMenu(menuSubset);
	menuAsk_for->addMenu(menuSubset);
	btPlaceType = new QToolButton(toolBar);
	btPlaceType->setMenu(menuPlaceType);
	btPlaceType->setPopupMode(QToolButton::InstantPopup);
	btPlaceType->setToolButtonStyle(Qt::ToolButtonTextOnly);
	btPlaceType->setText(tr("Place Types"));
	toolBar->addWidget(btPlaceType);
	
	btMap = new QToolButton(toolBar);
	btMap->setMenu(menuMap);
	btMap->setPopupMode(QToolButton::InstantPopup);
	btMap->setToolButtonStyle(Qt::ToolButtonTextOnly);
	toolBar->addWidget(btMap);
	
	btView = new QToolButton(toolBar);
	btView->setMenu(menuView);
	btView->setPopupMode(QToolButton::InstantPopup);
	btView->setToolButtonStyle(Qt::ToolButtonTextOnly);
	btView->setText(tr("View"));
	toolBar->addWidget(btView);
	
	
	myIO = new io(this);
	myIO->vLoadSettings();
	
	// when calling this in class io it doesn't work for some reason
	cbMatchBehaviour->setCurrentIndex(myIO->settings->value("iMatchBehaviour",1).toInt());
	cbSearchDistance->setCurrentIndex(myIO->settings->value("iSearchDistance",2).toInt());
	
	if(myIO->iFindQcf()==0) {
		qDebug() << "[E] No valid qcfx files found, exiting";
		QMessageBox msgBox;
		msgBox.setText(tr("Sorry, no valid qcfx files could be found."));
		msgBox.setInformativeText(tr("You might want to add a file through Map → Add map.")); // TODO
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
	
	agMap = new QActionGroup(this);
	bool foundMapName=false;
	for(int i=0; i<qlQcfxFiles.count(); i++) {
		QAction *menuItem;
// QIcon produces crash in WinCE
#ifdef Q_OS_WINCE
		menuItem = new QAction(qlQcfxFiles[i].mapName, this);
#else
		menuItem = new QAction(QIcon(qlQcfxFiles[i].path+"/background.png"), qlQcfxFiles[i].mapName, this);
#endif
		menuItem->setToolTip(QString(tr("Load map of %1")).arg(qlQcfxFiles[i].mapName));
		menuItem->setCheckable(true);
		if(i<9) menuItem->setShortcut(QKeySequence(tr("Ctrl+%1").arg(i+1)));
		connect(menuItem, SIGNAL(triggered()), this, SLOT(vReadQcf()));
		menuMap->addAction(menuItem);
		agMap->addAction(menuItem);
		// load the saved map…
		if(qlQcfxFiles[i].mapName==myIO->settings->value("qsCurrentMapName","DUMMYFILE").toString()) {
			menuItem->trigger();
			foundMapName=true;
		}
	}
	// …or the first map as fall-back
	if(!foundMapName) agMap->actions()[0]->trigger();
	menuMap->addSeparator();
	menuMap->addAction(actionAdd_Map);
	
	gridLayout->setSpacing(1);
	
	vSetPlaceType(qsCurrentPlaceType);
	
	vSetNumberOfPlayers(iNumberOfPlayers);
	
	if(bAgainstTime) actionAgainst_Time->trigger();
	else vSetAgainstTime();
	
	switch(iAskForMode) {
		case enNames:
			actionName_of_Place->trigger(); break;
		case enPositions:
		        actionPosition_of_Place->trigger(); break;
	}
	switch(iGameMode) {
		case enFind:
			actionFind_Place->trigger(); break;
		case enTraining:
			actionTraining->trigger(); break;
		case enLocal:
		        actionLocal->trigger(); break;
	}
	vSetToolMenuBarState(iToolMenuBarState);
	
	vRepaintCommonLabels();
	vRepaintPlayerLabels();
	
	gridLayout->removeWidget(lblCurrentRound); // we do not want to seg fault
	gridLayout->addWidget(lblCurrentRound,0,4);
	
	bAcceptingResizeEvent=true;
	vResize(dZoomFactor);
	
	show();
}

dart::~dart() {
	myIO->settings->setValue("iMatchBehaviour",cbMatchBehaviour->currentIndex());
	myIO->settings->setValue("iSearchDistance",cbSearchDistance->currentIndex());
	
	vRemoveAllCircles();
	vRemoveAllCommonPoints();
	//vSetNumberOfPlayers(0); // QGridLayout: Cannot add QLabel/lblTime to QGridLayout/gridLayout at row -1 column 4
        timer->stop();
        delete timer;
	resizeTimer->stop();
	delete resizeTimer;
        delete myIO;
}

/*!
  Draws distance circles for all players using the saved click-coordinates of the given place.
  @param n the place number in qlScoreHistory
  @see dart::vDrawCircle(int x, int y, int r, int player)
  @see dart::vDrawClickPositions(int n)
  */
void dart::vDrawDistanceCircles(int n) {
	bool drewCircle=true;
	
	for(int i=0; i<7 && drewCircle; i++) {
		drewCircle=false;
		
		for(int j=0; j<iNumberOfPlayers; j++) { // draw circles for each player
			if(i*RADIUS+3*PENWIDTH < qlScoreHistory[j][n-1].diffPx) {
				vDrawCircle(qlScoreHistory[j][n-1].x,qlScoreHistory[j][n-1].y,(i+1)*RADIUS,j);
				drewCircle=true;
			}
		} // for each player
		
		mySleep(iDelayNextCircle);
	} // for(i<7)
}

/*!
  Draws a distance circle.
  @param x x-coordinate
  @param y y-coordinate
  @param r radius
  @param player the player number (determines the color)
  @see dart::vDrawDistanceCircles(int n)
  */
void dart::vDrawCircle(int x, int y, int r, int player) {
	QLabel *circleLabel;
	circleLabel = new QCircleLabel(this,x,y,r,qlColorsOfPlayers[player],this);
	qlCircleLabels[player].append(circleLabel);
}

void dart::vRemoveAllCircles() {
	for(int i=0,max=qlCircleLabels.count();i<max;i++){
		qDebug()<<i;
		while(qlCircleLabels[i].count()!=0) {
			delete qlCircleLabels[i][0];
			qlCircleLabels[i].removeAt(0);
		}
	}
}

void dart::vRemoveAllCommonPoints() {
	while(qlPointLabels.count()!=0){
		delete qlPointLabels[0];
		qlPointLabels.removeAt(0);
	}
	while(qlDebugPlaceLabels.count()!=0){
		delete qlDebugPlaceLabels[0];
		qlDebugPlaceLabels.removeAt(0);
	}
}

void dart::vTimeout() {
        if(iTimerElapsed==iMaxTime) return;
        qDebug() << ++iTimerElapsed;
	lblTime->setText(QString("%1").arg(iMaxTime-iTimerElapsed));
}

void dart::vSetPlaceType() {
	if(!bNewGameIsSafe()) { vUpdateActionsIsCheckedStates(); return; }
	
	if(bCanLoseScore() &&
	   !bStartNewGameWarning(tr("Change Place Types"), tr("When you change this setting, your current score will be lost.\nDo you want to continue?"))) {
		vUpdateActionsIsCheckedStates();
		return;
	}
	
	bPlacesSubsetActive=false;
	
	qsCurrentPlaceType="";
	if(actionCountries->isChecked()) qsCurrentPlaceType+="country;";
	if(actionCapitals_of_Countries->isChecked()) qsCurrentPlaceType+="capitalOfCountry;";
	if(actionStates->isChecked()) qsCurrentPlaceType+="state;";
	if(actionCapitals_of_States->isChecked()) qsCurrentPlaceType+="capitalOfState;";
	if(actionCounties->isChecked()) qsCurrentPlaceType+="county;";
	if(actionCities->isChecked()) qsCurrentPlaceType+="city;";
	if(actionTowns->isChecked()) qsCurrentPlaceType+="town;";
	myIO->vFillCurrentTypePlaces();
	
	vSetGameMode(iGameMode);
}
void dart::vSetPlaceType(QString placetype) {
	qsCurrentPlaceType=placetype;
	vUpdateActionsIsCheckedStates();
	myIO->vFillCurrentTypePlaces();
}

void dart::vCreatePlacesSubsetsActions() {
	bool lastWasActive=false;
	int lastIndex=-1;
	
	if(!qlPlacesSubsetsActions.isEmpty()) {
		do { // the last menuitem must be removed since the label might be wrong
			lastWasActive=qlPlacesSubsetsActions.last()->isChecked();
			lastIndex=qlPlacesSubsetsActions.count()-1;
			menuSubset->removeAction(qlPlacesSubsetsActions.last());
			delete qlPlacesSubsetsActions.last();
			qlPlacesSubsetsActions.removeLast();
		} while(qlPlacesSubsetsActions.count()*10>=qlCurrentTypePlaces.count() && !qlPlacesSubsetsActions.isEmpty());
	}
	
	for(int i=qlPlacesSubsetsActions.count()*10; i<qlCurrentTypePlaces.count(); i+=10) {
		QAction *action = new QAction(tr("Place %1 to %2").arg(i+1).arg(qlCurrentTypePlaces.count() < i+10 ? qlCurrentTypePlaces.count() : i+10),this);
		action->setCheckable(true);
		connect(action, SIGNAL(triggered()), this, SLOT(vPlacesSubsetClicked()));
		menuSubset->addAction(action);
		qlPlacesSubsetsActions.append(action);
	}
	
	if(lastWasActive && lastIndex!=-1 && qlPlacesSubsetsActions.count()>lastIndex) qlPlacesSubsetsActions[lastIndex]->setChecked(true);
}

void dart::vPlacesSubsetClicked() {
	if(!bNewGameIsSafe() ||
	   (bCanLoseScore() &&
	   !bStartNewGameWarning(tr("Change Place Types"), tr("When you change this setting, your current score will be lost.\nDo you want to continue?"))) ) {
		static_cast<QAction*>(QObject::sender())->setChecked(!static_cast<QAction*>(QObject::sender())->isChecked());
		return;
	}
	
	myIO->vFillCurrentTypePlaces();
	vSetGameMode(iGameMode);
}

void dart::vUpdatePlacesSubsetActive() {
	bPlacesSubsetActive=false;
	for(int i=0; i<qlPlacesSubsetsActions.count() && !bPlacesSubsetActive; i++) {
		if(qlPlacesSubsetsActions[i]->isChecked()) bPlacesSubsetActive=true;
	}
}

void dart::vSetAgainstTime() {
	if(!bNewGameIsSafe() ||
	   (bCanLoseScore() &&
	    !bStartNewGameWarning(tr("Enable Against Time"), tr("When you change this setting, your current score will be lost.\nDo you want to continue?"))) ) {
		vUpdateActionsIsCheckedStates();
		return;
	}
	
	vSetAgainstTime(actionAgainst_Time->isChecked());

	myIO->settings->setValue("bAgainstTime", bAgainstTime);
	
	vSetGameMode(iGameMode);
}
// resetting timer
void dart::vSetAgainstTime(bool enable) {
        bAgainstTime=enable;
        iTimerElapsed=0;
        if(bAgainstTime) {
                lblTime->setText(QString("%1").arg(iMaxTime-iTimerElapsed));
                timer->start(1000);
        } else {
                lblTime->setText("");
                timer->stop();
        }
}

void dart::vSetNumberOfPlayers() {
	if(!bNewGameIsSafe()) return;
	
	bool ok;
	int min=iCurrentPlayer+1;
	int max=iPlaceCount>1 ? iNumberOfPlayers : 99;
	int n = QInputDialog::getInt(this, tr("Set Number Of Players"), tr("Number Of Players:"), iNumberOfPlayers, min, max, 1, &ok);
	if(n>15) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Warning"));
		msgBox.setText(tr("You want to play with more than 15 players.\nAlthough QeoDart theoretically supports an infinite number of players, some display problems might occur.\n\nDo you want to continue anyway?"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		msgBox.setIcon(QMessageBox::Warning);
		if(msgBox.exec()==QMessageBox::Cancel) return;
	}
	if(ok) vSetNumberOfPlayers(n);
	
	vToolbarOverflow();
	
	myIO->settings->setValue("iNumberOfPlayers", iNumberOfPlayers);
}
void dart::vSetNumberOfPlayers(int n) {
	iNumberOfPlayers=n;
	qDebug() << "[i] iNumberOfPlayers" << iNumberOfPlayers;
	if(iNumberOfPlayers>15) qDebug() << "[w] very much players";
	
	actionPlayers->setText(QString(tr("Players: %1")).arg(iNumberOfPlayers));
	
	if(qlPlayerLabels.count()>n) {
		
		for(int i=qlPlayerLabels.count()-1; i>iNumberOfPlayers-1; i--) {
			
// 			QList<QLabel*> *qlPlayerLabel=qlPlayerLabels[i];
			qDebug()<<"f"<<i;
			qDebug() << "fdddfff" << qlPlayerLabels[0].count();
			for(int j=0,max=qlPlayerLabels[i].count(); j<max; j++) {
				gridLayout->removeWidget(qlPlayerLabels[i][j]);
				delete qlPlayerLabels[i][j];
				qDebug()<<"ff";
			}
			qlPlayerLabels[i].clear();
// 			delete qlPlayerLabels[i];			//TODO we should do it somehow, shouldn't we?
			qlPlayerLabels.removeAt(i);
			
			qlScoreHistory[i].clear(); //TODO must delete sub-lists?
			qlScoreHistory.removeAt(i);
			qlColorsOfPlayers.removeAt(i);
			
		}
		
	} else if(qlPlayerLabels.count()<n) {
		
		while(qlPlayerLabels.count()<iNumberOfPlayers){
			int i=qlPlayerLabels.count();
			qDebug()<<"player labels available"<<i;
			
			QLabel *lblScore;
			lblScore = new QLabel(this);
			gridLayout->addWidget(lblScore,i,0);
			lblScore->setText(QString(tr("%n Point(s) &Oslash; %1, %2","",0)).arg(0.0,0,'f',1).arg(0.0,0,'f',1));
			lblScore->setTextFormat(Qt::RichText);
			
			QLabel *lblRating;
			lblRating = new QLabel(this);
			lblRating->setAlignment(Qt::AlignCenter);
			gridLayout->addWidget(lblRating,i,2);
			
			QList<QLabel*> qlPlayerLabel;
			qlPlayerLabel << lblScore << lblRating;
			qlPlayerLabels.append(qlPlayerLabel);
			
			QList<QLabel*> qlCircleLabel;
			qlCircleLabels.append(qlCircleLabel);
			
			QList<scoreHistory> qlHistory;
			qlScoreHistory.append(qlHistory);
			
			// usually we do this when starting a new game, but since we allow increasing the number of players
			// during the first round, we must add totalScore here
			while(qlTotalScores.count()<iNumberOfPlayers) {
				totalScore ts;
				ts.score=0;
				ts.mark=0;
				qlTotalScores.append(ts);
			}
			
			qlColorsOfPlayers.append(qcGetColorOfPlayer(qlPlayerLabels.count()-1));
		}
	}
	
	gridLayout->removeWidget(lblCurrentPlace);
	gridLayout->removeWidget(lblCurrentPlayer);
	gridLayout->removeWidget(lineEdit);
	gridLayout->removeWidget(lblTime);
	if(iNumberOfPlayers==1) {
		gridLayout->removeWidget(lblComment);
		gridLayout->addWidget(lblCurrentPlace,1*(iGameMode!=enTraining && iGameMode!=enFind),0);
		gridLayout->addWidget(lblCurrentPlayer,1,4);
		gridLayout->addWidget(lblComment,1,2);
		gridLayout->addWidget(lineEdit,1*(iGameMode!=enTraining && iGameMode!=enFind),0);
        gridLayout->addWidget(lblTime,1*(iGameMode!=enTraining && iGameMode!=enFind),4);
		lblComment->setText("");
		lblComment->show();
        lblCurrentPlayer->hide();
	} else {
		gridLayout->addWidget(lblCurrentPlace,iNumberOfPlayers,0);
		gridLayout->addWidget(lblCurrentPlayer,iNumberOfPlayers,4);
		gridLayout->addWidget(lineEdit,iNumberOfPlayers,0);
        gridLayout->addWidget(lblTime,iNumberOfPlayers-1,4);
		lblComment->hide();
        lblCurrentPlayer->show();
	}
	
	gridLayout->removeItem(spGridLayoutVertical);
	gridLayout->addItem(spGridLayoutVertical,iNumberOfPlayers+1,0);
	
	resizeEvent(0); // the label's font size and iPaddingTop must be recalculated
}

void dart::resizeEvent(QResizeEvent *event) {
	if(!bAcceptingResizeEvent) return;
	
	iMarginTop=toolBar->height()+menubar->height();
	
	dZoomFactor=iGetWindowSize()/600.0; //TODO must be calculated in another way
	
	vRepaintPlayerLabels();
	vRepaintCommonLabels();
	iPaddingTop=iGetPaddingTop();
	
	dZoomFactor=iGetWindowSize()/600.0;
	
	lblMouseClickOverlay->resize(600*dZoomFactor,600*dZoomFactor);
	lblMouseClickOverlay->move(0,iPaddingTop);
	
	vRepaintMap();
	
	gridLayoutWidget->setGeometry(QRect(0,0,600*dZoomFactor+1,iPaddingTop+50));
	
	lblAd->setGeometry(0,iPaddingTop+600*dZoomFactor,600*dZoomFactor,111);
	lblAd_2->setGeometry(600*dZoomFactor,iPaddingTop,245,600*dZoomFactor);

	qDebug() << "[i] iPaddingTop" << iPaddingTop << "iMarginTop" << iMarginTop << "dZoomFactor" << dZoomFactor << "fontSize" << iGetFontSize();
	
	myIO->settings->setValue("dZoomFactor", dZoomFactor);
	
	resizeTimer->start(200);
}

/*!
 * shortens the toolbar labels when the window becomes too narrow
 */
void dart::vToolbarOverflow() {
	if(!bAcceptingResizeEvent) return;
	mySleep(1);
	qDebug() << "[i] vToolbarOverflow";
	
	if(iToolMenuBarState==enMenuBarOnly) return;
	
	int d=0;
	if(iToolMenuBarState==enToolBarOnly) d=1;
		
	vShowTotalScores();
	
	if(width()>320)
		lblCurrentRound->setText(QString(tr("Place %1 of %2")).arg(iPlaceCount).arg(iMaxPlaceCount));
	else
		lblCurrentRound->setText(QString(tr("%1/%2")).arg(iPlaceCount).arg(iMaxPlaceCount));
	
	if(bShortenToolbarText) {
	
		QWidget *w; // last widget in toolbar
		w=toolBar->layout()->itemAt(toolBar->layout()->count()-1)->widget();
		
		QString longText;
		
		static_cast<QToolButton*>(toolBar->layout()->itemAt(3+d)->widget())->setText(tr("Against Time"));
		static_cast<QToolButton*>(toolBar->layout()->itemAt(0+d)->widget())->setText(tr("New Game"));
		static_cast<QToolButton*>(toolBar->layout()->itemAt(4+d)->widget())
			->setText(QString(tr("Ask for: %1")).arg(iAskForMode==enNames ? tr("Name of Place") : tr("Position of Place")));
		static_cast<QToolButton*>(toolBar->layout()->itemAt(6+d)->widget())
			->setText(QString(tr("Map: %1")).arg(qlQcfxFiles[iCurrentQcf].mapName));
		static_cast<QToolButton*>(toolBar->layout()->itemAt(2+d)->widget())
			->setText(QString(tr("Players: %1")).arg(iNumberOfPlayers));
		static_cast<QToolButton*>(toolBar->layout()->itemAt(5+d)->widget())->setText(tr("Place Types"));
		
		switch(iGameMode) {
			case enFind: longText=tr("Find Place"); break;
			case enTraining: longText=tr("Training"); break;
			case enLocal: longText=tr("Local"); break;
		}
		static_cast<QToolButton*>(toolBar->layout()->itemAt(1+d)->widget())->setText(longText);
		
		
		mySleep(1); // repaint
		
		// while the last toolbar button is visible or in overflow view, shorten the text of other items
		for(int i=0; (!w->isVisible() || w->y()>5) && i<9; i++) {
			
			switch(i) {
				case 0:
					static_cast<QToolButton*>(toolBar->layout()->itemAt(3+d)->widget())
						->setText("");
					break;
				case 1:
					static_cast<QToolButton*>(toolBar->layout()->itemAt(0+d)->widget())
						->setText("");
					break;
				case 2:
					static_cast<QToolButton*>(toolBar->layout()->itemAt(4+d)->widget())
						->setText(iAskForMode==enNames ? tr("Name of Place") : tr("Position of Place"));
					break;
				case 3:
					static_cast<QToolButton*>(toolBar->layout()->itemAt(6+d)->widget())
						->setText(QString(tr("%1")).arg(qlQcfxFiles[iCurrentQcf].mapName));
					break;
				case 4:
					static_cast<QToolButton*>(toolBar->layout()->itemAt(4+d)->widget())
						->setText(iAskForMode==enNames ? tr("Name") : tr("Position"));
					break;
				case 5:
					static_cast<QToolButton*>(toolBar->layout()->itemAt(2+d)->widget())
						->setText(QString(tr("%1")).arg(iNumberOfPlayers));
					break;
				case 6:
					static_cast<QToolButton*>(toolBar->layout()->itemAt(5+d)->widget())
						->setText(tr("Places"));
					break;
				case 7:
					static_cast<QToolButton*>(toolBar->layout()->itemAt(6+d)->widget())
						->setText(QString(tr("%1")).arg(qlQcfxFiles[iCurrentQcf].mapShortName));
					break;
				case 8:
					static_cast<QToolButton*>(toolBar->layout()->itemAt(1+d)->widget())
						->setText("");
					break;
			}
			
			mySleep(1);
		} // while the last toolbar button is visible or in overflow view, shorten the text of other items
	} // if(bShortenToolbarText)
}

void dart::vResize(double dNewZoomFactor) {
	showNormal();
	
	dZoomFactor=dNewZoomFactor;
	iPaddingTop=iGetPaddingTop();
	resize(600*dZoomFactor,iMarginTop+iPaddingTop+600*dZoomFactor);
}

void dart::vRepaintMap() {
	QString path=qlQcfxFiles[iCurrentQcf].path;
	
	for(int i=0; i<4; i++) {
		qlMapLayers[i]->resize(600*dZoomFactor,600*dZoomFactor);
		qlMapLayers[i]->setText(QString("<img src=\"%2/%3.png\" height=\"%1\" width=\"%1\"/>").arg(600*dZoomFactor).arg(path).arg(qlLayersNames[i]));
		qlMapLayers[i]->move(0,iPaddingTop);
	}
}

//repaints all labels which are player-specific
void dart::vRepaintPlayerLabels() {
	int fontSize=iGetFontSize();
	for(int i=0,max=qlPlayerLabels.count(); i<max; i++) {
		for(int j=0,max=qlPlayerLabels[i].count(); j<max; j++) {
			qlPlayerLabels[i][j]->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qlColorsOfPlayers[i].name()));
		}
	}
}

//repaints all labels which are not player-specific
void dart::vRepaintCommonLabels() {
	int fontSize=iGetFontSize();
	QString stylesheet=QString("color:%2;font-size:%1px;font-family:arial,sans-serif")
                        .arg(fontSize)
                        .arg(qlColorsOfPlayers[iCurrentPlayer].name());
	lblCurrentPlace->setStyleSheet(stylesheet);
	lblComment->setStyleSheet(stylesheet);
	lblCurrentRound->setStyleSheet(stylesheet);
	lblTime->setStyleSheet(stylesheet);
	lblCurrentPlayer->setStyleSheet(stylesheet);
	lblCurrentPlayer->setText(QString(tr("Player %1")).arg(iCurrentPlayer+1));
	lineEdit->setMaximumHeight(fontSize+2);
}

int dart::iGetFontSize() {
	return (20-iNumberOfPlayers/2)*dZoomFactor<10 ? 10 : (20-iNumberOfPlayers/2)*dZoomFactor;
}

int dart::iGetPaddingTop() {
	return (qlPlayerLabels.count()+1*(iGameMode!=enTraining && iGameMode!=enFind)) * (iGetFontSize()+6);
}

/*!
  Draws a point at P(x|y) with the label, and adds it to the given list.
  @param x x-coordinate
  @param y y-coordinate
  @param list the list to which a reference to the new QPointLabel is stored
  @param name the label text
  @param color the color
  @see dart::vDrawPoint(int x, int y, QList<QLabel*> &list, QColor color, QString name)
  */
void dart::vDrawPoint(int x, int y, QList<QLabel*> &list, QString name, QColor color) {
	QLabel *lblCurrentPlacePosition;
	lblCurrentPlacePosition = new QPointLabel(this, name, x, y, color, this);
	list.append(lblCurrentPlacePosition);
// 	lblCurrentPlace->setGeometry(x,y+iPaddingTop,50,50);
// 	lblCurrentPlace->show();
	qDebug() << "[i] drew point" << x << y << "+" << iMarginTop;
}
/*!
  overloads vDrawPoint(int x, int y, QList<QLabel*> &list, QString name, QColor color)
  */
void dart::vDrawPoint(int x, int y, QList<QLabel*> &list, QColor color, QString name) {
	vDrawPoint(x, y, list, name, color);
}

void dart::vDrawDebugPlace(int i) {
	if(QCoreApplication::arguments().contains("--debug-places") || QCoreApplication::arguments().contains("-dp")) {
		QLabel *lblRectangle;
		lblRectangle = new QRectangleLabel(this,qlCurrentTypePlaces[i]->x,qlCurrentTypePlaces[i]->y,qlCurrentTypePlaces[i]->dimx,qlCurrentTypePlaces[i]->dimy,this);
		qlDebugPlaceLabels.append(lblRectangle);
	}
}

/*!
  Draws the click positions of all players.
  @param n round
  @see dart::vDrawDistanceCircles(int n)
  */
void dart::vDrawClickPositions(int n) {
	for(int i=0; i<iNumberOfPlayers; i++) { //draw circles for every player
		vDrawPoint(qlScoreHistory[i][n-1].x,qlScoreHistory[i][n-1].y,qlCircleLabels[i],qlColorsOfPlayers[i]);
	}
}

void dart::vShowAllPlaces() {
	for(int i=0, max=qlCurrentTypePlaces.count(); i<max; i++){ //TODO WITH?
		vDrawPoint(qlCurrentTypePlaces[i]->x,qlCurrentTypePlaces[i]->y,qlPointLabels,qlCurrentTypePlaces[i]->name);
		vDrawDebugPlace(i);
	}
	lblMouseClickOverlay->raise();
}

void dart::vMouseClickEvent(int x, int y) {
	if(iGameMode==enFind) {
		vFindPlaceAround(iGetUnzoomed(x),iGetUnzoomed(y));
		return;
	}
	if(iAskForMode!=enPositions) {
		QString text=lineEdit->text();
		lineEdit->blockSignals(true);
		lineEdit->setText(tr("What is the name of the marked place?"));
		for(int i=0; i<3; i++) {
			lineEdit->setStyleSheet("background-color: red");
			mySleep(400);
			lineEdit->setStyleSheet("");
			mySleep(400);
		}
		lineEdit->setText(text);
		lineEdit->blockSignals(false);
		return;
	}
	if(!bAcceptingClickEvent) return;
	bAcceptingClickEvent=false;
	
	x=iGetUnzoomed(x);
	y=iGetUnzoomed(y);
	vDrawPoint(x,y,qlCircleLabels[iCurrentPlayer],qlColorsOfPlayers[iCurrentPlayer]);
	
	scoreHistory score;
	score.x=x;
	score.y=y;
	score.diffPxArea=dGetDistanceInPx(x,y,iPlaceCount-1); // respects area // TODO what about shown distance?
	score.diffPx=dGetDistanceInPxBetween(x,y,qlPlacesHistory[iPlaceCount-1]->x,qlPlacesHistory[iPlaceCount-1]->y);
	score.diffKm=dGetDistanceInKm(score.diffPxArea);
	score.mark=dGetMarkFromDistance(score.diffPxArea);
	score.score=dGetScore(score.mark);
	if(bAgainstTime) {
		score.score*=1-static_cast<double>(iTimerElapsed)/iMaxTime;
		score.mark=dGetMarkFromScore(score.score);
		timer->stop();
	}
	if(bGaveHint) {
		score.score*=0.75;
		score.mark=dGetMarkFromScore(score.score);
		bGaveHint=false;
	}
	qlScoreHistory[iCurrentPlayer].append(score);
	
	if(! (iGameMode==enTraining && iPlaceCount>=5) ) {
		qlTotalScores[iCurrentPlayer].score+=score.score;
		qlTotalScores[iCurrentPlayer].mark=dGetAverageMarkOfPlayer(iCurrentPlayer);
	}
	
	
	qDebug() << "Score for Player" << iCurrentPlayer << score.diffPx << "px" << score.diffPxArea << "px (area)" << score.diffKm << "km" << score.score << "p" << score.mark << "TOTAL" << qlTotalScores[iCurrentPlayer].score << "p" << qlTotalScores[iCurrentPlayer].mark;
	
	if(iCurrentPlayer<iNumberOfPlayers-1) { // next player
		
		mySleep(iDelayNextPlayer);
		
		if(bResetCursor) QCursor::setPos(QWidget::x()+5,QWidget::y()+iPaddingTop+iMarginTop+10);
		
		iCurrentPlayer++;
		vRemoveAllCircles();
		qDebug()<<"f";
		vRepaintPlayerLabels();
		vRepaintCommonLabels();
		vSetAgainstTime(bAgainstTime);
		
		bAcceptingClickEvent=true;
		
	} else { // show results
		
		iCurrentPlayer=0;
		
		vRemoveAllCircles();
		vDrawClickPositions(iPlaceCount);
		mySleep(iDelayNextCircle);
		vDrawDistanceCircles(iPlaceCount);
		
		// show real position
		vShowCurrentPlace();
		
		vShowScores();
		
		vShowComment();
		
		if(iGameMode==enTraining) mySleep(iDelayNextPlaceTraining);
		else mySleep(iDelayNextPlace);
		
		lblComment->setText("");
		
		vShowTotalScores();
		
		vRemoveAllCircles();
		vRemoveAllCommonPoints();
		
		if(iGameMode!=enTraining) vResetScoreLabels();
		
		vRepaintCommonLabels();
		
		vNextRound();
		
	}
}

void dart::vShowComment() {
	if(iNumberOfPlayers==1 && iGameMode!=enTraining) {
		// we shoudln't use the saved mark here as inappropriate comments could be shown
		// (e.g "clicked wrongly", although clicked correctly, but bad time)
		double mark=dGetMarkFromDistance(qlScoreHistory[0][iPlaceCount-1].diffPxArea);
		int i = rand() % 3 + 3*(static_cast<int>(mark-1));
		qDebug() << "[i]" << mark << "comment #" << i;
		lblComment->setText(qlComments[i]);
	}
}

void dart::vShowResultWindows() {
	bAcceptingClickEvent=false;
	int iHighlightHighScore=-1;
	
	for(int i=0,max=qlPlacesHistory.count(); i<max; i++) {
		vDrawPoint(qlPlacesHistory[i]->x, qlPlacesHistory[i]->y, qlPointLabels, qlPlacesHistory[i]->name);
	}
	
	for(int i=0; i<iNumberOfPlayers; i++) {
		resultWindow dialog(iHighlightHighScore,this,i,myIO);
		dialog.exec();
	}
	
	iPlaceCount=0; // needed for quit?-dialog
	
	if(iHighlightHighScore>-1 && bAutoShowHighScores) vShowHighScores(iHighlightHighScore);
	
	if(bAutoNewGame) vNewGame();
}

void dart::vResetScoreLabels() {
	for(int i=0; i<qlPlayerLabels.count(); i++) { // reset score labels of each player
		qlPlayerLabels[i][1]->setText("");
	}
}

void dart::vShowCurrentPlace() {
	if(iAskForMode==enPositions) vDrawPoint(qlPlacesHistory[iPlaceCount-1]->x,qlPlacesHistory[iPlaceCount-1]->y,qlPointLabels);
	else vDrawPoint(qlPlacesHistory[iPlaceCount-1]->x,qlPlacesHistory[iPlaceCount-1]->y,qlPointLabels, qlPlacesHistory[iPlaceCount-1]->name);
}

void dart::vShowScores() {
	for(int i=0; i<iNumberOfPlayers; i++) { // show score for each player
		QString km, score, missedBy;
		if(qlScoreHistory[i][iPlaceCount-1].diffKm==-1) km=tr("∞");
		else km=QString("%1").arg(qlScoreHistory[i][iPlaceCount-1].diffKm,0,'f',1);
		if(iGameMode!=enTraining) score=QString(" +%1").arg(qlScoreHistory[i][iPlaceCount-1].score);
		
		// the important score is not readable on small displays
		if(dZoomFactor>0.5) {
			if(km=="0.0") missedBy=tr("Hit the mark ");
			else missedBy=tr("Missed by %1 km ").arg(km);
		}
		else missedBy=QString(tr("%1 km ")).arg(km);
		
		if(dPxToKm==-1) missedBy="";
		
		qlPlayerLabels[i][1]->setText(QString(tr("%1(%2)%3"))
				      .arg(missedBy)
				      .arg(qlScoreHistory[i][iPlaceCount-1].mark,0,'f',1)
				      .arg(score));
	}
}

void dart::vShowTotalScores() {
	for(int i=0; i<qlPlayerLabels.count(); i++) { // show score for each player
		if(width()>300)
			qlPlayerLabels[i][0]->setText(QString(tr("%n Point(s) &Oslash; %1, %2","",qlTotalScores[i].score)).arg(dGetAverageScoreOfPlayer(i),0,'f',1).arg(qlTotalScores[i].mark,0,'f',1));
		else
			qlPlayerLabels[i][0]->setText(QString(tr("%1, &Oslash; %2, %3")).arg(qlTotalScores[i].score).arg(dGetAverageScoreOfPlayer(i),0,'f',1).arg(qlTotalScores[i].mark,0,'f',1));
	}
}

int dart::iGetWindowSize() {
	return (width()<height()-iMarginTop-iPaddingTop) ? width() : (height()-iMarginTop-iPaddingTop);
}

void dart::vClose() {
	close();
}

void dart::closeEvent(QCloseEvent *event) {
	if(iPlaceCount>1 && iGameMode!=enTraining) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Quit QeoDart"));
		msgBox.setText(tr("Do you really want to quit QeoDart?"));
		msgBox.setInformativeText(tr("Your current score will be lost."));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if(msgBox.exec()==QMessageBox::Yes) event->accept();
		else event->ignore();
	} else {
		event->accept();
	}
}

int dart::iGetUnzoomed(double x) {
	return x/dZoomFactor;
}

double dart::dGetZoomed(int x) {
	return x*dZoomFactor;
}

void dart::mySleep(int ms) {
	if(ms<0 || ms>5000) ms=0;
	
	Q_ASSERT(QCoreApplication::instance());
	QTime timer;
	timer.start();
	do {
		QCoreApplication::processEvents(QEventLoop::AllEvents, ms);
#ifdef Q_OS_UNIX
		struct timespec t;
		t.tv_sec = 0;
		t.tv_nsec = 10000;
		nanosleep(&t,NULL);
#endif
#ifdef Q_OS_WIN32
		_sleep(10);
#endif
	} while (timer.elapsed() < ms);
}

QColor dart::qcGetColorOfPlayer(int player) {
	int m=player%6, i=255/pow(2,player/6);
	QColor c=QColor(0,0,0,255);
	
	if(m==0 || m==3 || m==5) c.setBlue(i);
	if(m==1 || m==3 || m==4) c.setGreen(i*0.85); // *x makes the colors a bit less bright
	if(m==2 || m==4 || m==5) c.setRed(i*0.9);
	
	return c;
}

void dart::vSetGameMode() {
	if(!bNewGameIsSafe() ||
	   (bCanLoseScore() &&
	    !bStartNewGameWarning(tr("Change Game Mode"), tr("When you change the game mode, your current score will be lost.\nDo you want to continue?"))) ) {
		vUpdateActionsIsCheckedStates();
		return;
	}
	
	btGameMode->setText(QString("%1").arg(static_cast<QAction*>(QObject::sender())->text()));
	btGameMode->setIcon(static_cast<QAction*>(QObject::sender())->icon());
	vToolbarOverflow();
	
	if(QObject::sender()==actionFind_Place) {
		vSetGameMode(enFind);
	} else if(QObject::sender()==actionTraining) {
		vSetGameMode(enTraining);
	} else if(QObject::sender()==actionLocal) {
		vSetGameMode(enLocal);
	} else {
		qDebug() << "[E] vSetGameMode: unknown sender";
	}
	
	myIO->settings->setValue("iGameMode", iGameMode);
}
void dart::vSetGameMode(enGameModes mode) {
	// undo hiding lbls etc.
	switch(iGameMode) {
		case enFind:
			if(iAskForMode==enPositions) lblCurrentPlace->show();
			lblCurrentRound->show();
			lblCurrentPlayer->show();
			lblTime->show();
			qlPlayerLabels[0][0]->show();
			actionNumber_of_Players->setEnabled(true);
			actionPlayers->setEnabled(true);
			actionPlayers->setVisible(true);
			btAskForMode->setEnabled(true);
			actionName_of_Place->setEnabled(true);
			actionPosition_of_Place->setEnabled(true);
			actionAgainst_Time->setEnabled(true);
			
			gridLayout->removeItem(horizontalSpacer);
			gridLayout->addItem(horizontalSpacer,0,1);
			gridLayout->removeItem(horizontalSpacer_2);
			gridLayout->addItem(horizontalSpacer_2,0,3);
			
			if(iAskForMode!=enNames) lineEdit->hide();
			
			break;
		case enTraining:
			lblCurrentRound->show();
			lblCurrentPlayer->show();
			qlPlayerLabels[0][0]->show();
			actionNumber_of_Players->setEnabled(true);
			actionPlayers->setVisible(true);
			actionPlayers->setEnabled(true);
			qDebug()<<"shoudl be true"<<actionPlayers->isEnabled();
			vResetScoreLabels();
			break;
		case enLocal:
			break;
	}
	
	iGameMode=mode;
	
	vResetForNewGame();
	
	// TODO can we do it like this for everything here?
	cbSearchDistance->setVisible(iGameMode==enFind);
	cbMatchBehaviour->setVisible(iGameMode==enFind);
	
	switch(iGameMode) {
		case enFind:
			lblCurrentPlace->hide();
			lblCurrentRound->hide();
			lblCurrentPlayer->hide();
			lblTime->hide();
			qlPlayerLabels[0][0]->hide();
			actionNumber_of_Players->setEnabled(false);
			actionPlayers->setEnabled(false);
			actionPlayers->setVisible(false);
			btAskForMode->setEnabled(false);
			actionName_of_Place->setEnabled(false);
			actionPosition_of_Place->setEnabled(false);
			actionAgainst_Time->setEnabled(false);
			
			gridLayout->removeItem(horizontalSpacer);
			gridLayout->addItem(horizontalSpacer,0,4);
			gridLayout->removeItem(horizontalSpacer_2);
			
			gridLayout->removeWidget(cbMatchBehaviour);
			gridLayout->addWidget(cbMatchBehaviour,0,1);
			gridLayout->removeWidget(cbSearchDistance);
			gridLayout->addWidget(cbSearchDistance,0,3);
			
			lineEdit->show();
			gridLayout->removeWidget(lineEdit);
			gridLayout->addWidget(lineEdit,0,0);
			
			vSetNumberOfPlayers(1);
			lineEdit->clear();
			vTextEditedEvent(); return;
			break;
		case enTraining:
			lblCurrentRound->hide();
			lblCurrentPlayer->hide();
			qlPlayerLabels[0][0]->hide();
			actionNumber_of_Players->setEnabled(false);
			actionPlayers->setVisible(false);
			actionPlayers->setEnabled(false);
			
			vSetNumberOfPlayers(1);
			break;
		case enLocal:
			vSetNumberOfPlayers(myIO->settings->value("iNumberOfPlayers","1").toInt());
			break;
	}
	
	vNextRound();
}

void dart::vSetAskForMode() {
	if(!bNewGameIsSafe() ||
	   (bCanLoseScore() &&
	    !bStartNewGameWarning(tr("Change Mode"), tr("When you change this setting, your current score will be lost.\nDo you want to continue?"))) ) {
		vUpdateActionsIsCheckedStates();
		return;
	}
	
	btAskForMode->setText(QString(tr("Ask for: %1").arg(static_cast<QAction*>(QObject::sender())->text())));
	vToolbarOverflow();
	
	if(QObject::sender()==actionName_of_Place) {
		vSetAskForMode(enNames);
	} else if(QObject::sender()==actionPosition_of_Place) {
		vSetAskForMode(enPositions);
	} else {
		qDebug() << "[E] vSetAskForMode: unknown sender";
	}
	
	myIO->settings->setValue("iAskForMode", iAskForMode);
}
void dart::vSetAskForMode(enAskForModes mode) {
	iAskForMode=mode;
	
	vResetForNewGame();
	
	switch(iAskForMode) {
		case enPositions:
			lineEdit->hide();
			lblCurrentPlace->show();
#ifndef Q_OS_WINCE
			if(bUseOurCursor) lblMouseClickOverlay->setCursor(QCursor(QPixmap(":/icons/cursor.png"),1,1));
			else lblMouseClickOverlay->setCursor(Qt::ArrowCursor);
#endif
			break;
		case enNames:
			lineEdit->show();
			lblCurrentPlace->hide();
#ifndef Q_OS_WINCE
			lblMouseClickOverlay->setCursor(Qt::ArrowCursor);
#endif
			break;
	}
	
	vSetNumberOfPlayers(iNumberOfPlayers);
	
	vNextRound();
}

void dart::vResetForNewGame() {
	vRemoveAllCircles();
	vRemoveAllCommonPoints();
	qlPlacesHistory.clear();
	
	for(int i=qlScoreHistory.count()-1; i>-1; i--) {
		qlScoreHistory[i].clear(); //TODO must delete sub-lists?
	}
	
	qlTotalScores.clear();
	for(int i=0; i<iNumberOfPlayers; i++) {
		totalScore ts;
		ts.score=0;
		ts.mark=0;
		qlTotalScores.append(ts);
	}
	iPlaceCount=0;
	iCurrentPlayer=0;
	
	vShowTotalScores();
	if(qlPlayerLabels.count()!=0) vRepaintCommonLabels();
}

void dart::vNewGame() {
	if(!bNewGameIsSafe() ||
	   (bCanLoseScore() &&
	    !bStartNewGameWarning(tr("New Game"), tr("When you start a new game, your current score will be lost.\nDo you want to continue?"))) ) {
		return;
	}
	
	iCurrentPlayer=0;
	vSetNumberOfPlayers(iNumberOfPlayers);
	vSetGameMode(iGameMode);
}

void dart::vNextRound() {
	iCurrentPlayer=0;
	
	if(iPlaceCount==iMaxPlaceCount && iGameMode!=enTraining) {
		vShowResultWindows();
		return;
	}
	
	int pn=-1;
	place *ppn=NULL;
	
	if(iGameMode==enTraining && iPlaceCount>=5) {
		qDebug() << "Revise";
		for(int i=0; i<qlScoreHistory[0].count() && ppn==NULL; i++) {
		//for(int i=0; i<qlScoreHistory[0].count() && pn==-1; i++) {
			qDebug() << qlScoreHistory[0][i].mark << qlTotalScores[0].mark;
			if(qlScoreHistory[0][i].mark>=4 || (qlScoreHistory[0][i].mark>2 && qlScoreHistory[0][i].mark>qlTotalScores[0].mark) ) {
				qDebug() << qlPlacesHistory[i]->name;
				
				if(iPlaceCount==5 && i==4) {
					//we shouldn't ask for the last place immediatly
					//so we keep it in mind
					pTrainingPlaceNumber=qlPlacesHistory[i];
				} else {
					qlScoreHistory[0][i].mark=0;
					ppn=qlPlacesHistory[i];
				}
			} //if (badscore)
		} // for (scorehistory)
		if(ppn==NULL) {
			vResetForNewGame();
		}
	}
	
	if(ppn==NULL) {
		int i=0;
		do {
			qDebug() << qlCurrentTypePlaces.count();
			pn = rand() % qlCurrentTypePlaces.count();
		} while(qlPlacesHistory.contains(qlCurrentTypePlaces[pn]) && i++<10);
		
		if(iGameMode==enTraining && pTrainingPlaceNumber!=NULL && iPlaceCount==2) {
			ppn=pTrainingPlaceNumber;
			pTrainingPlaceNumber=NULL;
		} else {
			ppn=qlCurrentTypePlaces[pn];
		}
	}
	
	iPlaceCount++;
	qlPlacesHistory.append(ppn);
	
	qDebug() << "[i] next place:" << pn << qlPlacesHistory[iPlaceCount-1]->name << iPlaceCount << "/" << iMaxPlaceCount;
	
	switch(iAskForMode) {
		case enPositions:
			bAcceptingClickEvent=true;
			lblCurrentPlace->setText(qlPlacesHistory[iPlaceCount-1]->name);
			break;
		case enNames:
			vDrawPoint(qlPlacesHistory[iPlaceCount-1]->x, qlPlacesHistory[iPlaceCount-1]->y, qlPointLabels);
			lineEdit->clear();
			lineEdit->setStyleSheet("");
			lineEdit->setEnabled(true);
			lineEdit->setFocus(Qt::OtherFocusReason);
			break;
	};
	
	switch(iGameMode) {
		case enTraining:
			break;
		case enLocal:
			if(width()>320)
				lblCurrentRound->setText(QString(tr("Place %1 of %2")).arg(iPlaceCount).arg(iMaxPlaceCount));
			else
				lblCurrentRound->setText(QString(tr("%1/%2")).arg(iPlaceCount).arg(iMaxPlaceCount));
			break;
	};
	
	vSetAgainstTime(bAgainstTime);
}

/*!
  @return the distance between P(a|b) and Q(x|y)
  @param a unzoomed x-coordinate of the first point
  @param b unzoomed y-coordinate of the first point
  @param x unzoomed x-coordinate of the second point
  @param y unzoomed y-coordinate of the second point
 */
double dart::dGetDistanceInPxBetween(int a, int b, int x, int y) {
	if( (a==-1 && b==-1) || (x==-1 && y==-1) ) return -1;
	return sqrt( pow(a-x,2) + pow(b-y,2) ); //thx Pythagoras
}

/*!
  @return the distance between P(a|b) and place #n [>=0], respecting iScoreAreaMode
  @param a unzoomed x-coordinate of the point
  @param b unzoomed y-coordinate of the point
  @param n round number
 */
double dart::dGetDistanceInPx(int a, int b, int n) {
	if(a==-1 && b==-1) return -1;
	
	int x=qlPlacesHistory[n]->x;
	int y=qlPlacesHistory[n]->y;
	
	if(iAskForMode==enNames) return dGetDistanceInPxBetween(a,b,x,y);
	
	int dim2x=qlPlacesHistory[n]->dimx*(iScoreAreaMode/2.0)/2;
	int dim2y=qlPlacesHistory[n]->dimx*(iScoreAreaMode/2.0)/2;
	
	if(a>x+dim2x) a-=dim2x;
	else if(a<x-dim2x) a+=dim2x;
	else a=x;
	
	if(b>y+dim2y) b-=dim2y;
	else if(b<y-dim2y) b+=dim2y;
	else b=y;
	
	return dGetDistanceInPxBetween(a,b,x,y);
}

double dart::dGetDistanceInKm(double px) {
	if(px==-1) return -1;
	return px*dPxToKm;
}

/*!
  @return the mark (German system TODO other systems)
  @param distance unzoomed distance in px
 */
double dart::dGetMarkFromDistance(double distance) {
	if(distance==-1) return 6;
	
	if(distance>dGetZoomed(1)) distance-=dGetZoomed(1); // a difference of 1px is OK 
	else distance=0;

	double mark=distance/RADIUS+1;
	if(mark<4) {
		return mark<1 ? 1 : mark;
	} else {
		mark=4+(mark-4)/2;
		return mark>6 ? 6 : mark;
	}
}

double dart::dGetScore(double mark) {
	double score;
	cout<<-16.66*mark+116.66; // I really don't know why, but w/o this line the function never returns 100
	if(mark<4) score=-16.66*mark+116.66;
	else score=-25*mark+150;
	if(score<0) score=0;
	return score;
}

double dart::dGetMarkFromScore(double score) {
	double mark;
	if(score>=50) {
		mark=(score-116.66)/-16.66;
		return mark<1 ? 1 : mark;
	} else {
		mark=(score-150)/-25;
		return mark>6 ? 6 : mark;
	}
}

double dart::dGetAverageMarkOfPlayer(int player) {
	return dGetMarkFromScore(dGetAverageScoreOfPlayer(player));
}

double dart::dGetAverageScoreOfPlayer(int player) {
	if(qlScoreHistory[player].count()<1) return 0;
	return qlTotalScores[player].score/qlScoreHistory[player].count();
}

/*!
  Looks for lineEdit->text() in the list of places.
  @return the index of the place in qlAllPlaces
  @param f factor for possible penalty
 */
int dart::iFindInputInList(double &f) {
	QString input=lineEdit->text();
	qDebug() << "[i] input" << input;
	
	int index=-1;
	
	for(int l=0; (l<3 && index==-1); l++) {
		qDebug() << "l" << l;
		
		if(l==0) f=1;
		else if(l==1) f=0.75;
		else if(l==2) f=0.5;
		
		input=qsSimplifyString(input, l);
		
		for(int i=0,max=qlAllPlaces.count(); (i<max && index==-1); i++) {
			if(qsSimplifyString(qlAllPlaces[i].name,l)==input) index=i;
		}
	}
	
	if(index!=-1) qDebug() << "[i] found" << qlAllPlaces[index].name << f;
	else qDebug() << "[i] not found" << f;
	
	return index;
}

QString dart::qsSimplifyString(QString str, int l) {
	if(l>=0) {
		str=str.toLower();
		str=str.replace("am ", "a");
		str=str.replace("an ", "a");
		str=str.replace("der ", "d");
		str=str.replace(" ", "");
		
		if(l>=1) {
			str=str.replace("-", "");
			str=str.replace("_", "");
			str=str.replace("_", "");
			str=str.replace(".", "");
			str=str.replace(QRegExp("([aou])e"), "\\1");
			str=str.replace(QRegExp(QString::fromUtf8("[äáàãă]")), "a");
			str=str.replace(QRegExp(QString::fromUtf8("[éèẽ]")), "e");
			str=str.replace(QRegExp(QString::fromUtf8("[íìĩ]")), "i");
			str=str.replace(QRegExp(QString::fromUtf8("[öṏø]")), "o");
			str=str.replace(QRegExp(QString::fromUtf8("[üǘǜ]")), "u");
			str=str.replace(QRegExp(QString::fromUtf8("[ßśş]")), "s");
			str=str.replace("ss", "s");
			
			if(l>=2) {
				str=str.replace("ai", "ei");
				str=str.replace("ay", "ei");
				str=str.replace("ie", "i");
				str=str.replace("d", "t");
				str=str.replace("ck", "k");
				str=str.replace("c", "k");
				str=str.replace("kh", "ch");
				str=str.replace("g", "k");
				str=str.replace("w", "v");
				str=str.replace("f", "v");
				//remove double letters and lengthening
				for(int i=0; i<str.length()-1; i++) {
					if(
					   str[i]==str[i+1] ||
					   (QString(str[i]).contains(QRegExp("[aeiou]")) && str[i+1]=='h') ||
					   (QString(str[i]).contains(QRegExp("[aeou]")) && str[i+1]=='r') ||
					   (QString(str[i]).contains(QRegExp("[ou]")) && str[i+1]=='i')
					  ) {
						str=str.remove(i+1,1);
						i--;
					}
				}
				qDebug()<<str;
			} // 2
		} // 1
	} // 0
	
	return str;
}

void dart::vReturnPressedEvent() { // TODO split (net!)
	if(iAskForMode!=enNames || iGameMode==enFind) {
		qDebug() << "[i] vReturnPressedEvent rejected";
		return;
	}
	lineEdit->setEnabled(false);
	
	double f;
	int iIndexOfPlace=iFindInputInList(f);
	
	lineEdit->setStyleSheet("");
	if(f<1) {
		lineEdit->setStyleSheet("text-decoration:underline;"); // TODO underline is not always visible
		if(f<0.75) {
			lineEdit->setStyleSheet("text-decoration:underline;color:red");
			if(iIndexOfPlace==-1) {
				lineEdit->setStyleSheet("text-decoration:underline;color:red;background:black");
			}
		}
	}
	if(iPenalty==0 || (iPenalty==2 && f==0.75) ) f=1;
	
	int x,y;
	if(iIndexOfPlace!=-1) {
		x=qlAllPlaces[iIndexOfPlace].x;
		y=qlAllPlaces[iIndexOfPlace].y;
	} else {
		x=-1;
		y=-1;
	}

	scoreHistory score;
	score.x=x;
	score.y=y;
	score.diffPxArea=dGetDistanceInPx(x,y,iPlaceCount-1); // respects area // TODO what about shown distance?
	score.diffPx=dGetDistanceInPxBetween(x,y,qlPlacesHistory[iPlaceCount-1]->x,qlPlacesHistory[iPlaceCount-1]->y);
	score.diffKm=dGetDistanceInKm(score.diffPxArea);
	score.mark=dGetMarkFromDistance(score.diffPxArea);
	score.score=dGetScore(score.mark)*f;
	if(bAgainstTime) {
		//typing against time can be hard, so there are some bonusSeconds
		double bonusSeconds=lineEdit->text().length()/static_cast<double>(iLettersPerSecond);
		iTimerElapsed-=bonusSeconds;
		if(iTimerElapsed<0) iTimerElapsed=0;
		score.score*=1-static_cast<double>(iTimerElapsed)/iMaxTime;
		timer->stop();
	}
	if(bGaveHint) {
		score.score*=0.75;
		score.mark=dGetMarkFromScore(score.score);
		bGaveHint=false;
	}
	score.mark=dGetMarkFromScore(score.score);
	
	qlScoreHistory[iCurrentPlayer].append(score);
	
	if(! (iGameMode==enTraining && iPlaceCount>=5) ) {
		qlTotalScores[iCurrentPlayer].score+=score.score;
		qlTotalScores[iCurrentPlayer].mark=dGetAverageMarkOfPlayer(iCurrentPlayer);
	}
	
	
	qDebug() << "Score for Player" << iCurrentPlayer << score.diffPx << "px" << score.diffPxArea << "px (area)" << score.diffKm << "km" << score.score << "p" << score.mark << "TOTAL" << qlTotalScores[iCurrentPlayer].score << "p" << qlTotalScores[iCurrentPlayer].mark;
	
	if(iCurrentPlayer<iNumberOfPlayers-1) { // next player
		
		mySleep(iDelayNextPlayer);
		iCurrentPlayer++;
		vRemoveAllCircles();
		qDebug()<<"f";
		vRepaintPlayerLabels();
		vRepaintCommonLabels();
		
		lineEdit->clear();
		lineEdit->setStyleSheet("");
		lineEdit->setEnabled(true); //c//
		lineEdit->setFocus(Qt::OtherFocusReason);
		
		vSetAgainstTime(bAgainstTime);
		
	} else { // show results
		
		iCurrentPlayer=0;
		
		vRemoveAllCircles();
		vDrawClickPositions(iPlaceCount);
		mySleep(iDelayNextCircle);
		vDrawDistanceCircles(iPlaceCount);
		
		// show real position
		vShowCurrentPlace();
		
		vShowScores();
		
		vShowComment();
		
		if(iGameMode==enTraining) mySleep(iDelayNextPlaceTraining);
		else mySleep(iDelayNextPlace);
		
		lblComment->setText("");
		
		vShowTotalScores();
		
		vRemoveAllCircles();
		vRemoveAllCommonPoints();
		
		if(iGameMode!=enTraining) vResetScoreLabels();
		
		vRepaintCommonLabels();
		
		vNextRound();
		
	}
	//cE//
}

void dart::vReadQcf() {
	if(!bNewGameIsSafe() ||
	   (bCanLoseScore() &&
	    !bStartNewGameWarning(tr("Change Map"), tr("When you change the map, your current score will be lost.\nDo you want to continue?"))) ) {
		vUpdateActionsIsCheckedStates();
		return;
	}
	
	if(myIO->iReadQcf(static_cast<QAction*>(QObject::sender())->text())!=0) exit(-1);
	
	btMap->setText(QString(tr("Map: %1")).arg(static_cast<QAction*>(QObject::sender())->text()));
	vToolbarOverflow();
	
	cbSearchDistance->setItemText(0,QString(tr("%1 km")).arg(dGetDistanceInKm(10),0,'f',1));
	cbSearchDistance->setItemText(1,QString(tr("%1 km")).arg(dGetDistanceInKm(20),0,'f',1));
	cbSearchDistance->setItemText(2,QString(tr("%1 km")).arg(dGetDistanceInKm(30),0,'f',1));
	
	vRepaintMap();
	mySleep(1);
	vToggleMapLayer();
	vResetForNewGame();
	vNextRound();
	
	if(iGameMode==enFind) vTextEditedEvent();
	
	myIO->settings->setValue("qsCurrentMapName",static_cast<QAction*>(QObject::sender())->text()); // TODO use id
}

void dart::vSetToolMenuBarState() {
	if (actionMenu_Bar->isChecked() && actionToolbar->isChecked()) iToolMenuBarState=enBoth;
	else if (actionMenu_Bar->isChecked() && !actionToolbar->isChecked()) iToolMenuBarState=enMenuBarOnly;
	else if (!actionMenu_Bar->isChecked() && actionToolbar->isChecked()) iToolMenuBarState=enToolBarOnly;
	else { actionToolbar->trigger(); return; }
	
	switch(iToolMenuBarState) {
		case enBoth:
			toolBar->removeAction(actionBtApplication);
			menuHelp->setIcon(QIcon());
			menuApplication->setVisible(false);
			menubar->setVisible(true);
			toolBar->setVisible(true);
			break;
			
		case enToolBarOnly:
			toolBar->insertAction(actionNew_Game, actionBtApplication);
			menuHelp->setIcon(QIcon::fromTheme("help-contents", QIcon(":/icons/oxygen/help-contents.png")));
			menubar->setVisible(false);
			toolBar->setVisible(true);
			break;
			
		case enMenuBarOnly:
			menuHelp->setIcon(QIcon());
			menubar->setVisible(true);
			toolBar->setVisible(false);
			break;
	}
	
	resizeEvent(0);
	
	myIO->settings->setValue("iToolMenuBarState", iToolMenuBarState);
}
void dart::vSetToolMenuBarState(enToolMenuBarState state) {
	iToolMenuBarState=state;
	
	switch(iToolMenuBarState) {
		case enBoth:
			actionMenu_Bar->trigger();
			actionToolbar->trigger(); break;
		case enToolBarOnly:
			actionToolbar->trigger(); break;
		case enMenuBarOnly:
			actionMenu_Bar->trigger(); break;
	}
}

bool dart::bCanLoseScore() {
	return ( (iPlaceCount>1 || iCurrentPlayer!=0) && iGameMode!=enTraining && iGameMode!=enFind);
}

/*!
  Checks if we can start a new game safely (i.e. "sum up scores" and stuff has finished [otherwise we could get a segfault]).
  @return true when new game is safe
 */
bool dart::bNewGameIsSafe() {
	bool val=false;
	
	if(iPlaceCount==0) return true;
	
	switch(iAskForMode) {
		case enPositions:
			val=bAcceptingClickEvent; break;
		case enNames:
			val=lineEdit->isEnabled(); break;
		default:
			qDebug() << "[E] bNewGameIsSafe" << iGameMode;
	}
	
	if(!val) qDebug() << "[i] new game is unsafe!";
	
	return val;
}

void dart::vShowPreferences() {
	if(!bNewGameIsSafe()) return;
	preferences dialog(this,myIO);
	dialog.exec();
}

void dart::vShowHighScores(int iHighlightHighScore) {
	highScoreWindow dialog(iHighlightHighScore,this,myIO,this);
	dialog.exec();
}

void dart::vShowAboutWindow() {
	aboutWindow dialog(this,this);
	dialog.exec();
}

void dart::vRetranslate() {
	QTranslator translator;
	translator.load(QString(QCoreApplication::applicationDirPath()+"/lang/%1").arg(qsLanguage=="default" ? QLocale::system().name() : qsLanguage));
	QApplication::installTranslator(&translator);
	retranslateUi(this);
}

void dart::vUpdateActionsIsCheckedStates() {
	actionCountries->setChecked(qsCurrentPlaceType.contains("country"));
	actionCapitals_of_Countries->setChecked(qsCurrentPlaceType.contains("capitalOfCountry"));
	actionStates->setChecked(qsCurrentPlaceType.contains("state"));
	actionCapitals_of_States->setChecked(qsCurrentPlaceType.contains("capitalOfState"));
	actionCounties->setChecked(qsCurrentPlaceType.contains("county"));
	actionCities->setChecked(qsCurrentPlaceType.contains("city"));
	actionTowns->setChecked(qsCurrentPlaceType.contains("town"));
	
	agAskForMode->actions()[iAskForMode]->setChecked(true);
	
	switch(iGameMode) {
		case enFind:
			actionFind_Place->setChecked(true); break;
		case enTraining:
			actionTraining->setChecked(true); break;
		case enLocal:
			actionLocal->setChecked(true); break;
	}
	
	agMap->actions()[iCurrentQcf]->setChecked(true);
	
	actionAgainst_Time->setChecked(bAgainstTime);
	
	vToolbarOverflow(); // the "Against Time" label appears for some reason
}

/*!
  Shows a layer when the ckeckbox is checked and the layer is available.
 */
void dart::vToggleMapLayer() {
	for(int i=0; i<3; i++) {
		bool visible=agLayers->actions()[i]->isChecked() && agLayers->actions()[i]->isVisible();
		qlMapLayers[i+1]->setVisible(visible);
		if(agLayers->actions()[i]->isVisible()) myIO->settings->setValue(qlLayersNames[i+1],visible); // do not change the saved setting only because the layer is not available for the current map
	}
}

void dart::vFindPlaceAround(int x, int y) {
	vRemoveAllCommonPoints();
	
	//vircle!
	for(int i=0; i<qlCurrentTypePlaces.count(); i++) {
		double distance=dGetDistanceInPxBetween(qlCurrentTypePlaces[i]->x,qlCurrentTypePlaces[i]->y,x,y);
		
		if(distance<=10) {
			vDrawPoint(qlCurrentTypePlaces[i]->x, qlCurrentTypePlaces[i]->y, qlPointLabels, qlCurrentTypePlaces[i]->name, QColor(249,199,65,255));
			vDrawDebugPlace(i);
		} else if(distance<=20 && cbSearchDistance->currentIndex()>=1) {
			vDrawPoint(qlCurrentTypePlaces[i]->x, qlCurrentTypePlaces[i]->y, qlPointLabels, qlCurrentTypePlaces[i]->name, QColor(249,199,65,170));
		} else if(distance<=30 && cbSearchDistance->currentIndex()>=2) {
			vDrawPoint(qlCurrentTypePlaces[i]->x, qlCurrentTypePlaces[i]->y, qlPointLabels, qlCurrentTypePlaces[i]->name, QColor(249,199,65,85));
		}
	}
	lblMouseClickOverlay->raise();
}

void dart::vTextEditedEvent() {
	if(iGameMode!=enFind) return;
	
	qDebug() << "vTextEditedEvent";
	
	vRemoveAllCommonPoints();
	
	bool found=false;
	QString text=lineEdit->text().toLower();
	
	if(text=="") { vShowAllPlaces(); lineEdit->setStyleSheet(""); return; }
	
	const int m=cbMatchBehaviour->currentIndex();
	
	for(int i=0; i<qlCurrentTypePlaces.count(); i++) {
		if(m==0) { // match beginning
		
			if(qlCurrentTypePlaces[i]->name.indexOf(text,0,Qt::CaseInsensitive)==0) {
				vDrawPoint(qlCurrentTypePlaces[i]->x, qlCurrentTypePlaces[i]->y, qlPointLabels, qlCurrentTypePlaces[i]->name, QColor(249,199,65));
				found=true;
			}
			
		} else { // match beginning of word / contains
			
			if(qlCurrentTypePlaces[i]->name.contains(text,Qt::CaseInsensitive)) { // contains
				
				if(m==2 || (qlCurrentTypePlaces[i]->name.indexOf(text,0,Qt::CaseInsensitive)==0)) { // contains or very beginning
					vDrawPoint(qlCurrentTypePlaces[i]->x, qlCurrentTypePlaces[i]->y, qlPointLabels, qlCurrentTypePlaces[i]->name, QColor(249,199,65));
					vDrawDebugPlace(i);
					found=true;
					
				// if the character before the matching phrase matches [ -_/()]
				} else {
					
					qDebug() << QString(qlCurrentTypePlaces[i]->name[qlCurrentTypePlaces[i]->name.indexOf(text,0,Qt::CaseInsensitive)-1]) << QString(qlCurrentTypePlaces[i]->name[qlCurrentTypePlaces[i]->name.indexOf(text,0,Qt::CaseInsensitive)-1]).replace(QRegExp("[ _/()]"), "}").replace("-","}") << qlCurrentTypePlaces[i]->name;
					
					if(qlCurrentTypePlaces[i]->name.contains(QRegExp(QString("[ _/()]%1").arg(text),Qt::CaseInsensitive)) ||
					   qlCurrentTypePlaces[i]->name.contains(QString("-%1").arg(text),Qt::CaseInsensitive) ) {
//					if(QString(qlCurrentTypePlaces[i]->name[qlCurrentTypePlaces[i]->name.indexOf(text,0,Qt::CaseInsensitive)-1]).replace(QRegExp("[ _/()]"), "}").replace("-","}")=="}") {
						
						vDrawPoint(qlCurrentTypePlaces[i]->x, qlCurrentTypePlaces[i]->y, qlPointLabels, qlCurrentTypePlaces[i]->name, QColor(249,199,65));
						found=true;
						
					}
					
				}
				
			}
			
		}
	}
	
	// if there's no match
	for(int l=0; !found && l<4; l++) {
		QString textl=qsSimplifyString(text,l);
		
		for(int i=0; i<qlCurrentTypePlaces.count(); i++) {
			if(qsSimplifyString(qlCurrentTypePlaces[i]->name,l).contains(textl)) {
				vDrawPoint(qlCurrentTypePlaces[i]->x, qlCurrentTypePlaces[i]->y, qlPointLabels, qlCurrentTypePlaces[i]->name, QColor(249,199,65,255-51*(l+1)));
				found=true;
			}
		}
	}

	lineEdit->setStyleSheet(found ? "" : "color:red");
	
	lblMouseClickOverlay->raise();
}
//#include <QPropertyAnimation>
//#include <QGraphicsOpacityEffect>
void dart::vGiveHint() {
	if(qlPlacesHistory.size()<1 || iPlaceCount==0) return;
	
	if(iAskForMode==enPositions) {
		QRectangleLabel *lblHint;
		lblHint=new QRectangleLabel(this,qlPlacesHistory[iPlaceCount-1]->x,qlPlacesHistory[iPlaceCount-1]->y,this);
		bGaveHint=true;
		
		//TODO enable this; delete opacityEffect
//		QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
//		opacityEffect->setOpacity(0);
//		lblHint->setGraphicsEffect(opacityEffect);
//		QPropertyAnimation* anim = new QPropertyAnimation(this);
//		anim->setTargetObject(opacityEffect);
//		anim->setPropertyName("opacity");
//		anim->setDuration(700);
//		anim->setStartValue(opacityEffect->opacity());
//		anim->setEndValue(1);
//		anim->setEasingCurve(QEasingCurve::InQuad);
//		anim->start();
//		mySleep(1000);
//		anim->setStartValue(opacityEffect->opacity());
//		anim->setEndValue(0);
//		anim->setEasingCurve(QEasingCurve::OutQuad);
//		anim->start(QAbstractAnimation::DeleteWhenStopped);
		mySleep(1000);
		delete lblHint;
	} else if(iAskForMode==enNames) {
		lineEdit->setText(qlPlacesHistory[iPlaceCount-1]->name.left(1));
		bGaveHint=true;
	} else {
		qDebug() << "[i] no hints available in mode" << iAskForMode;
	}
}

void dart::vAddMap() {
	//: the translated wiki pages are called DE:Maps etc. Please do NOT translate when there is no such wiki page in your language
	vOpenLinkInBrowser(QUrl(tr("https://github.com/gulp21/QeoDart/wiki/Maps")));
}

void dart::vReportBug() {
	//: the translated wiki pages are called DE:Contribute etc. Please do NOT translate when there is no such wiki page in your language
	vOpenLinkInBrowser(QUrl(tr("https://github.com/gulp21/QeoDart/wiki/Contribute")));
}

void dart::vOpenLinkInBrowser(QUrl link) {
	if(!QDesktopServices::openUrl(link)) {
		QMessageBox msgBox;
		msgBox.setText(tr("The default browser could not be opened."));
		msgBox.setInformativeText(tr("The following URL could not be opened: %1").arg(link.toString()));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}

bool dart::bStartNewGameWarning(QString title, QString message) {
	QMessageBox msgBox;
	msgBox.setWindowTitle(title);
	msgBox.setText(message);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	if(msgBox.exec()==QMessageBox::Yes) {
		return true;
	}
	return false;
}

void dart::vShowHelp() {
	vOpenLinkInBrowser(QUrl("https://github.com/gulp21/QeoDart/wiki"));
}

void dart::vActivateContextHelp() {
	QWhatsThis::enterWhatsThisMode();
}
