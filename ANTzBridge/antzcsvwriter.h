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

#ifndef SYNGLYPHX_ANTZ_CSVREADERWRITER
#define SYNGLYPHX_ANTZ_CSVREADERWRITER

#include "ANTzBridge.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include "glyphgraph.h"
#include "glyphcolor.h"
#include <array>
#include "csvfilereader.h"
#include "csvfilewriter.h"
#include "antzgrid.h"

namespace SynGlyphXANTz {

    const unsigned short MaxPredefinedColors = 20;

	class ANTZBRIDGE_API ANTzCSVWriter : public boost::noncopyable
    {
    public:
        ANTzCSVWriter();
        ~ANTzCSVWriter();

		void Write(const std::string& nodeFilename, const std::string& tagFilename, const SynGlyphX::GlyphGraph::ConstSharedVector& trees, const std::vector<ANTzGrid>& grids);
		void WriteGlobals(const std::string& filename, const SynGlyphX::GlyphColor& backgroundColor);

        static ANTzCSVWriter& GetInstance();

		const SynGlyphX::CSVFileReader::CSVValues& GetNodeHeaders() const;
		const SynGlyphX::CSVFileReader::CSVValues& GetTagHeaders() const;
		const SynGlyphX::CSVFileReader::CSVValues& GetChannelHeaders() const;
		const SynGlyphX::CSVFileReader::CSVValues& GetChannelMapHeaders() const;

    private:
        static ANTzCSVWriter s_instance;

		unsigned long WriteHeaders(SynGlyphX::CSVFileWriter& nodeFile, SynGlyphX::CSVFileWriter& tagFile, const std::vector<ANTzGrid>& grids);

		unsigned long WriteGlyphAndChildren(SynGlyphX::CSVFileWriter& file, const SynGlyphX::GlyphGraph::ConstSharedPtr tree, const SynGlyphX::GlyphGraph::ConstGlyphIterator& glyph, unsigned long id, unsigned long parentId, unsigned long branchLevel, bool isLink);
		unsigned long WriteGlyph(SynGlyphX::CSVFileWriter& file, const SynGlyphX::GlyphGraph::ConstSharedPtr tree, const SynGlyphX::GlyphGraph::ConstGlyphIterator& glyph, unsigned long id, unsigned long parentId, unsigned long childId, unsigned long branchLevel, bool isLink);
		unsigned long WriteGlyph(SynGlyphX::CSVFileWriter& file, const SynGlyphX::Glyph& glyph, unsigned long numberOfChildren, unsigned long id, unsigned long parentId, unsigned long childId, unsigned long branchLevel, bool isLink);
		void WriteGlyphTag(SynGlyphX::CSVFileWriter& file, const SynGlyphX::GlyphGraph::ConstSharedPtr tree, const SynGlyphX::GlyphGraph::ConstGlyphIterator& glyph);
		unsigned long WriteGrids(SynGlyphX::CSVFileWriter& file, const std::vector<ANTzGrid>& grids, unsigned long firstId);
        unsigned short GetColorIndex(const SynGlyphX::GlyphColor& color);

		SynGlyphX::GlyphColor m_predefinedColors[MaxPredefinedColors];
		unsigned long m_numTagsWritten;

		SynGlyphX::CSVFileReader::CSVValues m_nodeHeaders;
		SynGlyphX::CSVFileReader::CSVValues m_tagHeaders;
		SynGlyphX::CSVFileReader::CSVValues m_channelHeaders;
		SynGlyphX::CSVFileReader::CSVValues m_channelMapHeaders;

		SynGlyphX::CSVFileReader::CSVValues m_cameras[5];

		std::unordered_map<SynGlyphX::GlyphGraph::Label, unsigned long> m_labelToANTzIDMap;
		std::unordered_map<SynGlyphX::GlyphGraph::Label, unsigned int> m_labelToANTzBranchLevelMap;
    };

} //namespace SynGlyphXANTz

#endif //SYNGLYPHX_ANTZ_CSVREADERWRITER