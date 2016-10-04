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

			GlyphInfoNode* node = static_cast<GlyphInfoNode*>(index.internalPointer());
			return node->m_properties[0];
		}

		return QVariant();
	}

	QModelIndex	GlyphForestInfoModel::index(int row, int column, const QModelIndex& parent) const {

		if (hasIndex(row, column, parent)) {

			if (parent.isValid()) {

				GlyphInfoNode* parentNode = static_cast<GlyphInfoNode*>(parent.internalPointer());
				if (row < parentNode->GetChildCount()) {

					return createIndex(row, 0, static_cast<void*>(parentNode->m_children[row]));
				}
			}
			else {
				
				std::shared_ptr<GlyphInfoTree> glyphTree = m_glyphs[row];
				return createIndex(row, 0, static_cast<void*>(glyphTree->m_root));
			}
		}

		return QModelIndex();
	}

	QModelIndex	GlyphForestInfoModel::parent(const QModelIndex& index) const {

		if ((index.isValid()) && (index.internalPointer() != nullptr)) {

			GlyphInfoNode* node = static_cast<GlyphInfoNode*>(index.internalPointer());
			
			if (!node->IsRoot()) {

				int row = 0;

				GlyphInfoNode* parentNode = node->m_parent;
				if (parentNode->IsRoot()) {

					for (auto& tree : m_glyphs) {

						if (parentNode == tree->m_root) {

							break;
						}
						++row;
					}
				}
				else {

					GlyphInfoNode* grandparentNode = parentNode->m_parent;

					for (int i = 0; i < grandparentNode->GetChildCount(); ++i) {

						if (grandparentNode->m_children[i] == parentNode) {

							row = i;
							break;
						}
					}
				}

				return createIndex(row, 0, static_cast<void*>(parentNode));
			}
		}

		return QModelIndex();
	}

	int	GlyphForestInfoModel::rowCount(const QModelIndex& parent) const {

		if (!parent.isValid()) {

			return m_glyphs.size();
		}

		if (parent.internalPointer() != nullptr) {

			GlyphInfoNode* parentNode = static_cast<GlyphInfoNode*>(parent.internalPointer());
			return parentNode->GetChildCount();
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

					std::shared_ptr<GlyphInfoTree> newGlyph(new GlyphInfoTree());
					if (id2GlyphTextProperties.count(newID) > 0) {

						newGlyph->m_root->m_properties = id2GlyphTextProperties[newID];
					}
					m_csvID2GlyphNode.insert(std::pair<unsigned long, GlyphInfoNode*>(newID, newGlyph->m_root));
					m_glyphs.push_back(newGlyph);
				}
				else {

					GlyphInfoNode* parentNode = m_csvID2GlyphNode.at(newParentID);
					GlyphInfoNode* newNode = new GlyphInfoNode(parentNode);
					m_csvID2GlyphNode[newID] = newNode;
					parentNode->m_children.push_back(newNode);
					if (id2GlyphTextProperties.count(newID) > 0) {

						newNode->m_properties = id2GlyphTextProperties[newID];
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

			GlyphInfoNode* node = static_cast<GlyphInfoNode*>(index.internalPointer());
			const QString& url = node->m_properties[2];
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

			GlyphInfoNode* node = m_csvID2GlyphNode.at(id);
			unsigned int row = 0;
			if (!node->IsRoot()) {

				GlyphInfoNode* parentNode = node->m_parent;
				for (int i = 0; i < parentNode->GetChildCount(); ++i) {

					if (parentNode->m_children[i] == node) {

						row = i;
						break;
					}
				}
			}
			else {

				for (auto& tree : m_glyphs) {

					if (node == tree->m_root) {

						break;
					}
					++row;
				}
			}

			return createIndex(row, 0, static_cast<void*>(node));
		}

		return QModelIndex();
	}

	const GlyphForestInfoModel::GlyphTextProperties& GlyphForestInfoModel::GetGlyphTextProperties(const QModelIndex& index) const {

		GlyphInfoNode* node = static_cast<GlyphInfoNode*>(index.internalPointer());
		return node->m_properties;
	}

} //namespace SynGlyphX