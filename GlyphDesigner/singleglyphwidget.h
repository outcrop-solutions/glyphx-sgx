#ifndef SINGLEGLYPHWIDGET_H
#define SINGLEGLYPHWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <boost/shared_ptr.hpp>
#include "coloralphawidget.h"
#include "xyzwidget.h"
#include "glyph.h"
#include "groupboxsinglewidget.h"

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

    void SetNumberOfChildren(unsigned int numChildren);
    unsigned int GetNumberOfChildren() const;

    void SetGlyphFromWidget(boost::shared_ptr<SynGlyphX::GlyphProperties> glyph);
	void SetWidgetFromGlyph(boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, bool isNotRootNode);

signals:
    void AddChildrenButtonClicked();
    
protected:
    void CreateWidgets(ChildOptions childOptions);
    QWidget* CreateChildrenWidget(ChildOptions childOptions);

    QComboBox* m_geometryShapeComboBox;
    QComboBox* m_geometrySurfaceComboBox;
    QComboBox* m_topologyComboBox;

    QSpinBox* m_childrenSpinBox;

    SynGlyphX::ColorAlphaWidget* m_colorWidget;

    XYZWidget* m_translateWidget;
    XYZWidget* m_rotateWidget;
    XYZWidget* m_scaleWidget;

    QDoubleSpinBox* m_ratioSpinBox;
	SynGlyphX::GroupBoxSingleWidget* m_ratioGroupBox;

private slots:
    void OnShapeComboBoxChanged(int index);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SingleGlyphWidget::ChildOptions)

#endif // SINGLEGLYPHWIDGET_H
