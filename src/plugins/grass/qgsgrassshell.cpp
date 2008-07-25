/***************************************************************************
     qgsgrassshell.cpp
     --------------------------------------
    Date                 : Sun Sep 16 12:06:10 AKDT 2007
    Copyright            : (C) 2007 by Radim Blazek 
    Revised and ported to Qt4: Tim Sutton 2008
    Email                : blazek at itc.it
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <iostream>
#include <vector>

#include <QString>
#include <QApplication>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QStringList>
#include <QSocketNotifier>
#include <QTcpSocket>
#include <QEvent>
#include <QTextBrowser>
#include <QTextBlock>
#include <QTextDocument>
#include <QRegExp>
#include <QCursor>
#include <QLayout>
#include <QClipboard>
#include <QFontMetrics>
#include <QFileInfo>
#include <QProgressBar>

#include "qgsapplication.h"
#include "qgsgrassshell.h"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#endif

#ifndef WIN32
#ifdef Q_OS_MACX
#include <util.h>
#else
#ifdef __NetBSD__
#include <util.h>
#else
#ifdef __FreeBSD__
#include <termios.h>
#include <libutil.h>
#else
#include <pty.h>
#endif
#endif
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#endif //!WIN32
}

QgsGrassShell::QgsGrassShell ( QWidget * parent, const char * name  ):
  QTextEdit(parent)
{
  setName("GrassShell");
  mValid = false;
  mSkipLines = 2;

#ifdef WIN32
  QMessageBox::warning( 0, "Warning", 
    "GRASS Shell is not supported on Windows." );
  return;
#else 

  //get focus from both mouse and keyboard
  setFocusPolicy(Qt::StrongFocus);
  setTabChangesFocus ( false );
  setReadOnly ( false );
  setWordWrapMode ( QTextOption::WrapAnywhere );

    // Set tab stops ???
  mTabStop.resize(200);
  for ( int i = 0 ; i * 8 < (int)mTabStop.size(); i++ )
  {
    mTabStop[i*8] = true;
  }
  
  mpProgressBar = new QProgressBar(this);
  mpProgressBar->hide();
  setFont(QFont ( "Courier", 10 ));

 

  //setFocusPolicy ( QWidget::NoFocus ); // To get key press directly
  append (tr( "GRASS shell ready...") ); 
#ifndef HAVE_OPENPTY
  append (tr( "GRASS shell is not supported") ); 
  return;
#endif


  mBlockNo = -1; // first will be 0

  mNewLine = true;

  for ( int i = 0; i < ModeCount; i++ )
  {
    resetMode(i);
  }

  int uid;
  seteuid( uid=getuid() );	/* Run unprivileged */

  // Get and open pseudo terminal 
  // Note: 0 (stdin), 1 (stdout) or 2 (stderr)
  int fdSlave; // slave file descriptor
  seteuid(0);
  int ret =  openpty ( &mFdMaster, &fdSlave, NULL, NULL, NULL );
  if ( ret != 0 )
  {
    QMessageBox::warning( 0, "Warning", "Cannot open pseudo terminal" );
    return;
  }
  fchown( fdSlave, uid, (gid_t)-1);
  fchmod( fdSlave, S_IRUSR | S_IWUSR);
  seteuid(uid);

#ifdef QGISDEBUG
  std::cerr << "mFdMaster = " << mFdMaster << std::endl;
  std::cerr << "fdSlave = " << fdSlave << std::endl;
#endif

  fcntl( mFdMaster, F_SETFL, O_NDELAY); 
  //fcntl( fdSlave, F_SETFL, O_NDELAY); // enable? 

  QString slaveName = ttyname(fdSlave);
#ifdef QGISDEBUG
  std::cerr << "master ttyname = " << ttyname(mFdMaster)  << std::endl;
  std::cerr << "slave ttyname = " << ttyname(fdSlave)  << std::endl;
#endif

  //close ( fdSlave ); // -> crash  

  // Fork slave and open shell 
  int pid = fork();
#ifdef QGISDEBUG
  std::cerr << "pid = " << pid << std::endl;
#endif
  if ( pid == -1 )
  {
    QMessageBox::warning( 0, tr("Warning"), tr("Cannot fork shell") );
    return;
  }

  // Child - slave
  if ( pid == 0 ) 
  {
#ifdef QGISDEBUG
    std::cerr << "child ->" << std::endl;
#endif

    // TODO close all opened file descriptors - close(0)???
#ifndef Q_OS_DARWIN
    // Makes child process unusable on Mac
    close ( mFdMaster );
#endif

    //close ( fdSlave ); // -> freeze  

    setsid();
    seteuid(0);

    int fd = open ( (char*) slaveName.ascii(), O_RDWR);
    if ( fd < 0 ) 
    {
      QMessageBox::warning( 0, tr("Warning"), tr("Cannot open slave file "
        "in child process") );
      return;
    }

    fchown(fd, uid, (gid_t)-1);
    fchmod(fd, S_IRUSR | S_IWUSR);
    setuid(uid);

    dup2 (fd, 0);  /* stdin */
    dup2 (fd, 1);  /* stdout */
    dup2 (fd, 2);  /* stderr */

    // TODO: test if shell is available
    QString shell = ( getenv("SHELL") );
    if ( shell.isEmpty() )
    {
      shell = "/bin/bash";
    }

    const char *norc = "";
    QFileInfo si(shell);
    if ( si.fileName() ==  "bash" || si.fileName() ==  "sh" )
    { 
      norc = "--norc";
    }
    else if ( si.fileName() ==  "tcsh" || si.fileName() ==  "csh" )
    { 
      norc = "-f";
    }

    // Warning: execle + --norc will not inherit not given variables
    // -> overwrite here
    const char *env = "GRASS_MESSAGE_FORMAT=gui";
    char *envstr = new char[strlen(env)+1];
    strcpy ( envstr, env );
    putenv( envstr );

    putenv ( (char *) "GISRC_MODE_MEMORY" );  // unset

    env = "PS1=GRASS > ";
    envstr = new char[strlen(env)+1];
    strcpy ( envstr, env );
    putenv( envstr );

    env = "TERM=vt100";
    envstr = new char[strlen(env)+1];
    strcpy ( envstr, env );
    putenv( envstr );

    //char *envar[] = { "PS1=GRASS > ", "TERM=vt100", "GISRC_MODE_MEMORY=", 
    //                  "GRASS_MESSAGE_FORMAT=gui", (char *)0 };

    //execle ( (char*)shell.ascii(), (char *)si.fileName().ascii(), 
    //         norc, (char *) 0, envar);
    execl ( (char*)shell.ascii(), (char *)si.fileName().ascii(), 
      norc, (char *) 0);

    // Failed (QMessageBox here does not work)
    fprintf ( stderr, "GRASS_INFO_ERROR(1,1): Cannot start shell %s\n", 
      (char*)shell.ascii() );
    exit(1);
  }

  mPid = pid;

  // Create socket notifier
  mOutNotifier = new QSocketNotifier ( mFdMaster, QSocketNotifier::Read, this);

  QObject::connect ( mOutNotifier, SIGNAL(activated(int)),
    this, SLOT(readStdout(int)));



  // Set trap to write history on SIGUSR1
  //QString trap = "trap 'history -w' SIGUSR1\015\012";
  QString trap = "trap 'history -w' SIGUSR1\015";
  write( mFdMaster, trap.ascii(), trap.length());

  resizeTerminal();
  mValid = true;
#endif // !WIN32
}

QgsGrassShell::~QgsGrassShell()
{
#ifdef QGISDEBUG
  std::cerr << "QgsGrassShell::~QgsGrassShell()" << std::endl;
#endif

#ifndef WIN32
  // This was old trick to write history
  /*
  write( mFdMaster, "exit\015\012", 6);
  while ( 1 ) 
  {
  readStdout(0);

  int status;
  if ( waitpid ( mPid, &status, WNOHANG ) > 0 ) break;

  struct timespec t, r;
  t.tv_sec = 0;
  t.tv_nsec = 10000000; // 0.01 s
  nanosleep ( &t, &r );
  }
  */

  // Write history
  if ( kill(mPid,SIGUSR1) == -1 )
  {
    std::cerr << "cannot write history (signal SIGUSR1 to pid = " << mPid << ")" << std::endl;
  }

  std::cerr << "kill shell pid = " << mPid << std::endl;
  if ( kill(mPid,SIGTERM ) == -1 )
  {
    std::cerr << "cannot kill shell pid = " << mPid << std::endl;
  }
#endif
}
  
void QgsGrassShell::focusInEvent ( QFocusEvent * event )
{
  //install an event filter to prevent other parts of the app
  //receiving keystrokes and e.g. invoking menu shortcuts
  qApp->installEventFilter(this);
  QTextEdit::focusInEvent(event);
}
  void QgsGrassShell::focusOutEvent ( QFocusEvent * event )
{
  qApp->removeEventFilter(this);
  QTextEdit::focusOutEvent(event);
}
bool QgsGrassShell::eventFilter(QObject *object, QEvent *event)
{
  //note object name must be wrapped in a QString because you can directly compare char[] with char[]
  if (QString(object->name()) == "GrassShell" && event->type() == QEvent::KeyPress)
  {
    if (event->type() != QEvent::KeyPress)
    {
      qDebug("Keypress event NOT accepted: ");
      return false;
    }
    qDebug("Keypress event accepted: ");
    QKeyEvent * mypKeyEvent = static_cast<QKeyEvent *>(event);
    keyPressEvent(mypKeyEvent);
    return true; //stop further processing by other widgets
  }
  else
  {
    return false;
  }
}

void QgsGrassShell::keyPressEvent( QKeyEvent * e  )
{
#ifdef QGISDEBUG
  std::cerr << "QgsGrassShell::keyPressEvent()" << std::endl;
#endif

  char s[10];
  int length = 0;
  int ret = 0;

  if ( !mValid ) return;

  mpProgressBar->setValue ( 0 );

  char c = (char) e->ascii();
#ifdef QGISDEBUG
  std::cerr << "c = " << (int)c << " key = " << e->key() 
    << " text = " << e->text().local8Bit().data() << std::endl;
#endif
  s[0] = c;
  length = 1;

  // Set key down 
  if ( e->key() == Qt::Key_Control ) mKeyDown[DownControl] = true;
  else if ( e->key() == Qt::Key_Shift ) mKeyDown[DownShift] = true;
  else if ( e->key() == Qt::Key_Alt ) mKeyDown[DownAlt] = true;
  else if ( e->key() == Qt::Key_Meta ) mKeyDown[DownMeta] = true;

  if ( c == 0 )
  {
    switch ( e->key() )
    {
    case Qt::Key_Up :
      strcpy ( s, "\033[A" );
      length = 3;
      break;

    case Qt::Key_Down :
      strcpy ( s, "\033[B" );
      length = 3;
      break;

    case Qt::Key_Right :
      strcpy ( s, "\033[C" );
      length = 3;
      break;

    case Qt::Key_Left :
      strcpy ( s, "\033[D" );
      length = 3;
      break;
    }
  }

  //pass the key press on to the text edit parent (results in double chars echoed to textedit
  //QTextEdit::keyPressEvent(  e  );
  ret = write( mFdMaster, s, length);
#ifdef QGISDEBUG
  std::cerr << "write ret = " << ret << std::endl;
#endif

}

void QgsGrassShell::keyReleaseEvent( QKeyEvent * e  )
{
#ifdef QGISDEBUG
  //    std::cerr << "QgsGrassShell::keyReleaseEvent()" << std::endl;
#endif

  // Reset key down 
  if ( e->key() == Qt::Key_Control ) mKeyDown[DownControl] = false;
  else if ( e->key() == Qt::Key_Shift ) mKeyDown[DownShift] = false;
  else if ( e->key() == Qt::Key_Alt ) mKeyDown[DownAlt] = false;
  else if ( e->key() == Qt::Key_Meta ) mKeyDown[DownMeta] = false;

}

void QgsGrassShell::readStdout( int socket )
{
#ifdef QGISDEBUG
  //std::cerr << "QgsGrassShell::readStdout()" << std::endl;
#endif

  char buf[4097];
  int len; 
  while ( (len = read( mFdMaster, buf, 4096)) > 0 )
  {	
    // Terminate string
    buf[len] = '\0';

    mStdoutBuffer.append ( buf );
  }

  printStdout();
}

void QgsGrassShell::printStdout()
{
  // Debug
#ifdef QGISDEBUG
  std::cerr << "****** buffer ******" << std::endl;
  std::cerr << "-->";
  for ( int i = 0; i < (int)mStdoutBuffer.length(); i++ )
  {
    int myCharInt = mStdoutBuffer[i];
    QString myString = "";
    if ( myCharInt > '\037' && myCharInt != '\177' ) // myControlChar characters
    {
      myString = (char) myCharInt;
      std::cerr << myString.local8Bit().data();
    }
    else
    {
      std::cerr << "(myCharInt=" << QString::number(myCharInt,8).local8Bit().data() << ")";
    }
  }
  std::cerr << "<--" << std::endl;
#endif

  eraseCursor();
  // To make it faster we want to print maximum length blocks from buffer
  while ( mStdoutBuffer.length() > 0 ) 
  {
#ifdef QGISDEBUG
    std::cerr << "------ cycle ------" << std::endl;
#endif

    // Search myControlChar character
    int myControlChar = -1; 
    for ( int i = 0; i < (int)mStdoutBuffer.length(); i++ )
    {
      int myCharInt = mStdoutBuffer[i];
      if ( myCharInt < '\037' || myCharInt == '\177' )
      {
        myControlChar = i;
        break;
      }
    }
#ifdef QGISDEBUG
    std::cerr << "myControlChar = " << myControlChar << std::endl;
#endif

    // Process myControlChar character if myFoundFlag at index 0
    if ( myControlChar == 0 ) 
    {
      int myCharInt = mStdoutBuffer[0];
#ifdef QGISDEBUG
      std::cerr << "myCharInt = " << QString::number(myCharInt,8).local8Bit().data() << std::endl;
#endif

      // myControlChar sequence
      if ( myCharInt == '\033' )
      {
        //std::cerr << "myControlChar sequence" << std::endl;

        bool myFoundFlag = false;

        // It is sequence, so it should be at least one more character
        // wait for more data 
        if ( mStdoutBuffer.length() < 2 ) break;
        if ( mStdoutBuffer[1] == ']' && mStdoutBuffer.length() < 3 ) break;

        // ESC ] Ps ; Pt BEL    (xterm title hack)
        QRegExp myRegExp ( "\\](\\d+);([^\\a]+)\\a" ); 
        if ( myRegExp.search ( mStdoutBuffer, 1 ) == 1 ) 
        {
          int mlen = myRegExp.matchedLength();
#ifdef QGISDEBUG
          std::cerr << "ESC(set title): " << myRegExp.cap(2).local8Bit().data() << std::endl;
#endif
          mStdoutBuffer.remove ( 0, mlen+1 );
          myFoundFlag = true;
        }

        if ( !myFoundFlag ) 
        {
          //    ESC [ Pn ; Pn FINAL
          // or ESC [ = Pn ; Pn FINAL
          // or ESC [ = Pn ; Pn FINAL
          // TODO: QRegExp captures only last of repeated patterns 
          //       ( ; separated nums - (;\\d+)* )
          myRegExp.setPattern ( "\\[([?=])*(\\d+)*(;\\d+)*([A-z])" ); 
          if ( myRegExp.search ( mStdoutBuffer, 1 ) == 1 ) 
          {
            int mlen = myRegExp.matchedLength();
            char final = myRegExp.cap(4).at(0).latin1();

            std::cerr << "final = " << final << std::endl;
            //std::cerr << "ESC: " << myRegExp.cap(0) << std::endl;

            switch ( final )
            {
            case 'l' : // RM - Reset Mode
            case 'h' : // SM - Set Mode
              {
                int mode = -1;
                switch ( myRegExp.cap(2).toInt() )
                {
                case 4 :
                  mode = Insert;
                  break;

                default:
                  std::cerr << "ESC ignored: " << myRegExp.cap(0).local8Bit().data() << std::endl;
                  break;
                }
                if ( mode >= 0 )
                {
                  if ( final == 'l' )
                    resetMode ( mode );
                  else
                    setMode ( mode );
                }
                break;
              }

            case 'm' : // SGR - Select Graphic Rendition
              if ( myRegExp.cap(2).isEmpty() || myRegExp.cap(2).toInt() == 0 )
              {
                for ( int i = 0; i < RenditionCount; i++ )
                {
                  mRendition[i] = false;
                }
              }
              else
              {
                std::cerr << "ESC SGR ignored: " << myRegExp.cap(0).local8Bit().data() << std::endl;
              }
              break;

            case 'P' : // DCH - Delete Character
              {
                int n = myRegExp.cap(2).toInt();
                textCursor().deleteChar();
                break;
              }

            case 'K' : // EL - Erase In Line
              if ( myRegExp.cap(2).isEmpty() || myRegExp.cap(2).toInt() == 0 )
              {
                //setSelectionAttributes ( 1, QColor(255,255,255), true );  
                textCursor().select( QTextCursor::LineUnderCursor );
                textCursor().removeSelectedText();    
              }
              break;

              // TODO: multiple tab stops
            case 'H' : // Horizontal Tabulation Set (HTS)
              //@TODO this should be using position in the line rather than the document I think [TS]
              mTabStop[ textCursor().position()] = true;
              std::cerr << "TAB set on " << textCursor().position() << std::endl;
              break;

            case 'g' : // Tabulation Clear (TBC)
              // ESC [ g 	Clears tab stop at the cursor
              // ESC [ 2 g 	Clears all tab stops in the line
              // ESC [ 3 g 	Clears all tab stops in the Page
              std::cerr << "TAB reset" << std::endl;
              if ( myRegExp.cap(2).isEmpty() || myRegExp.cap(2).toInt() == 0 )
              {
                //@TODO this should be using position in the line rather than the document I think [TS]
                mTabStop[textCursor().position()] = false;
              } 
              else
              {
                for (int i = 0; i < (int)mTabStop.size(); i++ ) 
                  //@TODO this should be using position in the line rather than the document I think [TS]
                  mTabStop[textCursor().position()] = false;
              }
              break;

            default:
              std::cerr << "ESC ignored: " << myRegExp.cap(0).local8Bit().data() << std::endl;
              break;
            }

            mStdoutBuffer.remove ( 0, mlen+1 );
            myFoundFlag = true;
          }
        }

        if ( !myFoundFlag ) 
        {
          // ESC # DIGIT
          myRegExp.setPattern ( "#(\\d)" ); 
          if ( myRegExp.search ( mStdoutBuffer, 1 ) == 1 )
          {
            std::cerr << "ESC ignored: " << myRegExp.cap(0).local8Bit().data() << std::endl;
            mStdoutBuffer.remove ( 0, 3 );
            myFoundFlag = true;
          }
        }

        if ( !myFoundFlag ) 
        {
          // ESC CHARACTER
          myRegExp.setPattern ( "[A-z<>=]" ); 
          if ( myRegExp.search ( mStdoutBuffer, 1 ) == 1 )
          {
            std::cerr << "ESC ignored: " << myRegExp.cap(0).local8Bit().data() << std::endl;
            mStdoutBuffer.remove ( 0, 2 );
            myFoundFlag = true;
          }
        }

        // TODO: it can happen that the sequence is not complete ->
        //       no match -> how to distinguish unknown sequence from
        //       missing characters
        if ( !myFoundFlag ) 
        {
          // For now move forward
          std::cerr << "UNKNOWN ESC ignored: " << mStdoutBuffer.mid(1,5).data() << std::endl;
          mStdoutBuffer.remove ( 0, 1 );
        }
      }
      else
      {
        // myControlChar character
        switch ( myCharInt ) 
        {
        case '\015' : // CR
          //std::cerr << "CR" << std::endl;
          mStdoutBuffer.remove ( 0, 1 );
          // TODO : back tab stops?
          break;

        case '\012' : // NL
          //std::cerr << "NL" << std::endl;
          newLine();
          mStdoutBuffer.remove ( 0, 1 );
          break;

        case '\010' : // BS 
          //std::cerr << "BS" << std::endl;
          //we must remove the selection first because in a shell backspace can only delete a 
          //single char
          textCursor().clearSelection ();
          textCursor().deletePreviousChar ();
          mStdoutBuffer.remove ( 0, 1 );
          break;

        case '\011' : // HT (tabulator)
          {
            //std::cerr << "HT" << std::endl;
            QString space;
            //@TODO this should be using position in the line rather than the document I think [TS]
            for ( int i = textCursor().position(); i < (int)mTabStop.size(); i++ )
            {
              space.append ( " " );
              if ( mTabStop[i] ) break;
            }
            insert (space);
            mStdoutBuffer.remove ( 0, 1 );
            break;
          }

        case '>' : // Keypad Numeric Mode 
          std::cerr << "Keypad Numeric Mode ignored: " 
              << QString::number(myCharInt,8).local8Bit().data() << std::endl;
          mStdoutBuffer.remove ( 0, 2 );
          break;

        default : // unknown myControlChar, do nothing
          std::cerr << "UNKNOWN myControlChar char ignored: " << QString::number(myCharInt,8).local8Bit().data() << std::endl;
          mStdoutBuffer.remove ( 0, 1 );
          break;
        }
      }
      continue;
    }

    // GRASS messages. GRASS messages start with GRASS_INFO_
    // and stop with \015\012 (\n)

    // first info
    QRegExp myRegExpinfo ( "GRASS_INFO_" );
    int message = myRegExpinfo.search(mStdoutBuffer);

    if ( message == 0 ) // Info myFoundFlag at index 0
    { 
      // First try percent
      QRegExp myRegExpPercent ( "GRASS_INFO_PERCENT: (\\d+)\\015\\012" );
      if ( myRegExpPercent.search(mStdoutBuffer) == 0 ) {
        int mlen = myRegExpPercent.matchedLength();
        int progress = myRegExpPercent.cap(1).toInt();
        mpProgressBar->setValue ( progress  );
        mStdoutBuffer.remove ( 0, mlen );
        continue;
      }

      QRegExp myRegExpwarning ( "GRASS_INFO_WARNING\\(\\d+,\\d+\\): ([^\\015]*)\\015\\012" );
      QRegExp myRegExperror ( "GRASS_INFO_ERROR\\(\\d+,\\d+\\): ([^\\015]*)\\015\\012" );
      QRegExp myRegExpend ( "GRASS_INFO_END\\(\\d+,\\d+\\)\\015\\012" );

      int mlen = 0; 
      QString msg;
      QString img;
      if ( myRegExpwarning.search(mStdoutBuffer) == 0 ) 
      {
        mlen = myRegExpwarning.matchedLength();
        msg = myRegExpwarning.cap(1);
        img = ":/grass/warning.png";
      }
      else if ( myRegExperror.search(mStdoutBuffer) == 0 ) 
      {
        mlen = myRegExperror.matchedLength();
        msg = myRegExperror.cap(1);
        img =  ":/grass/error.png";
      }

      if ( mlen > 0 ) // myFoundFlag error or warning
      {
#ifdef QGISDEBUG
        std::cerr << "MSG: " << msg.local8Bit().data() << std::endl;
#endif

        // Delete all previous empty paragraphs.
        // Messages starts with \n (\015\012) which is previously interpreted 
        // as new line, so it is OK delete it, but it is not quite correct
        // to delete more  because it can be regular module output
        // but it does not look nice to have empty rows before 
        removeEmptyParagraphs();

        textCursor().insertImage(img);
        insertPlainText ( msg );
        
        mBlockNo++;
        mNewLine = true;
        mStdoutBuffer.remove ( 0, mlen );
        continue;
      } 

      if ( myRegExpend.search(mStdoutBuffer) == 0 ) 
      {
        mlen = myRegExpend.matchedLength();
        mStdoutBuffer.remove ( 0, mlen );
        continue;
      }

      // No complete message myFoundFlag => wait for input 
      // TODO: 1) Sleep for a moment because GRASS writes 
      //          1 character in loop
      //       2) Fix GRASS to write longer strings
      break;
    }

    // Print plain text
    int length = mStdoutBuffer.length();
    if ( myControlChar >= 0 ) length = myControlChar;
    if ( message >= 0 && (myControlChar == -1 || myControlChar > message ) )
    {
      length = message;
    }

    if ( length > 0 ) 
    {
      QString out = mStdoutBuffer.left ( length ) ;
#ifdef QGISDEBUG
      std::cerr << "TXT: '" << out.local8Bit().data()<< "'" << std::endl;
#endif

      insert ( out );

      mStdoutBuffer.remove ( 0, length );
    }
  }


  ensureCursorVisible();
}

void QgsGrassShell::removeEmptyParagraphs()
{
  QTextDocument * mypDocument = document();
  QTextCursor myEndCursor(mypDocument);
  textCursor().setPosition(QTextCursor::Start); //start of document
  myEndCursor.setPosition(QTextCursor::End);//end of document
  while (textCursor().position() != myEndCursor.position())
  {
     textCursor().select(QTextCursor::BlockUnderCursor);
     textCursor().removeSelectedText();
     textCursor().setPosition(QTextCursor::NextBlock);//start of next block
     myEndCursor.setPosition(QTextCursor::End);//end of document
  }
  textCursor().setPosition(QTextCursor::End);
}

void QgsGrassShell::insert ( QString theString )
{
#ifdef QGISDEBUG
  std::cerr << "insert()" << std::endl;
#endif

  if ( theString.isEmpty() ) return; 

  // In theory mBlockNo == paragraphs()-1
  // but if something goes wrong (more paragraphs) we want to write 
  // at the end
  if ( mBlockNo > -1 && mBlockNo != document()->blockCount()-1 )
  {
    std::cerr << "WRONG mBlockNo!" << std::endl;
    mNewLine = true;
  }

  // Bug?: QTextEdit::setOverwriteMode does not work, always 'insert'
  //       -> if Insert mode is not set, delete first the string 
  //          to the right
  // setOverwriteMode ( !mMode[Insert] ); // does not work
  if ( !mMode[Insert] && !mNewLine && mBlockNo >= 0  ) 
  {
#ifdef QGISDEBUG
    std::cerr << "erase old " << textCursor().position() + theString.length() << " chars "  << std::endl;
#endif
    
    textCursor().clearSelection();
    textCursor().setPosition ( textCursor().position(), QTextCursor::MoveAnchor );
    //If the anchor() is kept where it is and the position() is moved, the text in between will be selected.
    textCursor().setPosition (  textCursor().position() + theString.length(), QTextCursor::KeepAnchor );
    textCursor().removeSelectedText ( );
  }

  if ( mNewLine )
  {
    // Start new paragraph
    append ( theString );
    //mBlockNo++;
    mBlockNo = document()->blockCount()-1;
    mNewLine = false;
  }
  else
  {
    insertPlainText ( theString );
    textCursor().setPosition(textCursor().position() + theString.length()); 
  }
}

void QgsGrassShell::newLine()
{
  if ( mSkipLines > 0 )
  {
    mSkipLines--;
  }
  if ( mNewLine ) 
  {
    append ( " " );
    //mBlockNo++; 
    // To be sure that we are at the end
    mBlockNo = document()->blockCount()-1;
  }
  mNewLine = true;
}

void QgsGrassShell::eraseCursor()
{
  // Remove space representing cursor from the end of current paragraph
  if ( !mNewLine && mCursorSpace && mBlockNo >= 0 ) 
  {
    textCursor().setPosition (  textCursor().position(), QTextCursor::MoveAnchor );
    //If the anchor() is kept where it is and the position() is moved, the text in between will be selected.
    textCursor().setPosition (  textCursor().position() + 1, QTextCursor::KeepAnchor );
    textCursor().removeSelectedText ( );
    
  }
  mCursorSpace = false;
}

/* Chuck this - we use the inbuilt QTextEdit cursor rather
void QgsGrassShell::showCursor()
{
  // Do not highlite cursor if last printed paragraph was GRASS message
  if ( mNewLine ) return;
  // If cursor is at the end of paragraph add space  
  if ( mBlockNo >= 0 && textCursor().atBlockEnd() )
  {
    textCursor().setPosition (  textCursor().position(), QTextCursor::MoveAnchor  );
    insert ( " " );
    // the space if after current position
    mCursorSpace = true;
  }

  // Highlight the cursor
  textCursor().setPosition (  textCursor().position(), QTextCursor::MoveAnchor );
    //If the anchor() is kept where it is and the position() is moved, the text in between will be selected.
  textCursor().setPosition (  textCursor().position() + 1, QTextCursor::KeepAnchor );
  //@todo work out how to do this in Qt4
  //setSelectionAttributes ( 1, QColor(0,0,0), true );  
}
*/

void QgsGrassShell::mousePressEvent(QMouseEvent* e)
{
#ifdef QGISDEBUG
  std::cerr << "mousePressEvent()" << std::endl;
#endif

  if ( !mValid ) return;

  //@TODO Add some logic to check click was in textedit
  // paste clipboard
  if ( e->button() == Qt::MidButton )
  {
    QClipboard *cb = QApplication::clipboard();
    QString text = cb->text(QClipboard::Selection);
    write( mFdMaster, (char*) text.ascii(), text.length() );
  }
  else
  {
    QTextEdit::mousePressEvent(e);
  }
}
void QgsGrassShell::resizeEvent(QResizeEvent *e)
{
#ifdef QGISDEBUG
  std::cerr << "resizeEvent()" << std::endl;
#endif
  resizeTerminal();
}
void QgsGrassShell::resizeTerminal()
{
#ifndef WIN32


  QFontMetrics fm ( font() );
  int col = (int) (width() / fm.width("x")); 
  int row = (int) (height() / fm.height()); 

  struct winsize winSize;
  memset(&winSize, 0, sizeof(winSize));
  winSize.ws_row = row;
  winSize.ws_col = col;

  ioctl( mFdMaster, TIOCSWINSZ, (char *)&winSize );
  setLineWrapMode(QTextEdit::FixedColumnWidth);
  setLineWrapColumnOrWidth(col);
#endif
}

void QgsGrassShell::readStderr()
{
}





