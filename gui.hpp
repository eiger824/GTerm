#ifndef GUI_HPP_
#define GUI_HPP_

#include <string>

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QStringList>

namespace gterm {
	class GTerm : public QWidget {
		Q_OBJECT
	public:
		GTerm(QWidget* parent=0);
		~GTerm();
	protected:
		void keyPressEvent(QKeyEvent* event);
	private slots:
		void textChangedSlot();
	private:
		std::string exec(const char* cmd);
		void moveCursor();
	private:
		QTextEdit* m_command_line;
		QVBoxLayout* m_main_layout;
		QString m_token;
		QTextEdit* m_output;
		QStringList m_history;
		unsigned m_history_index;
	};
}
#endif
