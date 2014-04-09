#ifndef SINGLEGLYPHWIDGET_H
#define SINGLEGLYPHWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <boost/shared_ptr.hpp>
#include "coloralphawidget.h"
#include "xyzwidget.h"
#include "typedefs.h"
#include "glyph.h"

class SingleGlyphWidget : public QWidget
{
    Q_OBJECT

public:
    SingleGlyphWidget(bool showNumberOfChildrenWidget = true, QWidget *parent = 0);
    ~SingleGlyphWidget();

    void SetWidgetFromGlyph(boost::shared_ptr<const SynGlyphX::Glyph> glyph);
    void SetGlyphFromWidget(boost::shared_ptr<SynGlyphX::Glyph> glyph);

private slots:
    

private:
    void CreateWidgets();
    QString ShapeToString(SynGlyphX::Geometry::Shape shape);
    QString SurfaceToString(SynGlyphX::Geometry::Surface surface);
    QString TopologyTypeToString(SynGlyphX::Topology::Type topo);

    QComboBox* m_geometryShapeComboBox;
    QComboBox* m_geometrySurfaceComboBox;
    QComboBox* m_topologyComboBox;

    QSpinBox* m_childrenSpinBox;

    ColorAlphaWidget* m_colorWidget;

    XYZWidget* m_translateWidget;
    XYZWidget* m_rotateWidget;
    XYZWidget* m_scaleWidget;
};

#endif // SINGLEGLYPHWIDGET_H
