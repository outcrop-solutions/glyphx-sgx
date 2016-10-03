#include "GlyphForestInfoModel.h"
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>

namespace SynGlyphX {

	GlyphForestInfoModel::GlyphForestInfoModel(QObject *parent)
		: QAbstractItemModel(parent)
	{

	}

	GlyphForestInfoModel::~GlyphForestInfoModel()
	{

	}

	QVariant GlyphForestInfoModel::data(const QModelIndex& index, int role) const {

		if ((role == Qt::DisplayRole) && index.isValid()) {

			GlyphInfoConstIterator iterator(static_cast<GlyphInfoNode*>(index.internalPointer()));
			return (*iterator)[0];
		}

		return QVariant();
	}

	QModelIndex	GlyphForestInfoModel::index(int row, int column, const QModelIndex& parent) const {

		if (hasIndex(row, column, parent)) {

			if (parent.isValid()) {

				GlyphInfoConstIterator parentIterator(static_cast<GlyphInfoNode*>(parent.internalPointer()));
				GlyphInfoConstIterator childIterator = parentIterator.owner()->child(parentIterator, row);
				if (childIterator.valid()) {

					return createIndex(row, 0, static_cast<void*>(childIterator.node()));
				}
			}
			else {
				
				const GlyphInfoTree& glyphTree = m_glyphs[row];
				return createIndex(row, 0, static_cast<void*>(glyphTree.root().node()));
			}
		}

		return QModelIndex();
	}

	QModelIndex	GlyphForestInfoModel::parent(const QModelIndex& index) const {

		if ((index.isValid()) && (index.internalPointer() != nullptr)) {

			GlyphInfoConstIterator iterator(static_cast<GlyphInfoNode*>(index.internalPointer()));
			const GlyphInfoTree* currentTree = static_cast<const GlyphInfoTree*>(iterator.owner());
			if (iterator != currentTree->root()) {

				GlyphInfoConstIterator parentIterator = currentTree->parent(iterator);
				if (parentIterator.valid()) {

					int row = 0;

					if (parentIterator == currentTree->root()) {

						for (auto tree : m_glyphs) {

							if (currentTree->root() == tree.root().constify()) {

								break;
							}
							++row;
						}
					}
					else {

						GlyphInfoConstIterator grandparent = currentTree->parent(parentIterator);
						if (grandparent.valid()) {

							for (int i = 0; i < currentTree->children(grandparent); ++i) {

								if (currentTree->child(grandparent, i) == parentIterator) {

									row = i;
									break;
								}
							}
						}
					}

					return createIndex(row, 0, static_cast<void*>(parentIterator.node()));
				}
			}
		}

		return QModelIndex();
	}

	int	GlyphForestInfoModel::rowCount(const QModelIndex& parent) const {

		if (!parent.isValid()) {

			return m_glyphs.size();
		}

		if (parent.internalPointer() != nullptr) {

			GlyphInfoConstIterator iterator(static_cast<GlyphInfoNode*>(parent.internalPointer()));
			return iterator.owner()->children(iterator);
		}

		return 0;
	}

	void GlyphForestInfoModel::ClearAndReset() {

		beginResetModel();
		Clear();
		endResetModel();
	}

	void GlyphForestInfoModel::Clear() {

		m_csvID2GlyphNode.clear();
		m_glyphs.clear();
	}

	void GlyphForestInfoModel::LoadGlyphForestInfoLegacy(const QString& nodeCSVFile, const QString& tagCSVFile) {

		beginResetModel();
		Clear();

		CSVFileReader csvReaderTag(tagCSVFile.toStdString());
		const CSVFileHandler::CSVValues& headers = csvReaderTag.GetHeaders();

		CSVFileReader csvReaderNode(nodeCSVFile.toStdString());
		csvReaderNode.GetHeaders();

		unsigned int idIndex = FindHeaderIndex(headers, L"id");
		unsigned int parentIDIndex = FindHeaderIndex(headers, L"parent_id");
		unsigned int typeIndex = FindHeaderIndex(headers, L"type");

		SynGlyphX::CSVFileHandler::CSVValues currentLineValues;
		while (!csvReaderNode.IsAtEndOfFile()) {

			currentLineValues = csvReaderNode.GetValuesFromLine(true);
			if (currentLineValues[typeIndex] == L"5") {

				//m_id2ParentIDMap[std::stoul(currentLineValues[idIndex])] = std::stoul(currentLineValues[parentIDIndex]);
			}
		}

		endResetModel();
	}

	Qt::ItemFlags GlyphForestInfoModel::flags(const QModelIndex& index) const {

		if (!index.isValid()) {
			return 0;
		}

		Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

		return flags;
	}

	bool GlyphForestInfoModel::OpenURLs(const QModelIndexList& indexList) {

		bool wereAnyURLsOpened = false;
		for (const QModelIndex& index : indexList) {

			GlyphInfoConstIterator iterator(static_cast<GlyphInfoNode*>(index.internalPointer()));
			const QString& url = (*iterator)[2];
			if ((!url.isEmpty()) && (url != "nourl.html")) {

				QUrl parsedUrl = QUrl::fromUserInput(url);
				if (parsedUrl.isValid()) {

					QDesktopServices::openUrl(parsedUrl);
					wereAnyURLsOpened = true;
				}
			}
		}
		return wereAnyURLsOpened;
	}

	unsigned int GlyphForestInfoModel::FindHeaderIndex(const CSVFileHandler::CSVValues& headers, const std::wstring& header) const {

		for (unsigned int i = 0; i < headers.size(); ++i) {

			if (header == headers[i]) {

				return i;
			}
		}
	}

	QModelIndex GlyphForestInfoModel::IndexFromCSVID(int id) const {

		if (m_csvID2GlyphNode.count(id) > 0) {

			GlyphInfoConstIterator iterator = m_csvID2GlyphNode.at(id);
			const GlyphInfoTree* currentTree = static_cast<const GlyphInfoTree*>(iterator.owner());
			unsigned int row = 0;
			if (iterator != currentTree->root()) {

				GlyphInfoConstIterator parentIterator = currentTree->parent(iterator);
				if (parentIterator.valid()) {

					for (int i = 0; i < currentTree->children(parentIterator); ++i) {

						if (currentTree->child(parentIterator, i) == iterator) {

							row = i;
							break;
						}
					}
				}
			}
			else {

				for (auto tree : m_glyphs) {

					if (currentTree->root() == tree.root().constify()) {

						break;
					}
					++row;
				}
			}

			return createIndex(row, 0, static_cast<void*>(iterator.node()));
		}

		return QModelIndex();
	}

	const GlyphForestInfoModel::GlyphTextProperties& GlyphForestInfoModel::GetGlyphTextProperties(const QModelIndex& index) const {

		GlyphInfoConstIterator iterator(static_cast<GlyphInfoNode*>(index.internalPointer()));
		return (*iterator);
	}

} //namespace SynGlyphX