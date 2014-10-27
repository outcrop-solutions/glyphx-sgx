#include "sharedactionlist.h"

namespace SynGlyphX {

	SharedActionList::SharedActionList(QObject *parent)
		: QList<QAction*>(),
		m_parent(parent)
	{

	}

	SharedActionList::SharedActionList(const SharedActionList& list) :
		QList<QAction*>(list),
		m_parent(list.m_parent) {


	}

	SharedActionList::~SharedActionList()
	{

	}

	QAction* SharedActionList::AddAction(const QString & text, const QKeySequence& shortcut) {

		QAction* action = new QAction(text, m_parent);
		action->setShortcut(shortcut);
		push_back(action);

		return action;
	}

	void SharedActionList::AddSeparator() {

		push_back(CreateSeparator(m_parent));
	}

	QAction* SharedActionList::CreateSeparator(QObject* parent) {

		QAction* action = new QAction(parent);
		action->setSeparator(true);
		return action;
	}

	void SharedActionList::EnableActions(bool enable) {

		Q_FOREACH(QAction* action, *this) {

			action->setEnabled(enable);
		}
	}

} //namespace SynGlyphX