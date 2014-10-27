#ifndef SHAREDACTIONLIST_H
#define SHAREDACTIONLIST_H

#include "sgxgui_global.h"
#include <QtCore/QObject>
#include <QtWidgets/QAction>

namespace SynGlyphX {

	class SGXGUI_EXPORT SharedActionList : public QList < QAction* >
	{
	public:
		SharedActionList(QObject *parent = nullptr);
		SharedActionList(const SharedActionList& list);
		~SharedActionList();

		QAction* AddAction(const QString & text = "", const QKeySequence& shortcut = QKeySequence());
		void AddSeparator();

		static QAction* CreateSeparator(QObject* parent);

		void EnableActions(bool enable);

	private:
		QObject* m_parent;
	};

} //namespace SynGlyphX

#endif // SHAREDACTIONLIST_H
