#ifndef NEWGLYPHTREEWIZARD_H
#define NEWGLYPHTREEWIZARD_H

#include "sgxglyphgui_global.h"
#include <QtWidgets/QWizard>
#include "visualglyphpropertieswidget.h"
#include "datamappingglyphgraph.h"

namespace SynGlyphX {

	class SGXGLYPHGUI_EXPORT NewGlyphTreeWizard : public QWizard
	{
		Q_OBJECT

	public:
		NewGlyphTreeWizard(unsigned int numberOfBranches, QWidget *parent);
		~NewGlyphTreeWizard();

		virtual void accept();
		DataMappingGlyphGraph::SharedPtr GetNewDataMappingGlyphGraph() const;

		static DataMappingGlyphGraph::SharedPtr RunNewGlyphTreeWizard(QWidget *parent);

	private:
		//Qt will take care of deleting the objects in this vector
		std::vector<SynGlyphX::VisualGlyphPropertiesWidget*> m_glyphWidgets;
		DataMappingGlyphGraph::SharedPtr m_dataMappingGlyphGraph;
	};

} //namespace SynGlyphX

#endif // NEWGLYPHTREEWIZARD_H
