#pragma once
#ifndef NEWGLYPHTREEWIZARD_H
#define NEWGLYPHTREEWIZARD_H

#include "sgxglyphgui_global.h"
#include <QtWidgets/QWizard>
#include <memory>

namespace SynGlyphX {

	class VisualGlyphPropertiesWidget;
	class DataMappingGlyphGraph;
	class SGXGLYPHGUI_EXPORT NewGlyphTreeWizard : public QWizard
	{
		Q_OBJECT

	public:
		NewGlyphTreeWizard(unsigned int numberOfBranches, QWidget *parent);
		~NewGlyphTreeWizard();

		virtual void accept();
		std::shared_ptr<DataMappingGlyphGraph> GetNewDataMappingGlyphGraph() const;

		static std::shared_ptr<DataMappingGlyphGraph> RunNewGlyphTreeWizard(QWidget *parent);

	private:
		//Qt will take care of deleting the objects in this vector
		std::vector<SynGlyphX::VisualGlyphPropertiesWidget*> m_glyphWidgets;
		std::shared_ptr<DataMappingGlyphGraph>  m_dataMappingGlyphGraph;
	};

} //namespace SynGlyphX

#endif // NEWGLYPHTREEWIZARD_H
