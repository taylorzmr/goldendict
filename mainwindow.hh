/* This file is (c) 2008-2010 Konstantin Isakov <ikm@users.berlios.de>
 * Part of GoldenDict. Licensed under GPLv3 or later, see the LICENSE file */

#ifndef __MAINWINDOW_HH_INCLUDED__
#define __MAINWINDOW_HH_INCLUDED__

#include <QMainWindow>
#include <QThread>
#include <QToolButton>
#include <QSystemTrayIcon>
#include <QNetworkAccessManager>
#include "ui_mainwindow.h"
#include "folding.hh"
#include "config.hh"
#include "dictionary.hh"
#include "article_netmgr.hh"
#include "instances.hh"
#include "article_maker.hh"
#include "scanpopup.hh"
#include "articleview.hh"
#include "wordfinder.hh"
#include "hotkeywrapper.hh"
#include "dictionarybar.hh"
#include "history.hh"
#include "hotkeywrapper.hh"

using std::string;
using std::vector;

class MainWindow: public QMainWindow, public DataCommitter
{
  Q_OBJECT

public:

  MainWindow( Config::Class & cfg );
  ~MainWindow();

  virtual void commitData( QSessionManager & );

private:

  void commitData();
  bool commitDataCompleted;

  QSystemTrayIcon * trayIcon;

  Ui::MainWindow ui;

  /// This widget is used as a title bar for the searchPane dock, and
  /// incorporates the next three objects inside
  QWidget searchPaneTitleBar;
  QHBoxLayout searchPaneTitleBarLayout;
  QLabel groupLabel;
  GroupComboBox groupList;

  QAction escAction, focusTranslateLineAction, addTabAction, closeCurrentTabAction,
          switchToNextTabAction, switchToPrevTabAction, showDictBarNamesAction;
  QToolBar * navToolbar;
  QAction * navBack, * navForward, * navPronounce, * enableScanPopup;
  QAction * zoomIn, * zoomOut, * zoomBase;
  QMenu trayIconMenu;
  QToolButton addTab;
  Config::Class & cfg;
  Config::Events configEvents;
  History history;
  DictionaryBar dictionaryBar;
  vector< sptr< Dictionary::Class > > dictionaries;
  /// Here we store unmuted dictionaries when the dictionary bar is active
  vector< sptr< Dictionary::Class > > dictionariesUnmuted;
  Instances::Groups groupInstances;
  ArticleMaker articleMaker;
  ArticleNetworkAccessManager articleNetMgr;
  QNetworkAccessManager dictNetMgr; // We give dictionaries a separate manager,
                                    // since their requests can be destroyed
                                    // in a separate thread

  WordFinder wordFinder;

  sptr< ScanPopup > scanPopup;

  sptr< HotkeyWrapper > hotkeyWrapper;

  QTimer newReleaseCheckTimer; // Countdown to a check for the new program
                               // release, if enabled
  sptr< QNetworkReply > latestReleaseReply;

  sptr< QPrinter > printer; // The printer we use for all printing operations
  
  QPrinter & getPrinter(); // Creates a printer if it's not there and returns it

  /// Applies the qt's stylesheet, given the style's name.
  void applyQtStyleSheet( QString const & displayStyle );

  /// Creates, destroys or otherwise updates tray icon, according to the
  /// current configuration and situation.
  void updateTrayIcon();

  void closeEvent( QCloseEvent * );

  void applyProxySettings();
  void makeDictionaries();
  void updateStatusLine();
  void updateGroupList();
  void updateDictionaryBar();
  void makeScanPopup();

  void updateMatchResults( bool finished );

  void updatePronounceAvailability();

  /// Updates word search request and active article view in response to
  /// muting or unmuting dictionaries, or showing/hiding dictionary bar.
  void applyMutedDictionariesState();

  virtual bool eventFilter( QObject *, QEvent * );

  /// Returns the reference to dictionaries stored in the currently active
  /// group, or to all dictionaries if there are no groups.
  vector< sptr< Dictionary::Class > > const & getActiveDicts();

  /// Brings the main window to front if it's not currently, or hides it
  /// otherwise. The hiding part is omitted if onlyShow is true.
  void toggleMainWindow( bool onlyShow = false );

  /// Creates hotkeyWrapper and hooks the currently set keys for it
  void installHotKeys();

  void applyZoomFactor();

  void mousePressEvent ( QMouseEvent * event );

private slots:

  void hotKeyActivated( int );

  /// If new release checks are on, santizies the next check time and starts
  /// the timer. Does nothing otherwise.
  void prepareNewReleaseChecks();

private slots:

  /// Does the new release check.
  void checkForNewRelease();

  /// Signalled when the lastestReleaseReply is finished()
  void latestReleaseReplyReady();

private slots:

  // Executed in response to a user click on an 'add tab' tool button
  void addNewTab();
  // Executed in response to a user click on an 'close' button on a tab
  void tabCloseRequested( int );
  // Closes current tab.
  void closeCurrentTab();
  void switchToNextTab();
  void switchToPrevTab();

  /// Triggered by the actions in the nav toolbar
  void backClicked();
  void forwardClicked();

  /// ArticleView's title has changed
  void titleChanged( ArticleView *, QString const & );
  /// ArticleView's icon has changed
  void iconChanged( ArticleView *, QIcon const & );

  void pageLoaded( ArticleView * );
  void tabSwitched( int );

  void dictionaryBarToggled( bool checked );

  /// Pronounces the currently displayed word by playing its first audio
  /// reference, if it has any.
  /// If view is 0, the operation is done for the currently open tab.
  void pronounce( ArticleView * view = 0 );

  void zoomin();
  void zoomout();
  void unzoom();

  /// If editDictionaryGroup is specified, the dialog positions on that group
  /// initially.
  void editDictionaries( unsigned editDictionaryGroup = Instances::Group::NoGroupId );
  /// Edits current group when triggered from the dictionary bar.
  void editCurrentGroup();
  void editPreferences();

  void currentGroupChanged( QString const & );
  void translateInputChanged( QString const & );
  void translateInputFinished();

  /// Closes any opened search in the article view, and focuses the translateLine.
  void handleEsc();

  /// Gives the keyboard focus to the translateLine and selects all the text
  /// it has.
  void focusTranslateLine();

  void prefixMatchUpdated();
  void prefixMatchFinished();

  void wordListItemActivated( QListWidgetItem * );
  void wordListSelectionChanged();

  /// Creates a new tab, which is to be populated then with some content.
  ArticleView * createNewTab( bool switchToIt,
                              QString const & name );

  void openLinkInNewTab( QUrl const &, QUrl const &, QString const &,
                         ArticleView::Contexts const & contexts );
  void showDefinitionInNewTab( QString const & word, unsigned group,
                               QString const & fromArticle,
                               ArticleView::Contexts const & contexts );
  void typingEvent( QString const & );

  void mutedDictionariesChanged();

  void showTranslationFor( QString const &, unsigned inGroup = 0 );

  void trayIconActivated( QSystemTrayIcon::ActivationReason );

  void scanEnableToggled( bool );

  void setAutostart( bool );

  void showMainWindow();

  void visitHomepage();
  void visitForum();
  void showAbout();

  void showDictBarNamesTriggered();

  void historyChanged();
  void menuHistoryTriggered( QAction * );
  void on_clearHistory_activated();

  void on_actionCloseToTray_activated();
  
  void on_pageSetup_activated();
  void on_printPreview_activated();
  void on_print_activated();
  void printPreviewPaintRequested( QPrinter * );
  
  void on_saveArticle_activated();

  void on_rescanFiles_activated();
};

#endif