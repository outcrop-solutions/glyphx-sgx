#include "itemfocusselectionmodel.h"

namespace SynGlyphX {

	ItemFocusSelectionModel::ItemFocusSelectionModel(QAbstractItemModel* model, QObject *parent)
		: QItemSelectionModel(model, parent)
	{

	}

	ItemFocusSelectionModel::~ItemFocusSelectionModel()
	{

	}

	void ItemFocusSelectionModel::ClearFocus() {

		if (!m_focusList.empty()) {

			m_focusList.clear();
			emit FocusChanged(QModelIndexList());
		}
	}

	void ItemFocusSelectionModel::SetFocus(const QModelIndex& index, FocusFlags command) {

		QModelIndexList indexes;
		indexes.push_back(index);
		SetFocus(indexes, command);
	}

	void ItemFocusSelectionModel::SetFocus(const QModelIndexList& indexes, FocusFlags command) {

		bool focusChanged = false;
		if (command.testFlag(FocusFlag::Clear)) {

			focusChanged = !m_focusList.empty();
			m_focusList.clear();
		}

		if (indexes.front().model() == model()) {

			if (command.testFlag(FocusFlag::Focus)) {

				if (m_focusList.empty()) {

					m_focusList = indexes;
					focusChanged = true;
				}
				else {
				
					focusChanged = (indexes.back() != m_focusList.back());
					Q_FOREACH(const QModelIndex& index, indexes) {

						m_focusList.removeOne(index);
					}
					m_focusList.append(indexes);
				}
			}

			if (command.testFlag(FocusFlag::Unfocus) && (!m_focusList.empty())) {

				QModelIndex oldLastIndex = m_focusList.back();
				Q_FOREACH(const QModelIndex& index, indexes) {

					m_focusList.removeOne(index);
				}
				if (oldLastIndex != m_focusList.back()) {

					focusChanged = true;
				}
			}

			if (command.testFlag(FocusFlag::Toggle)) {

				QModelIndex oldLastIndex = m_focusList.back();
				Q_FOREACH(const QModelIndex& index, indexes) {

					int pos = m_focusList.indexOf(index);
					if (pos == -1) {

						m_focusList.push_back(index);
					}
					else {

						m_focusList.removeAt(pos);
					}
				}
				if (oldLastIndex != m_focusList.back()) {

					focusChanged = true;
				}
			}
		}

		if (focusChanged) {

			if (m_focusList.empty()) {

				emit FocusChanged(QModelIndexList());
			}
			else {

				emit FocusChanged(m_focusList);
			}
 		}
	}

	const QModelIndexList& ItemFocusSelectionModel::GetFocusList() const {

		return m_focusList;
	}

} //namespace SynGlyphX