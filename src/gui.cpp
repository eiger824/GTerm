#include <iostream>
#include <unistd.h>
#include <QTextCursor>
#include "gui.hpp"

namespace gterm {
  GTerm::GTerm(QWidget* parent) : QWidget(parent),
				  m_token(">>"),
				  m_separator("\n\t\t@@@@@@@@@@@@@@@@@@@@@@\t\t"),
				  m_history_index(0),
				  m_pwd(""),
				  m_ctrl(false)
  {
    resize(700, 610);
    m_main_layout = new QVBoxLayout(this);
    m_main_layout->setAlignment(Qt::AlignCenter);
    m_output = new QTextEdit();
    m_output->setReadOnly(true);
    m_output->setFixedHeight(600);
    m_output->setStyleSheet("border: 2px solid black;");
    m_output->setAlignment(Qt::AlignLeft);
    m_output->clearFocus();
    m_command_line = new QTextEdit();
    resetPrompt();
    connect(m_command_line, SIGNAL(textChanged()), this, SLOT(textChangedSlot()));
    m_main_layout->addWidget(m_output);
    m_main_layout->addWidget(m_command_line);
    setTabOrder(m_command_line, m_output);
    setLayout(m_main_layout);
    show();
  }
  
  GTerm::~GTerm() {
    delete m_command_line;
    delete m_main_layout;
  }
  
  void GTerm::moveCursor() {
    QTextCursor tmp = m_command_line->textCursor();
    tmp.movePosition(QTextCursor::Right,
		     QTextCursor::MoveAnchor,
		     m_command_line->toPlainText().size());
    m_command_line->setTextCursor(tmp);
  }
  
  void GTerm::keyPressEvent(QKeyEvent* event) {
    if (event->key() == 70 && m_ctrl) { //'f', forward
      if (m_history_index < m_history.size()-1 && !m_history.isEmpty()) {
	++m_history_index;
	m_command_line->setText(m_pwd + m_history.at(m_history_index));
      }
    } else if (event->key() == 66 && m_ctrl) { //'b', backward
      if (m_history_index > 0 && !m_history.isEmpty()) {
	--m_history_index;
	m_command_line->setText(m_pwd + m_history.at(m_history_index));
      }
    } else if (event->key() == 16777249) m_ctrl = true;

    moveCursor();
  }

  void GTerm::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == 16777249) m_ctrl = false;
  }
  
  void GTerm::textChangedSlot() {
    QString text = m_command_line->toPlainText();
    if (text.at(text.size() - 1) == 10) { //enter
      QString command = text.mid(text.lastIndexOf(">")+1);
      command.chop(1);
      resetPrompt();
      std::cout << "Command to send: [" << command.toStdString() << "]\n";
      m_history << command;
      m_history_index = m_history.size() -1;
      //check if cd
      if (!command.contains("cd", Qt::CaseSensitive)) {
	if (command.contains("clear", Qt::CaseSensitive)) {
	  m_output->clear();
	} else if (command.contains("history", Qt::CaseSensitive)) {
	  m_output->append(m_history.join("\n") + m_separator);
	} else if (command.contains("exit", Qt::CaseSensitive)) {
	  this->close();
	} else {
	  std::cout << exec(command).toStdString() << std::endl;
	  m_output->append(exec(command)
			   + m_separator);
	}
      } else {
	int code = chdir(command.mid(3).toStdString().c_str());
	m_output->append("PWD now is " + exec("pwd")
			 + " (Return code was :" + QString::number(code) + ")"
			 + m_separator);
	resetPrompt();
      }
    } else if (text.size() == m_pwd.size()-1) { //the only case when the whole command is gone
      resetPrompt();
    } else if (text.at(text.size() - 1) == 9) { // tab character
      text.chop(1);
      m_command_line->setText(text);
      moveCursor();
      //do something
      if (text.at(text.size()-1) != 32 &&
	  text.at(text.size()-1) != '>') { //if there is actually some character
	QString command = "ls `dirname " + text.mid(text.lastIndexOf(" ")+1) + "`";
	QString candidates = getTabCandidates(text.mid(text.lastIndexOf(" ")+2), exec(command));
	std::cout << candidates.toStdString() << std::endl;
	if (!candidates.isEmpty()) {
	  std::cout << "Got the following candidates: ["
		    << candidates.toStdString() << "]\n";
	  m_output->append(candidates + m_separator);

	  if (!candidates.contains("\n")) { //just one element
	    m_command_line->setText(text.left(text.lastIndexOf(" ") +1) + exec(QString("dirname "
										       + text.mid(text.lastIndexOf(" ")+1))).remove("\n")
				    + candidates);
	    moveCursor();
	  }
	}
      }
    }
  }
  
  QString GTerm::exec(QString cmd/*const char* cmd*/) {
    FILE* pipe = popen(cmd.append(" 2>&1").toStdString().c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != NULL)
	result += buffer;
    }
    pclose(pipe);
    return QString::fromStdString(result);
  }

  void GTerm::resetPrompt() {
    m_pwd = exec("whoami").remove("\n") + "@" + exec("hostname").remove("\n")
      + "[" + exec("pwd").remove("\n") + "]" + m_token;    
    m_command_line->setText(m_pwd);
    moveCursor();
  }

  QString GTerm::getTabCandidates(const QString s, QString result) {
    QStringList candidates = result.split("\n");
    QStringList final;
    unsigned cnt=0;
    for (unsigned i=0; i<candidates.size(); ++i) {	
      for (unsigned j=0; j<s.size(); ++j) {
	if (candidates.at(i).at(j) == s.at(j)) {
	  ++cnt;
	} else {
	  break; //go to the next one
	}
      }
      if (cnt==s.size()) {
	final << candidates.at(i);
      }
      cnt = 0;
    }
    
    return final.join("\n");
  }
}
