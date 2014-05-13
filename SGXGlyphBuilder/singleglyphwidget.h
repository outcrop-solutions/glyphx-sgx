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
    enum ChildOption {
        Invisible = 0x00000000,
        ShowOnBottom = 0x00000001,
        ShowOnTop = 0x00000002,
        EnabledSpinBox = 0x00000004,
        AddChildrenButton = 0x00000008
    };

    Q_DECLARE_FLAGS(ChildOptions, ChildOption)

    SingleGlyphWidget(ChildOptions childOptions = Invisible, QWidget *parent = 0);
    ~SingleGlyphWidget();

    void SetGlyphFromWidget(boost::shared_ptr<SynGlyphX::Glyph> glyph);

public slots:
    void SetWidgetFromGlyph(boost::shared_ptr<const SynGlyphX::Glyph> glyph);

signals:
    void AddChildrenButtonClicked();
    
protected:
    void CreateWidgets(ChildOptions childOptions);
    QWidget* CreateChildrenWidget(ChildOptions childOptions);

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

Q_DECLARE_OPERATORS_FOR_FLAGS(SingleGlyphWidget::ChildOptions)

#endif // SINGLEGLYPHWIDGET_H
