#include <iostream>
#include <QTextCursor>
#include "gui.hpp"

namespace gterm {
	GTerm::GTerm(QWidget* parent) : QWidget(parent),
					m_token(">>"),
					m_history_index(0) {
		resize(700, 610);
		m_main_layout = new QVBoxLayout(this);
		m_main_layout->setAlignment(Qt::AlignCenter);
		m_output = new QTextEdit();
		m_output->setReadOnly(true);
		m_output->setFixedHeight(600);
		m_output->setStyleSheet("border: 2px solid black;");
		m_output->setAlignment(Qt::AlignLeft);
		//m_output->setEnabled(false);
		m_command_line = new QTextEdit("[" + QString::fromStdString(exec("pwd")) + "]" + m_token);
		//m_command_line->setFocus();
		moveCursor();
		connect(m_command_line, SIGNAL(textChanged()), this, SLOT(textChangedSlot()));
		m_main_layout->addWidget(m_output);
		m_main_layout->addWidget(m_command_line);
		setLayout(m_main_layout);
		show();
	}
	GTerm::~GTerm() {
		delete m_command_line;
		delete m_main_layout;
	}
	void GTerm::moveCursor() {
		QTextCursor tmp = m_command_line->textCursor();
                tmp.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, m_command_line->toPlainText().size());
                m_command_line->setTextCursor(tmp);
	}
	void GTerm::keyPressEvent(QKeyEvent* event) {
		std::cout << event->key() << std::endl;
		if (event->key() == 70) { //'f', forward
			if (m_history_index < m_history.size()-1) {
				++m_history_index;
				m_command_line->setText("[" + QString::fromStdString(exec("pwd")).remove("\n") + "]"+ m_token + m_history.at(m_history_index));
			}	
		} else if (event->key() == 66) { //'b', backward
			if (m_history_index > 0) {
				--m_history_index;
				m_command_line->setText("[" + QString::fromStdString(exec("pwd")).remove("\n") + "]"+ m_token + m_history.at(m_history_index));
			}
		}
	}
	void GTerm::textChangedSlot() {
		QString text = m_command_line->toPlainText();
		if (text.at(text.size() - 1) == 10) {
			QString command = text.mid(text.lastIndexOf(">")+1);
			command.chop(1);
			m_command_line->setText("[" + QString::fromStdString(exec("pwd")).remove("\n") + "]"+ m_token);
			moveCursor();
			std::cout << "Command to send: [" << command.toStdString() << "]\n";
			m_history << command;
			m_history_index = m_history.size() -1;
			m_output->append(QString::fromStdString(exec(command.toStdString().c_str())) + "\t\t@@@@@@@@@@@@@@@@@@@@@@\t\t");
		} //else if (text.at(text.size() -1) == '>') {
			//m_command_line->setText(m_token);
			//moveCursor();
		//}
	}
	std::string GTerm::exec(const char* cmd) {
		char command[200];
		strcpy(command,cmd);
		strcat(command," 2>&1");
		FILE* pipe = popen(command, "r");
    		if (!pipe) return "ERROR";
		char buffer[128];
		std::string result = "";
		while(!feof(pipe)) {
	        if(fgets(buffer, 128, pipe) != NULL)
        		result += buffer;
   		}
		pclose(pipe);
		return result;
	}
}
