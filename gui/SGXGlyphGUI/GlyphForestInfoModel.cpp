#include "GlyphForestInfoModel.h"
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include <cassert>
#include <stdexcept>
namespace SynGlyphX {

	GlyphForestInfoModel::GlyphForestInfoModel( QObject *parent )
		: QAbstractItemModel( parent )
	{

	}

	GlyphForestInfoModel::~GlyphForestInfoModel()
	{

	}

	QVariant GlyphForestInfoModel::data( const QModelIndex& index, int role ) const {

		if ( ( role == Qt::DisplayRole ) && index.isValid() ) {

			GlyphInfoNode* node = static_cast<GlyphInfoNode*>( index.internalPointer() );
			return node->m_properties[0];
		}

		return QVariant();
	}

	QModelIndex	GlyphForestInfoModel::index( int row, int column, const QModelIndex& parent ) const {

		if ( hasIndex( row, column, parent ) ) {

			if ( parent.isValid() ) {

				GlyphInfoNode* parentNode = static_cast<GlyphInfoNode*>( parent.internalPointer() );
				if ( row < int( parentNode->GetChildCount() ) ) {

					return createIndex( row, 0, static_cast<void*>( parentNode->m_children[row] ) );
				}
			}
			else {

				std::shared_ptr<GlyphInfoTree> glyphTree = m_glyphs[row];
				return createIndex( row, 0, static_cast<void*>( glyphTree->m_root ) );
			}
		}

		return QModelIndex();
	}

	QModelIndex	GlyphForestInfoModel::parent( const QModelIndex& index ) const {

		if ( ( index.isValid() ) && ( index.internalPointer() != nullptr ) ) {

			GlyphInfoNode* node = static_cast<GlyphInfoNode*>( index.internalPointer() );

			if ( !node->IsRoot() ) {

				int row = 0;

				GlyphInfoNode* parentNode = node->m_parent;
				if ( parentNode->IsRoot() ) {

					for ( auto& tree : m_glyphs ) {

						if ( parentNode == tree->m_root ) {

							break;
						}
						++row;
					}
				}
				else {

					GlyphInfoNode* grandparentNode = parentNode->m_parent;

					for ( int i = 0; i < int( grandparentNode->GetChildCount() ); ++i ) {

						if ( grandparentNode->m_children[i] == parentNode ) {

							row = i;
							break;
						}
					}
				}

				return createIndex( row, 0, static_cast<void*>( parentNode ) );
			}
		}

		return QModelIndex();
	}

	int	GlyphForestInfoModel::rowCount( const QModelIndex& parent ) const {

		if ( !parent.isValid() ) {

			return (int)m_glyphs.size();
		}

		if ( parent.internalPointer() != nullptr ) {

			GlyphInfoNode* parentNode = static_cast<GlyphInfoNode*>( parent.internalPointer() );
			return (int)parentNode->GetChildCount();
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

	void GlyphForestInfoModel::LoadGlyphForestInfo( const LoadedGlyphDataInterface& glyphdata )
	{
		beginResetModel();
		Clear();

		glyphdata.enumGlyphStrings( [this]( int id, int parent_id, int filtering_index, const char* tag, const char* url, const char* desc ) {
			GlyphTextProperties glyphTextProperties;
			glyphTextProperties[0] = tag;
			glyphTextProperties[1] = desc;
			glyphTextProperties[2] = url;

			unsigned long newID = id;
			unsigned long newParentID = parent_id;

			if ( m_csvID2GlyphNode.count( newParentID ) == 0 ) {

				std::shared_ptr<GlyphInfoTree> newGlyph( new GlyphInfoTree() );
				newGlyph->index = filtering_index;
				newGlyph->m_root->m_properties = glyphTextProperties;
				newGlyph->m_root->m_tree = newGlyph.get();
				m_csvID2GlyphNode.insert( std::make_pair( newID, newGlyph->m_root ) );
				m_glyphs.push_back( newGlyph );
			}
			else {

				GlyphInfoNode* parentNode = m_csvID2GlyphNode.at( newParentID );
				GlyphInfoNode* newNode = new GlyphInfoNode( parentNode );
				newNode->m_tree = parentNode->m_tree;
				m_csvID2GlyphNode[newID] = newNode;
				parentNode->m_children.push_back( newNode );
				newNode->m_properties = glyphTextProperties;
			}
		} );

		endResetModel();
	}

	QString GlyphForestInfoModel::GetTag( const std::wstring& title ) const {

		if ( title.empty() ) {

			return "";
		}

		if ( title.substr( 0, 8 ) == L"<a href=" ) {

			auto pos = title.find_first_of( '>', 8 ) + 1;
			return QString::fromStdWString( title.substr( pos, title.length() - pos - 4 ) );
		}
		else {

			return QString::fromStdWString( title );
		}
	}

	QString GlyphForestInfoModel::GetURL( const std::wstring& title ) const {

		if ( !title.empty() ) {

			if ( title.substr( 0, 8 ) == L"<a href=" ) {

				std::wstring url = title.substr( 8, title.find_first_of( '>', 8 ) - 8 );
				if ( url != L"nourl.html" ) {

					return QString::fromStdWString( url );
				}
			}
		}

		return "";
	}

	Qt::ItemFlags GlyphForestInfoModel::flags( const QModelIndex& index ) const {

		if ( !index.isValid() ) {
			return 0;
		}

		Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

		return flags;
	}

	bool GlyphForestInfoModel::OpenURLs( const QModelIndexList& indexList ) {

		bool wereAnyURLsOpened = false;
		for ( const QModelIndex& index : indexList ) {

			GlyphInfoNode* node = static_cast<GlyphInfoNode*>( index.internalPointer() );
			const QString& url = node->m_properties[2];
			if ( ( !url.isEmpty() ) && ( url != "nourl.html" ) ) {

				QUrl parsedUrl = QUrl::fromUserInput( url );
				if ( parsedUrl.isValid() ) {

					QDesktopServices::openUrl( parsedUrl );
					wereAnyURLsOpened = true;
				}
			}
		}
		return wereAnyURLsOpened;
	}

	unsigned int GlyphForestInfoModel::FindHeaderIndex( const CSVFileHandler::CSVValues& headers, const std::wstring& header ) const {

		for ( unsigned int i = 0; i < headers.size(); ++i ) {

			if ( header == headers[i] ) {

				return i;
			}
		}
		throw( std::invalid_argument( "GlyphForestInfoModel::FindHeaderIndex: couldn't find header in list" ) );
	}

	QModelIndex GlyphForestInfoModel::IndexFromCSVID( int id ) const {

		if ( m_csvID2GlyphNode.count( id ) > 0 ) {

			GlyphInfoNode* node = m_csvID2GlyphNode.at( id );
			while ( !node->IsRoot() ) node = node->m_parent;
			return createIndex( node->m_tree->index, 0, static_cast<void*>( node ) );
		}
		else
		{
			return QModelIndex();
		}
	}

	const GlyphForestInfoModel::GlyphTextProperties& GlyphForestInfoModel::GetGlyphTextProperties( const QModelIndex& index ) const {

		GlyphInfoNode* node = static_cast<GlyphInfoNode*>( index.internalPointer() );
		return node->m_properties;
	}

} //namespace SynGlyphX
