///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

#pragma once

#include <QtWidgets/QSplitter>
#include <unordered_map>
#include "DistinctValueFilteringParameters.h"
#include "xmlpropertytreefile.h"
#include "dataengineconnection.h"
#include <containers/ntree.hpp>
#include <QtCore/QMap>

namespace SynGlyphX {

	class TitleTreeWidget;
}

namespace DataEngine {

	class GlyphEngine;
}

class LoadingFilterWidget;
class QStackedWidget;
class QProgressBar;

class SharedVisualizationsFile : public SynGlyphX::XMLPropertyTreeFile {

public:
	SharedVisualizationsFile(QString baseDir = QString()) : SynGlyphX::XMLPropertyTreeFile(), m_baseDir(baseDir) {}
	~SharedVisualizationsFile() {}

	const stlplus::ntree<QStringList>& GetInfoTree() const { return m_groupedVisualizations; }
	const QMap<QString, QString>& GetFilenameToTitleMap() const { return m_filenameToTitleMap; }

protected:
	void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) override;
	void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const override;

	void ImportFromPropertyTree(const boost::property_tree::wptree& propertyTree, stlplus::ntree<QStringList>::iterator parent);

	stlplus::ntree<QStringList> m_groupedVisualizations;
	QMap<QString, QString> m_filenameToTitleMap;
	QString m_baseDir;
};

class SharedVisualizationsWidget : public QSplitter
{
	Q_OBJECT

public:

	SharedVisualizationsWidget(QWidget *parent);
	~SharedVisualizationsWidget();

	void Setup(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QProgressBar* prog);
	void Reset(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection);
	void ClearAll();
	
	bool DoCurrentNecessaryFiltersHaveSelection() const;
	QString GetCurrentFilename() const;
	bool CanCurrentHaveFilters() const;
	std::pair<MultiTableDistinctValueFilteringParameters, std::vector<std::wstring>> GetCurrentFilterValues() const;

	const QMap<QString, QString>& GetSharedVisualizationsInfo() const { return m_filenameToTitleMap; }

private slots:
	void OnFileSelected(QString filename);

private:
	SynGlyphX::TitleTreeWidget* m_viewListWidget;
	QStackedWidget* m_loadingFilterWidgetsStack;
	std::unordered_map<std::string, LoadingFilterWidget*> m_loadingFilterWidgetMap;
	QMap<QString, QString> m_filenameToTitleMap;
	std::string m_currentFilename;
	QProgressBar* progress;
	bool IsGlyphEd;
};

//#pragma once
