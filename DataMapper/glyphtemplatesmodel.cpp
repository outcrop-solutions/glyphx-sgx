#include "glyphtemplatesmodel.h"

GlyphTemplatesModel::GlyphTemplatesModel(QObject *parent)
	: QAbstractItemModel(parent)
{

}

GlyphTemplatesModel::~GlyphTemplatesModel()
{

}

QVariant GlyphTemplatesModel::headerData(int section, Qt::Orientation orientation, int role) const {

	//We don't need column headers in views so this gets rid of them
	return "";
}