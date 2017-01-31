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

#include <QtWidgets/QScrollArea>
#include "inputtable.h"
#include <unordered_map>
#include "TitleListWidget.h"
#include "FrontEndFilter.h"
#include "DistinctValueFilteringParameters.h"

namespace DataEngine {

	class GlyphEngine;
}

namespace SynGlyphX {

	class DataTransformMapping;
}

class QSplitter;

class LoadingFilterMissingChoice : public std::runtime_error {

public:
	LoadingFilterMissingChoice(const std::wstring& field);
	~LoadingFilterMissingChoice() {}

	const std::wstring& GetField() const { return m_field; }

private:
	std::wstring m_field;
};

class SingleLoadingFilterWidget : public SynGlyphX::TitleListWidget {

public:
	SingleLoadingFilterWidget(bool isRequired, QWidget* parent);
	~SingleLoadingFilterWidget() {}

	bool IsRequired() const { return m_isRequired; }

protected:
	bool m_isRequired;
};

class LoadingFilterWidget : public QWidget
{
	Q_OBJECT

public:
	LoadingFilterWidget(QWidget *parent);
	~LoadingFilterWidget();

	void SetFilters(DataEngine::GlyphEngine& glyphEngine, const SynGlyphX::DataTransformMapping& mapping);

	bool AreSelectionsValid() const;

	bool IsQueryNeeded(const SynGlyphX::InputTable& table) const;
	MultiTableDistinctValueFilteringParameters GetFilterValues() const;

private:
	typedef std::unordered_map<std::wstring, SingleLoadingFilterWidget*> FieldToWidgetMap;
	typedef std::unordered_map<SynGlyphX::InputTable, FieldToWidgetMap, SynGlyphX::InputTableHash> TableToWidgetsMap;

	QSplitter* AddFiltersForTable(DataEngine::GlyphEngine& glyphEngine, const SynGlyphX::DataTransformMapping& mapping);

	TableToWidgetsMap m_filterListWidgets;
};

//#pragma once
