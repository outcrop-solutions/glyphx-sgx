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

		std::unordered_map<unsigned long, GlyphTextProperties> id2GlyphTextProperties;
		ReadTagCSV(tagCSVFile, id2GlyphTextProperties);

		CSVFileReader csvReaderNode(nodeCSVFile.toStdString());
		const CSVFileHandler::CSVValues& headers = csvReaderNode.GetHeaders();

		unsigned int idIndex = FindHeaderIndex(headers, L"id");
		unsigned int parentIDIndex = FindHeaderIndex(headers, L"parent_id");
		unsigned int typeIndex = FindHeaderIndex(headers, L"type");

		SynGlyphX::CSVFileHandler::CSVValues currentLineValues;
		while (!csvReaderNode.IsAtEndOfFile()) {

			currentLineValues = csvReaderNode.GetValuesFromLine(true);
			if (currentLineValues.empty()) {

				continue;
			}
			if (currentLineValues[typeIndex] == L"5") {

				unsigned long newID = std::stoul(currentLineValues[idIndex]);
				unsigned long newParentID = std::stoul(currentLineValues[parentIDIndex]);

				if (m_csvID2GlyphNode.count(newParentID) == 0) {

					m_glyphs.push_back(GlyphInfoTree());
					GlyphInfoTree& newGlyphInfoTree = m_glyphs.back();
					if (id2GlyphTextProperties.count(newID) > 0) {

						m_csvID2GlyphNode[newID] = newGlyphInfoTree.insert(id2GlyphTextProperties[newID]);
					}
					else {

						m_csvID2GlyphNode[newID] = newGlyphInfoTree.insert(GlyphTextProperties());
					}
				}
				else {

					GlyphInfoIterator parentIterator = m_csvID2GlyphNode.at(newParentID);
					GlyphInfoTree* currentTree = const_cast<GlyphInfoTree*>(parentIterator.owner());
					if (id2GlyphTextProperties.count(newID) > 0) {

						m_csvID2GlyphNode[newID] = currentTree->append(parentIterator, id2GlyphTextProperties[newID]);
					}
					else {

						m_csvID2GlyphNode[newID] = currentTree->append(parentIterator, GlyphTextProperties());
					}
				}
			}
		}

		endResetModel();
	}

	void GlyphForestInfoModel::ReadTagCSV(const QString& filename, std::unordered_map<unsigned long, GlyphTextProperties>& id2GlyphTextProperties) {

		CSVFileReader csvReaderTag(filename.toStdString());
		const CSVFileHandler::CSVValues& headers = csvReaderTag.GetHeaders();

		unsigned int idIndex = FindHeaderIndex(headers, L"record_id");
		unsigned int titleIndex = FindHeaderIndex(headers, L"title");
		unsigned int descIndex = FindHeaderIndex(headers, L"description");

		SynGlyphX::CSVFileHandler::CSVValues currentLineValues;
		while (!csvReaderTag.IsAtEndOfFile()) {

			currentLineValues = csvReaderTag.GetValuesFromLine(false);
			if (currentLineValues.empty()) {

				continue;
			}
			GlyphTextProperties glyphTextProperties;
			glyphTextProperties[0] = GetTag(currentLineValues[titleIndex]);
			glyphTextProperties[2] = GetURL(currentLineValues[titleIndex]);
			if (descIndex < currentLineValues.size()) {

				glyphTextProperties[1] = QString::fromStdWString(currentLineValues[descIndex]);
			}

			id2GlyphTextProperties[std::stoul(currentLineValues[idIndex])] = glyphTextProperties;
		}
	}

	QString GlyphForestInfoModel::GetTag(const std::wstring& title) const {

		if (title.empty()) {

			return "";
		}

		if (title.substr(0, 8) == L"<a href=") {

			int pos = title.find_first_of('>', 8) + 1;
			return QString::fromStdWString(title.substr(pos, title.length() - pos - 4));
		}
		else {

			return QString::fromStdWString(title);
		}
	}

	QString GlyphForestInfoModel::GetURL(const std::wstring& title) const {

		if (!title.empty()) {

			if (title.substr(0, 8) == L"<a href=") {

				std::wstring url = title.substr(8, title.find_first_of('>', 8) - 8);
				if (url != L"nourl.html") {

					return QString::fromStdWString(url);
				}
			}
		}

		return "";
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

			GlyphInfoConstIterator iterator = m_csvID2GlyphNode.at(id).constify();
			const GlyphInfoTree* currentTree = iterator.owner();
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