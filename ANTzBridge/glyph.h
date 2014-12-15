#ifndef SYNGLYPHX_GLYPH
#define SYNGLYPHX_GLYPH

#include "ANTzBridge.h"
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <vector>
#include "glyphproperties.h"
#include <containers/ntree.hpp>

namespace SynGlyphX {

	class ANTZBRIDGE_API GlyphTree : public stlplus::ntree < GlyphProperties >
	{
	public:
		typedef boost::shared_ptr<GlyphTree> SharedPtr;
		typedef boost::shared_ptr<const GlyphTree> ConstSharedPtr;
		typedef std::vector<ConstSharedPtr> ConstSharedVector;

		GlyphTree();
		GlyphTree(pNPnode node);
		GlyphTree(const GlyphProperties& rootProperties);
		~GlyphTree();

		void AllocateChildSubtree(const std::vector<SynGlyphX::GlyphProperties::ConstSharedPtr>& templates, const std::vector<unsigned int> instances, const GlyphTree::iterator& parent);

	private:
		GlyphTree(const std::vector<SynGlyphX::GlyphProperties::ConstSharedPtr>& templates, const std::vector<unsigned int> instances, unsigned int depth);

		void SetPosition(GlyphTree::iterator& glyph, double xPosition);
	};

    /*class ANTZBRIDGE_API Glyph : public GlyphProperties
    {
    public:
		typedef boost::shared_ptr<Glyph> SharedPtr;
		typedef boost::shared_ptr<const Glyph> ConstSharedPtr;

        Glyph();
        Glyph(pNPnode node);
        Glyph(const std::vector<boost::shared_ptr<GlyphProperties>>& templates, const std::vector<unsigned int>& numberOfChildren);
        ~Glyph();

        void AddChild(boost::shared_ptr<Glyph> glyph);
        boost::shared_ptr<Glyph> GetChild(unsigned int index) const;

        void ClearChildren();

        virtual unsigned int GetNumberOfChildren() const;

    private:
        std::vector<boost::shared_ptr<Glyph>> m_children;
    };*/

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPH