
#include "sharedactionlist.h"
#include <QtCore/QObject>

namespace SynGlyphX {

	SharedActionList::SharedActionList(QObject *parent) :
		m_parent(parent)
	{

	}

	SharedActionList::SharedActionList(const SharedActionList& list) :
		m_parent(list.m_parent) {


	}

	SharedActionList::~SharedActionList()
	{

	}

	QAction* SharedActionList::AddAction(const QString & text, const QKeySequence& shortcut) {

		QAction* action = new QAction(text, m_parent);
		action->setShortcut(shortcut);
		actions.push_back(action);

		return action;
	}

	void SharedActionList::AddSeparator() {

		actions.push_back(CreateSeparator(m_parent));
	}

	QAction* SharedActionList::CreateSeparator(QObject* parent) {

		QAction* action = new QAction(parent);
		action->setSeparator(true);
		return action;
	}

	void SharedActionList::EnableActions(bool enable) {

		Q_FOREACH(QAction* action, actions) {

			action->setEnabled(enable);
		}
	}

} //namespace SynGlyphX
